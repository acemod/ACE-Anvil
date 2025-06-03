//-----------------------------------------------------------------------------------------------------------
class ACE_Medical_CharacterAbdomenHitZone : SCR_CharacterHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Apply second chance setting
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
			m_fACE_Medical_SecondChance = settings.m_fSecondChanceOnAbdomen;
	}
}
