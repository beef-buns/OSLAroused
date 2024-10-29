#include "ArousalManager.h"
#include "PersistedData.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Settings.h"
#include "ActorStateManager.h"

/*
 *
 *  Arousal

    Arousal = TimeRate * (DaysSinceLastOrgasm) + Sum(SingleExposure * ExposureRate)
    Arousal min value is 0 and max value 100, but sometimes -2 is returned for invalid/blocked actors
    ExposureRate can be set from MCM menu or by modders, values are 0-10
    TimeRate decays overtime, having sex increases TmeRate, values are 0-100


SexLab

    Each stage of foreplay animation adds +1 exposure
    Orgasm reduces exposure by "actual animation duration" / "default animation duration" * 20. Tip: extending sex duration would reduce more exposure after orgasm     time is stored to each actor
    Assault reduces exposure by additional 10 points to victim

Naked

    PC arousal increases if one sees naked NPC and vice versa
    If NPC sees naked PC, and PC set as Exhibitionist in MCM menu, then PCs exposure is increased

 */

using namespace PersistedData;

namespace ArousalManager {
    float GetArousal(RE::Actor *actorRef, bool bUpdateState) {
//        if (!Utilities::Actor::IsArousable(actorRef)) {
//            return -2.f;
//        }
//
        return ActorStateManager::GetSingleton()->GetActorArousal(actorRef);

        RE::FormID actorFormId = actorRef->formID;

        const auto LastCheckTimeData = LastCheckTimeData::GetSingleton();
        auto lastCheckTime = LastCheckTimeData->GetData(actorFormId, 0.f);
        float curTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
        float gameHoursPassed = (curTime - lastCheckTime) * 24;

        float newArousal = ArousalData::GetSingleton()->GetData(actorRef->formID, -2.f);

        //If set to update state, or we have never checked (last check time is 0), then update the lastchecktime
        if (bUpdateState || lastCheckTime == 0.f) {
            newArousal = CalculateArousal(actorRef, gameHoursPassed);
            LastCheckTimeData->SetData(actorFormId, curTime);
            SetArousal(actorRef, newArousal);

            LibidoManager::GetSingleton()->UpdateActorLibido(actorRef, gameHoursPassed, newArousal);
        }
        logger::info("Got Arousal for {} val: {}", actorRef->GetDisplayFullName(), newArousal);
        return newArousal;
    }

    void Update(RE::Actor *actorRef) {
        return ActorStateManager::GetSingleton()->UpdateActorArousal(actorRef);

        if (!Utilities::Actor::IsArousable(actorRef)) {
            return;
        }
        RE::FormID actorFormId = actorRef->formID;

        auto settings = Settings::GetSingleton();
        const auto LastCheckTimeData = LastCheckTimeData::GetSingleton();
        auto timeSinceLastCheck = LastCheckTimeData->GetData(actorFormId, -1.f);
        auto timeScale = RE::Calendar::GetSingleton()->GetTimescale();

        auto delta = timeSinceLastCheck += RE::GetSecondsSinceLastFrame();
        //If set to update state, or we have never checked (last check time is 0), then update the lastchecktime
        if (delta > settings->GetUpdateRate() || timeSinceLastCheck < 0.f) {
            logger::info("{} needs arousal update, delta: {}", actorRef->GetDisplayFullName(), delta);
            clib_util::Timer timer;
            timer.start();

            float gameHoursPassed = delta * timeScale / 3600;
            logger::info("gameHoursPassed: {} ", gameHoursPassed);
            float originalArousal = ArousalData::GetSingleton()->GetData(actorRef->formID, -2.f);
            float newArousal = CalculateArousal(actorRef, gameHoursPassed);

            logger::info("Updating Arousal for {}, {}->{}", actorRef->GetDisplayFullName(), originalArousal,
                         newArousal);
            SetArousal(actorRef, newArousal);

            LibidoManager::GetSingleton()->UpdateActorLibido(actorRef, gameHoursPassed, newArousal);

            timer.end();

            // Reset the counter now that we've updated.
            delta = 0;
            logger::info("Update Arousal for {} took: took {}μs / {}ms", actorRef->GetDisplayFullName(),
                         timer.duration_μs(), timer.duration_ms());
        }

        LastCheckTimeData->SetData(actorFormId, delta);
    }

