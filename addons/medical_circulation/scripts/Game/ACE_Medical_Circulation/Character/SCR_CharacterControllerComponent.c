//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! Unset "m_bWasRevived" when waking up
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);
		
		if (!Replication.IsServer() || (previousLifeState != ECharacterLifeState.INCAPACITATED) || (newLifeState != ECharacterLifeState.ALIVE))
			return;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return;
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(ownerChar.FindComponent(ACE_Medical_VitalsComponent));
		if (vitalsComponent)
			vitalsComponent.ClearReviveHistory();
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(ownerChar.GetDamageManager());
		if (damageManager)
			damageManager.ACE_Medical_UpdateResilienceRegenScale();
	}
}
