// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Items.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AActor_Items::AActor_Items()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(RootComponent);
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(BoxCollision);
	ItemPickUpWidget = CreateDefaultSubobject<UWidgetComponent>("ItemPickUpWidget");
	ItemPickUpWidget->SetupAttachment(BoxCollision);
}

// Called when the game starts or when spawned
void AActor_Items::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_Items::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

