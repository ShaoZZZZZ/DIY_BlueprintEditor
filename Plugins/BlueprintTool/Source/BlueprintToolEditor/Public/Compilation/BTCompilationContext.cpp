#include "BTCompilationContext.h"
#include "BlueprintEditor/Core/Architect/K3Node.h"
#include "Type/BTEType.h"
#include "BTCompilerVMBackend.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

void GetCurrentParmTerminal(FFunctionTerminal &FunctionTerminal, UEdGraphPin *Pin)
{

	if (Pin->PinType.PinCategory != FPC_Public::PC_Exec)
	{
		if (Pin->PinType.PinCategory == FPC_Public::PC_String)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_String;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Boolean)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Boolean;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Byte)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Byte;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Int)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Int;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Float)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Float;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Float)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Float;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Object)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Object;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Text)
		{
			FunctionTerminal.DefaultTextValue = Pin->DefaultTextValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Text;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Name)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Name;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_SoftObject)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_SoftObject;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Class)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Class;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_Struct)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_Struct;
		}
		else if (Pin->PinType.PinCategory == FPC_Public::PC_SoftClass)
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_SoftClass;
		}
		else
		{
			FunctionTerminal.DefaultValue = Pin->DefaultValue;
			FunctionTerminal.Name = Pin->PinName;
			FunctionTerminal.Type = FPC_Public::PC_DefaultBT;
		}
	}
}



void FBTCompilerContext::CompileFunction(FBTFunctionContext& Context)
{
	UEdGraphNode *EndNode = nullptr;
	for (int32 i = 0; i < Context.LinearExecutionList.Num(); i++)
	{
		UK3Node *K3 = Cast<UK3Node>(Context.LinearExecutionList[i]);
		if (K3->Function)
		{
			//±‡“Î”Ôæ‰
			TArray<EVMStatementType> ListStatement;
			if (i == 0)
			{
				ListStatement.Add(EVMStatementType::VMS_CallFunc);
			}

			ListStatement.Add(EVMStatementType::VMS_Nest);

			Context.ListVMStatement.Add(K3, ListStatement);
			EndNode = K3;

			// ’ºØ∂À◊”
			TArray<FFunctionTerminal> TerminalArray;
			for (UEdGraphPin *Pin : K3->Pins)
			{
				FFunctionTerminal FunctionTerminal;
				GetCurrentParmTerminal(FunctionTerminal, Pin);

				TerminalArray.Add(FunctionTerminal);
			}

			if (TerminalArray.Num() > 0)
			{
				Context.TerminalMap.Add(K3->Function, TerminalArray);
			}
		}
	}
	Context.ListVMStatement[EndNode].Add(EVMStatementType::VMS_Return);

	FVM VM(&Context);
	VM.GenerateBytecode();
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif