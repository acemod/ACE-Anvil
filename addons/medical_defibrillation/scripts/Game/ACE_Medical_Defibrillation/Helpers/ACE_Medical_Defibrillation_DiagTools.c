#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_DiagTools
{
    //------------------------------------------------------------------------------------------------
    static bool GetDiagTargetDefib(out IEntity target, out ACE_Medical_Defibrillation_DefibComponent defibComponent)
    {
        CameraManager cameraManager = GetGame().GetCameraManager();
        if (!cameraManager)
            return false;
        
        CameraBase camera = cameraManager.CurrentCamera();
        if (!camera)
            return false;
        
        target = IEntity.Cast(camera.GetCursorTarget());
        if (!target)
            return false;
        
        defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(target.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
        if (!defibComponent)
            return false;
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    static bool GetDiagNearestDefib(out IEntity target, out ACE_Medical_Defibrillation_DefibComponent defibComponent, float distance = 3)
    {
        IEntity player = GetGame().GetPlayerController().GetControlledEntity();
        if (!player)
            return false;
        
        vector playerPos = player.GetOrigin();
        
        ACE_Medical_Defibrillation_QueryNearestDefib query = new ACE_Medical_Defibrillation_QueryNearestDefib(distance);
        target = query.GetEntity(playerPos);
        if (!target)
            return false;
        
        defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(target.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
        if (!defibComponent)
            return false;
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    static ACE_Medical_VitalsComponent GetVitalsFromTarget(IEntity target)
    {
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(target);
        if (!character)
            return null;
        
        return ACE_Medical_VitalsComponent.Cast(character.FindComponent(ACE_Medical_VitalsComponent));
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetShockSuccessChance(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return ACE_Medical_Defibrillation_DefibComponent.CalculateShockSuccessChance(vitals);
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetBaseShockChance(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return ACE_Medical_Defibrillation_DecayCalculator.CalculateShockChance(vitals);
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetShockSpamPenalty(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return ACE_Medical_Defibrillation_DecayCalculator.CalculateSpamPenalty(vitals);
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetTimeSinceLastShock(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return vitals.GetTimeSinceLastShock() / 1000.0;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetTimeSinceArrestStart(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return vitals.GetTimeSinceArrestStart() / 1000.0;
    }
    
    //------------------------------------------------------------------------------------------------
    static string GetFormattedTimeSinceArrest(IEntity target)
    {
        float seconds = GetTimeSinceArrestStart(target);
        if (seconds < 0)
            return "N/A";
        
        int minutes = Math.Floor(seconds / 60);
        int remainingSeconds = Math.Floor(seconds - (minutes * 60));
        return string.Format("%1m %2s", minutes, remainingSeconds);
    }
    
    //------------------------------------------------------------------------------------------------
    static string GetFormattedTimeSinceLastShock(IEntity target)
    {
        float seconds = GetTimeSinceLastShock(target);
        if (seconds < 0)
            return "N/A";
        
        if (seconds >= 60)
        {
            int minutes = Math.Floor(seconds / 60);
            int remainingSeconds = Math.Floor(seconds - (minutes * 60));
            return string.Format("%1m %2s", minutes, remainingSeconds);
        }
        
        return string.Format("%1 seconds", seconds);
    }
    
    //------------------------------------------------------------------------------------------------
    static int GetShocksDelivered(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return vitals.GetShocksDelivered();
    }
    
    //------------------------------------------------------------------------------------------------
    static int GetTimesArrested(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return vitals.GetTimesArrested();
    }
    
    //------------------------------------------------------------------------------------------------
    static string GetCardiacRhythm(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return "Unknown";
        
        return SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_ECardiacRhythm, vitals.GetCardiacRhythm());
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetReviveBonus(IEntity target)
    {
        ACE_Medical_VitalsComponent vitals = GetVitalsFromTarget(target);
        if (!vitals)
            return -1;
        
        return ACE_Medical_Defibrillation_DecayCalculator.CalculateReviveBonus(vitals);
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetReviveChance(IEntity target)
    {
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(target);
        if (!character)
            return -1;
        
        ACE_Medical_CharacterContext context = new ACE_Medical_CharacterContext(character);
        ACE_Medical_ReviveTransition transition = new ACE_Medical_ReviveTransition(ACE_Medical_EVitalStateID.ANY, ACE_Medical_EVitalStateID.ANY);
        
        return transition.ComputeReviveChance(context);
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetMaxTotalReviveBonus()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fMaxTotalReviveBonus;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetBaseReviveBonus()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fBaseReviveBonus;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetBaseShockSuccessChance()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fBaseShockSuccessChance;
    }
    
    //------------------------------------------------------------------------------------------------
    static string GetShockDecayFormula()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return "Unknown";
        
        return SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_EDefibSettingDecayType, settings.m_eShockDecayFormula);
    }
    
    //------------------------------------------------------------------------------------------------
    static string GetReviveBonusDecayFormula()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return "Unknown";
        
        return SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_EDefibSettingDecayType, settings.m_eReviveBonusDecayFormula);
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetShockDecayRate()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fShockSuccessDecayRate;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetReviveBonusDecayRate()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fReviveBonusDecayRate;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetSpamTimeout()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fPunishSpamShocksTimeout;
    }
    
    //------------------------------------------------------------------------------------------------
    static bool IsPunishSpamEnabled()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return false;
        
        return settings.m_bPunishSpamShocks;
    }
    
    //------------------------------------------------------------------------------------------------
    static string GetActivePreset()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return "Unknown";
        
        return SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_EDefibSettingPreset, settings.m_eDefibSettingPreset);
    }
    
    //------------------------------------------------------------------------------------------------
    static bool IsDefibEnabled()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return false;
        
        return settings.m_bEnabled;
    }
    
    //------------------------------------------------------------------------------------------------
    static bool IsDefibMandatory()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return false;
        
        return settings.m_bDefibIsMandatory;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetAEDAnalysisDuration()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fAED_AnalysisDuration;
    }
    
    //------------------------------------------------------------------------------------------------
    static float GetAEDCPRCooldownDuration()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return -1;
        
        return settings.m_fAED_CPRCooldownDuration;
    }
    
    //------------------------------------------------------------------------------------------------
    static bool IsAEDPlayCPRPacingBeats()
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        if (!settings)
            return false;
        
        return settings.m_bAED_PlayCPRPacingBeats;
    }
    
    //------------------------------------------------------------------------------------------------
    static void DrawProgressBar(float value, float maxValue, int width, string color)
    {
        int filled = Math.Round((value / maxValue) * width);
        string bar = "";
        for (int i = 0; i < width; i++)
        {
            if (i < filled)
                bar += "█";
            else
                bar += "░";
        }
        int percent = Math.Round(value * 100);
        DbgUI.Text(string.Format("<color=%1>%2</color> %3", color, bar, percent));
    }
}
#endif