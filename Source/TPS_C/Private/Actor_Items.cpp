// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Items.h"

#include "BasePlayer.h"
#include "ItemsUserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AActor_Items::AActor_Items()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(RootComponent);
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SphereCollision->SetupAttachment(BoxCollision);
	SphereCollision->SetSphereRadius(150.f);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(BoxCollision);
	
	ItemPickUpWidget = CreateDefaultSubobject<UWidgetComponent>("ItemPickUpWidget");
	ItemPickUpWidget->SetupAttachment(BoxCollision);

	WeaponName = "DefaultName";

	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
}

// Called when the game starts or when spawned
void AActor_Items::BeginPlay()
{
	Super::BeginPlay();
	SetWidgetVisibility(false);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this,&AActor_Items::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this,&AActor_Items::OnOverlapEnd);

	ItemsWidget = Cast<UItemsUserWidget>(ItemPickUpWidget->GetWidget());

	if(ItemsWidget)
	{
		ItemsWidget->SetWeaponName(WeaponName);
	}
}

// Called every frame
void AActor_Items::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//设置Widget的可见性
void AActor_Items::SetWidgetVisibility(bool bVisible)
{
	ItemPickUpWidget->SetVisibility(bVisible);
}

void AActor_Items::CloseAllCollision()
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

//当物品与玩家重叠时
void AActor_Items::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		ABasePlayer* Player = Cast<ABasePlayer>(OtherActor);
		if(Player)
		{
			Player->UpdateCheckItemsCount(1);
		}
	}
		
}

//当物品与玩家结束重叠时
void AActor_Items::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		ABasePlayer* Player = Cast<ABasePlayer>(OtherActor);
		if(Player)
		{
			Player->UpdateCheckItemsCount(-1);
		}
	}
}

