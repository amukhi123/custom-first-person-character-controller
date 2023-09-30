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
	TObjectPtr<UInputMappingContext> m_InputMappingContext {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> m_LookInputAction {};
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> m_MoveInputAction {}; 
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& InputActionValue);	
	
	void Look(const FInputActionValue& InputActionValue);	
};
