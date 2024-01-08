// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cyclone_MovementManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XENOPHOBIA_EXP_API UCyclone_MovementManager : public UActorComponent
{
	GENERATED_BODY()

private:
	float GetDeltaSeconds;
	float MaxBendVelocity;
	float MaxWalkVelocity;
	float CurrentMaxVelocity;
	float Acceleration;
	float Deceleration;

public:	
	// Sets default values for this component's properties
	UCyclone_MovementManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Calculates character's velocity based on VelocityX and VelocityY parameters. Use if input is NOT NULL
	double CalculateMovement(double VelocityY, double Input);

	// Calculates character's velocity based on VelocityX and VelocityY parameters. Use if input is NULL
	FVector2D CalculateDeceleration(double VelocityX, double VelocityY);

	// Sphericaly interpolates between character rotation and desired rotation. Returns delta rotation
	FRotator CalculateRotation(FVector Forward, FVector TargetVector);

	// Calculates body bending velocity based on difference in mesh rotation and desired rotation. Returns float velocityX
	float CalculateBending(float VelocityX, FVector MeshForard, FVector MeshRight, FVector TargetForward);

	// Sets current max velocity based on is running bool
	void SetMaxVelocity(bool IsRunning);

	// Calculates camera relative forward vector of length 1
	FVector CalculateRelativeVector(FRotator Rotation, double FactorX, double FactorY);

	// Calculates jump distance and velocity
	FVector CalculateJump(FVector CurrentPosition, FVector DesiredPosition);
};
