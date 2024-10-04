// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayer.h"

#include "Actor_Items.h"
#include "BaseUserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

FOnTimelineFloat AimTimelineFloat;
FOnTimelineEvent AimTimelineFinish;
FOnTimelineFloat ShootTimelineFloat;
FOnTimelineEvent ShootTimelineFinish;
FHitResult ItemHitResult;

// Sets default values
ABasePlayer::ABasePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//绑定摄像机臂
	PlayerSpringArm = CreateDefaultSubobject<USpringArmComponent>("PlayerSpringArm");
	PlayerSpringArm->SetupAttachment(RootComponent);
	PlayerSpringArm->TargetArmLength = 300.f;
	PlayerSpringArm->bUsePawnControlRotation = true;
	PlayerSpringArm->SocketOffset = FVector(0.f,50.f,50.f);
	
	//绑定摄像机
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	PlayerCamera->SetupAttachment(PlayerSpringArm);
	PlayerCamera->bUsePawnControlRotation = false;

	//设置角色转向
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f,360.f,0.f);

	//默认值设置
	DefaultFOV = PlayerCamera->FieldOfView;
	AimFOV = 65.f;
	CurrentFOV = DefaultFOV;
	b_isAim = false;
	DefaultLookRate = 1.f;
	AimLookRate = 0.3f;
	CurrentLookRate = DefaultLookRate;
	ShootDuration = 0.2f;
	bFire = false;
	bIsShooting = false;

	//设置时间轴
	AimTimeline = CreateDefaultSubobject<UTimelineComponent>("AimTimeline");
	ShootTimeline = CreateDefaultSubobject<UTimelineComponent>("ShootTimeline");

	TouchCollision = CreateDefaultSubobject<UBoxComponent>("TouchCollision");
	TouchCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();

	//注册增强输入子系统
	APlayerController* PlayerControl = Cast<APlayerController>(Controller);
	if(PlayerControl)
	{
		UEnhancedInputLocalPlayerSubsystem* LocalSS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerControl->GetLocalPlayer());
		if(LocalSS)
		{
			LocalSS->AddMappingContext(PlayerInputMapping,0);
		}
		//设置摄像机视角限制
		PlayerControl->PlayerCameraManager->ViewPitchMax = 400.f;
		PlayerControl->PlayerCameraManager->ViewPitchMin = -40.f;
	}

	//绑定时间轴
	if(AimCurve)
	{
		AimTimelineFloat.BindUFunction(this,FName("AimUpdate"));
		AimTimeline->AddInterpFloat(AimCurve,AimTimelineFloat);
		AimTimelineFinish.BindUFunction(this,FName("AimFinish"));
		AimTimeline->SetTimelineFinishedFunc(AimTimelineFinish);
	}
	if(ShootCurve)
	{
		ShootTimelineFloat.BindUFunction(this,FName("ShootChUpdate"));
		ShootTimeline->AddInterpFloat(ShootCurve,ShootTimelineFloat);
		ShootTimelineFinish.BindUFunction(this,FName("ShootChFinish"));
		ShootTimeline->SetTimelineFinishedFunc(ShootTimelineFinish);
		
	}
	
	//HUD
	BaseHUD = CreateWidget<UBaseUserWidget>(PlayerControl,BaseUserWidget);
	if(BaseHUD)
	{
		BaseHUD->AddToViewport(0);
		BaseHUD->UpdatedHP(.9f);
	}

	bCheckItems = false;


}

// Called every frame
void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//检视可交互物品
	CheckItems();
}

// Called to bind functionality to input
void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* PlayerInputC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if(PlayerInputC)
	{
		PlayerInputC->BindAction(PlayerMove,ETriggerEvent::Triggered,this,&ABasePlayer::Move);
		PlayerInputC->BindAction(PlayerLook,ETriggerEvent::Triggered,this,&ABasePlayer::Look);
		PlayerInputC->BindAction(PlayerJump,ETriggerEvent::Started,this,&ABasePlayer::Jump);
		PlayerInputC->BindAction(PlayerJump,ETriggerEvent::Completed,this,&ABasePlayer::StopJumping);
		PlayerInputC->BindAction(PlayerShoot,ETriggerEvent::Started,this,&ABasePlayer::Fire_Start);
		PlayerInputC->BindAction(PlayerAim,ETriggerEvent::Started,this,&ABasePlayer::AimIn);
		PlayerInputC->BindAction(PlayerAim,ETriggerEvent::Completed,this,&ABasePlayer::AimOut);
		PlayerInputC->BindAction(PlayerShoot,ETriggerEvent::Completed,this,&ABasePlayer::ShootComplete);
	}
}

