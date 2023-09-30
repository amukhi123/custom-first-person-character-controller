#include "FirstPersonCharacterController.h"

AFirstPersonCharacterController::AFirstPersonCharacterController()
{
	PrimaryActorTick.bCanEverTick = true;	
}

void AFirstPersonCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void AFirstPersonCharacterController::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Player"))
}

void AFirstPersonCharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
}

