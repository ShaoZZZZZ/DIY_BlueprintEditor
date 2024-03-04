#include "BTCompilerVMBackend.h"
#include "K3Node.h"
#include "BlueprintToolScript/Public/BTScript.h"
#include "Misc/DefaultValueHelper.h"
#include "PropertyPortFlags.h"
#include "Internationalization/TextNamespaceUtil.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

namespace Scripttecode
{
	class FScripttecodeWriter : public FArchiveUObject
	{
	public:
		TArray<uint8>& ScriptBuffer;
	public:
		FScripttecodeWriter(TArray<uint8>& InScriptBuffer)
			: ScriptBuffer(InScriptBuffer)
		{
		}

		virtual void Serialize(void* V, int64 Length) override
		{
			int32 iStart = ScriptBuffer.AddUninitialized(Length);
			FMemory::Memcpy(&(ScriptBuffer[iStart]), V, Length);
		}

		using FArchiveUObject::operator<<;

		FArchive& operator<<(FName& Name) override
		{
			FArchive& Ar = *this;

			FScriptName ScriptName = NameToScriptName(Name);
			Ar << ScriptName.ComparisonIndex;
			Ar << ScriptName.DisplayIndex;
			Ar << ScriptName.Number;

			return Ar;
		}

		FArchive& operator<<(UObject*& Res) override
		{
			ScriptPointerType D = (ScriptPointerType)Res;
			FArchive& Ar = *this;

			Ar << D;
			return Ar;
		}

		FArchive& operator<<(TCHAR* S)
		{
			Serialize(S, FCString::Strlen(S) + 1);
			return *this;
		}

		FArchive& operator<<(EVMCommand E)
		{
			checkSlow(E < 0xFF);

			uint8 B = E;
			Serialize(&B, 1);
			return *this;
		}

		FArchive& operator<<(EBlueprintTextLiteralType E)
		{
			static_assert(sizeof(__underlying_type(EBlueprintTextLiteralType)) == sizeof(uint8), "EBlueprintTextLiteralType is expected to be a uint8");

			uint8 B = (uint8)E;
			Serialize(&B, 1);
			return *this;
		}

		FArchive& operator<<(FLazyObjectPtr& LazyObjectPtr) override
		{
			return FArchive::operator<<(LazyObjectPtr);
		}
	};
}

struct FScriptBuilder
{
public:
	Scripttecode::FScripttecodeWriter Writer;
public:
	FScriptBuilder(TArray<uint8>& InScriptBuffer)
		: Writer(Scripttecode::FScripttecodeWriter(InScriptBuffer))
	{
		VectorStruct = TBaseStructure<FVector>::Get();
		RotatorStruct = TBaseStructure<FRotator>::Get();
		TransformStruct = TBaseStructure<FTransform>::Get();
	}
private:
	UScriptStruct* VectorStruct;
	UScriptStruct* RotatorStruct;
	UScriptStruct* TransformStruct;
public:

	void EmitStringLiteral(const FString& String)
	{
		if (FCString::IsPureAnsi(*String))
		{
			Writer << EVMCommand::VMC_ConstString;
			uint8 OutCh;
			for (const TCHAR* Ch = *String; *Ch; ++Ch)
			{
				OutCh = CharCast<ANSICHAR>(*Ch);
				Writer << OutCh;
			}

			OutCh = 0;
			Writer << OutCh;
		}
		else
		{

		}
	}

