#include "FirstPersonCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AFirstPersonCharacterController::AFirstPersonCharacterController() : m_InputMappingContext {}, m_MoveInputAction {}
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
			if (m_InputMappingContext)
			{
				localPlayerSubsystem->AddMappingContext(m_InputMappingContext, 0);	
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
		enhancedInputComponent->BindAction(m_MoveInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Move);
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
