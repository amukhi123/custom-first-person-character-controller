#include "FirstPersonCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/RotationMatrix.h"
#include "Math/Vector.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Sound/SoundWave.h"

namespace
{
	// Movement	
	// The average walking speed is 5 km/h or 138.889 cm/s
	constexpr float GWalkSpeed {138.889f};
	// The average sprinting speed for a human male is 31.414395 km/h or 872.62208 cm/s	
	constexpr float GSprintSpeed {872.62208f};
	// Crouch walking is 62.5% slower than walking on average
	constexpr float GCrouchSpeed {52.083375f};
	// Used for head bobbing when idle	
	constexpr float GIdleSpeed {30.f};
	
	// Look	
	constexpr float GLookSensitivityX {1.f};
	constexpr float GLookSensitivityY {1.f};
	constexpr float GEyesHeight {160.f};	
	
	// Head Bob	
	constexpr float GHeadBobAmplitude {5.f};
	constexpr float GHeadBobFrequency {.025f};
	
	// Crouch
	constexpr float GCrouchHeight {GEyesHeight / 2};
}

AFirstPersonCharacterController::AFirstPersonCharacterController() : MInputMappingContext {nullptr}, MMoveInputAction {}, MSprintInputAction {}, MCrouchInputAction {}, MWalkSpeed {GWalkSpeed}, MSprintSpeed {GSprintSpeed}, MCrouchSpeed {GCrouchSpeed}, MCrouchHeight {GCrouchHeight}, MLookSensitivityX {GLookSensitivityX}, MLookSensitivityY {GLookSensitivityY}, MHeadBobFrequency {GHeadBobFrequency}, MHeadBobAmplitude {GHeadBobAmplitude}, MCameraComponent {CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"))}, MEyesOffset {}, MGrass {}, MStone {}, MStoneWalkSoundEffect {}, MGrassWalkSoundEffect {}, MCharacterMovementComponent {CastChecked<UCharacterMovementComponent>(GetMovementComponent())}, MCapsuleComponent {GetCapsuleComponent()}, ECurrentPlayerState {EPlayerState::Idle}, MIsPlayingSound {}
{
	PrimaryActorTick.bCanEverTick = true;	
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MCameraComponent->SetupAttachment(GetRootComponent());
	MCameraComponent->bUsePawnControlRotation = true;
	
	MCapsuleComponent->SetCapsuleHalfHeight(GEyesHeight);

	const FVector3d EyesPosition {0.0, 0.0, MCameraComponent->GetComponentLocation().Z + MCapsuleComponent->GetScaledCapsuleHalfHeight() + MEyesOffset};
	
	MCameraComponent->SetRelativeLocation(EyesPosition);	
	
	MCharacterMovementComponent->MaxWalkSpeed = GWalkSpeed;
	MCharacterMovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;
	MCharacterMovementComponent->SetCrouchedHalfHeight(MCrouchHeight + MEyesOffset);
}

void AFirstPersonCharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	if (const TObjectPtr<const APlayerController> PlayerController {Cast<APlayerController>(GetController())})
	{
		if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> LocalPlayerSubsystem {ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())})
		{
			if (MInputMappingContext)
			{
				LocalPlayerSubsystem->AddMappingContext(MInputMappingContext, 0);	
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

	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent {CastChecked<UEnhancedInputComponent>(PlayerInputComponent)})
	{
		if (MMoveInputAction)
		{
			EnhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateMovement);			EnhancedInputComponent->BindAction(MMoveInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateMovement);
		}

		if (MLookInputAction)
		{
			EnhancedInputComponent->BindAction(MLookInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::Look);
		}
		
		if (MSprintInputAction)
		{
			EnhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateSprint);
			EnhancedInputComponent->BindAction(MSprintInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateMovement);
		}

		if (MCrouchInputAction)
		{
			EnhancedInputComponent->BindAction(MCrouchInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateCrouch);
			EnhancedInputComponent->BindAction(MCrouchInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateCrouch);
		}
		
		if (MJumpInputAction)
		{
			EnhancedInputComponent->BindAction(MJumpInputAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacterController::ActivateJump);
			EnhancedInputComponent->BindAction(MJumpInputAction, ETriggerEvent::Completed, this, &AFirstPersonCharacterController::DeactivateMovement);
		}
	}
}

void AFirstPersonCharacterController::ActivateMovement(const FInputActionValue& InInputActionValue)
{
	if (GetController() && InInputActionValue.GetValueType() == EInputActionValueType::Axis2D)
	{
		switch (ECurrentPlayerState)
		{
			case EPlayerState::Crouch:
				MCharacterMovementComponent->MaxWalkSpeedCrouched = MCrouchSpeed;
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
		
		const FVector2d MovementInput {InInputActionValue.Get<FVector2D>()};
		
		if (MovementInput.Size() > 0)
		{
			const FRotator YawRotation {0.0, GetControlRotation().Yaw, 0.0};

			if (MovementInput.X != 0)
			{
				const FVector3d ForwardDirection {FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)};
				
				AddMovementInput(ForwardDirection, MovementInput.X);
			}
    
			if (MovementInput.Y != 0)
			{
				const FVector3d RightDirection {FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)};
			
				AddMovementInput(RightDirection, MovementInput.Y);
			}
			
			PlaySoundHelper(ECurrentPlayerState);	
		}
	}
}

void AFirstPersonCharacterController::ActivateSprint(const FInputActionValue& InInputActionValue)
{
	if (InInputActionValue.Get<bool>())
	{
		ECurrentPlayerState = EPlayerState::Sprint;
	}
}

