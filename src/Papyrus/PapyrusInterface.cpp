#include "PapyrusInterface.h"

#include "PersistedData.h"
#include "Managers/ActorStateManager.h"
#include "Managers/LibidoManager.h"
#include "Utilities/Utils.h"
#include <Settings.h>
#include <Integrations/DevicesIntegration.h>

float PapyrusInterface::GetArousal(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return ActorStateManager::GetArousal(actorRef);
}

std::vector<float>
PapyrusInterface::GetArousalMultiple(RE::StaticFunctionTag *, RE::reference_array<RE::Actor *> actorRefs) {
    std::vector<float> results;

    for (const auto actorRef: actorRefs) {
        if (!actorRef) {
            Utilities::logInvalidArgsVerbose(__FUNCTION__);
            results.push_back(0);
            continue;
        }
        results.push_back(ActorStateManager::GetArousal(actorRef));
    }

    return results;
}

float PapyrusInterface::GetArousalNoSideEffects(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return ActorStateManager::GetArousal(actorRef);
}

float PapyrusInterface::SetArousal(RE::StaticFunctionTag *, RE::Actor *actorRef, float value) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    ActorStateManager::SetArousal(actorRef, value);

    return ActorStateManager::GetArousal(actorRef);
}

void
PapyrusInterface::SetArousalMultiple(RE::StaticFunctionTag *, RE::reference_array<RE::Actor *> actorRefs, float value) {
    for (const auto actorRef: actorRefs) {
        if (!actorRef) {
            Utilities::logInvalidArgsVerbose(__FUNCTION__);
            continue;
        }
        ActorStateManager::SetArousal(actorRef, value);
    }
}

float PapyrusInterface::ModifyArousal(RE::StaticFunctionTag *, RE::Actor *actorRef, float value) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    ActorStateManager::ModifyArousal(actorRef, value);

    return ActorStateManager::GetArousal(actorRef);
}

void PapyrusInterface::ModifyArousalMultiple(RE::StaticFunctionTag *, RE::reference_array<RE::Actor *> actorRefs,
                                             float value) {
    for (const auto actorRef: actorRefs) {
        if (!actorRef) {
            Utilities::logInvalidArgsVerbose(__FUNCTION__);
            continue;
        }
        ActorStateManager::ModifyArousal(actorRef, value);
    }
}

float PapyrusInterface::GetArousalMultiplier(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return ActorStateManager::GetSingleton()->GetActorExposureRate(actorRef);
}

float PapyrusInterface::SetArousalMultiplier(RE::StaticFunctionTag *, RE::Actor *actorRef, float value) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    value = std::clamp(value, 0.0f, 100.f);

    ActorStateManager::GetSingleton()->SetActorExposureRate(actorRef, value);
    return ActorStateManager::GetSingleton()->GetActorExposureRate(actorRef);
}

float PapyrusInterface::ModifyArousalMultiplier(RE::StaticFunctionTag *, RE::Actor *actorRef, float value) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    float curMult = PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
    float newVal = curMult + value;
    PersistedData::ArousalMultiplierData::GetSingleton()->SetData(actorRef->formID, newVal);
    return newVal;
}

float PapyrusInterface::GetExposure(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return ActorStateManager::GetExposure(actorRef);
}

float PapyrusInterface::GetExposureRate(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return ActorStateManager::GetExposureRate(actorRef);
}

float PapyrusInterface::ModifyExposure(RE::StaticFunctionTag *, RE::Actor *actorRef, float value) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }

    ActorStateManager::ModifyExposure(actorRef, value);

    return ActorStateManager::GetExposure(actorRef);
}

void
PapyrusInterface::ModifyExposureMultiple(RE::StaticFunctionTag *, RE::reference_array<RE::Actor *> actorRefs,
                                         float value) {
    for (const auto actorRef: actorRefs) {
        if (!actorRef) {
            Utilities::logInvalidArgsVerbose(__FUNCTION__);
            continue;
        }
//        LibidoManager::GetSingleton()->ModifyBaseLibido(actorRef, value);
        ActorStateManager::ModifyExposure(actorRef, value);
    }
}

float PapyrusInterface::GetTimeRate(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return ActorStateManager::GetSingleton()->GetActorTimeRate(actorRef);
}

// DEPRECATED: Use Exposure
float PapyrusInterface::GetLibido(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return LibidoManager::GetSingleton()->GetBaseLibido(actorRef);
}

// DEPRECATED: Use Exposure
float PapyrusInterface::SetLibido(RE::StaticFunctionTag *, RE::Actor *actorRef, float newVal) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return LibidoManager::GetSingleton()->SetBaseLibido(actorRef, newVal);
}

float PapyrusInterface::GetDaysSinceLastOrgasm(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    float lastOrgasmTime = PersistedData::LastOrgasmTimeData::GetSingleton()->GetData(actorRef->formID, 0.f);
    if (lastOrgasmTime < 0) {
        lastOrgasmTime = 0;
    }

    return RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastOrgasmTime;
}

bool PapyrusInterface::IsNaked(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return false;
    }
    return Utilities::Actor::IsNakedCached(actorRef);
}

bool PapyrusInterface::IsViewingNaked(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return false;
    }
    return Utilities::Actor::IsViewingNaked(actorRef);
}

bool PapyrusInterface::IsInScene(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return false;
    }
    return Utilities::Actor::IsParticipatingInScene(actorRef);
}

bool PapyrusInterface::IsViewingScene(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return false;
    }
    return Utilities::Actor::IsViewingScene(actorRef);
}

bool PapyrusInterface::IsWearingEroticArmor(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return false;
    }
    if (!Utilities::Actor::IsNakedCached(actorRef)) {
        if (const auto eroticKeyword = Settings::GetSingleton()->GetEroticArmorKeyword()) {
            const auto wornKeywords = Utilities::Actor::GetWornArmorKeywords(actorRef);
            return wornKeywords.contains(eroticKeyword->formID);
        }
    }
    return false;
}

float PapyrusInterface::WornDeviceBaselineGain(RE::StaticFunctionTag *, RE::Actor *actorRef) {
    if (!actorRef) {
        Utilities::logInvalidArgsVerbose(__FUNCTION__);
        return 0;
    }
    return DevicesIntegration::GetSingleton()->GetArousalBaselineFromDevices(actorRef);
}

bool PapyrusInterface::RegisterFunctions(RE::BSScript::IVirtualMachine *vm) {
    vm->RegisterFunction("GetArousal", "OSLArousedNative", GetArousal);
    vm->RegisterFunction("GetArousalMultiple", "OSLArousedNative", GetArousalMultiple);
    vm->RegisterFunction("GetArousalNoSideEffects", "OSLArousedNative", GetArousalNoSideEffects);

    vm->RegisterFunction("SetArousal", "OSLArousedNative", SetArousal);
    vm->RegisterFunction("SetArousalMultiple", "OSLArousedNative", SetArousalMultiple);
    vm->RegisterFunction("ModifyArousal", "OSLArousedNative", ModifyArousal);
    vm->RegisterFunction("ModifyArousalMultiple", "OSLArousedNative", ModifyArousalMultiple);

    vm->RegisterFunction("SetArousalMultiplier", "OSLArousedNative", SetArousalMultiplier);
    vm->RegisterFunction("GetArousalMultiplier", "OSLArousedNative", GetArousalMultiplier);
    vm->RegisterFunction("ModifyArousalMultiplier", "OSLArousedNative", ModifyArousalMultiplier);

    vm->RegisterFunction("GetExposure", "OSLArousedNative", GetExposure);
    vm->RegisterFunction("GetArousalBaseline", "OSLArousedNative", GetExposure);

    vm->RegisterFunction("ModifyArousalBaseline", "OSLArousedNative", ModifyExposure);
    vm->RegisterFunction("ModifyArousalBaseline", "OSLArousedNative", ModifyExposureMultiple);

    vm->RegisterFunction("SetArousalBaseline", "OSLArousedNative", SetLibido);
    vm->RegisterFunction("GetLibido", "OSLArousedNative", GetLibido);
    vm->RegisterFunction("SetLibido", "OSLArousedNative", SetLibido);

    vm->RegisterFunction("GetExposureRate", "OSLArousedNative", GetExposureRate);
    vm->RegisterFunction("GetTimeRate", "OSLArousedNative", GetTimeRate);

    vm->RegisterFunction("GetDaysSinceLastOrgasm", "OSLArousedNative", GetDaysSinceLastOrgasm);

    //Explainer
    vm->RegisterFunction("IsNaked", "OSLArousedNative", IsNaked);
    vm->RegisterFunction("IsViewingNaked", "OSLArousedNative", IsViewingNaked);
    vm->RegisterFunction("IsInScene", "OSLArousedNative", IsInScene);
    vm->RegisterFunction("IsViewingScene", "OSLArousedNative", IsViewingScene);
    vm->RegisterFunction("IsWearingEroticArmor", "OSLArousedNative", IsWearingEroticArmor);
    vm->RegisterFunction("WornDeviceBaselineGain", "OSLArousedNative", WornDeviceBaselineGain);

    return true;
}
