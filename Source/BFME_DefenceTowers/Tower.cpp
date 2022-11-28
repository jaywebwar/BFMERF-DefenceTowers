// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "BFME_DefenceTowersCharacter.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));

	Mesh->SetupAttachment(GetRootComponent());
	RangeSphere->SetupAttachment(Mesh);

	GarrisonBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GarrisonBox"));
	GarrisonBox->SetupAttachment(Mesh);


	AttackRate = 1.f;

	bCanFire = true;

	RangeSphere->InitSphereRadius(ShootingWindows.AttackRange);

	TeamNumber = 1;

	
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();

	TArray<TEnumAsByte<EObjectTypeQuery>> TypeFilter;
	UClass* ClassFilter = AActor::StaticClass();
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Init(this, 1);
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(this, RangeSphere->GetComponentLocation(), ShootingWindows.AttackRange, TypeFilter, ClassFilter, IgnoreActors, OutActors);
	for (auto actor : OutActors)
	{
		if (IsValid(actor))
		{
			ATower* tower = Cast<ATower>(actor);
			if (IsValid(tower))
			{
				EnemiesInRange.Add(actor);
			}

			ABFME_DefenceTowersCharacter* character = Cast<ABFME_DefenceTowersCharacter>(actor);
			if (IsValid(character))
			{
				if (character->GetTeam() != TeamNumber)
					EnemiesInRange.Add(actor);
			}
		}
	}
	
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnEnterRange);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATower::OnExitRange);

	GarrisonBox->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnEnterGarrison);
	GarrisonBox->OnComponentEndOverlap.AddDynamic(this, &ATower::OnExitGarrison);

	for (auto unitWindow : ShootingWindows.UnitWindowSlots)
	{
		UnitWindowOccupationMap.Add(unitWindow, false);
	}
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemiesInRange.Num() > 0 && bCanFire)
		Fire();
}

void ATower::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemies in Range Num = %u"), EnemiesInRange.Num())

	//Attack

	//Acquire target prioritizing units
	AActor* target = AcquireTarget();
	
	//Select appropriate shooting window
	FVector shootingLocation = FVector(0.f);//assumes the world location of a window will never be (0, 0, 0).
	for (auto relativeLocation : ShootingWindows.DefaultWindowSlots)
	{
		auto WindowLocation = GetActorLocation() + relativeLocation;
		if (shootingLocation == FVector(0.f))
		{
			shootingLocation = WindowLocation;
		}
		else
		{
			float currentDistance = FVector::Distance(shootingLocation, target->GetActorLocation());
			float checkingDistance = FVector::Distance(WindowLocation, target->GetActorLocation());
			shootingLocation = checkingDistance < currentDistance ? WindowLocation : shootingLocation;
		}
	}
	if(ShootingWindows.DefaultWindowSlots.Num() != 0)
		DrawDebugLine(GetWorld(), shootingLocation, target->GetActorLocation(), FColor::Blue, false, .25f);

	for (auto slotLocation : ShootingWindows.UnitWindowSlots)
	{
		if (UnitWindowOccupationMap[slotLocation])
		{
			auto WindowLocation = GetActorLocation() + slotLocation;
			DrawDebugLine(GetWorld(), WindowLocation, EnemiesInRange[0]->GetActorLocation(), FColor::Red, false, .25f);
		}
	}

	//Stop continuous attacks
	bCanFire = false;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ATower::ReloadAttack, AttackRate);
}

void ATower::ReloadAttack()
{
	//reset for next attack
	bCanFire = true;
}

void ATower::OnEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		ATower* tower = Cast<ATower>(OtherActor);
		if (IsValid(tower))
		{
			EnemiesInRange.Add(OtherActor);
		}

		ABFME_DefenceTowersCharacter* character = Cast<ABFME_DefenceTowersCharacter>(OtherActor);
		if (IsValid(character))
		{
			if(character->GetTeam() != TeamNumber)
				EnemiesInRange.Add(OtherActor);
		}
	}
}

void ATower::OnExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor))
	{
		if (EnemiesInRange.Num() > 0)
		{
			if (EnemiesInRange.Find(OtherActor) != INDEX_NONE)
			{
				EnemiesInRange.Remove(OtherActor);
			}
		}
	}
}

void ATower::OnEnterGarrison(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		ABFME_DefenceTowersCharacter* character = Cast<ABFME_DefenceTowersCharacter>(OtherActor);
		if (IsValid(character))
		{
			if (character->GetTeam() == TeamNumber && ThereIsAnAvailableUnitWindow())
			{
				character->SetActorHiddenInGame(true);
				UnitWindowOccupationMap[GetAvailableUnitWindow()] = true;
			}
		}
	}
}

void ATower::OnExitGarrison(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

bool ATower::ThereIsAnAvailableUnitWindow()
{
	for (auto window : ShootingWindows.UnitWindowSlots)
	{
		if (!UnitWindowOccupationMap[window])
			return true;
	}
	return false;
}

FVector ATower::GetAvailableUnitWindow()
{
	for (auto window : ShootingWindows.UnitWindowSlots)
	{
		if (!UnitWindowOccupationMap[window])
			return window;
	}
	return FVector(0.f); //Should never reach here if ThereIsAnAvailableUnitWindow() is called before this function.
}

AActor* ATower::AcquireTarget()
{
	for (int i=0; i<EnemiesInRange.Num(); i++)
	{
		ABFME_DefenceTowersCharacter* character = Cast<ABFME_DefenceTowersCharacter>(EnemiesInRange[i]);
		if (IsValid(character))
			return EnemiesInRange[i];//first enemy unit that was found
		if (i == EnemiesInRange.Num() - 1)
			return EnemiesInRange[0];//first enemy (building/non-unit) that was found
	}
	UE_LOG(LogTemp, Warning, TEXT("Should not happen"));
	return nullptr;
}