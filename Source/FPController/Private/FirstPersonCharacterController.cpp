#include "FirstPersonCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

namespace
{
	// The average walking speed is 5 km/h or 1.38889 m/s
	constexpr float GPLAYER_MOVEMENT_SPEED {1.38889f};
	// The average sprinting speed for a human male is 31.414395 km/h or 8.7262208 m/s	
	constexpr float GPLAYER_RUNNING_SPEED {8.7262208f};
	constexpr float GLOOK_SENSITIVITY_X {1.f};
	constexpr float GLOOK_SENSITIVITY_Y {1.f};
}

AFirstPersonCharacterController::AFirstPersonCharacterController() : MInputMappingContext {}, MMoveInputAction {}, MSprintInputAction {}, MCurrentPlayerMovementSpeed {GPLAYER_MOVEMENT_SPEED}
{
	PrimaryActorTick.bCanEverTick = true;	
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
			enhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Sprint);
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
			AddMovementInput(GetActorForwardVector(), movementInput.X * MCurrentPlayerMovementSpeed);
		}

		if (movementInput.Y != 0)
		{
			AddMovementInput(GetActorRightVector(), movementInput.Y * MCurrentPlayerMovementSpeed);
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
		if (MCurrentPlayerMovementSpeed == GPLAYER_RUNNING_SPEED)
		{
			MCurrentPlayerMovementSpeed = GPLAYER_MOVEMENT_SPEED;
		}
		else
		{
			MCurrentPlayerMovementSpeed = GPLAYER_RUNNING_SPEED;
		}
	}
}
