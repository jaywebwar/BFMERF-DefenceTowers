// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Components/SphereComponent.h"
#include "DefenceTowersCharacter.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));

	Mesh->SetupAttachment(GetRootComponent());
	RangeSphere->SetupAttachment(GetRootComponent());
	
	
	AttackRate = 1.f;
	
	bCanFire = true;

	RangeSphere->InitSphereRadius(ShootingWindows.AttackRange);
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetupAttachment(GetRootComponent());
	RangeSphere->SetupAttachment(GetRootComponent());
	
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnOverlapBegin);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATower::OnOverlapEnd);
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
	//Attack
	DrawDebugLine(GetWorld(), ShootingWindows.DefaultWindowSlots[0], EnemiesInRange[0]->GetActorLocation(), FColor::Blue, false, .25f);

	//Stop continuous attacks
	bCanFire = false;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ATower::ReloadAttack, AttackRate);
}

void ATower::ReloadAttack()
{
	//reset for next attack
	bCanFire = true;
}

void ATower::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATower* tower = Cast<ATower>(OtherActor);
		if (tower)
		{
			EnemiesInRange.Add(OtherActor);
		}

		ADefenceTowersCharacter* character = Cast<ADefenceTowersCharacter>(OtherActor);
		if (character)
		{
			EnemiesInRange.Add(OtherActor);
		}
	}
}

void ATower::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (EnemiesInRange.Find(OtherActor) != INDEX_NONE)
		{
			EnemiesInRange.Remove(OtherActor);
		}
	}
}

