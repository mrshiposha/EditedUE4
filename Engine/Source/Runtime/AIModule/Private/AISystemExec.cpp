// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AIModulePrivate.h"
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#include "Misc/CoreMisc.h"

struct FAISystemExec : public FSelfRegisteringExec
{
	FAISystemExec()
	{
	}

	// Begin FExec Interface
	virtual bool Exec(UWorld* Inworld, const TCHAR* Cmd, FOutputDevice& Ar) OVERRIDE;
	// End FExec Interface
};

FAISystemExec AISystemExecInstance;

bool FAISystemExec::Exec(UWorld* Inworld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (Inworld == NULL)
	{
		return false;
	}

	UAISystem* AISys = UAISystem::GetCurrent(Inworld);	
	bool bHandled = false;

	if (AISys != NULL)
	{
		if (FParse::Command(&Cmd, TEXT("AIIgnorePlayers")))
		{
			AISys->AIIgnorePlayers();
			bHandled = true;
		}
		else if (FParse::Command(&Cmd, TEXT("AILoggingVerbose")))
		{
			AISys->AILoggingVerbose();
			bHandled = true;
		}

		// void UAISystem::RunEQS(const FString& QueryName)
	}

	return bHandled;
}

#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)