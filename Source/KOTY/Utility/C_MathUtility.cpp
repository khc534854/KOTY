// Fill out your copyright notice in the Description page of Project Settings.


#include "C_MathUtility.h"

float UC_MathUtility::EaseOutElastic(float x)
{
	const float c4 = (2 * UE_PI) / 3.f;

	if (x <= 0.f) return 0.f;
	if (x >= 1.f) return 1.f;

	// -10 : 진동 폭, 
	return FMath::Pow(2.f, -20.f * x) * FMath::Sin((x * 25.f - 0.f) * c4) + 1.f;
}
