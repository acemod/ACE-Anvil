//------------------------------------------------------------------------------------------------
//! Apply settings to medical kit
modded class SCR_HealSupportStationComponent : SCR_BaseDamageHealSupportStationComponent
{
	[RplProp(onRplName: "ACE_Medical_OnMedicalKitMaxHealScaledChanged")]
	protected float m_fACE_Medical_MedicalKitMaxHealScaled = 0.5;
	
	//------------------------------------------------------------------------------------------------
	override protected void DelayedInit(IEntity owner)
	{
		super.DelayedInit(owner);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (!settings)
			return;
		
		SCR_ResourceComponent resource = SCR_ResourceComponent.Cast(owner.FindComponent(SCR_ResourceComponent));
		if (resource)
			resource.SetResourceTypeEnabled(settings.m_bHealSupplyUsageEnabled);
		
		// Set how much medical kits can heal
		if (InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent)))
		{
			m_fACE_Medical_MedicalKitMaxHealScaled = settings.m_fMedicalKitMaxHealScaled;
			ACE_Medical_OnMedicalKitMaxHealScaledChanged();
			Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnMedicalKitMaxHealScaledChanged()
	{
		m_fMaxHealScaled = m_fACE_Medical_MedicalKitMaxHealScaled;
	}
}
