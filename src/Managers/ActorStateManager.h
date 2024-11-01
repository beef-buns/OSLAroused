#pragma once

#include "Utilities/LRUCache.h"
#include "ActorState.h"

bool IsActorNaked(RE::Actor *actorRef);

const static RE::FormID kActorTypeCreatureKeywordFormId = 0x13795;
const static RE::FormID kActorTypeAnimalKeywordFormId = 0x13798;

class ActorStateManager : public ISingleton<ActorStateManager> {
public:


    ActorStateManager() :
            wornKeywordCache(),
            m_ActorNakedStateCache(std::function < bool(RE::Actor * ) > (IsActorNaked), 100),
            m_CreatureKeyword((RE::BGSKeyword *) RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId)),
            m_AnimalKeyword((RE::BGSKeyword *) RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId)) {}

    bool GetActorNaked(RE::Actor *actorRef);

    std::optional<Arousal::ActorState *> GetActorState(RE::Actor *actorRef);

    float GetActorArousal(RE::Actor *actorRef);

    float GetActorExposure(RE::Actor *actorRef);

    float GetActorExposureRate(RE::Actor *actorRef);

    float GetActorTimeRate(RE::Actor *actorRef);

    inline static float GetArousal(RE::Actor *actorRef) {
        return GetSingleton()->GetActorArousal(actorRef);
    }

    inline static float GetExposure(RE::Actor *actorRef) {
        return GetSingleton()->GetActorExposure(actorRef);
    }

    inline static float GetExposureRate(RE::Actor *actorRef) {
        return GetSingleton()->GetActorExposureRate(actorRef);
    }

    void UpdateActorArousal(RE::Actor *actor);

    void UpdateActorExposureModifier(RE::Actor *actor, const std::string_view &name, float value);

    void ModifyActorExposureModifier(RE::Actor *actor, const std::string_view &name, float value);

    inline static void SetArousal(RE::Actor *actorRef, float value) {
        GetSingleton()->UpdateActorExposureModifier(actorRef, "SetArousal", value);
    }

    inline static void ModifyArousal(RE::Actor *actorRef, float value) {
        GetSingleton()->UpdateActorExposureModifier(actorRef, "SetArousal", value);
    }

    inline static void SetExposure(RE::Actor *actorRef, float value) {
        GetSingleton()->UpdateActorExposureModifier(actorRef, "SetExposure", value);
    }

    inline static void ModifyExposure(RE::Actor *actorRef, float value) {
        GetSingleton()->ModifyActorExposureModifier(actorRef, "SetExposure", value);
    }

    void ActorNakedStateChanged(RE::Actor *actorRef, bool newNaked);

    bool GetActorSpectatingNaked(RE::Actor *actorRef);

    void UpdateActorsSpectating(const std::set<RE::Actor *>& spectators);

    //Returns true if actor is non-creature, non-animal npc
    bool IsHumanoidActor(RE::Actor *actorRef);

    void UpdateWornKeywordCache(RE::Actor *actorRef, RE::TESObjectARMO *armor, bool equipped);

    bool Load(SKSE::SerializationInterface *a_intfc);

    bool Save(SKSE::SerializationInterface *a_intfc);

    void SetActorExposureRate(RE::Actor *actorRef, float value);

private:

    using Lock = std::recursive_mutex;
    using Locker = std::lock_guard<Lock>;
    mutable Lock m_Lock;

    Utilities::LRUCache<RE::Actor *, bool> m_ActorNakedStateCache;

//    std::map<RE::FormID,
//    std::map<RE::FormID, std::map<RE::FormID, int>> wornKeywordCache;
    std::map<RE::FormID, std::map<std::string_view, int>> wornKeywordCache;

    std::map<RE::Actor *, float> m_NakedSpectatingMap;

    RE::BGSKeyword *m_CreatureKeyword;
    RE::BGSKeyword *m_AnimalKeyword;

    std::map<RE::FormID, Arousal::ActorState> Actors;

};
