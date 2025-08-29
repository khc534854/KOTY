// Fill out your copyright notice in the Description page of Project Settings.


#include "AIKartNew.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Chaos/Utilities.h"
#include "Components/BoxComponent.h"
#include "InputMappingContext.h"


// Sets default values
AAIKartNew::AAIKartNew()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//박스 콜라이더
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(100.0f, 70.0f, 10.0f));
	BoxComp->SetMobility(EComponentMobility::Movable);
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetCollisionObjectType(ECC_PhysicsBody);
	BoxComp->SetMassOverrideInKg(NAME_None, 1.f); // 1200kg

	
	SetRootComponent(BoxComp);

	BodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	BodyMeshComp->SetupAttachment(RootComponent);
	BodyMeshComp->SetWorldScale3D(FVector(1.0f,0.76f, 0.2f));
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	//만약 메쉬 로딩을 성공했다면

	if (tempMesh.Succeeded())
	{
		//MeshComp의 staticMesh를 업데이트 하고싶다.
		BodyMeshComp->SetStaticMesh(tempMesh.Object);
	}
	ConstructorHelpers::FObjectFinder<UMaterial> tempMat(TEXT("/Script/Engine.Material'/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial'"));
	if (tempMat.Succeeded())
	{
		BodyMeshComp->SetMaterial(0,tempMat.Object);
	}

	
	FRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FRWheel"));
	FRWheel->SetupAttachment(RootComponent);
	FRWheel->SetRelativeLocation(FVector(50.f, 30.f, 1.f));
	FRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FRWheelMesh"));
	FRWheelMesh->SetupAttachment(FRWheel);
	FRWheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FRWheelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	
	FLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FLWheel"));
	FLWheel->SetupAttachment(RootComponent);
	FLWheel->SetRelativeLocation(FVector(50.f, -30.f, 1.f));
	FLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLWheelMesh"));
	FLWheelMesh->SetupAttachment(FLWheel);
	FLWheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FLWheelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	
	BRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BRWheel"));
	BRWheel->SetupAttachment(RootComponent);
	BRWheel->SetRelativeLocation(FVector(-50.f, 30.f, 1.f));
	BRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BRWheelMesh"));
	BRWheelMesh->SetupAttachment(BRWheel);
	BRWheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BRWheelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	BLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BLWheel"));
	BLWheel->SetupAttachment(RootComponent);
	BLWheel->SetRelativeLocation(FVector(-50.f, -30.f, 1.f));
	BLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BLWheelMesh"));
	BLWheelMesh->SetupAttachment(BLWheel);
	BLWheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BLWheelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempWheelMesh(TEXT("/Script/Engine.StaticMesh'/Engine/EngineMeshes/Cylinder.Cylinder'"));
	if (TempWheelMesh.Succeeded())
	{
		//WheelMesh 반지름
		WheelMeshRadius = FMath::Max(TempWheelMesh.Object->GetBoundingBox().GetSize().X,TempWheelMesh.Object->GetBoundingBox().GetSize().Y) * 0.5f;

		FRWheelMesh->SetStaticMesh(TempWheelMesh.Object);
		FRWheelMesh->SetWorldScale3D(FVector(1.0f,0.9f, 1.0f));

		FLWheelMesh->SetStaticMesh(TempWheelMesh.Object);
		FLWheelMesh->SetWorldScale3D(FVector(1.0f,0.9f, 1.0f));

		BRWheelMesh->SetStaticMesh(TempWheelMesh.Object);
		BRWheelMesh->SetWorldScale3D(FVector(1.0f,0.9f, 1.0f));

		BLWheelMesh->SetStaticMesh(TempWheelMesh.Object);
		BLWheelMesh->SetWorldScale3D(FVector(1.0f,0.9f, 1.0f));
		
	}

	ConstructorHelpers::FObjectFinder<UMaterial> TempWheelMat(TEXT("/Script/Engine.Material'/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial'"));
	if (TempWheelMat.Succeeded())
	{
		FRWheelMesh->SetMaterial(0, TempWheelMat.Object);
		FLWheelMesh->SetMaterial(0, TempWheelMat.Object);
		BRWheelMesh->SetMaterial(0, TempWheelMat.Object);
		BLWheelMesh->SetMaterial(0, TempWheelMat.Object);
	}

	// ... (기존 컴포넌트들 생성 코드 위/아래 어디든 OK)

	// 스프링암 생성 + 루트(BoxComp)에 부착
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// 카트 뒤쪽/위쪽에서 보도록 기본 위치 조정
	// X는 앞(+), 뒤(-)이므로 뒤에 두려면 -X로 이동
	CameraBoom->SetRelativeLocation(FVector(-50.f, 0.f, 60.f)); // 암의 기준점
	CameraBoom->TargetArmLength = 350.f;                        // 카트로부터의 거리
	CameraBoom->SetRelativeRotation(FRotator(-5.f, 0.f, 0.f));  // 약간 아래로 내려다보는 각도

	// 카메라 움직임 품질 옵션
	CameraBoom->bUsePawnControlRotation = false;   // 컨트롤러 회전에 종속 안 함(차량 뒤를 고정 추적)
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw   = true;
	CameraBoom->bInheritRoll  = false;

	CameraBoom->bEnableCameraLag = true;           // 카메라 래그로 부드럽게
	CameraBoom->CameraLagSpeed   = 10.f;
	CameraBoom->CameraLagMaxDistance = 60.f;

	CameraBoom->bDoCollisionTest = true;           // 벽에 가려지면 암이 당겨짐(원하면 false)

	// 카메라 생성 + 스프링암 끝에 부착
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 컨트롤러 회전 무시
	
	// 에셋 경로: Content Browser에서 IMC_Vehicle 우클릭 → Copy Reference
	// static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextObj(
	// 	TEXT("/Script/EnhancedInput.InputMappingContext'/Game/AI_Player/Kart/Input/IMC_Vehicle.IMC_Vehicle'")
	// );

	// static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextObj(
	// 	TEXT("/Game/AI_Player/Kart/Input/IMC_Vehicle.IMC_Vehicle")
	// );
	
