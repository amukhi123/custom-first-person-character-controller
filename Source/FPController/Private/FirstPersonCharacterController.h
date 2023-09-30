#pragma once

#include "CoreMinimal.h"
#include "Gameframework/Character.h"
#include "FirstPersonCharacterController.generated.h"

class UInputComponent;

UCLASS()
class FPCONTROLLER_API AFirstPersonCharacterController : public ACharacter
{
	GENERATED_BODY()
public:
	AFirstPersonCharacterController();

private:
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