    float SetArousal(RE::Actor *actorRef, float value) {
        if (!Utilities::Actor::IsArousable(actorRef)) {
            return -2.f;
        }

        value = std::clamp(value, 0.0f, 100.f);

        ArousalData::GetSingleton()->SetData(actorRef->formID, value);

        const auto lastEventData = LastEventData::GetSingleton();

        float lastReportedArousal = lastEventData->GetData(actorRef->formID, 100.f);
        auto delta = std::abs(value - lastReportedArousal);

        if (delta > 1.f) {
            Papyrus::Events::SendActorArousalUpdatedEvent(actorRef, value);
            lastEventData->SetData(actorRef->formID, value);
        }

        return value;
    }

    float ModifyArousal(RE::Actor *actorRef, float modValue) {
        modValue *= PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);

        float currentArousal = GetArousal(actorRef, false);
        return SetArousal(actorRef, currentArousal + modValue);
    }

    float CalculateArousal(RE::Actor *actorRef, float gameHoursPassed) {
        float currentArousal = ArousalData::GetSingleton()->GetData(actorRef->formID, -2.f);

        //If never calculated, regen
        if (currentArousal < -1) {
            currentArousal = Utilities::GenerateRandomFloat(10.f, 50.f);
            //logger::debug("Random Arousal: {} Val: {}", actorRef->GetDisplayFullName(), currentArousal);
            return currentArousal;
        }

        float currentArousalBaseline = LibidoManager::GetSingleton()->GetBaselineArousal(actorRef);

        float epsilon = Settings::GetSingleton()->GetArousalChangeRate();
        //logger::trace("CalculateArousal: epsilon: {}", epsilon);


        float t = 1.f - std::pow(epsilon, gameHoursPassed);
        float newArousal = std::lerp(currentArousal, currentArousalBaseline, t);
        //logger::trace("CalculateArousal: {} from: {} newArousal {} Diff: {}  t: {}", actorRef->GetDisplayFullName(), currentArousal, newArousal, newArousal - currentArousal, t);

        return newArousal;
    }

    float GetArousalExt(RE::Actor *actorRef) {
        if (!Utilities::Actor::IsArousable(actorRef)) {
            return -2.f;
        }
        float newArousal = CalculateArousal(actorRef, 0.0f);
        return newArousal;
    }

    float SetArousalExt(RE::Actor *actorRef, float value, bool sendevent) {
        if (!Utilities::Actor::IsArousable(actorRef)) {
            return -2.f;
        }

        value = std::clamp(value, 0.0f, 100.f);
        ArousalData::GetSingleton()->SetData(actorRef->formID, value);

        //send event
        if (sendevent) {
            auto loc_handle = actorRef->GetHandle().native_handle();
            SKSE::GetTaskInterface()->AddTask([loc_handle, value] {
                Papyrus::Events::SendActorArousalUpdatedEvent(RE::Actor::LookupByHandle(loc_handle).get(), value);
            });
        }

        return value;
    }

    float ModifyArousalExt(RE::Actor *actorRef, float modValue, bool sendevent) {
        if (!Utilities::Actor::IsArousable(actorRef)) {
            return -2.f;
        }

        modValue *= PersistedData::ArousalMultiplierData::GetSingleton()->GetData(actorRef->formID, 1.f);
        float currentArousal = GetArousalExt(actorRef);
        auto loc_res = SetArousalExt(actorRef, currentArousal + modValue, sendevent);
        return loc_res;
    }
}
