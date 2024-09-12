// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TPS_C_API UBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void virtual NativeConstruct() override;
	void UpdatedHP(float Percent);
private:
	UPROPERTY(Transient,meta=(BindWidget))
	class UProgressBar* HPBar;
	
};
