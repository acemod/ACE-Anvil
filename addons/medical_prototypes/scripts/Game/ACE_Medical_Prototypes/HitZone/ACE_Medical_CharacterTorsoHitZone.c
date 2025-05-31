//-----------------------------------------------------------------------------------------------------------
//! Apply second chance setting
class ACE_Medical_CharacterTorsoHitZone : SCR_CharacterHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings)
			return;
		
		m_bACE_Medical_SecondChanceEnabled = settings.m_bSecondChanceOnTorsoEnabled;
	}
}