// 	UInputMappingContext* MappingContext = LoadObject<UInputMappingContext>(
// 	nullptr, TEXT("/Script/EnhancedInput.InputMappingContext'/Game/AI_Player/Kart/Input/IMC_Vehicle.IMC_Vehicle'")
// );
// 	
// 	if (MappingContextObj.Succeeded())
// 	{
// 		// MappingContextObj.Object 는 TObjectPtr<UInputMappingContext>
// 		// 따라서 Get() 으로 꺼내야 함
//
// 		AiKartMappingContext = MappingContextObj.Object;
// 	}
	ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextObj(
		TEXT("'/Game/AI_Player/Kart/Input/IMC_Vehicle.IMC_Vehicle'"));
	
	if (MappingContextObj.Succeeded())
	{
		AiKartMappingContext = MappingContextObj.Object; // ✅ Get() 호출
	}
	
	
	
	ConstructorHelpers::FObjectFinder<UInputAction> AccelerateActionObj(
		TEXT("/Script/EnhancedInput.InputAction'/Game/AI_Player/Kart/Input/IA_Accelerate.IA_Accelerate'")
	);
	ConstructorHelpers::FObjectFinder<UInputAction> SteeringActionObj(
		TEXT("/Script/EnhancedInput.InputAction'/Game/AI_Player/Kart/Input/IA_Steer.IA_Steer'")
	);

	if (AccelerateActionObj.Succeeded())
	{
		IA_Accelerate = AccelerateActionObj.Object;
	}
	if (SteeringActionObj.Succeeded())
	{
		IA_Steer = SteeringActionObj.Object;
	}

	PowerCurve = LoadObject<UCurveFloat>(
nullptr, 
TEXT("/Script/Engine.CurveFloat'/Game/AI_Player/Kart/Vehicles/PowerCurve.PowerCurve'")
);

	SteeringCurve = LoadObject<UCurveFloat>(
		nullptr,
		TEXT("/Script/Engine.CurveFloat'/Game/AI_Player/Kart/Vehicles/SteeringCurve.SteeringCurve'")
		);
	
}

