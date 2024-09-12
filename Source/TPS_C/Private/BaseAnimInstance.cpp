// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"

#include "BasePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BasePlayer = Cast<ABasePlayer>(TryGetPawnOwner());
}

void UBaseAnimInstance::UpdateAnim(float DeltaTime)
{
	if(BasePlayer == nullptr)
	{
		BasePlayer = Cast<ABasePlayer>(TryGetPawnOwner());
	}
	else
	{
		FVector PlayerVelocity = BasePlayer->GetVelocity();
		f_Speed = PlayerVelocity.Size();
		b_isInAir = BasePlayer->GetCharacterMovement()->IsFalling();
		f_Acceleration = BasePlayer->GetCharacterMovement()->GetCurrentAcceleration().Size();
		if(f_Acceleration > 0.f)
		{
			b_isMove = true;
		}
		else
		{
			b_isMove = false;
		}

		//计算角色移动方向
		FRotator Player_AimRotator = BasePlayer->GetBaseAimRotation();
		FRotator Player_MoveRotator = UKismetMathLibrary::MakeRotFromX(PlayerVelocity);
		f_Direction = UKismetMathLibrary::NormalizedDeltaRotator(Player_AimRotator,Player_MoveRotator).Yaw;
		if(f_Speed>0.f)
		{
			f_LastDirection = f_Direction;
		}

		//同步角色状态
		bIsAim = BasePlayer->GetIsAim();
	}
}