//瞄准相关更新
void ABasePlayer::AimUpdate(float Alpha)
{
	float updatedFOV = FMath::Lerp(CurrentFOV,AimFOV,Alpha);
	PlayerCamera->SetFieldOfView(updatedFOV);
	
	CurrentLookRate = DefaultLookRate;
	float updatedLookRate = FMath::Lerp(CurrentLookRate,AimLookRate,Alpha);
	CurrentLookRate = updatedLookRate;

	BaseHUD->UpdateAimChTranslate(Alpha,10.f);
}

void ABasePlayer::AimFinish()
{
	isAiming =false;
	BaseHUD->GetCurrentChTranslate();
}

//射击时准星更新
void ABasePlayer::ShootChUpdate(float Alpha)
{
	BaseHUD->UpdateShootChTranslate(Alpha,-5.f);
}

void ABasePlayer::ShootChFinish()
{
	
}


void ABasePlayer::Move(const FInputActionValue& Value)
{
	const FVector2d F2d = Value.Get<FVector2d>();
	if(Controller)
	{
		const FRotator Ro = Controller->GetControlRotation();
		const FRotator YawRo(0,Ro.Yaw,0);
		const FVector PlayerF = FRotationMatrix(YawRo).GetUnitAxis(EAxis::X);
		const FVector PlayerR = FRotationMatrix(YawRo).GetUnitAxis(EAxis::Y);
		AddMovementInput(PlayerF,F2d.Y);
		AddMovementInput(PlayerR,F2d.X);
	}
}

void ABasePlayer::Look(const FInputActionValue& Value)
{
	const FVector2d F2d = Value.Get<FVector2d>();
	if(Controller)
	{
		AddControllerPitchInput(F2d.Y*CurrentLookRate);
		AddControllerYawInput(F2d.X*CurrentLookRate);
	}
}

void ABasePlayer::Shoot()
{
	if(ShootSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ShootSound,GetActorLocation());
		//设置AI听力感知接收：噪音
		MakeNoise(1.f,this,GetActorLocation());
	}

	const FVector SocketLoaction = GetMesh()->GetSocketLocation("MuzzleSocket");
	const FRotator SocketRotation = GetMesh()->GetSocketRotation("MuzzleSocket");
	const FVector SocketScale(1,1,1);

	//设置射击枪口粒子
	if(P_ShootMuzzle)
	{
		UGameplayStatics::SpawnEmitterAttached(P_ShootMuzzle,GetMesh(),"MuzzleSocket",SocketLoaction,SocketRotation,SocketScale,EAttachLocation::KeepWorldPosition);
	}
	
	FVector2d Size_ViewPort;
	if(GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(Size_ViewPort);
	}
	const FVector2d CrossHair2D(Size_ViewPort.X/2.f,Size_ViewPort.Y/2.f);	//获取屏幕中心点
	bool SetScreenToWorld;
	FVector CrossHairWp;	//世界坐标
	FVector CrossHairWd;	//方向

	//获取屏幕中心点的世界坐标
	SetScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this,0),CrossHair2D,CrossHairWp,CrossHairWd);
	if(SetScreenToWorld)
	{
		FHitResult ShootHit;
		const FVector ShootStart = CrossHairWp;	//射击起点
		const FVector ShootEnd = CrossHairWp + CrossHairWd*30000.f;	//射击终点
		FVector ShootBeamEnd;	//射击光束终点
		//射线检测
		GetWorld()->LineTraceSingleByChannel(ShootHit,ShootStart,ShootEnd,ECC_Pawn);
		if(ShootHit.bBlockingHit)
		{
			//DrawDebugPoint(GetWorld(),ShootHit.Location,6.f,FColor::Green,false,3.f);
			//设置射击命中粒子
			if(P_ShootHit)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),P_ShootHit,ShootHit.Location);
			}
			ShootBeamEnd = ShootHit.Location;
		}

		//设置射击光束粒子
		if(P_ShootBeam)
		{
			const FRotator MuzzleSocketRotation = GetMesh()->GetSocketRotation("MuzzleSocket");
			UParticleSystemComponent* P_Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),P_ShootBeam,ShootStart,MuzzleSocketRotation);
			P_Beam->SetVectorParameter("Target",ShootBeamEnd);
		}
	}
}

void ABasePlayer::ShootComplete()
{
	bFire = false;
}

