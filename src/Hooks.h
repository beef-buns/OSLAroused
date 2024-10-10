#pragma once

#include "Utilities/Utils.h"
#include "Managers/ArousalManager.h"

namespace Hooks {
// REL_ID(38022), Character something
    class ActorUpdateHook {
    public:
        inline static void Install() {
            auto &trampoline = SKSE::GetTrampoline();

            // NOTE: This one actually works:
            auto hook = REL::Relocation<std::uintptr_t>(REL::ID(37348), 0x1AA);
//            auto hook = REL::Relocation<std::uintptr_t>(REL::ID(40447), 0xA7F);
//            auto hook = REL::Relocation<std::uintptr_t>(REL::ID(40450), 0x14A);
            ActorUpdateFunction =
                    trampoline.write_call<5>(hook.address(), &ActorUpdate);
        }

    private:
        inline static std::int32_t ActorUpdate(RE::Actor *actor) {
//    if (Utils::IsValid(actor)) {
//      UpdateManager::GetSingleton()->Update(actor);
//    }

//            if (Utilities::Actor::IsArousable(actor)) {
//                float currentGameTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
//                logger::info("Updating {}", actor->GetDisplayFullName());
            if (!RE::UI::GetSingleton()->GameIsPaused()) {
                ArousalManager::Update(actor);
            }
            return ActorUpdateFunction(actor);
        }

        inline static REL::Relocation<decltype(ActorUpdate)> ActorUpdateFunction;
    };

    class HighProcessDataSetHeadTrackTarget {
    public:
        inline static void Install() {
            auto &trampoline = SKSE::GetTrampoline();

            auto hook = REL::Relocation<std::uintptr_t>(REL::ID(38009), 0x594);
            _SetTarget = trampoline.write_call<5>(hook.address(), &SetTarget);

            auto scriptHook = REL::Relocation<std::uintptr_t>(REL::ID(22212), 0xBC);
            _SetHeadTrackTargetScript =
                    trampoline.write_call<5>(scriptHook.address(), &SetTargetScript);

            auto procedureHook = REL::Relocation<std::uintptr_t>(REL::ID(39650), 0xFFD);
            _SetHeadTrackTargetProcedure =
                    trampoline.write_call<5>(procedureHook.address(), &SetTargetProcedure);

            auto combatHook = REL::Relocation<std::uintptr_t>(REL::ID(38565), 0x193);
            _SetHeadTrackTargetCombat =
                    trampoline.write_call<5>(combatHook.address(), &SetTargetCombat);

            auto dialogueHook = REL::Relocation<std::uintptr_t>(REL::ID(37541), 0x181);
            _SetHeadTrackTargetDialogue =
                    trampoline.write_call<5>(dialogueHook.address(), &SetTargetDialogue);
        }

    private:
        inline static std::int64_t SetTarget(RE::AIProcess *a1,
                                             RE::TESObjectREFR *a2) {
            if (a1 && a2) {
                auto target = a2->As<RE::Actor>();
                if (target) {
                    logger::info("[Default] {} is looking at {}",
                                 a1->GetUserData()->GetName(), target->GetName());
                }
            }
            return _SetTarget(a1, a2);
        }

        inline static std::int64_t SetTargetProcedure(RE::AIProcess *a1,
                                                      RE::TESObjectREFR *a2) {
            if (a1 && a2) {
                auto target = a2->As<RE::Actor>();
                if (target) {
                    logger::info("[Procedure] {} is looking at {}",
                                 a1->GetUserData()->GetName(), target->GetName());
                }
            }
            return _SetHeadTrackTargetProcedure(a1, a2);
        }

        inline static std::int64_t SetTargetScript(RE::AIProcess *a1,
                                                   RE::TESObjectREFR *a2) {
            if (a1 && a2) {
                auto target = a2->As<RE::Actor>();
                if (target) {
                    logger::info("[Script] {} is looking at {}",
                                 a1->GetUserData()->GetName(), target->GetName());
                }
            }
            return _SetHeadTrackTargetScript(a1, a2);
        }