// Called when the game starts or when spawned
void AAIKartNew::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어 컨트롤러 가져오기
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// 매핑 컨텍스트 추가
			if (AiKartMappingContext)
			{
				Subsystem->AddMappingContext(AiKartMappingContext, 0);
			}
		}
	}
}

// Called every frame
void AAIKartNew::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SimulateSuspension(FRWheel, FRWheelMesh);
	SimulateSuspension(FLWheel, FLWheelMesh);
	SimulateSuspension(BRWheel, BRWheelMesh);
	SimulateSuspension(BLWheel, BLWheelMesh);

	SimulateSteering(FRWheel, FRWheelMesh);
	SimulateSteering(FLWheel, FLWheelMesh);
	SimulateFriction(BRWheel, BRWheelMesh);
	SimulateFriction(BLWheel, BLWheelMesh);
	// SimulateLSteering(FLWheel, FLWheelMesh);

	SimulateAccelerate(FRWheel, FRWheelMesh);
	SimulateAccelerate(FLWheel, FLWheelMesh);
	SimulateAccelerate(BRWheel, BRWheelMesh);
	SimulateAccelerate(BLWheel, BLWheelMesh);
	
	
}

// Called to bind functionality to input
void AAIKartNew::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Accelerate)
		{
			EnhancedInput->BindAction(IA_Accelerate, ETriggerEvent::Triggered, this, &AAIKartNew::OnAccelerate);
			EnhancedInput->BindAction(IA_Accelerate, ETriggerEvent::Completed, this, &AAIKartNew::OnAccelerateReleased);

		}

		if (IA_Steer)
		{
			EnhancedInput->BindAction(IA_Steer, ETriggerEvent::Triggered, this, &AAIKartNew::OnSteer);
			// 키에서 손 뗐을 때 (Completed)
			EnhancedInput->BindAction(IA_Steer, ETriggerEvent::Completed, this, &AAIKartNew::OnSteerReleased);
		}
	}
}

void AAIKartNew::OnAccelerate(const FInputActionValue& Value)
{
	AccelInput = Value.Get<float>();
	UE_LOG(LogTemp, Display, TEXT("%f"),AccelInput);

}

void AAIKartNew::OnAccelerateReleased(const FInputActionValue& Value)
{
	AccelInput = 0.0f;

}



void AAIKartNew::OnSteer(const FInputActionValue& Value)
{
	SteeringInput = Value.Get<float>();
	// UE_LOG(LogTemp, Display, TEXT("%f"),SteeringInput);

}
void AAIKartNew::OnSteerReleased(const FInputActionValue& Value)
{
	SteeringInput = 0.0f;  // 아무 것도 안 누르면 0
}

