scriptname OSLArousedNative hidden

;Retrieves arousal value for a given Actor
;In SLAroused mode, This is a calculated value based off exposure and time rate
;NOTE: THIS FUNCTION HAS SIDE EFFECTS. Will internally update arousal based off time. Use GetArousalNoSideEffects if you want a pure read.
float function GetArousal(Actor actor) global native
;Retrieves arousal for passed in actors. Results in *same* order as passed in array
float[] function GetArousalMultiple(Actor[] actorArray) global native

;Retrieves arousal value for a given Actor. Performs no side effects and will not update internal state.
;Usually you will want to use GetArousal, this is for special cases
float function GetArousalNoSideEffects(Actor actor) global native

;Sets the arousal value for a given actor
;In SLAroused Mode this is the Exposure Value (Arousal is calculated)
float function SetArousal(Actor actor, float value) global native
function SetArousalMultiple(Actor[] actorArray, float value) global native

float function GetExposure(Actor actor) global native
float function SetExposure(Actor actor, float newVal) global native

float function GetTimeRate(Actor actor) global native

;Modifies the arousal value by the specified amount (Relative to current arousal value)
float function ModifyArousal(Actor actor, float value) global native

;Modifies all actors in array by sepcified amount (relative to current arousal)
function ModifyArousalMultiple(Actor[] actorArray, float value) global native

;Retrieves Arousal Baseline
float function GetArousalBaseline(Actor actor) global native
float function SetArousalBaseline(Actor actor, float newVal) global native
float function ModifyArousalBaseline(Actor actor, float value) global native
function ModifyArousalBaselineMultiple(Actor[] actorArray, float value) global native

;Retrieves Actor Base Libido
float function GetLibido(Actor actor) global native
;Sets Actor Base Libido
float function SetLibido(Actor actor, float newVal) global native

;Modifies the arousal multiplier by the specified amount
float function ModifyArousalMultiplier(Actor actor, float value) global native

;Sets the arousal multiplier for a given actor
float function SetArousalMultiplier(Actor actor, float value) global native

;Gets the actors current arousal multiplier
float function GetArousalMultiplier(Actor actor) global native


;Gets the number of days since this actor last orgasmed
float function GetDaysSinceLastOrgasm(Actor actor) global native

; =================== ACTOR STATE =======================

;Returns if Actor is Naked
bool function IsActorNaked(Actor actor) global native

;Called on scene start to register new scene in native
function RegisterSceneStart(bool bIsOStim, int sceneId, Actor[] participants) global native

;Called when scene ends to flag removal
function RemoveScene(bool bIsOstim, int sceneId) global native

;Called when an actor from a scene orgasms
function RegisterActorOrgasm(Actor actorRef) global native

; ==================== SETTINGS =========================

;Sets weather PlayerNudityCheck should be enabled or disabled in SKSE module
function UpdatePlayerNudityCheck(bool enabled) global native

;Sets Amount to increase arousal per game hour when exposed to nudity
function UpdateHourlyNudityArousalModifier(float arousalMod) global native

;Sets Which arousal mode to use
function UpdateDefaultArousalMultiplier(float newMultiplier) global native

;Sets Amount to increase arousal per game hour for players in scene
function UpdateHourlySceneParticipantArousalModifier(float arousalMod) global native

;Sets Amount to increase arousal per game hour for spectators of a scene
function UpdateHourlySceneViewerArousalModifier(float arousalMod) global native

; ==================== KEYWORDS =========================

bool function AddKeywordToForm(Form form, Keyword keyword) global native
bool function RemoveKeywordFromForm(Form form, Keyword keyword) global native

bool function FormHasKeywordString(form form, string kwd) global native
string[] function GetRegisteredKeywords() global native
bool function RegisterNewKeyword(string kwd) global native

; ==================== UTILITIES =========================

float function GenerateRandomFloat(float min, float max) global native
float function ClampFloat(float val, float min, float max) global native

;====== DEBUG ============

function DumpArousalData() global native
function ClearSecondaryArousalData() global native
function ClearAllArousalData() global native

;======= Explainer =========
bool function IsNaked(Actor actor) global native
bool function IsViewingNaked(Actor actor) global native
bool function IsInScene(Actor actor) global native
bool function IsViewingScene(Actor actor) global native
bool function IsWearingEroticArmor(Actor actor) global native
float function WornDeviceBaselineGain(Actor actor) global native