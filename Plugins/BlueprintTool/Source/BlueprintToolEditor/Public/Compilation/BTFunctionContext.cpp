#include "BTFunctionContext.h"

FFunctionTerminal FBTFunctionContext::FindTerm(UFunction* FindFunc, UProperty *Pin)
{
	TArray<FFunctionTerminal> FunctionTerminal;
	for (auto &Tmp : TerminalMap)
	{
		if (Tmp.Key == FindFunc)
		{
			FunctionTerminal = Tmp.Value;
		}
	}

	for (auto &Tmp : FunctionTerminal)
	{
		if (Tmp.Name == Pin->GetFName())
		{
			return Tmp;
		}
	}

	return FFunctionTerminal();
}

