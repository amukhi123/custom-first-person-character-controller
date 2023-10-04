#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Gameframework/Character.h"
#include "FirstPersonCharacterController.generated.h"

class UInputComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UCapsuleComponent;
class USoundWave;

struct FInputActionValue;

UCLASS()
class FPCONTROLLER_API AFirstPersonCharacterController : public ACharacter
{
	GENERATED_BODY()
public:
	AFirstPersonCharacterController();

private:
	enum class EPlayerState
	{
		Idle,
		Crouch,
		Walk,
		Sprint,
		Jump
	};
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> MInputMappingContext {};
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MMoveInputAction {}; 
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MLookInputAction {};
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MSprintInputAction {};
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MCrouchInputAction {};
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MJumpInputAction {};
	
	UPROPERTY(EditAnywhere, Category = Movement, meta=(ForceUnits="cm/s"))
	float MWalkSpeed {};
	
	UPROPERTY(EditAnywhere, Category = Movement, meta=(ForceUnits="cm/s"))
	float MSprintSpeed {};
	
	UPROPERTY(EditAnywhere, Category = Movement, meta=(ForceUnits="cm/s"))
	float MCrouchSpeed {};
	
	UPROPERTY(EditAnywhere, Category = Movement, meta=(ForceUnits=m))
	float MCrouchHeight {};
	
	UPROPERTY(EditAnywhere, Category = Movement)
	float MLookSensitivityX {};
	
	UPROPERTY(EditAnywhere, Category = Movement)
	float MLookSensitivityY {};
	
	UPROPERTY(EditAnywhere, Category = HeadBob)
	float MHeadBobFrequency {};

	UPROPERTY(EditAnywhere, Category = HeadBob)
	float MHeadBobAmplitude {};
	
	UPROPERTY(EditAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> MCameraComponent {};	
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float MEyesOffset {};

	UPROPERTY(EditAnywhere, Category = PhysicalMaterial)
	TObjectPtr<UMaterialInstance> MGrass {};
	
	UPROPERTY(EditAnywhere, Category = PhysicalMaterial)
	TObjectPtr<UMaterialInstance> MStone {};
	
	UPROPERTY(EditAnywhere, Category = Audio)		
	TObjectPtr<USoundWave> MStoneWalkSoundEffect {};
	
	UPROPERTY(EditAnywhere, Category = Audio)		
	TObjectPtr<USoundWave> MGrassWalkSoundEffect {};
	
	TObjectPtr<UCharacterMovementComponent> MCharacterMovementComponent {};	
	
	TObjectPtr<UCapsuleComponent> MCapsuleComponent {};	

	EPlayerState ECurrentPlayerState {};

	bool MIsPlayingSound {};
	
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void ActivateMovement(const FInputActionValue& InInputActionValue);	
	
	void ActivateSprint(const FInputActionValue& InInputActionValue);	
	
	void ActivateCrouch(const FInputActionValue& InInputActionValue);	
	
	void ActivateJump(const FInputActionValue& InInputActionValue);	
	
	void Look(const FInputActionValue& InInputActionValue);
	
	void DeactivateMovement();

	void DeactivateCrouch();
	
	void HeadBob() const;
	
	void PlaySound();
	
	void PlaySoundHelper(const EPlayerState& InPlayerState);	
	
	TWeakObjectPtr<UPhysicalMaterial> CheckSurface() const;
};
