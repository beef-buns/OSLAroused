ScriptName OSLAroused_Main Extends Quest Hidden
; This Script is Based off OAroused by Sairion350
; All the good things are from Sairion and all the bad things are by me :) 

OSLAroused_Main Function Get() Global
	return Game.GetFormFromFile(0x806, "OSLAroused.esp") as OSLAroused_Main
EndFunction

Actor Property PlayerRef Auto 

slaFrameworkScr SlaFrameworkStub

float Property ScanDistance = 5120.0 AutoReadOnly

OSLAroused_ArousalBar Property ArousalBar Auto
OSLAroused_Conditions Property ConditionVars Auto 

bool property OStimAdapterLoaded = false Auto Hidden
OSLAroused_AdapterOStim Property OStimAdapter Auto

bool property SexLabAdapterLoaded = false Auto Hidden
OSLAroused_AdapterSexLab Property SexLabAdapter Auto

OSLAroused_AdapterDebug Property DebugAdapter Auto

; ============ SETTINGS ============
int CheckArousalKey = 157
bool EnableNudityIncreasesArousal = true
float HourlyNudityArousalModifier = 20.0
bool Property EnableArousalStatBuffs = true Auto
float DefaultArousalMultiplier = 1.0

int property kArousalMode_SLAroused = 0 autoreadonly
int property kArousalMode_OAroused = 1 autoreadonly
int SelectedArousalMode = 1


; ============ SPELLS =============
;OAroused Spells
Spell Property SLADesireSpell Auto

;SL Aroused Spells
Spell Property OArousedHornySpell Auto
Spell Property OArousedRelievedSpell Auto

; ============== CORE LIFECYCLE ===================

Event OnInit()
	;Initialize multiplier to 2 for player
	OSLArousedNative.SetArousalMultiplier(PlayerRef, DefaultArousalMultiplier)
	
	OnGameLoaded()

	Log("OSLAroused installed")
	Debug.Notification("OSLAroused installed")
EndEvent

Function OnGameLoaded()
	RegisterForModEvent("OSLA_ActorArousalUpdated", "OnActorArousalUpdated")
	RegisterForModEvent("OSLA_ActorNakedUpdated", "OnActorNakedUpdated")
	
	SlaFrameworkStub = Game.GetFormFromFile(0x4290F, "SexLabAroused.esm") as slaFrameworkScr
	if(SlaFrameworkStub)
		SlaFrameworkStub.OnGameLoaded()
	endif

	OStimAdapterLoaded = OStimAdapter.LoadAdapter()
	Log("OStim Integration Status: " + OStimAdapterLoaded)

	SexLabAdapterLoaded = SexLabAdapter.LoadAdapter()
	Log("SexLab IntegrationStatus: " + SexLabAdapterLoaded)

	DebugAdapter.LoadAdapter()

	RegisterForKey(CheckArousalKey)

	; Bootstrap settings
	; Need to notify skse dll whether to check for player nudity
	OSLArousedNative.UpdatePlayerNudityCheck(EnableNudityIncreasesArousal)
	OSLArousedNative.UpdateHourlyNudityArousalModifier(HourlyNudityArousalModifier)
	; This updates Abilities and Sends mode to native
	SetCurrentArousalMode(SelectedArousalMode)	
	
	float arousal = OSLArousedNative.GetArousal(PlayerRef)
	ArousalBar.InitializeBar(arousal / 100)

	;Initial Player Naked State
	if(OSLArousedNative.IsActorNaked(PlayerRef))
		OnActorNakedUpdated("", "", 1, PlayerRef)
	else
		OnActorNakedUpdated("", "", 0, PlayerRef)
	endif
EndFunction

;@TODO: This causes Error: Incorrect number of arguments passed. Expected 1, got 4. to throw in papyrus log.
;Works as expected need to debug through papyrus
event OnActorArousalUpdated(string eventName, string strArg, float newArousal, Form sender)
	Actor act = sender as Actor

	if(act == PlayerRef)
		ArousalBar.SetPercent(newArousal / 100.0)

		ConditionVars.OSLAroused_PlayerArousal = newArousal
		ConditionVars.OSLAroused_PlayerTimeRate = OSLArousedNative.GetTimeRate(PlayerRef)

		if EnableArousalStatBuffs
			; We check for OArousedMode so we can bypass an arousal fetch and directly use updated val
			if(SelectedArousalMode == kArousalMode_OAroused)
				ApplyOArousedEffects(newArousal as int)
			else
				ApplyArousedEffects()
			endif
		else  
			RemoveAllArousalSpells()
		endif

		DebugAdapter.OnPlayerArousalUpdated(newArousal)
	endif

	if(SlaFrameworkStub)
		SlaFrameworkStub.OnActorArousalUpdated(act, newArousal)
	endif
