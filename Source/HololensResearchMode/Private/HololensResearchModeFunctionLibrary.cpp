// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HololensResearchModeFunctionLibrary.h"
#include "HololensResearchModeModule.h"
#include "HololensAccelerometer.h"
#include "HololensResearchModeContext.h"
#include "TextureResource.h"
#include "Math/Color.h"
#include "Serialization/BulkData.h"
#include "Templates/SharedPointer.h"
#include "Engine/Texture.h"

UHololensResearchModeFunctionLibrary::UHololensResearchModeFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


UFUNCTION(BlueprintCallable, Category = "HololensResearchMode")
UHololensSensor* UHololensResearchModeFunctionLibrary::CreateSensor(EHololensSensorType Type, UObject* Outer)
{
	if (!Outer)
	{
		Outer = (UObject*)GetTransientPackage();
	}
	return FHololensResearchModeModule::Get()->CreateSensor(Type, Outer);
}


void UHololensResearchModeFunctionLibrary::SetEnableEyeSelection(bool enable)
{
#if PLATFORM_HOLOLENS
	auto Context = FHololensResearchModeModule::Get()->Context;
	if (!Context)
	{
		return;
	}

	if (enable)
	{
		HRESULT hr = Context->SensorDevice->EnableEyeSelection();
		if (FAILED(hr))
		{
			FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorDevice::EnableEyeSelection"));
			return;
		}
	}
	else
	{
		HRESULT hr = Context->SensorDevice->DisableEyeSelection();
		if (FAILED(hr))
		{
			FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorDevice::DisableEyeSelection"));
			return;
		}
	}
#endif
}

TArray<EHololensSensorType> UHololensResearchModeFunctionLibrary::GetAllAvailableSensors()
{
	TArray<EHololensSensorType> Result;

#if PLATFORM_HOLOLENS
	auto Context = FHololensResearchModeModule::Get()->Context;
	if (!Context)
	{
		return Result;
	}

	auto SensorDevice = Context->SensorDevice;
	size_t SensorCount = 0;
	TArray<ResearchModeSensorDescriptor> SensorDescriptors;

	HRESULT hr = SensorDevice->GetSensorCount(&SensorCount);
	if (FAILED(hr))
	{
		FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorDevice::GetSensorCount"));
		return Result;
	}

	SensorDescriptors.SetNum(SensorCount);

	hr = SensorDevice->GetSensorDescriptors(SensorDescriptors.GetData(), SensorDescriptors.Num(), &SensorCount);
	if (FAILED(hr))
	{
		FHololensResearchModeUtility::PrintComError(hr, TEXT("IResearchModeSensorDevice::GetSensorDescriptors"));
		return Result;
	}

	for (auto Desc : SensorDescriptors)
	{
		Result.Add((EHololensSensorType)Desc.sensorType);
	}
#endif

	return Result;
}

void UHololensResearchModeFunctionLibrary::UpdateLatestWristTransforms(bool bNewLeftHandstate, bool bNewRightHandstate, FTransform& LeftWristTransform, FTransform& RightWristTransform, FVector HeadsetPosition, FQuat HeadsetOrientation, FTransform TrackingToWorldTransform)
{
#if PLATFORM_HOLOLENS
	FHololensResearchModeModule::Get()->UpdateLatestWristTransformsFromHandTracker(bNewLeftHandstate, bNewRightHandstate, LeftWristTransform, RightWristTransform, HeadsetPosition, HeadsetOrientation, TrackingToWorldTransform);
#endif
}

// Resource: https://isaratech.com/ue4-reading-the-pixels-from-a-utexture2d/
void UHololensResearchModeFunctionLibrary::GetCameraSensorTextureData(UTexture2D* CameraSensorTexture, TArray<uint8>& uncompressedBGRA)
{	
	//#if WITH_EDITORONLY_DATA
	//	TextureMipGenSettings OldMipGenSettings = CameraSensorTexture->MipGenSettings;
	//	CameraSensorTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	//#endif
		// Update Settings
		//TextureCompressionSettings OldCompressionSettings = CameraSensorTexture->CompressionSettings; 
		//bool OldSRGB = CameraSensorTexture->SRGB;
		//CameraSensorTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
		//CameraSensorTexture->SRGB = false;
		//CameraSensorTexture->UpdateResource();
		//const FColor* FormatedImageData = reinterpret_cast<const FColor*>(CameraSensorTexture->PlatformData->Mips[0].BulkData.LockReadOnly());

		FTexture2DMipMap* TileMipMap = &CameraSensorTexture->PlatformData->Mips[0];
		FByteBulkData* RawImageData = &TileMipMap->BulkData;
		//const FColor* FormatedImageData = reinterpret_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));
		uint8* FormatedImageData = (uint8*)(RawImageData->Lock(LOCK_READ_ONLY));
		uint32 TextureWidth = TileMipMap->SizeX, TextureHeight = TileMipMap->SizeY;
		//FColor* PixelColor = nullptr;
		int BitDepth = 4;
		int TextureResolution = TextureWidth * TextureHeight * BitDepth;
		//TSharedPtr<TArray<uint8>> uncompressedBGRA = MakeShared<TArray<uint8>>();
		uncompressedBGRA.SetNum(TextureResolution);
		//std::memcpy(uncompressedBGRA.GetData(), FormatedImageData, sizeof(FormatedImageData));

		//for (uint32 X = 0; X < TextureWidth; X++)
		//{
		//	for (uint32 Y = 0; Y < TextureHeight; Y++)
		//	{
		//		*PixelColor = FormatedImageData[Y * TextureWidth + X];
		//		UE_LOG(LogHLResearch, Error, TEXT("RGBA: %s"), *PixelColor->ToString());
		//}
	
	for (auto i = 0; i < TextureResolution; i++)
	{
		//UE_LOG(LogHLResearch, Error, TEXT("uint8: %d"), FormatedImageData[i]);
		uncompressedBGRA[i] = FormatedImageData[i];
	}
	RawImageData->Unlock();
	
	//UE_LOG(LogHLResearch, Error, TEXT("Recorded Sensor Data"));
	// Update Settings
	//#if WITH_EDITORONLY_DATA
	//	CameraSensorTexture->MipGenSettings = OldMipGenSettings;
	//#endif
	//
	//	CameraSensorTexture->CompressionSettings = OldCompressionSettings;
	//	CameraSensorTexture->SRGB = OldSRGB;
	//	CameraSensorTexture->UpdateResource();
	
	/* THIS WORKS */
	//FTexture2DMipMap* TileMipMap = &CameraSensorTexture->PlatformData->Mips[0];
	//FByteBulkData* RawImageData = &TileMipMap->BulkData;
	//FColor* FormatedImageData = reinterpret_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));
	//
	//
	//uint8 PixelX = 20, PixelY = 50;
	//uint32 TextureWidth = TileMipMap->SizeX, TextureHeight = TileMipMap->SizeY;
	//FColor PixelColor;
	//
	//if (PixelX >= 0 && PixelX < TextureWidth && PixelY >= 0 && PixelY < TextureHeight)
	//{
	//	PixelColor = FormatedImageData[PixelY * TextureWidth + PixelX];
	//	UE_LOG(LogHLResearch, Error, TEXT("RGBA: %s"), *PixelColor.ToString());
	//}
	//RawImageData->Unlock();
	/* THIS WORKS END */
}