#include "Factory/Blueprint/BlueprintToolFactory.h"
#include "BoardGrapNode.h"
#include "SBoardGrapPin.h"
#include "SBoardGrapNode.h"
#include "ConnectionDrawingPolicy1.h"
#include "PBToolSchema.h"
#include "NodeFactory.h"

TSharedPtr<class SGraphNode> FBToolPanelNodeFactory::CreateNode(class UEdGraphNode* Node) const
{
	if (UBoardGrapNode* MarkerNode = Cast<UBoardGrapNode>(Node))
	{
		return SNew(SK3Node, MarkerNode)
			.NodeName(MarkerNode->GetNodeTitle());
	}

	return NULL;
}

TSharedPtr<class SGraphPin> FBToolPanelPinFactory::CreatePin(class UEdGraphPin* Pin) const
{
	TSharedPtr<class SGraphPin> NewPin;
	if (UBoardGrapNode* MarkerNode = Cast<UBoardGrapNode>(Pin->GetOuter()))
	{
		if (const UPBToolSchema* K2Schema = Cast<const UPBToolSchema>(Pin->GetSchema()))
		{
			NewPin = FNodeFactory::CreateK2PinWidget(Pin);
		}

		if (!NewPin.IsValid())
		{
			NewPin = SNew(SBPToolGraphPin, Pin)
				.PinName(FText::FromName(Pin->PinName));
		}
	}

	return NewPin;
}

class FConnectionDrawingPolicy* FBToolPanelPinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	if (Schema->IsA(UPBToolSchema::StaticClass()))
	{
		return new FBPToolConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}

	return nullptr;
}
