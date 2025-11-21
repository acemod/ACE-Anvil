//------------------------------------------------------------------------------------------------
//! Apply settings to medical kit
modded class SCR_HealSupportStationComponent : SCR_BaseDamageHealSupportStationComponent
{
	[RplProp(onRplName: "ACE_Medical_OnMedicalKitSettingsChanged")]
	protected float m_fACE_Medical_HealingPerExecution = 10;
	
	//------------------------------------------------------------------------------------------------
	override protected void DelayedInit(IEntity owner)
	{
		super.DelayedInit(owner);
		
		if (!Replication.IsServer() || !owner)
			return;
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (!settings)
			return;
		
		// Set how much medical kits can heal
		if (InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent)))
		{
			m_fACE_Medical_HealingPerExecution = settings.m_fMedicalKitHealingPerExecution;
			ACE_Medical_OnMedicalKitSettingsChanged();
			Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnMedicalKitSettingsChanged()
	{
		m_iDamageHealedEachExecution = m_fACE_Medical_HealingPerExecution;
	}
}
