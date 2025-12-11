//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Add handling for tongue-based obstruction
	override protected void ACE_Medical_OnUnconsciousPoseChanged()
	{
		super.ACE_Medical_OnUnconsciousPoseChanged();
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!char)
			return;
		
		RplComponent rpl = char.GetRplComponent();
		if (!rpl || rpl.IsProxy())
			return;
		
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		if (vitals)
			vitals.SetIsAirwayObstructed(ACE_Medical_ShouldAirwayBeObstructed(char));
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (damageManager)
			damageManager.ACE_Medical_UpdateResilienceRegenScale();
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ACE_Medical_ShouldAirwayBeObstructed(SCR_ChimeraCharacter char)
	{
		// Only supine position can cause obstruction
		if (m_eACE_Medical_UnconsciousPose != ACE_Medical_EUnconsciousPose.BACK)
			return false;
		
		// No obstruction is possible when a King LT inserted
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(char.FindComponent(ACE_EquipmentStorageComponent));
		if (storageComponent && storageComponent.IsSlotOccupied(ACE_EEquipementSlot.LARYNGEAL_TUBE))
			return false;
		
		ACE_Medical_Breathing_Settings settings = ACE_SettingsHelperT<ACE_Medical_Breathing_Settings>.GetModSettings();
		if (!settings)
			return false;
		
		return (Math.RandomFloat(0, 1) < settings.m_fAirwayObstructionChance);
	}
}
