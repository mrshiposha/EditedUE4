// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// AnimSequenceExporterFBX
//=============================================================================

#pragma once
#include "Exporters/Exporter.h"
#include "AnimSequenceExporterFBX.generated.h"

UCLASS()
class UAnimSequenceExporterFBX : public UExporter
{
	GENERATED_BODY()
public:
	UAnimSequenceExporterFBX(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	// Begin UExporter Interface
	virtual bool ExportBinary( UObject* Object, const TCHAR* Type, FArchive& Ar, FFeedbackContext* Warn, int32 FileIndex = 0, uint32 PortFlags=0 ) override;
	// End UExporter Interface
};



