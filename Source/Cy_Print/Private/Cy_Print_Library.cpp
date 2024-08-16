// Copyright CyberAlien ,All Rights Reserved , 2024


#include "Cy_Print_Library.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"


void UCy_Print_Library::Cy_PrintString(const UObject* WorldContextObject, const FString& InString, bool Enabled ,bool bPrintToScreen , bool bPrintToLog , FLinearColor TextColor , float Duration , const FName Key ,  const FString& InPrefix , const Cy_Print_Type LogTypeInternal )
{
	if ( Enabled )
	{
		#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) || USE_LOGGING_IN_SHIPPING // Do not Print in Shipping or Test unless explictly enabled.

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	FString Prefix;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch(World->GetNetMode())
			{
				case NM_Client:
					// GPlayInEditorID 0 is always the server, so 1 will be first client.
					// You want to keep this logic in sync with GeneratePIEViewportWindowTitle and UpdatePlayInEditorWorldDebugString
					Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
					break;
				case NM_DedicatedServer:
				case NM_ListenServer:
					Prefix = FString::Printf(TEXT("Server: "));
					break;
				case NM_Standalone:
					break;
			}
		}
	}
/*
#if DO_BLUEPRINT_GUARD
	if (UE::Blueprint::Private::bBlamePrintString && !FBlueprintContextTracker::Get().GetCurrentScriptStack().IsEmpty())
	{
		const TArrayView<const FFrame* const> ScriptStack = FBlueprintContextTracker::Get().GetCurrentScriptStack();
		Prefix = FString::Printf(TEXT("Blueprint Object: %s\nBlueprint Function: %s\n%s"), 
			*ScriptStack.Last()->Node->GetPackage()->GetPathName(),
			*ScriptStack.Last()->Node->GetName(),
			*Prefix);
	}

#endif
*/

	const FString FinalDisplayString = Prefix + InString;
	FString FinalLogString = FinalDisplayString;

	static const FBoolConfigValueHelper DisplayPrintStringSource(TEXT("Kismet"), TEXT("bLogPrintStringSource"), GEngineIni);
	if (DisplayPrintStringSource)
	{
		const FString SourceObjectPrefix = FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject));
		FinalLogString = SourceObjectPrefix + FinalLogString;
	}

	if (bPrintToLog)
	{
		switch ( LogTypeInternal )
		{
		case Cy_Print_Type::Log:
			UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s : %s"), *InPrefix ,*FinalLogString);
			break;
		case Cy_Print_Type::Warning:
			UE_LOG(LogBlueprintUserMessages, Warning, TEXT("%s : %s"), *InPrefix ,*FinalLogString);
			break;
		case Cy_Print_Type::Error:
			UE_LOG(LogBlueprintUserMessages, Error, TEXT("%s : %s"), *InPrefix ,*FinalLogString);
			break;
		case Cy_Print_Type::Fatal:
			UE_LOG(LogBlueprintUserMessages, Fatal, TEXT("%s : %s"), *InPrefix ,*FinalLogString);
			break;
		case Cy_Print_Type::Display:
			UE_LOG(LogBlueprintUserMessages, Display, TEXT("%s : %s"), *InPrefix ,*FinalLogString);
			break;
		}
		// UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s"), *FinalLogString);
		
		APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : NULL);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalDisplayString);
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Verbose, TEXT("%s"), *FinalLogString);
	}

	// Also output to the screen, if possible
	if (bPrintToScreen)
	{
		if (GAreScreenMessagesEnabled)
		{
			if (GConfig && Duration < 0)
			{
				GConfig->GetFloat( TEXT("Kismet"), TEXT("PrintStringDuration"), Duration, GEngineIni );
			}
			uint64 InnerKey = -1;
			if (Key != NAME_None)
			{
				InnerKey = GetTypeHash(Key);
			}
			GEngine->AddOnScreenDebugMessage(InnerKey, Duration, TextColor.ToFColor(true), FinalDisplayString);
		}
		else
		{
			UE_LOG(LogBlueprint, VeryVerbose, TEXT("Screen messages disabled (!GAreScreenMessagesEnabled).  Cannot print to screen."));
		}
	}
#endif
	}
}
