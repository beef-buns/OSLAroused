#include "ActorStateManager.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Managers/LibidoManager.h"

bool IsActorNaked(RE::Actor *actorRef) {
    return Utilities::Actor::IsNaked(actorRef);
}

bool ActorStateManager::GetActorNaked(RE::Actor *actorRef) {
    return m_ActorNakedStateCache(actorRef);
}

void ActorStateManager::ActorNakedStateChanged(RE::Actor *actorRef, bool newNaked) {
    m_ActorNakedStateCache.UpdateItem(actorRef, newNaked);
    Papyrus::Events::SendActorNakedUpdatedEvent(actorRef, newNaked);

    //Actor Naked updated so remove libido cache entry to force refresh on next fetch
    LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(actorRef);
}

bool ActorStateManager::GetActorSpectatingNaked(RE::Actor *actorRef) {
    if (const auto lastViewedGameTime = m_NakedSpectatingMap[actorRef]) {
        //@TODO: Calculate time based off global update cycle [not just 0.72 game hours]
        if (RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastViewedGameTime < 0.1f) {
            return true;
        }
    }
    return false;
}

void ActorStateManager::UpdateActorsSpectating(std::set<RE::Actor *> spectators) {
    //Remove any old spectators from map who are not in spectators set
    //Need to do this to purge libido modifier cache
    for (auto itr = m_NakedSpectatingMap.begin(); itr != m_NakedSpectatingMap.end();) {
        if (!spectators.contains((*itr).first)) {
            LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated((*itr).first);
            itr = m_NakedSpectatingMap.erase(itr);
        } else {
            itr++;
        }
    }

    float currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
    for (const auto spectator: spectators) {
        m_NakedSpectatingMap[spectator] = currentTime;
        LibidoManager::GetSingleton()->ActorLibidoModifiersUpdated(spectator);
    }
}

bool ActorStateManager::IsHumanoidActor(RE::Actor *actorRef) {
    if (!m_CreatureKeyword) {
        m_CreatureKeyword = (RE::BGSKeyword *) RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId);
    }
    if (!m_AnimalKeyword) {
        m_AnimalKeyword = (RE::BGSKeyword *) RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId);
    }

    if (m_CreatureKeyword && m_AnimalKeyword) {
        return !actorRef->HasKeyword(m_CreatureKeyword) && !actorRef->HasKeyword(m_AnimalKeyword);
    }

    return false;
}

void ActorStateManager::UpdateWornKeywordCache(RE::Actor *actorRef, RE::TESObjectARMO *armor, bool equipped) {
    if (!Utilities::Actor::IsValid(actorRef)) {
        return;
    }

    std::vector<std::string_view> registeredKeywords = {"EroticArmor", "SLA_"};
    auto &data = wornKeywordCache[actorRef->GetFormID()];

    if (data.empty()) {
        const auto actorEquipment = actorRef->GetInventory(
                [=](RE::TESBoundObject &a_object) { return a_object.IsArmor(); });

        for (const auto editorID: registeredKeywords) {
            for (const auto &[item, invData]: actorEquipment) {
                const auto &[count, entry] = invData;
                if (count > 0 && entry->IsWorn()) {
                    if (item->As<RE::TESObjectARMO>() &&
                        item->As<RE::TESObjectARMO>()->ContainsKeywordString(editorID)) {
                        if (!data.contains(editorID)) {
                            data[editorID] = 0;
                        }
                        data[editorID]++;
                        logger::info("Updated worn keyword cache for actor: {} with: {} x {}",
                                     actorRef->GetDisplayFullName(),
                                     editorID,
                                     data[editorID]);
                    }
                }
            }
        }
    } else {
        for (const auto editorID: registeredKeywords) {
//            if (armor && armor->ContainsKeywordString(editorID)) {
            if (armor) {
                armor->ForEachKeyword([&](const RE::BGSKeyword *keyword) {
                    if (!keyword) {
                        return RE::BSContainer::ForEachResult::kContinue;
                    }

                    if (keyword->formEditorID == editorID) {
                        if (equipped) {
                            data[editorID]++;
                        } else {
                            data[editorID]--;
                        }
                        logger::info("Updated worn keyword cache for actor: {} with: {} x {}",
                                     actorRef->GetDisplayFullName(),
                                     editorID,
                                     data[editorID]);
                    }
                    return RE::BSContainer::ForEachResult::kContinue;
                });
            }
        }
    }
}

