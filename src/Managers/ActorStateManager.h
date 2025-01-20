#pragma once
#include "PCH.h"
#include "Utilities/LRUCache.h"

bool IsActorNaked(RE::Actor* actorRef);

const static RE::FormID kActorTypeCreatureKeywordFormId = 0x13795;
const static RE::FormID kActorTypeAnimalKeywordFormId = 0x13798;

class ActorStateManager
{
public:

	static ActorStateManager* GetSingleton()
	{
		static ActorStateManager singleton;
		return &singleton;
	}

	ActorStateManager() :
		m_ActorNakedStateCache(std::function<bool(RE::Actor*)>(IsActorNaked), 100),
		m_CreatureKeyword((RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeCreatureKeywordFormId)),
		m_AnimalKeyword((RE::BGSKeyword*)RE::TESForm::LookupByID(kActorTypeAnimalKeywordFormId)) {}

	bool GetActorNaked(RE::Actor* actorRef);
	void ActorNakedStateChanged(RE::Actor* actorRef, bool newNaked);

	bool GetActorSpectatingNaked(RE::Actor* actorRef);
	void UpdateActorsSpectating(std::set<RE::Actor*> spectators);

	//Returns true if actor is non-creature, non-animal npc
	bool IsHumanoidActor(RE::Actor* actorRef);

	void OnActorArousalUpdated(RE::Actor* actorRef, float newArousal);

	RE::Actor* GetMostArousedActorInLocation();

private:
	void HandlePlayerArousalUpdated(RE::Actor* actorRef, float newArousal);

private:

	Utilities::LRUCache<RE::Actor*, bool> m_ActorNakedStateCache;

	std::map<RE::Actor*, float> m_NakedSpectatingMap;

	RE::BGSKeyword* m_CreatureKeyword;
	RE::BGSKeyword* m_AnimalKeyword;

	//Player Variables
	float m_PreviousPlayerArousal = 0.f;
	float m_LastNotificationTime = 0.f;
	bool m_WasPlayerDispleased = false;

	//Actor Variables
	RE::ActorHandle m_MostArousedActor;
	float m_MostArousedActorArousal = 0.f;
};
