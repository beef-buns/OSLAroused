#pragma once

const static RE::FormID DominantFactionFormId = 0x8F88A;
const static RE::FormID SubmissiveFactionFormId = 0x8F88B;
const static RE::FormID sla_ArousalFormID = RE::FormID(0x3FC36);
const static RE::FormID sla_ExposureFormID = RE::FormID(0x25837);
const static RE::FormID sla_ExhibitionistFormID = RE::FormID(0x713DA);
const static RE::FormID sla_TimeRateFormID = 0x7C025;
const static RE::FormID sla_ExposureRateFormID = RE::FormID(0x7649B);
const static RE::FormID AttractedToMalesFactionFormId = 0x8F8A2;
const static RE::FormID AttractedToFemalesFactionFormId = 0x8F8A3;
const static RE::FormID sla_NakedFormID = 0x77F87;


namespace GameForms {
#define LOOKUPMOD(form, formid, mod)                                                                               \
    form = RE::TESDataHandler::GetSingleton()->LookupForm<std::remove_pointer<decltype(form)>::type>(formid, mod); \
    if (!form) {                                                                                                   \
        logger::critical("Failed to lookup form {} in {}", #form, mod);                                            \
        return false;                                                                                              \
    }

#define LOOKUPGAME(form, formid)                                                       \
    form = RE::TESForm::LookupByID<std::remove_pointer<decltype(form)>::type>(formid); \
    if (!form) {                                                                       \
        logger::critical("Failed to lookup form {}", #form);                           \
        return false;                                                                  \
    }

    // Vanilla Forms
    inline RE::BGSKeyword *Vampire;
    inline RE::BGSKeyword *ActorTypeNPC;
    inline RE::BGSKeyword *m_CreatureKeyword;
    inline RE::BGSKeyword *m_AnimalKeyword;


    // SexLab Forms
    inline const RE::TESFaction *GenderFaction;
    inline const RE::TESFaction *AnimatingFaction;
    inline const RE::TESFaction *ForbiddenFaction;

    inline RE::TESFaction *m_DominantFaction;
    inline RE::TESFaction *m_SubmissiveFaction;
    inline RE::TESFaction *m_AttractedToMalesFaction;
    inline RE::TESFaction *m_AttractedToFemalesFaction;

    inline RE::TESFaction *sla_Naked;
    inline RE::TESFaction *sla_Exhibitionist;
    inline RE::TESFaction *sla_Exposure;
    inline RE::TESFaction *sla_ExposureRate;
    inline RE::TESFaction *sla_Arousal;
    inline RE::TESFaction *sla_TimeRate;

//    ;SLAX_AttractedToFutasFaction
//    ;SLAX_SadistFaction
//    ;SLAX_PainHatedFaction
//    ;SLAX_PainDislikedFaction
//    ;SLAX_PainIndifferentFaction
//    ;SLAX_PainLikedFaction
//    ;SLAX_PainAddictFaction
//    ;SLAX_PainFearfulFaction
//    SexLabAroused.esm|0x08F8A4,SexLabAroused.esm|0x08F8A5,SexLabAroused.esm|0x08F8A6,SexLabAroused.esm|0x08F8A7,SexLabAroused.esm|0x08F8A8,SexLabAroused.esm|0x08F8A9,SexLabAroused.esm|0x08F8AA,SexLabAroused.esm|0x08F8AB


    inline bool LoadData() {
        LOOKUPGAME(Vampire, 0xA82BB)
        LOOKUPGAME(ActorTypeNPC, 0x13794)

        LOOKUPMOD(GenderFaction, 0x043A43, "SexLab.esm")
        LOOKUPMOD(AnimatingFaction, 0xE50F, "SexLab.esm")
        LOOKUPMOD(ForbiddenFaction, 0x049068, "SexLab.esm")

//        m_CreatureKeyword((RE::BGSKeyword *) RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId))
//        m_AnimalKeyword((RE::BGSKeyword *) RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId))

        LOOKUPMOD(m_DominantFaction, DominantFactionFormId, "SexLabAroused.esm")
        LOOKUPMOD(m_SubmissiveFaction, SubmissiveFactionFormId, "SexLabAroused.esm")
        LOOKUPMOD(sla_Naked, sla_NakedFormID, "SexLabAroused.esm")
        LOOKUPMOD(sla_Exhibitionist, sla_ExhibitionistFormID, "SexLabAroused.esm")
        LOOKUPMOD(sla_Arousal, sla_ArousalFormID, "SexLabAroused.esm")
        LOOKUPMOD(sla_TimeRate, sla_TimeRateFormID, "SexLabAroused.esm")
        LOOKUPMOD(sla_Exposure, sla_ExposureFormID, "SexLabAroused.esm")
        LOOKUPMOD(sla_ExposureRate, sla_ExposureRateFormID, "SexLabAroused.esm")
        LOOKUPMOD(m_AttractedToMalesFaction, AttractedToMalesFactionFormId, "SexLabAroused.esm")
        LOOKUPMOD(m_AttractedToFemalesFaction, AttractedToFemalesFactionFormId, "SexLabAroused.esm")

        return true;
    }
}
