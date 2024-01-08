// Fill out your copyright notice in the Description page of Project Settings.


#include "Cyclone_MovementManager.h"

// Sets default values for this component's properties
UCyclone_MovementManager::UCyclone_MovementManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	GetDeltaSeconds = 0;
	MaxBendVelocity = 0.1f;
	MaxWalkVelocity = 0.4f;
	CurrentMaxVelocity = 0.8f;
	Acceleration = 0.025f;
	Deceleration = 0.025f;
}

// Called when the game starts
void UCyclone_MovementManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UCyclone_MovementManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GetDeltaSeconds = DeltaTime;
}

// Calculates character's rotation relative to controller
FRotator UCyclone_MovementManager::CalculateRotation(FVector Forward, FVector TargetVector)
{
	// slerp rotation
	FRotator Rotation = FRotator(0, Forward.Rotation().Yaw, 0);
	FRotator TargetRotation = FRotator(0, TargetVector.Rotation().Yaw, 0);
	Rotation = (FQuat::Slerp(Rotation.Quaternion(), TargetRotation.Quaternion(), 0.985f)).Rotator();

	// return deltha rotation
	return TargetRotation - Rotation;
}

// Calculates body bending velocity based on difference in mesh rotation and desired rotation. Returns float velocityX
float UCyclone_MovementManager::CalculateBending(float VelocityX, FVector MeshForard, FVector MeshRight, FVector TargetForward)
{
	// find angle between mesh forward and target forward vectors
	double TargetX;
	double MeshToTargetAngle = FMath::Acos(FVector::DotProduct(MeshForard, TargetForward));
	MeshToTargetAngle *= FVector::DotProduct(MeshRight, TargetForward) >= 0 ? 1 : -1;

	// calculate new velocity x, where maximum bending performs at PI / 4 degrees and more 
	if (abs(MeshToTargetAngle) > PI / 4)
		TargetX = MeshToTargetAngle > 0 ? 1 : -1;
	else
		TargetX = MeshToTargetAngle / (PI / 4);

	VelocityX = FMath::Lerp(VelocityX, abs(TargetX) > 0.25 ? TargetX : 0, MaxBendVelocity);

	return VelocityX;
}

// Calculates character's velocity based on VelocityX and VelocityY parameters. Use if input is NOT NULL
double UCyclone_MovementManager::CalculateMovement(double VelocityY, double Input)
{
	// lock or reset input 
	Input = Input <= MaxWalkVelocity ? MaxWalkVelocity : CurrentMaxVelocity;

	// accelerate or decelerate velocity according to input
	if (abs(Input - VelocityY) > Acceleration)
		VelocityY += Input > VelocityY ? Acceleration : -Acceleration;
	else
		VelocityY = Input;

	return VelocityY;
}

// Calculates character's velocity based on VelocityX and VelocityY parameters. Use if input is NULL
FVector2D UCyclone_MovementManager::CalculateDeceleration(double VelocityX, double VelocityY)
{
	// calculate Y deceleration
	VelocityY = VelocityY > Deceleration ? VelocityY - Deceleration : 0;

	// calculate X deceleration
	if (VelocityX > MaxBendVelocity)
		VelocityX -= MaxBendVelocity;
	else if (VelocityX < -MaxBendVelocity)
		VelocityX += MaxBendVelocity;
	else
		VelocityX = 0;

	return FVector2D(VelocityX, VelocityY);
}

// Sets current max velocity based on is running bool
void UCyclone_MovementManager::SetMaxVelocity(bool IsRunning)
{
	CurrentMaxVelocity = IsRunning ? 1.0f : 0.8f;
}

// Calculates jump distance and velocity
FVector UCyclone_MovementManager::CalculateJump(FVector CurrentPosition, FVector DesiredPosition)
{
	FVector2D PositionXY = FMath::Lerp(FVector2D(CurrentPosition.X, CurrentPosition.Y), FVector2D(DesiredPosition.X, DesiredPosition.Y), 0.087);
	double PositionZ = FMath::Lerp(CurrentPosition.Y, DesiredPosition.Y, 0.087);

	return FVector(PositionXY.X, PositionXY.Y, PositionZ);
}

// Calculates camera relative forward vector of length 1
FVector UCyclone_MovementManager::CalculateRelativeVector(FRotator Rotation, double FactorX, double FactorY)
{
	// calculate
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	FVector Result = Forward * FactorY + Right * FactorX;

	// normalize
	return Result / Result.Length();
}