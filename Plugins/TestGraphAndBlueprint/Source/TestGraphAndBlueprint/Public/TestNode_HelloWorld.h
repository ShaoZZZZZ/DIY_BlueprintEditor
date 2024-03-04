#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "TestNode_HelloWorld.generated.h"

UCLASS(MinimalAPI)
class UTestNode_HelloWorld :public UEdGraphNode
{
	GENERATED_BODY()

public:

	UTestNode_HelloWorld();

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};