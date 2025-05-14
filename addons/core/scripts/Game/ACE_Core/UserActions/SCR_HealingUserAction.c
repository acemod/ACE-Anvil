//------------------------------------------------------------------------------------------------
modded class SCR_HealingUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Return the health code color based on bleeding rate
	SCR_ENearbyInteractionContextColors ACE_GetHealthStatus()
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return SCR_ENearbyInteractionContextColors.DEFAULT;
		
		SCR_CharacterDamageManagerComponent ownerDamageManager = SCR_CharacterDamageManagerComponent.Cast(ownerChar.GetDamageManager());
		if (!ownerDamageManager)
			return SCR_ENearbyInteractionContextColors.DEFAULT;
		
		return ownerDamageManager.ACE_GetGroupBleedingStatus(m_eHitZoneGroup);
	}
}