void AAIKartNew::SimulateSuspension(USceneComponent* InSuspensionStartPos, UStaticMeshComponent* WheelMesh)
{
	FVector SuspensionStartPos = InSuspensionStartPos->GetComponentLocation();
	FVector SpringDir = InSuspensionStartPos->GetUpVector();
	FVector WheelVelocity = BoxComp->GetPhysicsLinearVelocityAtPoint(SuspensionStartPos);
	// UE_LOG(LogTemp, Display, TEXT("%f,%f,%f"), WheelVelocity.X, WheelVelocity.Y, WheelVelocity.Z);
	
	FHitResult OutHit;

	float Offset;
	float Velocity;
	float Force;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // 자기 자신 무시
	FVector End = SuspensionStartPos + (RestDistance * -1 * SpringDir);	
	WheelTouchingGround = GetWorld()->LineTraceSingleByProfile(OutHit, SuspensionStartPos, End, "BlockAll", TraceParams);

	// Debug 선
	DrawDebugLine(
		GetWorld(),
		SuspensionStartPos,
		End,
		WheelTouchingGround ? FColor::Red : FColor::Green, // 충돌 여부에 따라 색상
		false,      // PersistentLines: false → 1프레임만
		0.f,       // Duration: 2초
		0,          // Depth Priority
		2.0f        // Thickness
	);
	
	if (WheelTouchingGround) 
	{
		//서스펜션 계산
		WheelMeshLocation = OutHit.ImpactPoint + SpringDir * WheelMeshRadius;
		Offset = RestDistance - OutHit.Distance;
		Velocity = FVector::DotProduct(WheelVelocity, SpringDir);
		Force = (Offset * SpringStrength) - (Velocity * SpringDamper);
		

		//서스펜션 힘 추가
		BoxComp->AddForceAtLocation(SpringDir * Force, SuspensionStartPos);
		SuspensionForceMagnitude = (SpringDir * Force).Length();


	} else
	{
		//공중에서의 Wheel Mesh 위치
		WheelMeshLocation = SuspensionStartPos + SpringDir * -1 * (RestDistance - WheelMeshRadius);
	}

	// UE_LOG(LogTemp, Display, TEXT("%f,%f,%f"), WheelMeshLocation.X, WheelMeshLocation.Y, WheelMeshLocation.Z);
	if (DrawDebug)
	{
		DrawDebugDirectionalArrow(GetWorld(), SuspensionStartPos, SuspensionStartPos + SpringDir * Force * 0.003, 100, FColor::Green, false, 0.f, 0, 3.f);
	}
	WheelMesh->SetWorldLocation(WheelMeshLocation);

}



void AAIKartNew::SimulateSteering(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh)
{
	FVector WheelStartPos = Wheel->GetComponentLocation();
	FVector SpringDir = Wheel->GetUpVector();
	// UE_LOG(LogTemp, Display, TEXT("%f,%f,%f"), WheelVelocity.X, WheelVelocity.Y, WheelVelocity.Z);
	
	float TireGripFactor = 1.0f;
	float TireMass = 2.0f;
	
	
	float WheelTargetAngle = SteeringCurve->GetFloatValue(NormalizedCurrentSpeed) * SteeringInput * MaxSteeringAngle;
	UE_LOG(LogTemp, Warning, TEXT("%f"), WheelTargetAngle);


	Wheel->SetRelativeRotation(FRotator(
		Wheel->GetRelativeRotation().Pitch,   // 기존 Pitch 유지
		WheelTargetAngle,                     // 원하는 Yaw 값
		Wheel->GetRelativeRotation().Roll     // 기존 Roll 유지
	));
	
	FHitResult OutHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // 자기 자신 무시
	FVector End = WheelStartPos + (RestDistance * -1 * SpringDir);	
	WheelTouchingGround = GetWorld()->LineTraceSingleByProfile(OutHit, WheelStartPos, End, "BlockAll", TraceParams);

	if (WheelTouchingGround)
	{
		FVector SteeringDir = Wheel->GetRightVector();
		// UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), SteeringDir.X, SteeringDir.Y, SteeringDir.Z );
		FVector WheelVelocity = BoxComp->GetPhysicsLinearVelocityAtPoint(WheelStartPos);
		
		// UE_LOG(LogTemp, Warning, TEXT("%s %f %f %f"),*Wheel->GetName(),  WheelVelocity.X, WheelVelocity.Y, WheelVelocity.Z );

		float SteeringVel = FVector::DotProduct(SteeringDir, WheelVelocity);

		float DesiredVelChange = -SteeringVel * TireGripFactor;

		float DesiredAccel = DesiredVelChange / GetWorld()->GetDeltaSeconds();

		if (DrawDebug)
		{
			DrawDebugDirectionalArrow(GetWorld(), WheelStartPos, WheelStartPos + SteeringDir * TireMass * DesiredAccel * 0.1, 100, FColor::Red, false, 0.f, 0, 3.f);
		}
		
		BoxComp->AddForceAtLocation(SteeringDir * TireMass * DesiredAccel, WheelStartPos);

		// UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), DesiredVelChange, TireMass, DesiredAccel);
	}
	
}

