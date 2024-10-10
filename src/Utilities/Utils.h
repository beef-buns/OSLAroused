#pragma once

#include <random>
#include "Managers/SceneManager.h"
#include "Managers/ActorStateManager.h"

#include "GameForms.h"

namespace Utilities {
    void logInvalidArgsVerbose(const char *fnName);

    inline float GenerateRandomFloat(float min, float max) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(mt);
    }

    namespace Forms {
        RE::FormID ResolveFormId(uint32_t modIndex, RE::FormID rawFormId);
    }

    //Keyword logic based off powerof3's CommonLibSSE implementation
    namespace Keywords {
        bool AddKeyword(RE::TESForm *form, RE::BGSKeyword *keyword);

        bool RemoveKeyword(RE::TESForm *form, RE::BGSKeyword *keyword);

        void DistributeKeywords();
    }

    namespace Actor {
        inline bool IsNaked(RE::Actor *actorRef) {
            return actorRef->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) == nullptr;
        }

        inline bool IsNakedCached(RE::Actor *actorRef) {
            return ActorStateManager::GetSingleton()->GetActorNaked(actorRef);
        }

        inline bool IsViewingNaked(RE::Actor *actorRef) {
            return ActorStateManager::GetSingleton()->GetActorSpectatingNaked(actorRef);
        }

        inline bool IsParticipatingInScene(RE::Actor *actorRef) {
            return SceneManager::GetSingleton()->IsActorParticipating(actorRef);
        }

        inline bool IsViewingScene(RE::Actor *actorRef) {
            return SceneManager::GetSingleton()->IsActorViewing(actorRef);
        }

        std::vector<RE::TESForm *> GetWornArmor(RE::Actor *actorRef);

        std::set<RE::FormID> GetWornArmorKeywords(RE::Actor *actorRef, RE::TESForm *armorToIgnore = nullptr);

        inline bool IsValid(RE::TESObjectREFR *actor) {
            if (!actor || actor->IsChild() || actor->IsDisabled()) {
                return false;
            }

            if (!actor->As<RE::Actor>()) {
                return false;
            }

            return true;
        }

        inline bool IsArousable(RE::Actor *actor) {
            if (!IsValid(actor)) {
                return false;
            }

            if (actor->IsDead() || !actor->Is3DLoaded()) {
                return false;
            }

            return true;
        }


        // Copied from OAR source
        inline static int32_t *g_RelationshipRankTypeIdsByIndex = (int32_t *) REL::VariantID(502260, 369311,
                                                                                             0x1E911A0).address();
        using tTESNPC_GetRelationshipRankIndex = int32_t(*)(RE::TESNPC *a_npc1, RE::TESNPC *a_npc2);
        inline static REL::Relocation<tTESNPC_GetRelationshipRankIndex> TESNPC_GetRelationshipRankIndex{
                REL::VariantID(23624, 24076, 0x355790)};  // 345ED0, 35C270, 355790
        inline bool GetRelationshipRank(RE::TESNPC *a_npc1, RE::TESNPC *a_npc2, int32_t &a_outRank) {
            if (a_npc1 && a_npc2) {
                a_outRank = g_RelationshipRankTypeIdsByIndex[TESNPC_GetRelationshipRankIndex(a_npc1, a_npc2)];
                return true;
            }

            return false;
        }

        inline bool ValidateTarget(RE::Actor *actor, RE::Actor *target) {
            if (!IsValid(target) || !IsArousable(actor)) {
                return false;
            }

            if (target == actor) {
                return false;
            }

            if (target->IsDeleted() || target->IsDisabled() || !target->Is3DLoaded()) {
                return false;
            }

            if (target->GetRace()) {
                std::string raceEdid = target->GetRace()->GetFormEditorID();
                if (raceEdid == "ManakinRace") {
                    return false;
                }
            }

            return true;
        }

        inline std::vector<RE::Actor *> GetDetectedActors(RE::Actor *a_actor) {
            std::vector<RE::Actor *> detectors;

            if (!IsArousable(a_actor)) {
                return detectors;
            }

            if (a_actor->GetActorRuntimeData().currentProcess) {
                if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
                    for (auto &targetHandle: processLists->highActorHandles) {
                        if (const auto target = targetHandle.get(); target &&
                                                                    target->GetActorRuntimeData().currentProcess) {
                            if (const auto base = target->GetActorBase(); base && !base->AffectsStealthMeter()) {
                                continue;
                            }

//                            actorsExist = true;
                            if (ValidateTarget(a_actor, target.get())) {
//                                RE::Actor *detector = isDetectedByCondition ? target.get() : a_actor;
//                                RE::Actor *detectee = isDetectedByCondition ? a_actor : target.get();

                                if (a_actor->RequestDetectionLevel(target.get()) > 0) {
                                    logger::info("{} detects {}", a_actor->GetDisplayFullName(),
                                                 target->GetDisplayFullName());
                                    detectors.push_back(target.get());
                                }
                            }
                        }
                    }

//                        if (actorsExist) { // This if-condition exists because this function gets called even in main menu.
//                            // The loop doesn't trigger for player. Thus, check player separately
//                            const RE::FormID playerRefId = 0x14;
//                            const auto playerRef = RE::TESForm::LookupByID(playerRefId);
//                            const auto playerActor = playerRef->As<RE::Actor>();
//                            if (ValidateTarget(a_actor, playerActor)) {
//                                RE::Actor *detector = isDetectedByCondition ? playerActor : a_actor;
//                                RE::Actor *detectee = isDetectedByCondition ? a_actor : playerActor;
//
//                                if (detector->RequestDetectionLevel(detectee) > 0) {
//                                    detectors.push_back(playerActor);
//                                }
//                            }
//                        }

//                    if (!detectors.empty()) {
//                        // Sorting is actually simply useless. The only difference would be in resultText
//                        //
//                        //RE::NiPoint3 a_actor_pos = a_actor->GetPosition();
//                        //std::sort(detectors.begin(), detectors.end(), [&a_actor_pos](const auto& a, const auto& b)
//                        //	{
//                        //		return a->GetPosition().GetSquaredDistance(a_actor_pos) < b->GetPosition().GetSquaredDistance(a_actor_pos);
//                        //	});
//
//                        for (const auto &target: detectors) {
////                                bool passedEvaluation = CheckMultiCondition(a_actor, target, a_refr, a_clipGenerator,
////                                                                            a_parentSubMod);
////
////                                if (passedEvaluation) {
////                                    if (target->GetName()) {
////                                        resultText = target->GetName();
////                                    }
////
////                                    return true;
////                                }
//                        }
//                    }
                }
            }

            return detectors;
        }

        inline bool IsAttractedToMen(RE::Actor *actorRef) {
            if (!IsArousable(actorRef)) {
                return false;
            }

            return actorRef->IsInFaction(GameForms::m_AttractedToMalesFaction);
        }

        inline bool IsAttractedToWomen(RE::Actor *actorRef) {
            if (!IsArousable(actorRef)) {
                return false;
            }

            return actorRef->IsInFaction(GameForms::m_AttractedToFemalesFaction);
        }

    }

    namespace World {
        void ForEachReferenceInRange(RE::TESObjectREFR *origin, float radius,
                                     std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR *ref)> callback);
    }
}
#pragma once
