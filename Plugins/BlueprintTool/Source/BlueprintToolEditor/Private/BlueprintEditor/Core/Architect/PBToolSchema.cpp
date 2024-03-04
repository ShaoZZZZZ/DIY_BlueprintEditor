#include "BlueprintEditor/Core/Architect/PBToolSchema.h"
#include "BlueprintEditor/Core/Architect/PBToolSchemaAction.h"
#include "BlueprintEditor/GraphNode/BoardGrapNode.h"
#include "Utils/BlueSchemaUtils.h"
#include "SimpleCode.h"

#define LOCTEXT_NAMESPACE "PBToolSchema"

UPBToolSchema::UPBToolSchema(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UPBToolSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TArray<UClass*> CodeClassArray;
	GetDerivedClasses(USimpleCode::StaticClass(), CodeClassArray, false);

	for (UClass * TmpCode : CodeClassArray)
	{
		for (TFieldIterator<UFunction> i(TmpCode); i; ++i)
		{
			UFunction* Func = *i;
			if (Func)
			{
				FBPTSchemaUtils::CreateAction<UBoardGrapNode>(Func->GetName(), Func->GetMetaData("Group"), Func->GetToolTipText(), ContextMenuBuilder, Func);
			}
		}

		for (TFieldIterator<UProperty> i(TmpCode); i; ++i)
		{
			UProperty* Prop = *i;
			if (Prop)
			{
				FBPTSchemaUtils::CreateAction<UBoardGrapNode>(Prop->GetName(), Prop->GetMetaData("Group"), Prop->GetToolTipText(), ContextMenuBuilder, Prop);
			}
		}
	}
}

void UPBToolSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
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


bool UPBToolSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	bool ConnectionNade = true;
	if (ConnectionNade)
	{
		for (const auto *TmpB : A->GetOwningNodeUnchecked()->GetAllPins())
		{
			if (TmpB == B)
			{
				ConnectionNade = false;
				break;
			}
		}	
	}

	if (ConnectionNade)
	{
		UEdGraphSchema::TryCreateConnection(A, B);
	}

	return ConnectionNade;
}

void UPBToolSchema::GetActionList(UEdGraph* OwnerBPGraph, TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions) const
{
	TArray<UClass*> CodeClassArray;
	GetDerivedClasses(USimpleCode::StaticClass(), CodeClassArray, false);

	for (UClass * TmpCode : CodeClassArray)
	{
		for (TFieldIterator<UFunction> i(TmpCode); i; ++i)
		{
			UFunction* Func = *i;
			if (Func)
			{
				FBPTSchemaUtils::CreateAction<UBoardGrapNode>(Func->GetName(), Func->GetMetaData("Group"), Func->GetToolTipText(), OwnerBPGraph, OutActions, Func);
			}
		}

		for (TFieldIterator<UProperty> i(TmpCode); i; ++i)
		{
			UProperty* Prop = *i;
			if (Prop)
			{
				FBPTSchemaUtils::CreateAction<UBoardGrapNode>(Prop->GetName(), Prop->GetMetaData("Group"), Prop->GetToolTipText(), OwnerBPGraph, OutActions, Prop);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE