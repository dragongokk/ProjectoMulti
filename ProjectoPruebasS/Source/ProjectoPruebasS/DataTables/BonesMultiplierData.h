#pragma once
#include "Engine/DataTable.h"

#include "BonesMultiplierData.generated.h"

USTRUCT(BlueprintType)
struct FBonesMultiplierData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (DisplayName = "Damage Multiplier"))
	float DamageMultiplier;

};