endevent

event OnActorNakedUpdated(string eventName, string strArg, float actorNakedFloat, Form sender)
	bool isActorNaked = actorNakedFloat > 0
	Actor act = sender as Actor
	
	if(SlaFrameworkStub && act)
		SlaFrameworkStub.OnActorNakedUpdated(act, isActorNaked)
	endif
endevent


; ========== AROUSAL EFFECTS ===========
function SetArousalEffectsEnabled(bool enabled)
	EnableArousalStatBuffs = enabled
	if EnableArousalStatBuffs
		ApplyArousedEffects()
	else  
		RemoveAllArousalSpells()
	endif
endfunction

Function ApplyArousedEffects()
	if(SelectedArousalMode == kArousalMode_SLAroused)
		PlayerRef.RemoveSpell(SLADesireSpell)
		PlayerRef.AddSpell(SLADesireSpell, false)
	elseif(SelectedArousalMode == kArousalMode_OAroused)
		;Use no side effects variant since this method can get called within the arousal updated callback loop
		int arousal = OSLArousedNative.GetArousalNoSideEffects(PlayerRef) as int
		ApplyOArousedEffects(arousal)
	endif
EndFunction

Function ApplyOArousedEffects(int arousal)
	if arousal >= 40
		arousal -= 40
		float percent = arousal / 60.0
		ApplyHornySpell((percent * 25) as int)
	elseif arousal <= 10
		ApplyReliefSpell(10)
	else 
		RemoveAllArousalSpells()
	endif 
endfunction

Function ApplyHornySpell(int magnitude)
	OArousedHornySpell.SetNthEffectMagnitude(0, magnitude)
	OArousedHornySpell.SetNthEffectMagnitude(1, magnitude)

	playerref.RemoveSpell(OArousedRelievedSpell)
	playerref.RemoveSpell(OArousedHornySpell)
	playerref.AddSpell(OArousedHornySpell, false)
EndFunction

Function ApplyReliefSpell(int magnitude)
	OArousedRelievedSpell.SetNthEffectMagnitude(0, magnitude)
	OArousedRelievedSpell.SetNthEffectMagnitude(1, magnitude)

	playerref.RemoveSpell(OArousedHornySpell)
	playerref.RemoveSpell(OArousedRelievedSpell)
	playerref.AddSpell(OArousedRelievedSpell, false)
EndFunction

Function RemoveAllArousalSpells()
	playerref.RemoveSpell(SLADesireSpell)
	playerref.RemoveSpell(OArousedHornySpell)
	playerref.RemoveSpell(OArousedRelievedSpell)
EndFunction

Event OnKeyDown(int keyCode)
	if Utility.IsInMenuMode()
		return 
	endif
	if keyCode == CheckArousalKey
		ArousalBar.DisplayBarWithAutohide(10.0)
	endif 	
EndEvent


; ========= SETTINGS UPDATE =================
int function GetShowArousalKeybind()
	return CheckArousalKey
endfunction

float function GetDefaultArousalMultiplier()
	return DefaultArousalMultiplier
endfunction

bool function GetEnableNudityIncreasesArousal()
	return EnableNudityIncreasesArousal
endfunction

float function GetHourlyNudityArousalModifier()
	return HourlyNudityArousalModifier
endfunction

int function GetCurrentArousalMode()
	return SelectedArousalMode
endfunction

function SetCurrentArousalMode(int newMode)
	;log("Setting Current Arousal Mode to: " + newMode)
	if(newMode < 0 || newMode > 1)
		return
	endif
	SelectedArousalMode = newMode
	OSLArousedNative.UpdateArousalMode(newMode)
	;Update arousal spells
	RemoveAllArousalSpells()
	if(EnableArousalStatBuffs)
		ApplyArousedEffects()
	endif
endfunction

function SetDefaultArousalMultiplier(float newVal)
	if(newVal < 0 || newVal > 10)
		return
	endif
	DefaultArousalMultiplier = newVal
	OSLArousedNative.UpdateDefaultArousalMultiplier(newVal)
endfunction

function SetPlayerNudityIncreasesArousal(bool newVal)
	EnableNudityIncreasesArousal = newVal
	OSLArousedNative.UpdatePlayerNudityCheck(newVal)
endfunction

function SetHourlyNudityArousalModifier(float newVal)
	HourlyNudityArousalModifier = newVal
	log("Update Hourly: " + newVal)
	OSLArousedNative.UpdateHourlyNudityArousalModifier(newVal)
endfunction

function SetShowArousalKeybind(int newKey)
	UnregisterForKey(CheckArousalKey)
	CheckArousalKey = newKey
	RegisterForKey(newKey)
endfunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- " + msg)
endfunction
