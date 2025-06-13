//-----------------------------------------------------------------------------------------------------------
class ACE_Medical_HeartHitZone : ACE_Medical_OrganHitZone
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
			m_fACE_Medical_SecondChance = settings.m_fSecondChanceOnHeart;
	}
}
