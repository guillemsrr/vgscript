#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FVgScriptEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};