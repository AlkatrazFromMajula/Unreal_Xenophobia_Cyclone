// Fill out your copyright notice in the Description page of Project Settings.


#include "Cyclone_AnimInstance.h"

// Sets proper movement mode when jumping
void UCyclone_AnimInstance::SetMovementMode(bool Value)
{
	if (Value)
	{
		if (UCharacterMovementComponent* MovComp = Cast<UCharacterMovementComponent>(TryGetPawnOwner()->GetMovementComponent()))
		{
			MovComp->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}
	else
	{
		if (UCharacterMovementComponent* MovComp = Cast<UCharacterMovementComponent>(TryGetPawnOwner()->GetMovementComponent()))
		{
			MovComp->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}
}

// Translates controller to jump destination
void UCyclone_AnimInstance::Jump()
{
	if (APawn* Pawn = TryGetPawnOwner())
	{
		FVector CurrentPosition = Pawn->GetActorLocation();

		FVector2D PositionXY = FMath::Lerp(FVector2D(CurrentPosition.X, CurrentPosition.Y), FVector2D(CurrentJumpDestionation.X, CurrentJumpDestionation.Y), 0.1);
		double PositionZ = FMath::Lerp(CurrentPosition.Z, CurrentJumpDestionation.Z, 0.1);

		Pawn->TeleportTo(FVector(PositionXY.X, PositionXY.Y, PositionZ), Pawn->GetActorRotation(), false, false);
	}
}

// Sets the movement speed variable
void UCyclone_AnimInstance::SetMovementSpeed(float Value)
{
	MovementSpeed = Value;
	UE_LOG(LogTemp, Warning, TEXT("VelocityY_Anim: %f"), Value);
}
// Sets the movement direction variable
void UCyclone_AnimInstance::SetMovementDirection(float Value)
{
	MovementDirection = Value;
	UE_LOG(LogTemp, Warning, TEXT("VelocityX_Anim: %f"), Value);
}
// Sets if character is crouching
void UCyclone_AnimInstance::SetIsCrouching(bool Value)
{
	IsCrouching = Value;
}
void UCyclone_AnimInstance::SetJumpData(bool Value, FVector JumpDestination)
{
	IsJumping = Value;
	CurrentJumpDestionation = JumpDestination;
}