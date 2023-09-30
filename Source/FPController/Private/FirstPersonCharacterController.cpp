#include "FirstPersonCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
	// The average walking speed is 5 km/h or 138.889 cm/s
	constexpr float GPLAYER_WALK_SPEED {138.889f};
	// The average sprinting speed for a human male is 31.414395 km/h or 872.62208 cm/s	
	constexpr float GPLAYER_SPRINT_SPEED {872.62208f};
	constexpr float GLOOK_SENSITIVITY_X {1.f};
	constexpr float GLOOK_SENSITIVITY_Y {1.f};
}

AFirstPersonCharacterController::AFirstPersonCharacterController() : MInputMappingContext {}, MMoveInputAction {}, MSprintInputAction {}, MCharacterMovementComponent {CastChecked<UCharacterMovementComponent>(GetMovementComponent())}, MWalkSpeed {GPLAYER_WALK_SPEED}, MSprintSpeed {GPLAYER_SPRINT_SPEED}, MLookSensitivityX {GLOOK_SENSITIVITY_X}, MPlayerLookSensitivityY {GLOOK_SENSITIVITY_Y}
{
	PrimaryActorTick.bCanEverTick = true;	
	
	MCharacterMovementComponent->MaxWalkSpeed = GPLAYER_WALK_SPEED;	
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
	MCharacterMovementComponent->VisualizeMovement();
	UE_LOG(LogTemp, Warning, TEXT("%f"), MCharacterMovementComponent->MaxWalkSpeed)
}

void AFirstPersonCharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> enhancedInputComponent {CastChecked<UEnhancedInputComponent>(PlayerInputComponent)})
	{
		if (MMoveInputAction)
		{
			enhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Move);
		}

		if (MLookInputAction)
		{
			enhancedInputComponent->BindAction(MLookInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Look);
		}
		
		if (MSprintInputAction)
		{
			enhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Started, this, &AFirstPersonCharacterController::Sprint);
		}
	}
}

void AFirstPersonCharacterController::Move(const FInputActionValue& InputActionValue)
{
	if (GetController())
	{
		const FVector2D movementInput {InputActionValue.Get<FVector2D>()};
		
		if (movementInput.X != 0)
		{
			AddMovementInput(GetActorForwardVector(), movementInput.X);
		}

		if (movementInput.Y != 0)
		{
			AddMovementInput(GetActorRightVector(), movementInput.Y);
		}
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

void AFirstPersonCharacterController::Sprint(const FInputActionValue& InputActionValue)
{
	if (InputActionValue.Get<bool>())
	{
		if (MCharacterMovementComponent->MaxWalkSpeed == MSprintSpeed)
		{
			MCharacterMovementComponent->MaxWalkSpeed = MWalkSpeed;
		}
		else
		{
			MCharacterMovementComponent->MaxWalkSpeed = MSprintSpeed;
		}
	}
}
