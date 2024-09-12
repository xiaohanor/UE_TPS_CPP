// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayer.h"

#include "BaseUserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

FOnTimelineFloat AimTimelineFloat;

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

	//设置时间轴
	AimTimeline = CreateDefaultSubobject<UTimelineComponent>("AimTimeline");
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
		PlayerControl->PlayerCameraManager->ViewPitchMax = 10.f;
		PlayerControl->PlayerCameraManager->ViewPitchMin = -10.f;
	}

	//绑定时间轴
	if(CurveAim)
	{
		AimTimelineFloat.BindUFunction(this,FName("AimUpdate"));
		AimTimeline->AddInterpFloat(CurveAim,AimTimelineFloat);
	}
	
	//HUD
	BaseHUD = CreateWidget<UBaseUserWidget>(PlayerControl,BaseUserWidget);
	if(BaseHUD)
	{
		BaseHUD->AddToViewport(0);
		BaseHUD->UpdatedHP(.9f);
	}
}

// Called every frame
void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		PlayerInputC->BindAction(PlayerShoot,ETriggerEvent::Started,this,&ABasePlayer::Shoot);
		PlayerInputC->BindAction(PlayerAim,ETriggerEvent::Started,this,&ABasePlayer::AimIn);
		PlayerInputC->BindAction(PlayerAim,ETriggerEvent::Completed,this,&ABasePlayer::AimOut);


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

void ABasePlayer::Shoot(const FInputActionValue& Value)
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

	if(P_ShootMuzzle)
	{
		UGameplayStatics::SpawnEmitterAttached(P_ShootMuzzle,GetMesh(),"MuzzleSocket",SocketLoaction,SocketRotation,SocketScale,EAttachLocation::KeepWorldPosition);
	}
	
	FVector2d Size_ViewPort;
	if(GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(Size_ViewPort);
	}
	const FVector2d CrossHair2D(Size_ViewPort.X/2.f,Size_ViewPort.Y/2.f);
	bool SetScreenToWorld;
	FVector CrossHairWp;
	FVector CrossHairWd;
	SetScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this,0),CrossHair2D,CrossHairWp,CrossHairWd);
	if(SetScreenToWorld)
	{
		FHitResult ShootHit;
		const FVector ShootStart = CrossHairWp;
		const FVector ShootEnd = CrossHairWp + CrossHairWd*30000.f;
		FVector ShootBeamEnd;
		//射线检测
		GetWorld()->LineTraceSingleByChannel(ShootHit,ShootStart,ShootEnd,ECC_Visibility);
		if(ShootHit.bBlockingHit)
		{
			//DrawDebugPoint(GetWorld(),ShootHit.Location,6.f,FColor::Green,false,3.f);
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

void ABasePlayer::AimIn()
{
	b_isAim = true;
	AimTimeline->Play();
}

void ABasePlayer::AimOut()
{
	b_isAim = false;
	AimTimeline->Reverse();
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