void AFirstPersonCharacterController::ActivateCrouch(const FInputActionValue& InInputActionValue)
{
	if (InInputActionValue.Get<bool>())
	{
		ECurrentPlayerState = EPlayerState::Crouch;
		
		Crouch();
	}
}

void AFirstPersonCharacterController::ActivateJump(const FInputActionValue& InInputActionValue)
{
	if (InInputActionValue.Get<bool>())
	{
		ECurrentPlayerState = EPlayerState::Jump;
		
		Jump();
	}
}

void AFirstPersonCharacterController::Look(const FInputActionValue& InInputActionValue)
{
	if (GetController())
	{
		const FVector2D RotationInput {InInputActionValue.Get<FVector2D>()};

		if (RotationInput.Y != 0)
		{
			AddControllerPitchInput(RotationInput.Y * GLookSensitivityY);
		}
		
		if (RotationInput.X != 0)
		{
			AddControllerYawInput(RotationInput.X * GLookSensitivityX);
		}
	}
}

void AFirstPersonCharacterController::DeactivateMovement()
{
	ECurrentPlayerState = EPlayerState::Idle;
	
	MCharacterMovementComponent->MaxWalkSpeed = GIdleSpeed;	
}

void AFirstPersonCharacterController::DeactivateCrouch()
{
	ECurrentPlayerState = EPlayerState::Idle;

	UnCrouch();
}

void AFirstPersonCharacterController::HeadBob() const
{
	if (MCameraComponent)
	{
		float SpeedModifier {};	
	
		switch (ECurrentPlayerState)
		{
			case EPlayerState::Idle:
				SpeedModifier = GIdleSpeed;
				break;
    		case EPlayerState::Crouch:
    			SpeedModifier = MCharacterMovementComponent->MaxWalkSpeedCrouched;
    			break;
			default:
				SpeedModifier = MCharacterMovementComponent->MaxWalkSpeed;
				break;
		}
	
		const double HeadMovementOffset {MHeadBobAmplitude * FMath::Sin(UGameplayStatics::GetRealTimeSeconds(GetWorld()) * (GHeadBobFrequency * SpeedModifier))};
	
		const FVector3d NewCameraPosition {0.0, 0.0, HeadMovementOffset};	
		
		MCameraComponent->SetRelativeLocation(NewCameraPosition);
	}
}

void AFirstPersonCharacterController::PlaySound()
{
	const TWeakObjectPtr ValidPhysicalMaterial {CheckSurface()};
	
	if (ValidPhysicalMaterial.IsValid())
	{
		if (const TObjectPtr<const UPhysicalMaterial> HitPhysicalMaterial {ValidPhysicalMaterial.Get()})
		{
			if (HitPhysicalMaterial->SurfaceType == MGrass->PhysMaterial->SurfaceType)
			{
				const TObjectPtr<USoundBase> GrassFootstepSoundEffect {CastChecked<USoundBase>(MGrassWalkSoundEffect)};
    		
				UGameplayStatics::PlaySoundAtLocation(this, GrassFootstepSoundEffect, GetActorLocation(), GetControlRotation());
			}
			else if (HitPhysicalMaterial->SurfaceType == MStone->PhysMaterial->SurfaceType)
			{
				const TObjectPtr<USoundBase> StoneFootstepSoundEffect {CastChecked<USoundBase>(MStoneWalkSoundEffect)};
    		
				UGameplayStatics::PlaySoundAtLocation(this, StoneFootstepSoundEffect, GetActorLocation(), GetControlRotation());
			}
		}
	}

	if (MIsPlayingSound)
	{
		MIsPlayingSound = false;
	}
}

TWeakObjectPtr<UPhysicalMaterial> AFirstPersonCharacterController::CheckSurface() const
{
	if (const TObjectPtr<const UWorld> World {GetWorld()})
	{
		FHitResult HitResult {};

		FCollisionQueryParams CollisionQueryParams {};

		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.bReturnPhysicalMaterial = true;
		
		constexpr float DownVectorMultiplier {1000.f};	
	
		World->LineTraceSingleByChannel(HitResult, GetActorLocation(), FVector3d::DownVector * DownVectorMultiplier, ECC_WorldStatic, CollisionQueryParams);

		return HitResult.PhysMaterial;
	}

	return nullptr;
}

void AFirstPersonCharacterController::PlaySoundHelper(const EPlayerState& InPlayerState)
{
	constexpr float CrouchSoundDelay {1.1f};	
	constexpr float SprintSoundDelay {.15f};	
	constexpr float WalkSoundDelay {.75f};	
	constexpr float DefaultSoundDelay {0.f};	
	
	float SoundDelay {};

	switch (InPlayerState)
	{
    	case EPlayerState::Crouch:
    		SoundDelay = CrouchSoundDelay;
    		break;
    	case EPlayerState::Walk:
    		SoundDelay = WalkSoundDelay;
    		break;
    	case EPlayerState::Sprint:
    		SoundDelay = SprintSoundDelay;
    		break;
    	default:
    		SoundDelay = DefaultSoundDelay;
    		break;
	}
	
	if (MIsPlayingSound == false && SoundDelay != DefaultSoundDelay)
	{
		MIsPlayingSound = true;
		
		FTimerHandle SoundHandle {};
		
		GetWorldTimerManager().SetTimer(SoundHandle, this, &AFirstPersonCharacterController::PlaySound, SoundDelay, false);
	}	
}
