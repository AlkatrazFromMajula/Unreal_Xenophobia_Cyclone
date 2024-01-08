// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Cyclone_AnimInstance.generated.h"

/**
 * Base class for animation blueprint
 */
UCLASS()
class XENOPHOBIA_EXP_API UCyclone_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Sets proper movement mode when jumping
	UFUNCTION(BlueprintCallable, category = "Functions")
	void SetMovementMode(bool isJumping);
	// Translates controller to jump destination
	UFUNCTION(BlueprintCallable, category = "Functions")
	void Jump();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	float MovementDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool IsCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool IsRunning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool IsJumping;

	FVector CurrentJumpDestionation;

public:
	// Sets the movement speed variable
	void SetMovementSpeed(float Value);
	// Sets the movement direction variable
	void SetMovementDirection(float Value);
	// Sets if character is crouching
	void SetIsCrouching(bool Value);
	// Sets if character is jumping and jump destination
	void SetJumpData(bool Value, FVector JumpDestination);

	
};
