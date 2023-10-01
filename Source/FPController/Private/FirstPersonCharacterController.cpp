#include "FirstPersonCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/RotationMatrix.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	// Movement	
	// The average walking speed is 5 km/h or 138.889 cm/s
	constexpr float GWALK_SPEED {138.889f};
	// The average sprinting speed for a human male is 31.414395 km/h or 872.62208 cm/s	
	constexpr float GSPRINT_SPEED {872.62208f};
	// Crouch walking is 62.5% slower than walking on average
	constexpr float GCROUCH_SPEED {52.083375f};
	// Used for head bobbing when idle	
	constexpr float GIDLE_SPEED {30.f};
	
	// Look	
	constexpr float GLOOK_SENSITIVITY_X {1.f};
	constexpr float GLOOK_SENSITIVITY_Y {1.f};
	constexpr float GEYES_DEFAULT_HEIGHT {160.f};	
	
	// Head Bob	
	constexpr float GHEAD_BOB_AMPLITUDE {5.f};
	constexpr float GHEAD_BOB_FREQUENCY {.025f};
	
	// Crouch
	constexpr float GCROUCH_HEIGHT {GEYES_DEFAULT_HEIGHT / 2};
}

AFirstPersonCharacterController::AFirstPersonCharacterController() : MInputMappingContext {nullptr}, MMoveInputAction {nullptr}, MSprintInputAction {nullptr}, MCrouchInputAction {nullptr}, MWalkSpeed {GWALK_SPEED}, MSprintSpeed {GSPRINT_SPEED}, MCrouchSpeed {GCROUCH_SPEED}, MCrouchHeight {GCROUCH_HEIGHT}, MLookSensitivityX {GLOOK_SENSITIVITY_X}, MLookSensitivityY {GLOOK_SENSITIVITY_Y}, MHeadBobFrequency {GHEAD_BOB_FREQUENCY}, MHeadBobAmplitude {GHEAD_BOB_AMPLITUDE}, MCameraComponent {CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"))}, MEyesOffset {}, ECurrentPlayerState {EPlayerState::Idle}, MCharacterMovementComponent {CastChecked<UCharacterMovementComponent>(GetMovementComponent())}, MCapsuleComponent {GetCapsuleComponent()}
{
	PrimaryActorTick.bCanEverTick = true;	
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MCameraComponent->SetupAttachment(GetRootComponent());
	MCameraComponent->bUsePawnControlRotation = true;
	
	AdjustPlayerHeight(GEYES_DEFAULT_HEIGHT);
	
	MCharacterMovementComponent->MaxWalkSpeed = GWALK_SPEED;	
}

void AFirstPersonCharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	if (const TObjectPtr<APlayerController> playerController {Cast<APlayerController>(GetController())})
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> localPlayerSubsystem {ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer())})
		{
			if (MInputMappingContext)
			{
				localPlayerSubsystem->AddMappingContext(MInputMappingContext, 0);	
			}
		}
	}
}

void AFirstPersonCharacterController::Tick(float DeltaTime)
{
	HeadBob();
}

void AFirstPersonCharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> enhancedInputComponent {CastChecked<UEnhancedInputComponent>(PlayerInputComponent)})
	{
		if (MMoveInputAction)
		{
			enhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateWalk);			enhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateMovement);
		}

		if (MLookInputAction)
		{
			enhancedInputComponent->BindAction(MLookInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Look);
		}
		
		if (MSprintInputAction)
		{
			enhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateSprint);
			enhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateMovement);
		}

		if (MCrouchInputAction)
		{
			enhancedInputComponent->BindAction(MCrouchInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateCrouch);
			enhancedInputComponent->BindAction(MCrouchInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateCrouch);
		}
	}
}

void AFirstPersonCharacterController::ActivateWalk(const FInputActionValue& InputActionValue)
{
	if (GetController() && InputActionValue.GetValueType() == EInputActionValueType::Axis2D)
	{
		switch (ECurrentPlayerState)
		{
			case EPlayerState::Crouch:
				MCharacterMovementComponent->MaxWalkSpeed = MCrouchSpeed;
				break;
			case EPlayerState::Idle:
				ECurrentPlayerState = EPlayerState::Walk;
			case EPlayerState::Walk:
				MCharacterMovementComponent->MaxWalkSpeed = MWalkSpeed;
				break;		
			case EPlayerState::Sprint:
				MCharacterMovementComponent->MaxWalkSpeed = MSprintSpeed;
				break;
			default:
				break;
		}

		const FVector2d movementInput {InputActionValue.Get<FVector2D>()};
		
		if (movementInput.Size() > 0)
		{
			const FRotator yawRotation {0.0, GetControlRotation().Yaw, 0.0};


			if (movementInput.X != 0)
			{
				const FVector3d forwardDirection {FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X)};
				
				AddMovementInput(forwardDirection, movementInput.X);
			}
    
			if (movementInput.Y != 0)
			{
				const FVector3d rightDirection {FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y)};
			
				AddMovementInput(rightDirection, movementInput.Y);
			}
		}
	}
}

void AFirstPersonCharacterController::ActivateSprint(const FInputActionValue& InputActionValue)
{
	if (InputActionValue.Get<bool>())
	{
		ECurrentPlayerState = EPlayerState::Sprint;
	}
}

void AFirstPersonCharacterController::ActivateCrouch(const FInputActionValue& InputActionValue)
{
	if (InputActionValue.Get<bool>())
	{
		ECurrentPlayerState = EPlayerState::Crouch;

		AdjustPlayerHeight(GEYES_DEFAULT_HEIGHT / 2);	
	}
}

void AFirstPersonCharacterController::Look(const FInputActionValue& InputActionValue)
{
	if (GetController())
	{
		const FVector2D rotationInput {InputActionValue.Get<FVector2D>()};

		if (rotationInput.Y != 0)
		{
			AddControllerPitchInput(rotationInput.Y * GLOOK_SENSITIVITY_Y);
		}
		
		if (rotationInput.X != 0)
		{
			AddControllerYawInput(rotationInput.X * GLOOK_SENSITIVITY_X);
		}
	}
}

void AFirstPersonCharacterController::DeactivateMovement()
{
	ECurrentPlayerState = EPlayerState::Idle;
	
	MCharacterMovementComponent->MaxWalkSpeed = GIDLE_SPEED;	
}

void AFirstPersonCharacterController::DeactivateCrouch()
{
	ECurrentPlayerState = EPlayerState::Idle;

	AdjustPlayerHeight(GEYES_DEFAULT_HEIGHT);
}

void AFirstPersonCharacterController::HeadBob() const
{
	if (MCameraComponent)
	{
		const double headMovement {MHeadBobAmplitude * FMath::Sin(UGameplayStatics::GetRealTimeSeconds(GetWorld()) * (GHEAD_BOB_FREQUENCY * MCharacterMovementComponent->MaxWalkSpeed))};
	
		const FVector3d newCameraPosition {0.0, 0.0, headMovement};	
		
		MCameraComponent->SetRelativeLocation(newCameraPosition);
	}
}

void AFirstPersonCharacterController::AdjustPlayerHeight(const float& InNewHeight) const
{
	const float	capsuleHalfHeight {MCapsuleComponent->GetScaledCapsuleHalfHeight()};
	
	MCapsuleComponent->SetCapsuleHalfHeight(InNewHeight);	
	
	const FVector3d eyesPosition {0.0, 0.0, MCameraComponent->GetComponentLocation().Z + capsuleHalfHeight + MEyesOffset};
	
	MCameraComponent->SetRelativeLocation(eyesPosition);	
}
