// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "HololensCameraSensor.h"

#include "HololensDepthCam.generated.h"


UCLASS(BlueprintType, Category = "HololensResearchMode")
class HOLOLENSRESEARCHMODE_API UHololensDepthCam : public UHololensCameraSensor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	bool GetDepthCamShortThrowData(TArray<uint8>& OutData, int32& OutHeight, int32& OutWidth);

	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	bool GetDepthCamLongThrowData(TArray<uint8>& OutData, int32& OutHeight, int32& OutWidth);

protected:
	virtual void SensorLoop() override;
	virtual void CalcTextureResolution(int32& InOutWidth, int32& InOutHeight) const override;

	uint8* StoredImagePtr{ nullptr }; // pointer to the start of the image array
	TArray<uint8> StoredDepthImageShortArray;
	TArray<uint8> StoredDepthImageLongArray;
	uint32 SensorSize{ 0 }; // size of sensor being read
	uint32 Height{ 0 };
	uint32 Width{ 0 };

};

