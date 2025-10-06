//-----------------------------------------------------------------------------------------------------------
modded class ACE_Medical_CharacterChestHitZone : SCR_CharacterHitZone
{
	protected static const ref array<EDamageType> PTX_DAMAGE_TYPE_WHITELIST = {EDamageType.MELEE, EDamageType.KINETIC};
	
	//-----------------------------------------------------------------------------------------------------------
	//! Handle pneumothorax
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		super.OnDamage(damageContext);
		
		if (!Replication.IsServer() || !PTX_DAMAGE_TYPE_WHITELIST.Contains(damageContext.damageType) || damageContext.damageValue < GetCriticalHealthThreshold())
			return;
		
		/*****
		ACE_Medical_RespiratoryComponent respiratoryComponent = ownerChar.ACE_Medical_GetRespiratoryComponent();
		if (!respiratoryComponent)
			return;
		
		ACE_Medical_Breathing_Settings settings = ACE_SettingsHelperT<ACE_Medical_Breathing_Settings>.GetModSettings();
		if (!settings)
			return;
		
		respiratoryComponent.SetPneumothoraxVolume(settings.m_RespiratorySystem.m_fMaxPneumothoraxVolumeML / 4);
		*****/
	}
}
