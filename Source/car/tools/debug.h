#pragma once
//#ifndef TOOLS_DEBUG_H
//#define TOOLS_DEBUG_H
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"	
//#include "Engine.h"

namespace tools
{
	inline void DubugPoint(const UWorld* world, const FVector& pos, const FColor& color, const FString& text, bool isOnScreen = true)
	{
		DrawDebugPoint(world, pos, 15.0f, color, false, 0.01f);
		DrawDebugString(world, pos + FVector{ 0.0f, 0.0f, 30.0f }, text, nullptr, color, 0.01f);
	}
	inline void DubugPointOnScreen(const UWorld* world, const FVector& pos, const FColor& color, const FString& text)
	{
		DrawDebugPoint(world, pos, 15.0f, color, false, 0.01f);
		DrawDebugString(world, pos + FVector{ 0.0f, 0.0f, 30.0f }, text, nullptr, color, 0.01f);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, color, text + " " + pos.ToString());
	}
}
//#endif // TOOLS_DEBUG_H

