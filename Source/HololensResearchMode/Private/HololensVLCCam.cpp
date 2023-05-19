// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HololensVLCCam.h"

#include <cstring>
#include "HololensResearchModeUtility.h"
#include "VideoTextureGenerator.h"
#include "VideoTextureResource.h"


UHololensVLCCam::UHololensVLCCam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHololensVLCCam::SensorLoop() 
{
#if PLATFORM_HOLOLENS
	if (!BeginSensorLoop())
	{
		UE_LOG(LogHLResearch, Error, TEXT("Sensor %s cannot start frame"), FHololensResearchModeUtility::GetResearchModeSensorTypeName(Type));
		return;
	}
	auto SensorVLCFrame = Sensor->QueryCurrentFrame<IResearchModeSensorVLCFrame>();
	if (!SensorVLCFrame) 
	{ 
		FScopeLock Lock(&Mutex);
		bIsInitialized = false;
		return;
	}

	uint32 TempGain;
	HRESULT hr = SensorVLCFrame->GetGain(&TempGain);
	if (FAILED(hr))
	{
		FScopeLock Lock(&Mutex);
		FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorVLCFrame::GetGain for %s"), FHololensResearchModeUtility::GetResearchModeSensorTypeName(Type));
		bIsInitialized = false;
		return;
	}

	uint64 TempExposure;
	hr = SensorVLCFrame->GetExposure(&TempExposure);
	if (FAILED(hr))
	{
		FScopeLock Lock(&Mutex);
		FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorVLCFrame::GetExposure for %s"), FHololensResearchModeUtility::GetResearchModeSensorTypeName(Type));
		bIsInitialized = false;
		return;
	}

	if (Gain != TempGain || Exposure != TempExposure)
	{
		FScopeLock Lock(&Mutex);
		Gain = TempGain;
		Exposure = TempExposure;
	}

	size_t OutBufferCount = 0;
	const uint8* Image = nullptr;

	hr = SensorVLCFrame->GetBuffer(&Image, &OutBufferCount);
	if (FAILED(hr))
	{
		FScopeLock Lock(&Mutex);
		FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorVLCFrame::GetBuffer for %s"), FHololensResearchModeUtility::GetResearchModeSensorTypeName(Type));
		bIsInitialized = false;
		return;
	}

	uint32* mappedTexture = (uint32*)VideoTextureGenerator->MapWriteCPUTexture();
	if (!mappedTexture)
	{
		FScopeLock Lock(&Mutex);
		UE_LOG(LogHLResearch, Error, TEXT("Sensor %s can't map output frame"), FHololensResearchModeUtility::GetResearchModeSensorTypeName(Type));
		bIsInitialized = false;
		return;
	}

	uint32 RowPitch = VideoTextureGenerator->GetRowPitch();

	// Save image data for getter function
	Height = SensorHeight;
	Width = SensorWidth;
	SensorSize = SensorHeight * SensorWidth;          // Initialize Sensor Size
	StoredImageArray.SetNumUninitialized(SensorSize); // Initialize Image Array

	// Loop through image vector and save to each respective field.
	for (int32 i = 0; i < SensorHeight; i++)
	{
		for (int32 j = 0; j < SensorWidth; j++)
		{
			uint32 pixel = 0;
			uint8 inputPixel = Image[SensorHeight * j + i];

			// Store pixel data
			pixel = inputPixel | (inputPixel << 8) | (inputPixel << 16);
			if (Type == EHololensSensorType::LEFT_FRONT || Type == EHololensSensorType::RIGHT_RIGHT)
			{
				mappedTexture[(RowPitch / 4) * i + (SensorWidth - j - 1)] = pixel;        // original mappedTexture. Used for visulizing stream in unreal engine
				StoredImageArray[(i * SensorWidth) + (SensorWidth - j - 1)] = inputPixel; // AugRE Add. Array used to send out over sensor_msgs/Image topic.
			}
			else if (Type == EHololensSensorType::RIGHT_FRONT || Type == EHololensSensorType::LEFT_LEFT)
			{
				mappedTexture[(RowPitch / 4) * (SensorHeight - i - 1) + j] = pixel;       // original mappedTexture. Used for visulizing stream in unreal engine
				StoredImageArray[(SensorHeight - i - 1) * SensorWidth + j] = inputPixel;  // AugRE Add. Array used to send out over sensor_msgs/Image topic.
			}
			else
			{
				check(false);
			}
		}
	}
	VideoTextureGenerator->UnmapCPUTextureAndCopyOnDevice();
#endif
}

void UHololensVLCCam::CalcTextureResolution(int32& InOutWidth, int32& InOutHeight) const
{
	int32 tmp = InOutWidth;
	InOutWidth = InOutHeight;
	InOutHeight = tmp;
}


bool UHololensVLCCam::GetIntrinsics(int32& OutGain, int64& OutExposure)
{
	FScopeLock Lock(&Mutex);
	OutGain = Gain;
	OutExposure = Exposure;
	return bIsInitialized;
}


// AugRE Add. Blueprint function.
bool UHololensVLCCam::GetVlcCamData(TArray<uint8>& OutData, int32& OutHeight, int32& OutWidth)
{
#if PLATFORM_HOLOLENS
	OutHeight = Height;
	OutWidth = Width;
	OutData = StoredImageArray;
	return true;
#endif

	UE_LOG(LogHLResearch, Error, TEXT("[Target not PLATFORM_HOLOLENS] Cannot Get VLC Cam Data"));
	return false;
}

