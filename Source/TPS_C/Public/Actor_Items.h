// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_Items.generated.h"

UCLASS()
class TPS_C_API AActor_Items : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_Items();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetWidgetVisibility(bool bVisible);

	UFUNCTION()
	void CloseAllCollision();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Component",meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Component",meta=(AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Component",meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* ItemPickUpWidget;

	UPROPERTY()
	class UItemsUserWidget* ItemsWidget;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Attribute",meta=(AllowPrivateAccess = "true"))
	FString WeaponName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Component",meta=(AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;

};
