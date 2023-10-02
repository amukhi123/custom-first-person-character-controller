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
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MInputMappingContext {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MMoveInputAction {}; 
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MLookInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MSprintInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MCrouchInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MJumpInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MWalkSpeed {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MSprintSpeed {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MCrouchSpeed {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MCrouchHeight {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MLookSensitivityX {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MLookSensitivityY {};
	
	UPROPERTY(EditAnywhere, Category = "Head Bob")
	float MHeadBobFrequency {};

	UPROPERTY(EditAnywhere, Category = "Head Bob")
	float MHeadBobAmplitude {};
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> MCameraComponent {};	
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MEyesOffset {};
	
	TObjectPtr<UCharacterMovementComponent> MCharacterMovementComponent {};	
	
	TObjectPtr<UCapsuleComponent> MCapsuleComponent {};	

	EPlayerState ECurrentPlayerState {};	
	
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void ActivateWalk(const FInputActionValue& InputActionValue);	
	
	void ActivateSprint(const FInputActionValue& InputActionValue);	
	
	void ActivateCrouch(const FInputActionValue& InputActionValue);	
	
	void ActivateJump(const FInputActionValue& InInputActionValue);	
	
	void Look(const FInputActionValue& InputActionValue);
	
	void DeactivateMovement();

	void DeactivateCrouch();
	
	void HeadBob() const;
};
