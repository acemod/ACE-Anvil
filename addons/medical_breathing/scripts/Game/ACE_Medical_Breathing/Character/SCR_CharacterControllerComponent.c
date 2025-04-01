//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Add handling for tongue-based occlusion
	override protected void ACE_Medical_OnUnconsciousPoseChanged()
	{
		super.ACE_Medical_OnUnconsciousPoseChanged();
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!char)
			return;
		
		RplComponent rpl = char.GetRplComponent();
		if (!rpl || rpl.IsProxy())
			return;
		
		// Only supine position can cause occlusion
		if (m_eACE_Medical_UnconsciousPose != ACE_Medical_EUnconsciousPose.BACK)
			return;
		
		// No occulusion is possible when a King LT inserted
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(char.FindComponent(ACE_EquipmentStorageComponent));
		if (storageComponent && storageComponent.IsSlotOccupied(ACE_EEquipementSlot.LARYNGEAL_TUBE))
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings)
			return;
		
		if (Math.RandomFloat01() > settings.m_RespiratorySystem.m_fAirwayObstructionChance)
			return;
		
		ACE_Medical_RespiratoryComponent respiratoryComponent = char.ACE_Medical_GetRespiratoryComponent();
		if (!respiratoryComponent)
			return;
		
		respiratoryComponent.SetAreAirwaysObstructed(true);
	}
}
