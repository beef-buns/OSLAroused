
scriptname OSLAroused_ModInterface hidden

float function GetArousal(Actor target) global
    float arousal = OSLArousedNative.GetArousal(target)
    Log("GetArousal: " + target.GetDisplayName() + " val: " + arousal)
    return arousal
endfunction

function ModifyArousal(Actor target, float value) global
    Log("ModifyArousal: " + target.GetDisplayName() + "modified by val: " + value)
    OSLArousedNative.ModifyArousal(target, value)
endfunction

function ModifyArousalMultiple(Actor[] actorArray, float value) global
    Log("ModifyArousalMultiple: " + actorArray.Length + " actors modified by val: " + value)
    OSLArousedNative.ModifyArousalMultiple(actorArray, value)
endfunction

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [ModInterface] " + msg)
endfunction