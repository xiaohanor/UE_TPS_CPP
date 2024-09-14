// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUserWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

float Default_ch_l_x;
float Default_ch_r_x;
float Default_ch_u_y;
float Default_ch_d_y;
float CrossHair_L_x;
float CrossHair_R_x;
float CrossHair_U_y;
float CrossHair_D_y;
float Current_ch_l_x;
float Current_ch_r_x;
float Current_ch_u_y;
float Current_ch_d_y;

void UBaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//设置准星默认值
	Default_ch_l_x=CrossHair_L->GetRenderTransform().Translation.X;
	Default_ch_r_x=CrossHair_R->GetRenderTransform().Translation.X;
	Default_ch_u_y=CrossHair_U->GetRenderTransform().Translation.Y;
	Default_ch_d_y=CrossHair_D->GetRenderTransform().Translation.Y;
	
	/*CrossHair_L->SetRenderTranslation(FVector2D(-20.f,0.f));
	CrossHair_R->SetRenderTranslation(FVector2D(20.f,0.f));
	CrossHair_U->SetRenderTranslation(FVector2D(0.f,-20.f));
	CrossHair_D->SetRenderTranslation(FVector2D(0.f,20.f));*/
	
}

void UBaseUserWidget::UpdatedHP(float Percent)
{
	HPBar->SetPercent(Percent);
}

/*瞄准时准星变换*/
void UBaseUserWidget::UpdateAimChTranslate(float Alpha, float Value)
{
	CrossHair_L_x=FMath::Lerp(Default_ch_l_x,Default_ch_l_x+Value,Alpha);
	CrossHair_R_x=FMath::Lerp(Default_ch_r_x,Default_ch_r_x-Value,Alpha);
	CrossHair_U_y=FMath::Lerp(Default_ch_u_y,Default_ch_u_y+Value,Alpha);
	CrossHair_D_y=FMath::Lerp(Default_ch_d_y,Default_ch_d_y-Value,Alpha);
	CrossHair_L->SetRenderTranslation(FVector2d(CrossHair_L_x,0.f));
	CrossHair_R->SetRenderTranslation(FVector2d(CrossHair_R_x,0.f));
	CrossHair_U->SetRenderTranslation(FVector2d(0.f,CrossHair_U_y));
	CrossHair_D->SetRenderTranslation(FVector2d(0.f,CrossHair_D_y));
}

/*射击时准星变换*/
void UBaseUserWidget::UpdateShootChTranslate(float Alpha, float Value)
{
	CrossHair_L_x=FMath::Lerp(Current_ch_l_x,Current_ch_l_x+Value,Alpha);
	CrossHair_R_x=FMath::Lerp(Current_ch_r_x,Current_ch_r_x-Value,Alpha);
	CrossHair_U_y=FMath::Lerp(Current_ch_u_y,Current_ch_u_y+Value,Alpha);
	CrossHair_D_y=FMath::Lerp(Current_ch_d_y,Current_ch_d_y-Value,Alpha);
	CrossHair_L->SetRenderTranslation(FVector2d(CrossHair_L_x,0.f));
	CrossHair_R->SetRenderTranslation(FVector2d(CrossHair_R_x,0.f));
	CrossHair_U->SetRenderTranslation(FVector2d(0.f,CrossHair_U_y));
	CrossHair_D->SetRenderTranslation(FVector2d(0.f,CrossHair_D_y));
}

/*获取当前准星位置*/
void UBaseUserWidget::GetCurrentChTranslate() const
{
	Current_ch_l_x=CrossHair_L->GetRenderTransform().Translation.X;
	Current_ch_r_x=CrossHair_R->GetRenderTransform().Translation.X;
	Current_ch_u_y=CrossHair_U->GetRenderTransform().Translation.Y;
	Current_ch_d_y=CrossHair_D->GetRenderTransform().Translation.Y;
}
