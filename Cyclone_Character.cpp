// Fill out your copyright notice in the Description page of Project Settings.


#include "Cyclone_Character.h"

// Sets default values
ACyclone_Character::ACyclone_Character()
{
	// Fix root motion movement velocity
	SetAnimRootMotionTranslationScale(2);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void ACyclone_Character::BeginPlay()
{
	Super::BeginPlay();

	// Initialise components
	Mesh = GetMesh();
	AnimInstance = Cast<UCyclone_AnimInstance>(Mesh->GetAnimInstance());
	MovementManager = NewObject<UCyclone_MovementManager>();
	OutDetSph = Cast<USphereComponent>(GetComponentsByTag(USphereComponent::StaticClass(), FName("OuterDetectionSphere"))[0]);

	// Declare delegates for overlap events
	OutDetSph->OnComponentBeginOverlap.AddDynamic(this, &ACyclone_Character::OverlapBegin);
	OutDetSph->OnComponentEndOverlap.AddDynamic(this, &ACyclone_Character::OverlapEnd);

	// Add mapping context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

// Called to bind functionality to input
void ACyclone_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACyclone_Character::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::None, this, &ACyclone_Character::StopMoving);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACyclone_Character::Look);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ACyclone_Character::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ACyclone_Character::Run);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ACyclone_Character::Crouch);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACyclone_Character::Jump);
	}
}

void ACyclone_Character::Move(const FInputActionValue& Value)
{
	// get input vector
	FVector2D InputVector = Value.Get<FVector2D>();
	double InputY = InputVector.Y;
	double InputX = InputVector.X;

	// calculate input relative directions
	const FRotator Rotation = Controller->GetControlRotation();
	const FVector RelativeForwardVector = MovementManager->CalculateRelativeVector(Rotation, InputX, InputY);

	// calculate character rotation
	if (Mesh->GetComponentRotation() != Rotation)
	{
		// add spherical rotation to mesh
		GetMesh()->AddLocalRotation(MovementManager->CalculateRotation(Mesh->GetForwardVector(), RelativeForwardVector));
		
		// calculate velocity for bending animation
		VelocityX = MovementManager->CalculateBending(VelocityX, Mesh->GetForwardVector(), Mesh->GetRightVector(), RelativeForwardVector);
		AnimInstance->SetMovementDirection(VelocityX);
	}

	// calculate absolute input
	double AbsoluteInput = sqrt(pow(InputY, 2) + pow(InputX, 2));

	// set Y velocity
	VelocityY = MovementManager->CalculateMovement(VelocityY, AbsoluteInput);
	AnimInstance->SetMovementSpeed(VelocityY);

	//UE_LOG(LogTemp, Warning, TEXT("VelocityY_Cyclone: %f"), VelocityY);
	//UE_LOG(LogTemp, Warning, TEXT("VelocityX_Cyclone: %f"), VelocityX);
	UE_LOG(LogTemp, Warning, TEXT("Mesh: %s"), *(Mesh->GetForwardVector()).ToString());
	UE_LOG(LogTemp, Warning, TEXT("Target: %s"), *RelativeForwardVector.ToString());
}

void ACyclone_Character::StopMoving()
{
	// calculate character movement
	FVector2D MovementVector = MovementManager->CalculateDeceleration(VelocityX, VelocityY);
	VelocityX = MovementVector.X;
	VelocityY = MovementVector.Y;
	
	// set animation instance variables
	AnimInstance->SetMovementSpeed(VelocityY);
	AnimInstance->SetMovementDirection(VelocityX);
}

void ACyclone_Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}
}

void ACyclone_Character::Run(const FInputActionValue& Value)
{
	IsRunning = !IsRunning;
	// if is running max velocity will be increased
	MovementManager->SetMaxVelocity(IsRunning);
}

void ACyclone_Character::Crouch(const FInputActionValue& Value)
{
	IsCrouching = !IsCrouching;
	AnimInstance->SetIsCrouching(IsCrouching);
	// if is crouching max velocity will be increased
	MovementManager->SetMaxVelocity(IsCrouching);
}

void ACyclone_Character::Jump(const FInputActionValue& Value)
{
	if (!UpperEdges.IsEmpty())
	{
		for (AActor* Actor : UpperEdges)
		{
			AEnvironment_UpperEdge* Edge = Cast<AEnvironment_UpperEdge>(Actor);
			if (Edge->GetJumpTargetLocation() != FVector::ZeroVector)
			{
				AnimInstance->SetJumpData(true, Edge->GetJumpTargetLocation());
				break;
			}
		}
	}
}

void ACyclone_Character::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("UpperEdge")))
	{
		// add edge to the list
		if (UpperEdges.Find(OtherActor) == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("IN_Cyclone!"));
			UpperEdges.Add(OtherActor);
		}
	}
}
void ACyclone_Character::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("UpperEdge")))
	{
		// remove edge from the list
		if (UpperEdges.Find(OtherActor) != INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("OUT_Cuclone!"));
			UpperEdges.RemoveSwap(OtherActor);
		}
	}
}