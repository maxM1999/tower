// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"

#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"



AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	Base->SetupAttachment(GetRootComponent());

	Trigger = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Mesh"));
	Trigger->SetupAttachment(GetRootComponent());

	TriggerBehavior = ETriggerBehavior::ETB_Waiting;
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnTriggerEntered);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Initialiser manuellement le start et le end selon la pos initiale et la move distance.
	ElevatorStart = GetActorLocation();
	ElevatorEnd = FVector(ElevatorStart.X, ElevatorStart.Y, ElevatorStart.Z + ElevatorMoveDistance);

	TriggerStart = Trigger->GetComponentLocation();
	TriggerEnd = FVector(TriggerStart.X, TriggerStart.Y, TriggerStart.Z + TriggerMoveDistance);
}

void AElevator::OnTriggerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if(TriggerBehavior == ETriggerBehavior::ETB_Waiting)
		{
			TriggerBehavior = ETriggerBehavior::ETB_Pressed;
			bShouldTriggerMove = true;
		}
	}
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bShouldTriggerMove)
	{
		MoveTrigger(DeltaTime);
	}

	if(bShouldElevatorMove)
	{
		MoveElevator(DeltaTime);
	}
}

void AElevator::MoveTrigger(float DeltaTime)
{
	if(!Trigger) return;
	
	const FVector NewTriggerLoc = FMath::VInterpTo(Trigger->GetComponentLocation(), TriggerEnd, DeltaTime, TriggerInterpSpeed);

	Trigger->SetWorldLocation(NewTriggerLoc);

	if(Trigger->GetComponentLocation().Equals(TriggerEnd, 0.1f))
	{
		Trigger->SetWorldLocation(TriggerEnd);
		bShouldTriggerMove = false;
		
		// Lorsque la trigger a fini de remonter.
		if(TriggerBehavior == ETriggerBehavior::ETB_Released)
		{
			TriggerBehavior = ETriggerBehavior::ETB_Waiting;
			SetTriggerStartEndPoint();
		}
		// Lorsque la trigger fini de descendre.
		else if(TriggerBehavior == ETriggerBehavior::ETB_Pressed)
		{
			bShouldElevatorMove = true;
		}
	}
}

void AElevator::MoveElevator(float DeltaTime)
{
	const FVector NewLoc = FMath::VInterpTo(GetActorLocation(), ElevatorEnd, DeltaTime, CurrElevatorInterpSpeed);
	SetActorLocation(NewLoc);
	
	CurrElevatorInterpSpeed += IncrementSpeed;
	if(CurrElevatorInterpSpeed > ElevatorInterpSpeed)
	{
		CurrElevatorInterpSpeed = ElevatorInterpSpeed;
	}

	if(GetActorLocation().Equals(ElevatorEnd, 0.5f))
	{
		bShouldElevatorMove = false;
		SetActorLocation(ElevatorEnd);
		SwapVectors(ElevatorStart, ElevatorEnd);
		CurrElevatorInterpSpeed = 0.1f;

		// Une fois arrivée à destination, set le nouveau start et end du trigger.
		SetTriggerStartEndPoint();
		bShouldTriggerMove = true;
		TriggerBehavior = ETriggerBehavior::ETB_Released;
	}
}

void AElevator::SetTriggerStartEndPoint()
{
	TriggerMoveDistance = -TriggerMoveDistance;
	const FVector CurrTriggerLoc = Trigger->GetComponentLocation();
	TriggerStart = CurrTriggerLoc;
	TriggerEnd = FVector(TriggerStart.X, TriggerStart.Y, TriggerStart.Z + TriggerMoveDistance);
}

void AElevator::SwapVectors(FVector& Vec1, FVector& Vec2)
{
	const FVector Temp = Vec1;
	Vec1 = Vec2;
	Vec2 = Temp;
}

