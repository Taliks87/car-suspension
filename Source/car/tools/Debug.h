#pragma once
//#ifndef TOOLS_DEBUG_H
//#define TOOLS_DEBUG_H
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
//#include "Engine.h"

namespace tools
{
	inline void DebugPoint(const UWorld* world, const FVector& pos, const FColor& color, const FString& text, bool isOnScreen = true)
	{
		DrawDebugPoint(world, pos, 15.0f, color, false, 0.001f);
		DrawDebugString(world, pos + FVector{ 0.0f, 0.0f, 30.0f }, text, nullptr, color, 0.001f);
	}

	inline void DebugPointOnScreen(const UWorld* world, const FVector& pos, const FColor& color, const FString& text)
	{
		DrawDebugPoint(world, pos, 15.0f, color, false, 0.001f);
		DrawDebugString(world, pos + FVector{ 0.0f, 0.0f, 30.0f }, text, nullptr, color, 0.001f);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, color, text + " " + pos.ToString());
		}
	}
}
//#endif // TOOLS_DEBUG_H

