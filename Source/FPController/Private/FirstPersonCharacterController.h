#pragma once

#include "CoreMinimal.h"
#include "Gameframework/Character.h"
#include "FirstPersonCharacterController.generated.h"

class UInputComponent;
class UInputMappingContext;
class UInputAction;
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
	
	float MCurrentPlayerMovementSpeed {};	
	
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& InputActionValue);	
	
	void Look(const FInputActionValue& InputActionValue);

	void Sprint(const FInputActionValue& InputActionValue);
};