	virtual void EmitTermExpr(UProperty* CoerceProperty, const FFunctionTerminal &ValueAddr)
	{
		if (CoerceProperty->IsA(UStrProperty::StaticClass()))
		{
			EmitStringLiteral(ValueAddr.DefaultValue);
		}
		else if (CoerceProperty->IsA(UIntProperty::StaticClass()))
		{
			int32 Value = FCString::Atoi(*ValueAddr.DefaultValue);

			Writer << VMC_ConstInt32;
			Writer << Value;
		}
		else if (CoerceProperty->IsA(UBoolProperty::StaticClass()))
		{
			bool Value = ValueAddr.DefaultValue.ToBool();

			Writer << (Value ? VMC_True : VMC_False);
		}
		else if (CoerceProperty->IsA(UFloatProperty::StaticClass()))
		{
			float Value = FCString::Atof(*ValueAddr.DefaultValue);

			Writer << VMC_FloatConst;
			Writer << Value;
		}
		else if (CoerceProperty->IsA(UNameProperty::StaticClass()))
		{
			FName LiteralName(*ValueAddr.DefaultValue);

			Writer << VMC_NameConst;
			Writer << LiteralName;
		}
		else if (CoerceProperty->IsA(UTextProperty::StaticClass()))
		{
			Writer << VMC_TextConst;
			const FString& StringValue = FTextInspector::GetDisplayString(ValueAddr.DefaultTextValue);
			
			if (ValueAddr.DefaultTextValue.IsEmpty())
			{
				Writer << EBlueprintTextLiteralType::Empty;
			}
			else if (ValueAddr.DefaultTextValue.IsCultureInvariant())
			{
				Writer << EBlueprintTextLiteralType::InvariantText;
				EmitStringLiteral(StringValue);
			}
			else
			{
				bool bIsLocalized = false;
				FString Namespace;
				FString Key;
				const FString* SourceString = FTextInspector::GetSourceString(ValueAddr.DefaultTextValue);

				if (SourceString && ValueAddr.DefaultTextValue.ShouldGatherForLocalization())
				{
					bIsLocalized = FTextLocalizationManager::Get().FindNamespaceAndKeyFromDisplayString(FTextInspector::GetSharedDisplayString(ValueAddr.DefaultTextValue), Namespace, Key);
				}

				if (bIsLocalized)
				{
					// BP bytecode always removes the package localization ID to match how text works at runtime
					// If we're gathering editor-only text then we'll pick up the version with the package localization ID from the property/pin rather than the bytecode
					Namespace = TextNamespaceUtil::StripPackageNamespace(Namespace);

					Writer << EBlueprintTextLiteralType::LocalizedText;
					EmitStringLiteral(*SourceString);
					EmitStringLiteral(Key);
					EmitStringLiteral(Namespace);
				}
				else
				{
					Writer << EBlueprintTextLiteralType::LiteralString;
					EmitStringLiteral(StringValue);
				}
			}
		}
		else if (CoerceProperty->IsA(UObjectProperty::StaticClass()) || CoerceProperty->IsA(UClassProperty::StaticClass()))
		{
			Writer << VMC_NoObject;
			/*if (ValueAddr.DefaultValue.IsEmpty())
			{
				Writer << VMC_NoObject;
			}
			else
			{				
				UObject *ObjInstance = NewObject<UObject>(*ValueAddr.DefaultValue);
				Writer << VMC_ObjectConst;
				Writer << ObjInstance;
			}*/
		}
		else if (CoerceProperty->IsA(UStructProperty::StaticClass()))
		{
			UStructProperty* StructProperty = Cast<UStructProperty>(CoerceProperty);
			UScriptStruct* Struct = StructProperty->Struct;
			if (Struct == VectorStruct)
			{
				FVector V = FVector::ZeroVector;
				if (!ValueAddr.DefaultValue.IsEmpty())
				{
					const bool bParsedUsingCustomFormat = FDefaultValueHelper::ParseVector(ValueAddr.DefaultValue, /*out*/ V);
					if (!bParsedUsingCustomFormat)
					{
						Struct->ImportText(*ValueAddr.DefaultValue, &V, nullptr, EPropertyPortFlags::PPF_None, nullptr, GetPathNameSafe(StructProperty));
					}
				}
				Writer << VMC_VectorConst;
				Writer << V;
			}
			else if (Struct == RotatorStruct)
			{
				FRotator R = FRotator::ZeroRotator;
				if (!ValueAddr.DefaultValue.IsEmpty())
				{
					const bool bParsedUsingCustomFormat = FDefaultValueHelper::ParseRotator(ValueAddr.DefaultValue, /*out*/ R);
					if (!bParsedUsingCustomFormat)
					{
						Struct->ImportText(*ValueAddr.DefaultValue, &R, nullptr, PPF_None, nullptr, GetPathNameSafe(StructProperty));
					}
				}
				Writer << VMC_RotationConst;
				Writer << R;
			}
			else if (Struct == TransformStruct)
			{
				FTransform T = FTransform::Identity;
				if (!ValueAddr.DefaultValue.IsEmpty())
				{
					const bool bParsedUsingCustomFormat = T.InitFromString(ValueAddr.DefaultValue);
					if (!bParsedUsingCustomFormat)
					{
						Struct->ImportText(*ValueAddr.DefaultValue, &T, nullptr, PPF_None, nullptr, GetPathNameSafe(StructProperty));
					}
				}
				Writer << VMC_TransformConst;
				Writer << T;
			}
			else
			{
				//有可能是结构体...
				//
			}
		}
	}

};

void FVM::GenerateBytecode()
{
	if (FunctionList)
	{
		for (auto &Tmp : FunctionList->ListVMStatement)
		{
			UK3Node *Node = Cast<UK3Node>(Tmp.Key);
			if (Node->Function)
			{
				FScriptBuilder Writer(Node->Function->Script);
				for (EVMStatementType &Statement : Tmp.Value)
				{
					switch (Statement)
					{
					case VMS_CallFunc:
					{
						Writer.Writer << EVMCommand::VMC_Funtion;
						Writer.Writer << Node->Function;

						break;
					}
					case VMS_Nest:
					{
						//序列化我们的参数
						for (TFieldIterator<UProperty> i(Node->Function); i; ++i)
						{
							UProperty *UProp = *i;
							FFunctionTerminal NewTerminal = FunctionList->FindTerm(Node->Function, UProp);
						
							Writer.EmitTermExpr(UProp, NewTerminal);
						}

						//找到当前的node
						UEdGraphNode *CurrentNode = Node;
						//for (UEdGraphNode *NodeTmp : FunctionList->LinearExecutionList)
						//{
						//	UK3Node *NewNode = Cast<UK3Node>(NodeTmp);
						//	if (NewNode->Function == Node->Function && NewNode != Node)
						//	{
						//		CurrentNode = Node;
						//		break;
						//	}
						//}
						//寻找当前node对应的序列
						int32 index = FunctionList->LinearExecutionList.Find(CurrentNode);
						index++;

						if (FunctionList->LinearExecutionList.IsValidIndex(index))
						{
							UEdGraphNode *NestNode = FunctionList->LinearExecutionList[index];
							UK3Node *NestNode3 = Cast<UK3Node>(NestNode);

							//写入字节码
							Writer.Writer << EVMCommand::VMC_NestFunction;
							Writer.Writer << NestNode3->Function;
						}

						break;
					}
					case VMS_Return:
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif