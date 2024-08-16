// Copyright CyberAlien ,All Rights Reserved , 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cy_Print_Library.generated.h"

UENUM(BlueprintType)
enum Cy_Print_Type
{
	Log,
	Warning,
	Error,
	Fatal,
	Display
};

UCLASS()
class CY_PRINT_API UCy_Print_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable , meta = (WorldContext = "WorldContextObject" , AdvancedDisplay = "WorldContextObject,bPrintToScreen,bPrintToLog,TextColor,Duration,Key,InPrefix,LogTypeInternal" ), Category = "Cy_Print")
	static void Cy_PrintString( const UObject* WorldContextObject, const FString& InString, bool Enabled = true , bool bPrintToScreen = false , bool bPrintToLog = true , FLinearColor TextColor = FLinearColor::Green , float Duration = 0.0 , const FName Key = "none", const FString& InPrefix = "", const Cy_Print_Type LogTypeInternal = Cy_Print_Type::Log );
	
};