bool ActorStateManager::Load(SKSE::SerializationInterface *a_intfc) {
    assert(a_intfc);

    std::size_t recordDataSize;
    a_intfc->ReadRecordData(recordDataSize);

    Locker locker(m_Lock);
    Actors.clear();

    RE::FormID formId;

    for (auto i = 0; i < recordDataSize; i++) {
        auto formIdResult = stl::ReadFormID(a_intfc);
        if (!formIdResult) {
            logger::error("Failed to read form ID for data record {}"sv, i);
            return false;
        }
        formId = formIdResult.value();

        if (!Actors[formId].Load(a_intfc)) {
            logger::error("Failed to read Actor State for formID {}"sv, formId);
            return false;
        }
    }

    return true;
}

bool ActorStateManager::Save(SKSE::SerializationInterface *a_intfc) {
    assert(a_intfc);

    const std::size_t numRegs = Actors.size();
    logger::info("arousalData total to save {}", numRegs);
    if (!a_intfc->WriteRecordData(numRegs)) {
        logger::error("Failed to save number of regs ({})", numRegs);
        return false;
    }

    for (auto &&[formID, data]: Actors) {
        logger::info("Attempting to save {}'s arousalData", formID);
        if (!a_intfc->WriteRecordData(formID)) {
            logger::error("Failed to save reg ({:X})", formID);
            continue;
        }

        if (!data.Save(a_intfc)) {
            logger::error("Failed to save arousalData for {}", formID);
            return false;
        }
    }

    return true;
}

std::optional<Arousal::ActorState *> ActorStateManager::GetActorState(RE::Actor *actorRef) {
    if (!Utilities::Actor::IsArousable(actorRef)) {
        return std::nullopt;
    }

    Locker locker(m_Lock);
    auto formId = actorRef->GetFormID();

    return std::make_optional(&Actors[formId]);
}

float ActorStateManager::GetActorArousal(RE::Actor *actorRef) {
    const auto actorState = GetActorState(actorRef);
    if (!actorState) {
        return -2.f;
    }

    return actorState.value()->GetArousal();
}

void ActorStateManager::UpdateActorArousal(RE::Actor *actorRef) {
    auto actorData = GetActorState(actorRef);
    if (!actorData) {
        return;
    }

    auto data = actorData.value();

    auto oldArousal = data->GetArousal();
    float currTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();

    data->CalculateArousal();

    float delta = data->GetArousal() - oldArousal;

    logger::info(
            "[Arousal] {}: {} -> {}, changed by {}", actorRef->GetName(), oldArousal, data->GetArousal(), delta);


    if (currTime - data->lastUpdate > 10) {
        data->lastUpdate = currTime;
        Papyrus::Events::SendActorArousalUpdatedEvent(actorRef, data->GetArousal());
    }
}

float ActorStateManager::GetActorExposure(RE::Actor *actorRef) {
    const auto actorState = GetActorState(actorRef);
    if (!actorState) {
        return -2.f;
    }

    return actorState.value()->GetExposure();
}

float ActorStateManager::GetActorExposureRate(RE::Actor *actorRef) {
    const auto actorState = GetActorState(actorRef);
    if (!actorState) {
        return -2.f;
    }

    return actorState.value()->GetExposureRate();
}

float ActorStateManager::GetActorTimeRate(RE::Actor *actorRef) {
    const auto actorState = GetActorState(actorRef);
    if (!actorState) {
        return -2.f;
    }

    return actorState.value()->GetTimeRate();
}

void ActorStateManager::UpdateActorExposureModifier(RE::Actor *actorRef, const std::string_view &name, float value) {
    const auto actorState = GetActorState(actorRef);
    if (!actorState) {
        return;
    }

    auto data = actorState.value();

    data->SetExposureModifier(name, value);
}
