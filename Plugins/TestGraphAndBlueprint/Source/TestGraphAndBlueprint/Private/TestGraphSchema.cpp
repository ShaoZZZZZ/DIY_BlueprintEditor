#include "TestGraphSchema.h"
#include "TestNode_HelloWorld.h"
#include "MyConnectionDrawingPolicy.h"

#define LOCTEXT_NAMESPACE "TestGraphSchema"

void UTestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FTestGraphSchemaAction> NewNodeAction(new 
		FTestGraphSchemaAction(
			LOCTEXT("A", "Grap Node"),
			LOCTEXT("Desc","MyTestAA"), 
			LOCTEXT("NewGraphText", "Add a Node"),
			0));

	NewNodeAction->NodeHelloWorld = NewObject<UTestNode_HelloWorld>(ContextMenuBuilder.OwnerOfTemporaries);
	ContextMenuBuilder.AddAction(NewNodeAction);

	//NewNodeAction->K2Node_Event = NewObject<UK2Node_Event>(ContextMenuBuilder.OwnerOfTemporaries);
	//ContextMenuBuilder.AddAction(NewNodeAction);
}

void UTestGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	MenuBuilder->BeginSection(TEXT("Hall"));
	{
		MenuBuilder->AddWidget(
			SNew(SImage),
			LOCTEXT("IamgeTask", "just Iamge Task")
		);

		MenuBuilder->AddEditableText(
			LOCTEXT("OK_A", "just a Hello"),
			LOCTEXT("OK_B", "just a task Hello"),
			FSlateIcon(),
			LOCTEXT("TTT", "just a TTTTTT")
		);
	}
	MenuBuilder->EndSection();
}

//class FConnectionDrawingPolicy* UTestGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
//{
//	return new FMyConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
//}

UEdGraphNode* FTestGraphSchemaAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode *UEdResultNode = nullptr;

	//if (NodeHelloWorld != nullptr)
	//{
	//	const FScopedTransaction Transaction(LOCTEXT("FF", "Hell:NewNode"));

	//	ParentGraph->Modify();

	//	if (FromPin != nullptr)
	//	{
	//		FromPin->Modify();
	//	}

	//	K2Node_Event->Rename(nullptr, ParentGraph);
	//	ParentGraph->AddNode(K2Node_Event, true, bSelectNewNode);

	//	K2Node_Event->CreateNewGuid();
	//	K2Node_Event->PostPlacedNewNode();
	//	//	K2Node_Event->AllocateDefaultPins();
	//	//	K2Node_Event->AutowireNewNode(FromPin);

	//	K2Node_Event->NodePosX = Location.X;
	//	K2Node_Event->NodePosY = Location.Y;

	//	K2Node_Event->SetFlags(RF_Transactional);

	//	UEdResultNode = NodeHelloWorld;
	//}

	if (NodeHelloWorld != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("FF", "Hell:NewNode"));

		ParentGraph->Modify();

		if (FromPin != nullptr)
		{
			FromPin->Modify();
		}

		NodeHelloWorld->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeHelloWorld, true, bSelectNewNode);

		NodeHelloWorld->CreateNewGuid();
		NodeHelloWorld->PostPlacedNewNode();
		NodeHelloWorld->AllocateDefaultPins();
		NodeHelloWorld->AutowireNewNode(FromPin);

		NodeHelloWorld->NodePosX = Location.X;
		NodeHelloWorld->NodePosY = Location.Y;

		NodeHelloWorld->SetFlags(RF_Transactional);

		UEdResultNode = NodeHelloWorld;
	}

	return UEdResultNode;
}

#undef LOCTEXT_NAMESPACE