//瞄准状态切换
void ABasePlayer::AimIn()
{
	b_isAim = true;
	AimTimeline->Play();
	isAiming = true;
}
void ABasePlayer::AimOut()
{
	b_isAim = false;
	isAiming = true;
	AimTimeline->Reverse();
}

//开火定时器开始
void ABasePlayer::Fire_Start()
{
	//设置定时器
	if(bIsShooting)
	{
		
	}
	else
	{
		bFire = true;
		bIsShooting = true;
		Shoot();
		GetWorldTimerManager().SetTimer(ShootTimer,this,&ABasePlayer::Fire_Finish,ShootDuration);

		//如果定时器没有停止情况下射击，不重新获取当前变换; 如果停止况下重新射击则重新获取变换值
		if(isAiming)
		{
			ShootTimeline->Stop();
		}
		else
		{
			if(ShootTimeline->IsPlaying())
			{
				ShootTimeline->PlayFromStart();
			}
			else
			{
				BaseHUD->GetCurrentChTranslate();
				ShootTimeline->PlayFromStart();
			}
		}
	}
}

//开火定时器结束
void ABasePlayer::Fire_Finish()
{
	bIsShooting = false;
	ShootTimeline->Reverse();
	if(bFire)
	{
		Fire_Start();
	}
	else
	{
		GetWorldTimerManager().SetTimer(ShootFinishTimer,this,&ABasePlayer::CheckCh,0.5f);

	}

}

//射击时准星校正
void ABasePlayer::CheckCh()
{
	if(b_isAim)
	{
		AimTimeline->Play();
	}
	else
	{
		AimTimeline->Reverse();
	}
}

bool ABasePlayer::ItemTrace(FHitResult& HitResult)
{
	FVector2d Size_ViewPort;
	if(GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(Size_ViewPort);
	}
	const FVector2d CrossHair2D(Size_ViewPort.X/2.f,Size_ViewPort.Y/2.f);	//获取屏幕中心点
	bool SetScreenToWorld;
	FVector CrossHairWp;	//世界坐标
	FVector CrossHairWd;	//方向

	//获取屏幕中心点的世界坐标
	SetScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this,0),CrossHair2D,CrossHairWp,CrossHairWd);
	if(SetScreenToWorld)
	{
		const FVector LookStart = CrossHairWp;	//起点
		const FVector LookEnd = CrossHairWp + CrossHairWd*3000.f;	//终点
		//射线检测
		GetWorld()->LineTraceSingleByChannel(HitResult,LookStart,LookEnd,ECC_Visibility);
		if(HitResult.bBlockingHit)
		{
			return true;
		}
	}
	return false;
}

void ABasePlayer::UpdateCheckItemsCount(int Count)
{
	CheckItemsCount+=Count;
	if(CheckItemsCount>0)
	{
		bCheckItems = true;
	}
	else
	{
		bCheckItems = false;
	}
}

void ABasePlayer::CheckItems()
{
	if(bCheckItems)
	{
		//交互物品射线检测
		bool bLookedItem=ItemTrace(ItemHitResult);
		if(bLookedItem)
		{
			//设置物品拾取UI可见
			AActor_Items* Item = Cast<AActor_Items>(ItemHitResult.GetActor());
			if(Item)
			{
				if(Item==BeforeLookedItem)
				{
					return;
				}
				if(BeforeLookedItem)
				{
					BeforeLookedItem->SetWidgetVisibility(false);
				}
				BeforeLookedItem = Item;
				BeforeLookedItem->SetWidgetVisibility(true);
			}
			else
			{
				if(BeforeLookedItem)
				{
					BeforeLookedItem->SetWidgetVisibility(false);
					BeforeLookedItem = nullptr;
				}
			}
		}
	}
	else
	{
		if(BeforeLookedItem)
		{
			BeforeLookedItem->SetWidgetVisibility(false);
			BeforeLookedItem = nullptr;
		}
	}
}

/*void ABasePlayer::InterpFOV(float DeltaTime)
{
	if(b_isAim)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,AimFOV,DeltaTime,InterpSpeedFOV);
		CurrentLookRate = FMath::FInterpTo(CurrentLookRate,AimLookRate,DeltaTime,InterpSpeedFOV);
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,DefaultFOV,DeltaTime,InterpSpeedFOV);
		CurrentLookRate = FMath::FInterpTo(CurrentLookRate,DefaultLookRate,DeltaTime,InterpSpeedFOV);
	}
	PlayerCamera->SetFieldOfView(CurrentFOV);
}*/

