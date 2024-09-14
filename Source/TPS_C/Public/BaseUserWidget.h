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
	UFUNCTION()
	void UpdatedHP(float Percent);
	UFUNCTION()
	void UpdateAimChTranslate(float Alpha,float Value);
	UFUNCTION()
	void UpdateShootChTranslate(float Alpha,float Value);
	UFUNCTION()
	void GetCurrentChTranslate() const;
private:
	UPROPERTY(Transient,meta=(BindWidget))
	class UProgressBar* HPBar;
	UPROPERTY(Transient,meta=(BindWidget))
	class UImage* CrossHair_R;
	UPROPERTY(Transient,meta=(BindWidget))
	class UImage* CrossHair_L;
	UPROPERTY(Transient,meta=(BindWidget))
	class UImage* CrossHair_U;
	UPROPERTY(Transient,meta=(BindWidget))
	class UImage* CrossHair_D;
};
