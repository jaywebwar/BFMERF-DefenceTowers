// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

USTRUCT(BlueprintType)
struct FWindows
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget = "true"), Category = "Shooting Windows")
	TArray<FVector> DefaultWindowSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting Windows")
	TArray<FVector> UnitWindowSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting Windows")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting Windows")
	AActor* EnemyType;
};

UCLASS()
class BFME_DEFENCETOWERS_API ATower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATower();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting Windows")
	FWindows ShootingWindows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting Windows")
	class USphereComponent* RangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooting Windows")
	TArray<AActor*> EnemiesInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting Windows")
	float AttackRate;

	FTimerHandle AttackTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooting Windows")
	bool bCanFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(BlueprintCallable)
	void ReloadAttack();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
