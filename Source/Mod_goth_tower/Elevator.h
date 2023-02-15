// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"

enum class ETriggerBehavior : uint8
{
	ETB_Pressed,
	ETB_Released,
	ETB_Waiting
};

UCLASS()
class MOD_GOTH_TOWER_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	AElevator();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerEntered( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	/* Components */
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Base;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Box;


	/* Elevator */
	
	FVector ElevatorStart;

	FVector ElevatorEnd;

	UPROPERTY(EditInstanceOnly, Category = "Speed")
	float ElevatorMoveDistance = 10.f;

	UPROPERTY(EditInstanceOnly, Category = "Speed")
	float ElevatorInterpSpeed;

	/** Vitesse qui incrémentera à chaque frame pour pouvoir commencer le déplacement de manière smooth */
	float CurrElevatorInterpSpeed = 0.1f;

	bool bShouldElevatorMove = false;


	/* Trigger */
	
	UPROPERTY(EditInstanceOnly, Category = "Speed")
	float TriggerInterpSpeed;

	FVector TriggerStart;

	FVector TriggerEnd;

	UPROPERTY(EditInstanceOnly, Category = "Speed")
	float TriggerMoveDistance = 10.f;

	bool bShouldTriggerMove = false;

	/** État du trigger. Sert à savoir si le trigger est pesé, relâché ou en attente d'être pesé. */
	ETriggerBehavior TriggerBehavior;

	UPROPERTY(EditInstanceOnly, Category = "Speed")
	float IncrementSpeed = 0.005f;
	

	void MoveTrigger(float DeltaTime);
	void MoveElevator(float DeltaTime);
	void SetTriggerStartEndPoint();
	void SwapVectors(FVector& Vec1, FVector& Vec2);

};




