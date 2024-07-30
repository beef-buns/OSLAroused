ScriptName OSLAroused_AdapterSexLab Extends Quest Hidden

OSLAroused_Main Main

bool SLSODetected = false

bool function LoadAdapter()
	;Looks like Sexlab not Installed
    if (Game.GetModByName("SexLab.esm") == 255)
		return false
    endif

    Main = OSLAroused_Main.Get()

	RegisterForModEvent("HookAnimationStart", "OnAnimationStart")
	RegisterForModEvent("HookAnimationEnd", "OnAnimationEnd")
	RegisterForModEvent("HookStageStart", "OnStageStart")
	RegisterForModEvent("SexLabOrgasm", "OnSexLabOrgasm")

    SLSODetected = true
    return true
endfunction

event OnAnimationStart(int tid, bool hasPlayer)
    SexLabFramework sexlab = SexLabUtil.GetAPI()
    if(!sexlab)
        return
    endif
    SexLabThread thread = SexLab.GetThread(tid)
    ; sslThreadController controller = sexlab.GetController(tid)
    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (thread.HasTag("OStimLab"))
        return
    EndIf
    Log("OnAnimationStart")

    Actor[] actors = thread.GetPositions()
    OSLArousedNative.RegisterSceneStart(false, tid, actors)

    ;TODO: Add lewd faction check or whatever it is

    if thread.IsConsent() || Main.VictimGainsArousal
        ;OArousal mode sends a blast on scene start
        OSLAroused_ModInterface.ModifyArousalMultiple(actors, Main.SceneBeginArousalGain, "Sexlab Animation Start")
    else
        int i = actors.Length
        while(i > 0)
            Actor act = actors[i]
            if !thread.GetSubmissive(act)
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneBeginArousalGain, "SexLab Animation Start - Aggressor")
            else
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneBeginArousalGain * -1, "SexLab Animation Start - Aggressor")
            endif
        endwhile

    endif
endevent

event OnAnimationEnd(int tid, bool hasPlayer)
    OSLArousedNative.RemoveScene(false, tid)

    SexLabFramework sexlab = SexLabUtil.GetAPI()
    if(!sexlab)
        return
    endif
    ;increase arousal for actors who did not org
    ; sslThreadController controller = sexlab.GetController(tid)
    SexLabThread thread = SexLab.GetThread(tid)

    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (thread.HasTag("OStimLab"))
        return
    EndIf

    OSLArousedNative.RemoveScene(false, tid)

    Actor[] actors = thread.GetPositions()

    int i = actors.Length
    while(i > 0)
        i -= 1
        Actor act = actors[i]


        ; if((controller.ActorAlias(act) as sslActorAlias).GetOrgasmCount() > 0)
        ;     OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalOrgasmChange, "sexlab end - SLSO orgasm")
        if(thread.IsConsent())
            OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalNoOrgasmChange, "sexlab end - SLSO no orgasm")
        elseif (!thread.GetSubmissive(act) || (Main.VictimGainsArousal && thread.GetSubmissive(act)))
            OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalNoOrgasmChange, "sexlab end - SLSO no orgasm")
        endif
    endwhile
endevent

Event OnStageStart(int tid, bool HasPlayer)
    If (Main.StageChangeArousalGain == 0.0)
        return
    EndIf

    SexLabFramework sexlab = SexLabUtil.GetAPI()
    if(!sexlab)
        return
    endif

    sslThreadController controller = sexlab.GetController(tid)

    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (controller.HasTag("OStimLab"))
        return
    EndIf

    Actor[] actors = controller.Positions
    if(actors.length < 1)
        return
    endif

    if(Main.VictimGainsArousal || controller.Victims.Length == 0)
        OSLAroused_ModInterface.ModifyArousalMultiple(actors, Main.StageChangeArousalGain, "sexlab scene change")
    else
        int i = actors.Length
        while(i > 0)
            i -= 1
            If (!controller.IsVictim(actors[i]))
                OSLAroused_ModInterface.ModifyArousal(actors[i], Main.StageChangeArousalGain, "sexlab scene change")
            EndIf
        endwhile
    endif
endevent

Event OnSexLabOrgasm(Form actorForm, int enjoyment, int orgasmCount)
    Actor act = actorForm as Actor
    SexLabFramework sexlab = SexLabUtil.GetAPI()
    if(!act || !sexlab)
        return
    endif

    SexLabThread thread = SexLab.GetThreadByActor(act)
    ; sslThreadController controller = sexlab.GetActorController(act)
    if(!thread)
        return
    endif

    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (thread.HasTag("OStimLab"))
        return
    EndIf

    Log("OnSexLabOrgasm: " + actorForm + " enjoyment: " + enjoyment)
    OSLArousedNative.RegisterActorOrgasm(act)

    if (!thread.IsConsent() && thread.GetSubmissive(act))
        Log("OnSexLabOrgasm(Victim Orgasm): " + actorForm + " enjoyment: " + enjoyment)
        Log("WIP: Decide what to do here")
    endif

    ;Update arousal for any victims
    ;@TODO: Tie this into a lewdness system
    ; if(controller.Victims.Length > 0)
    ;     OSLAroused_ModInterface.ModifyArousalMultiple(controller.Victims, -20, "being sexlab victim")
    ; endif

    ;Lower arousal on orgasm
    ;@TODO: Improve this function
    int exposureMod = Main.OrgasmArousalChange as int
    ;@TODO: Check for belt
    OSLAroused_ModInterface.ModifyArousal(act, exposureMod, "sexlab orgasm")
EndEvent

; ========== SCENE RELATED ==================
function StartMasturbationScene(Actor target)
    sslBaseAnimation[] animations
    Actor[] actors = new Actor[1]
    actors[0] = target

    SexLabFramework SexLab = SexLabUtil.GetAPI()
    String tags = "Masturbation,"
    If 0 == target.GetLeveledActorBase().GetSex()
        tags = tags + "M"
    Else
        tags = tags + "F"
    EndIf

    SexLabThread thread = SexLab.StartScene(actors, tags)
    If thread == none
        Debug.Notification("SexLab animation failed to start [masturbation]")
    EndIf
endfunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---Beef Buns' OSLAroused--- [SexLabAdapter] " + msg)
    ConsoleUtil.PrintMessage("---Beef Buns' OSLAroused--- [SexLabAdapter] " + msg)
endfunction
