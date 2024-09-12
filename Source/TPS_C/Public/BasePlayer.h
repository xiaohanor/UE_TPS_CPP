// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "BasePlayer.generated.h"

struct FInputActionValue;

UCLASS()
class TPS_C_API ABasePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasePlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PlayerCamera",meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* PlayerSpringArm;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PlayerCamera",meta=(AllowPrivateAccess="true"))
	class UCameraComponent* PlayerCamera;

	//玩家输入映射
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	class UInputMappingContext* PlayerInputMapping;

	//玩家输入Action
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	class UInputAction* PlayerMove;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	class UInputAction* PlayerLook;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	class UInputAction* PlayerJump;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	class UInputAction* PlayerShoot;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	class UInputAction* PlayerAim;

	//射击相关属性
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	class USoundCue* ShootSound;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	class UParticleSystem* P_ShootMuzzle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	class UParticleSystem* P_ShootHit;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	class UParticleSystem* P_ShootBeam;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	class UAnimMontage* AM_Shoot;

	//FOV
	float DefaultFOV;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	float AimFOV;
	float CurrentFOV;
	bool b_isAim;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"),meta=(ClampMin="0.0",ClampMax="1.0",UIMin="0.0",UIMax="1.0"))
	float AimLookRate;
	float DefaultLookRate;
	float CurrentLookRate;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shoot",meta=(AllowPrivateAccess="true"))
	class UCurveFloat* CurveAim;
	class UTimelineComponent* AimTimeline;

	UFUNCTION()
	void AimUpdate(float Alpha);

	

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);
	void AimIn();
	void AimOut();
	//void InterpFOV(float DeltaTime);

	//UI
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="HUD",meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UUserWidget> BaseUserWidget;
	class UBaseUserWidget* BaseHUD;

public:
	FORCEINLINE bool GetIsAim() const {return b_isAim;}
};