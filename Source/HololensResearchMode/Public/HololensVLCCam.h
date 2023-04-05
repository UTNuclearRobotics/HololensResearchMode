// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "HololensCameraSensor.h"
#include "HololensVLCCam.generated.h"

class FVideoTextureResource;
class FVideoTextureGenerator;

UCLASS(BlueprintType, Category = "HololensResearchMode")
class HOLOLENSRESEARCHMODE_API UHololensVLCCam : public UHololensCameraSensor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	bool GetIntrinsics(int32& OutGain, int64& OutExposure);

	UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
	bool GetVlcCamData(TArray<uint8>& OutData, int32& OutHeight, int32& OutWidth);

protected:
	virtual void SensorLoop() override;
	virtual void CalcTextureResolution(int32& InOutWidth, int32& InOutHeight) const override;

	uint8* StoredImagePtr{ nullptr }; // pointer to the start of the image array
	TArray<uint8> StoredImageArray;
	uint32 SensorSize{ 0 }; // size of sensor being read
	uint32 Height{ 0 };
	uint32 Width{ 0 };
	uint32 Gain = 0;
	uint64 Exposure = 0;
};

