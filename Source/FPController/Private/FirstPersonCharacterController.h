#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Gameframework/Character.h"
#include "FirstPersonCharacterController.generated.h"

class UInputComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;

struct FInputActionValue;

UCLASS()
class FPCONTROLLER_API AFirstPersonCharacterController : public ACharacter
{
	GENERATED_BODY()
public:
	AFirstPersonCharacterController();

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MInputMappingContext {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MMoveInputAction {}; 
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MLookInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MSprintInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MWalkSpeed {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MSprintSpeed {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MLookSensitivityX {};
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MLookSensitivityY {};
	
	UPROPERTY(EditAnywhere, Category = "Head Bob")
	float MHeadBobFrequency {};

	UPROPERTY(EditAnywhere, Category = "Head Bob")
	float MHeadBobAmplitude {};
	
	TObjectPtr<UCharacterMovementComponent> MCharacterMovementComponent {};	

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> MCameraComponent {};	
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MEyesOffset {};
	
	bool bIsSprinting {};	
	
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void ActivateMovement(const FInputActionValue& InputActionValue);	
	
	void ActivateSprint(const FInputActionValue& InputActionValue);	
	
	void Look(const FInputActionValue& InputActionValue);
	
	void DeactivateWalk();
	
	void DeactivateSprint();

	void HeadBob();
};
