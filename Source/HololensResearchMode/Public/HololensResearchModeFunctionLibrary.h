// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HololensSensor.h"
#include "Engine/Texture2D.h"
#include "Templates/SharedPointer.h"

#include "HololensResearchModeFunctionLibrary.Generated.h"




UCLASS(BlueprintType, Category = "HololensResearchMode")
class HOLOLENSRESEARCHMODE_API UHololensResearchModeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	static UHololensSensor* CreateSensor(EHololensSensorType Type, UObject* Outer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	static void SetEnableEyeSelection(bool enable);

	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	static TArray<EHololensSensorType> GetAllAvailableSensors();

	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	static void GetCameraSensorTextureData(UTexture2D* CameraSensorTexture, TArray<uint8>& uncompressedBGRA);
};