void AAIKartNew::SimulateFriction(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh)
{
	FVector WheelStartPos = Wheel->GetComponentLocation();
	FVector SpringDir = Wheel->GetUpVector();
	// UE_LOG(LogTemp, Display, TEXT("%f,%f,%f"), WheelVelocity.X, WheelVelocity.Y, WheelVelocity.Z);
	
	float TireGripFactor = 1.0f;
	float TireMass = 3.0f;
	

	FHitResult OutHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // 자기 자신 무시
	FVector End = WheelStartPos + (RestDistance * -1 * SpringDir);	
	WheelTouchingGround = GetWorld()->LineTraceSingleByProfile(OutHit, WheelStartPos, End, "BlockAll", TraceParams);

	if (WheelTouchingGround)
	{
		FVector SteeringDir = -Wheel->GetRightVector();
		// UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), SteeringDir.X, SteeringDir.Y, SteeringDir.Z );
		FVector WheelVelocity = BoxComp->GetPhysicsLinearVelocityAtPoint(WheelStartPos);
		
		// UE_LOG(LogTemp, Warning, TEXT("%s %f %f %f"),*Wheel->GetName(),  WheelVelocity.X, WheelVelocity.Y, WheelVelocity.Z );

		float SteeringVel = FVector::DotProduct(SteeringDir, WheelVelocity);

		float DesiredVelChange = -SteeringVel * TireGripFactor;

		float DesiredAccel = DesiredVelChange / GetWorld()->GetDeltaSeconds();

		
		if (DrawDebug)
		{
			DrawDebugDirectionalArrow(GetWorld(), WheelStartPos, WheelStartPos + SteeringDir * TireMass * DesiredAccel * 0.1, 100, FColor::Red, false, 0.f, 0, 3.f);
		}
		
		BoxComp->AddForceAtLocation(SteeringDir * TireMass * DesiredAccel, WheelStartPos);

		//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), DesiredVelChange, TireMass, DesiredAccel);
	}
	
	
}

void AAIKartNew::SimulateAccelerate(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh)
{
	FVector WheelStartPos = Wheel->GetComponentLocation();
	FVector SpringDir = Wheel->GetUpVector();
	
	FHitResult OutHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // 자기 자신 무시
	FVector End = WheelStartPos + (RestDistance * -1 * SpringDir);	
	WheelTouchingGround = GetWorld()->LineTraceSingleByProfile(OutHit, WheelStartPos, End, "BlockAll", TraceParams);


	
	if (WheelTouchingGround)
	{
		FVector AccelDir = Wheel->GetForwardVector();

		
		if (AccelInput != 0)
		{
			float CarSpeed = FVector::DotProduct(BoxComp->GetForwardVector(), GetVelocity());

			NormalizedCurrentSpeed = FMath::Clamp(FMath::Abs(CarSpeed) / CarTopSpeed,0,1);

			float AvailableTorque = PowerCurve->GetFloatValue(NormalizedCurrentSpeed) * AccelInput;
			float ForceScale = 5000.f; 
			BoxComp->AddForceAtLocation(AccelDir * AvailableTorque * ForceScale, WheelStartPos);

	
			
			if (DrawDebug)
			{
				DrawDebugDirectionalArrow(GetWorld(), WheelStartPos, WheelStartPos + AccelDir * AvailableTorque, 100, FColor::Blue, false, 0.f, 0, 3.f);
			}			
		}
	}
	
}



