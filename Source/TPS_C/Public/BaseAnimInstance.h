// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TPS_C_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnim(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	class ABasePlayer* BasePlayer;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	float f_Speed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	bool b_isInAir;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	bool b_isMove;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	float f_Acceleration;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	float f_Direction;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	float f_LastDirection;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim",meta=(AllowPrivateAccess = "true"))
	bool bIsAim;
};
