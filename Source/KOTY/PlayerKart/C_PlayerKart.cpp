// Fill out your copyright notice in the Description page of Project Settings.


#include "KOTY/PlayerKart/C_PlayerKart.h"
#include "DrawDebugHelpers.h" // 디버그 라인 그리기용 (선택 사항)

// Sets default values
AC_PlayerKart::AC_PlayerKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);
	
	for (int a = 0; a < 4; a++)
	{
		WheelsComponents.Push(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelComponent") + a));
		WheelsComponents[a]->SetupAttachment(BoxComponent);
	}
	
	WheelsComponents[0]->SetRelativeLocation(FVector( 120,  70, 0));
	WheelsComponents[1]->SetRelativeLocation(FVector( 120, -70, 0));
	WheelsComponents[2]->SetRelativeLocation(FVector(-120,  70, 0));
	WheelsComponents[3]->SetRelativeLocation(FVector(-120, -70, 0));

	MaxAccelerationTime = Acceleration * 2;
}

// Called when the game starts or when spawned
void AC_PlayerKart::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			// 입력 매핑 컨텍스트를 추가합니다.
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

// Called every frame
void AC_PlayerKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAcceleration)
	{
		DegreaseAccelerationTime();
	}
	
	for (int a = 0; a < 4; a++)
	{
		Suspension(WheelsComponents[a]);
	}
	
	SetCurrentSpeed();
	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * CurSpeed * DeltaTime;

	SetActorLocation(NewLocation);
	
	

	UE_LOG(LogTemp, Display, TEXT("Acceleration Time : %f"), AccelerationTime);
	UE_LOG(LogTemp, Display, TEXT("Current Speed : %f"), CurSpeed);
	
}

// Called to bind functionality to input
void AC_PlayerKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input Component로 형변환합니다.
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// 입력 액션을 함수에 바인딩합니다.
		if (AccelerationAction)
		{
			EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Triggered, this, &AC_PlayerKart::StartAccelerator);
			EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Completed, this, &AC_PlayerKart::EndAccelerator);
		}
	}
}

void AC_PlayerKart::SetCurrentSpeed()
{
	if (AccelerationTime > 0)
	{
		if (AccelerationTime >= MaxAccelerationTime)
		{
			CurSpeed = MaxSpeed;
			AccelerationTime = MaxAccelerationTime;
		}
		else
		{
			CurSpeed = clamp(FMath::Sin(Acceleration * AccelerationTime) * MaxSpeed, 0.f, MaxSpeed);
		}
		
	}
	else
	{
		if (AccelerationTime <= MaxAccelerationTime * -1.f)
		{
			CurSpeed = MaxSpeed * -1.f;
			AccelerationTime = MaxAccelerationTime * -1.f;
		}
		else
		{
			CurSpeed = clamp(FMath::Sin(Acceleration * AccelerationTime) * MaxSpeed, MaxSpeed * -1.f, 0.f);
		}
	}
}

void AC_PlayerKart::DegreaseAccelerationTime()
{
	if (AccelerationTime > 0)
	{
		AccelerationTime = FMath::Clamp(AccelerationTime - (GetWorld()->GetDeltaSeconds() * BrakePower), 0.f, MaxAccelerationTime);
	}
	else
	{
		AccelerationTime = FMath::Clamp(AccelerationTime + (GetWorld()->GetDeltaSeconds() * BrakePower), MaxAccelerationTime * -1.f,0.f);
	}
}

void AC_PlayerKart::Suspension(USceneComponent* Component)
{
	FVector StartLocation = Component->GetComponentLocation();
	
	// 라인 트레이스 끝 위치 (예: 현재 위치에서 아래로 1000cm)
	FVector EndLocation = StartLocation - FVector(0, 0, 100);
	
	// 충돌 결과를 담을 구조체
	FHitResult HitResult;
	
	// 충돌 쿼리 매개변수
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 충돌 체크에서 자기 자신을 무시하도록 설정
	QueryParams.bTraceComplex = false; // 단순 충돌(Simple Collision)만 체크
	
	// Line Trace 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility, // 충돌 채널 (예: Visibility)
		QueryParams
	);
	
	// 디버그 라인 그리기 (선택 사항)
	DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		bHit ? FColor::Red : FColor::Green,
		false, // 영구 표시 여부
		-1.0f, // 표시 시간 (-1.0f는 한 프레임만)
		0,
		1.0f // 선의 두께
	);
	
	// 충돌이 발생했을 때의 로직
	if (bHit)
	{
		FVector Force =
			(1 - FMath::GetMappedRangeValueClamped(FVector2f(0, 100), FVector2f(0.0f, 1.0f), HitResult.Distance)) *
			(HitResult.TraceStart - HitResult.TraceEnd) * 65000;
		
		BoxComponent->AddForceAtLocation(Force, Component->GetComponentLocation());
	}
	
}

void AC_PlayerKart::StartAccelerator(const FInputActionValue& Value)
{
	bIsAcceleration = true;
	float AccelerationDir = Value.Get<float>();
	
	if (AccelerationDir > 0)
	{
		AccelerationTime += GetWorld()->DeltaTimeSeconds;
	}
	else
	{
		AccelerationTime -= GetWorld()->DeltaTimeSeconds;
	}

	AccelerationTime = FMath::Clamp(AccelerationTime, -MaxAccelerationTime, MaxAccelerationTime); 
}

void AC_PlayerKart::EndAccelerator(const FInputActionValue& Value)
{
	bIsAcceleration = false;
}

