#pragma once
#include "CoreMinimal.h"

struct FFunctionTerminal
{
	FName Name;

	FName Type;
	FString DefaultValue;
	UObject* DefaultObject;
	FText DefaultTextValue;
};