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
	// Used for head bobbing when idle	
	constexpr float GIDLE_SPEED {75.f};
	
	// Look	
	constexpr float GLOOK_SENSITIVITY_X {1.f};
	constexpr float GLOOK_SENSITIVITY_Y {1.f};
	
	// Head Bob	
	constexpr float GHEAD_BOB_AMPLITUDE {5.f};
	constexpr float GHEAD_BOB_FREQUENCY {.025f};
}

AFirstPersonCharacterController::AFirstPersonCharacterController() : MInputMappingContext {nullptr}, MMoveInputAction {nullptr}, MSprintInputAction {nullptr}, MWalkSpeed {GWALK_SPEED}, MSprintSpeed {GSPRINT_SPEED}, MLookSensitivityX {GLOOK_SENSITIVITY_X}, MLookSensitivityY {GLOOK_SENSITIVITY_Y}, MHeadBobFrequency {GHEAD_BOB_FREQUENCY}, MHeadBobAmplitude {GHEAD_BOB_AMPLITUDE}, MCharacterMovementComponent {CastChecked<UCharacterMovementComponent>(GetMovementComponent())}, MCameraComponent {CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"))}, MEyesOffset {0.f}, bIsSprinting {false}
{
	PrimaryActorTick.bCanEverTick = true;	
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MCameraComponent->SetupAttachment(GetRootComponent());
	MCameraComponent->bUsePawnControlRotation = true;
	
	const float	capsuleHalfHeight {GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
	
	const FVector3d eyesPosition {0.0, 0.0, MCameraComponent->GetComponentLocation().Z + capsuleHalfHeight + MEyesOffset};
	
	MCameraComponent->SetRelativeLocation(eyesPosition);	
	
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
			enhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateMovement);			enhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateWalk);
		}

		if (MLookInputAction)
		{
			enhancedInputComponent->BindAction(MLookInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Look);
		}
		
		if (MSprintInputAction)
		{
			enhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateSprint);
			enhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateSprint);
		}
	}
}

void AFirstPersonCharacterController::ActivateMovement(const FInputActionValue& InputActionValue)
{
	if (GetController() && InputActionValue.GetValueType() == EInputActionValueType::Axis2D)
	{
		if (bIsSprinting)
		{
			MCharacterMovementComponent->MaxWalkSpeed = MSprintSpeed;
		}
		else
		{
			MCharacterMovementComponent->MaxWalkSpeed = MWalkSpeed;
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
	bIsSprinting = InputActionValue.Get<bool>();
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

void AFirstPersonCharacterController::HeadBob()
{
	if (MCameraComponent)
	{
		const double headMovement {MHeadBobAmplitude * FMath::Sin(UGameplayStatics::GetRealTimeSeconds(GetWorld()) * (GHEAD_BOB_FREQUENCY * MCharacterMovementComponent->MaxWalkSpeed))};
	
		const FVector3d newCameraPosition {0.0, 0.0, headMovement};	
		
		MCameraComponent->SetRelativeLocation(newCameraPosition);
	}
}

void AFirstPersonCharacterController::DeactivateWalk()
{
	MCharacterMovementComponent->MaxWalkSpeed = GIDLE_SPEED;
}

void AFirstPersonCharacterController::DeactivateSprint()
{
	bIsSprinting = false;

	MCharacterMovementComponent->MaxWalkSpeed = GIDLE_SPEED;
}
