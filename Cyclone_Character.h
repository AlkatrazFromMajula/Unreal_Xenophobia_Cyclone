// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/SphereComponent.h"
#include "Environment_UpperEdge.h"
#include "Kismet/GameplayStatics.h"
#include "Cyclone_MovementManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Cyclone_AnimInstance.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cyclone_Character.generated.h"

UCLASS()
class XENOPHOBIA_EXP_API ACyclone_Character : public ACharacter
{
	GENERATED_BODY()

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera;
	// Expose input mapping context
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMapping;
	// Expose input actions
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RunAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	class UCyclone_MovementManager* MovementManager;

	UCyclone_AnimInstance* AnimInstance;

public:
	// Sets default values for this character's properties
	ACyclone_Character();

private:
	// Declare variables for animation blueprint
	double VelocityY{ 0 };
	double VelocityX{ 0 };
	bool IsRunning{ false };
	bool IsCrouching{ false };
	bool IsJumping{ false };
	USkeletalMeshComponent* Mesh;
	USphereComponent* OutDetSph;
	TArray<AActor*> UpperEdges;

private:
	/** Called for movement acceleration input */
	void Move(const FInputActionValue& Value);
	/** Called for movement deceleration input */
	void StopMoving();
	/** Called for run input */
	void Run(const FInputActionValue& Value);
	/** Called for crouch input */
	void Crouch(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	/** Called for looking input */
	void Jump(const FInputActionValue& Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Returns CameraBoom subobject
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Returns FollowCamera subobject
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
