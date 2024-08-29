//------------------------------------------------------------------------------------------------
modded class SCR_InspectCasualtyUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Make action available when pain hit zone is damaged or vitals are lowered
	override bool CanBeShownScript(IEntity user)
	{
		if (super.CanBeShownScript(user))
			return true;
		
		ChimeraCharacter ownerChar = ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(ownerChar.GetDamageManager());
		if (!damageManager)
			return false;
		
		ACE_Medical_PainHitZone painHZ = damageManager.ACE_Medical_GetPainHitZone();
		if (painHZ && painHZ.GetDamageState() != EDamageState.UNDAMAGED)
			return true;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(ownerChar.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent && cardiovascularComponent.GetVitalState() > ACE_Medical_EVitalState.STABLE)
			return true;
		
		return false;
	}
}
