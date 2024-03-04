#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "PBToolSchema.generated.h"


UCLASS()
class UPBToolSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const;

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT("Not implemented by this schema"));
	}

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	void GetActionList( UEdGraph* OwnerBPGraph,TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions) const;
};