        inline static std::int64_t SetTargetCombat(RE::AIProcess *a1,
                                                   RE::TESObjectREFR *a2) {
            if (a1 && a2) {
                auto target = a2->As<RE::Actor>();
                if (target) {
                    logger::info("[Combat] {} is looking at {}",
                                 a1->GetUserData()->GetName(), target->GetName());
                }
            }
            return _SetHeadTrackTargetCombat(a1, a2);
        }

        inline static std::int64_t SetTargetDialogue(RE::AIProcess *a1,
                                                     RE::TESObjectREFR *a2) {
            if (a1 && a2) {
                auto target = a2->As<RE::Actor>();
                if (target) {
                    logger::info("[Dialogue] {} is looking at {}",
                                 a1->GetUserData()->GetName(), target->GetName());
                }
            }
            return _SetHeadTrackTargetDialogue(a1, a2);
        }

        inline static REL::Relocation<decltype(SetTarget)> _SetTarget;
        inline static REL::Relocation<decltype(SetTarget)> _SetHeadTrackTargetScript;
        inline static REL::Relocation<decltype(SetTarget)>
                _SetHeadTrackTargetProcedure;
        inline static REL::Relocation<decltype(SetTarget)> _SetHeadTrackTargetCombat;
        inline static REL::Relocation<decltype(SetTarget)>
                _SetHeadTrackTargetDialogue;
    };

    class HeadTracking {
    public:
        inline static void Install() {
            auto &trampoline = SKSE::GetTrampoline();

            auto hook = REL::Relocation<std::uintptr_t>(REL::ID(42841), 0x7);
            _ClearTarget = trampoline.write_call<5>(hook.address(), &ClearTarget);
        }

    private:
        inline static std::int64_t ClearTarget(RE::Actor *actor) {
            if (actor) {
                logger::info("{} stopped looking", actor->GetName());
            }
            return _ClearTarget(actor);
        }

        inline static REL::Relocation<decltype(ClearTarget)> _ClearTarget;
    };

    class IsOverEncumberedHook {
    public:
        inline static void Install() {
            auto &trampoline = SKSE::GetTrampoline();

            // 1406D7870
            /* auto notificationHook = REL::Relocation<std::uintptr_t>(REL::ID(40444),
            0x114); _IsOverEncumberedFunction =
            trampoline.write_call<5>(notificationHook.address(), &IsOverEncumbered);
            auto fastTravelHook = REL::Relocation<std::uintptr_t>(REL::ID(40445),
            0x114); _IsOverEncumberedFunction =
            trampoline.write_call<5>(fastTravelHook.address(), &IsOverEncumbered); */
            auto hook = REL::Relocation<std::uintptr_t>(REL::ID(37453), 0x22);
            _IsOverEncumberedFunction =
                    trampoline.write_call<5>(hook.address(), &IsOverEncumbered);
            auto canRunHook = REL::Relocation<std::uintptr_t>(REL::ID(37235), 0x40);
            _CanRunFunction_IsOnMount =
                    trampoline.write_call<5>(canRunHook.address(), &IsOverEncumbered);
        }

    private:
        inline static bool IsOverEncumbered(RE::Actor *actor) {
            auto result = _IsOverEncumberedFunction(actor);

//    if (actor && (actor->IsPlayer() || actor->IsPlayerRef()) &&
//        Utils::IsNaked(actor)) {
//      logger::info("{}: Is Over Encumbered i think {}",
//                   actor->GetDisplayFullName(), result);
//      return true;
//    }
            return result;
        }

        inline static REL::Relocation<decltype(IsOverEncumbered)>
                _IsOverEncumberedFunction;
        inline static REL::Relocation<decltype(IsOverEncumbered)>
                _CanRunFunction_IsOnMount;
    };

    inline static void Install() {
        SKSE::AllocTrampoline(120);
        ActorUpdateHook::Install();
        // HighProcessDataSetHeadTrackTarget::Install();
        // HeadTracking::Install();
    }
} // namespace Hooks
