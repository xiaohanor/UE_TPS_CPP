// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUserWidget.h"

#include "Components/ProgressBar.h"

void UBaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UBaseUserWidget::UpdatedHP(float Percent)
{
	HPBar->SetPercent(Percent);
}
