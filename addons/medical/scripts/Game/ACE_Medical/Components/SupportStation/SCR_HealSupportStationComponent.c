//------------------------------------------------------------------------------------------------
modded class SCR_HealSupportStationComponent : SCR_BaseDamageHealSupportStationComponent
{
	//------------------------------------------------------------------------------------------------
	//! Apply settings
	override protected void DelayedInit(IEntity owner)
	{
		super.DelayedInit(owner);
		
		ACE_Medical_Settings settings =	ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings)
			return;
		
		SCR_ResourceComponent resource = SCR_ResourceComponent.Cast(GetOwner().FindComponent(SCR_ResourceComponent));
		if (!resource)
			return;
		
		resource.SetResourceTypeEnabled(settings.m_bHealSupplyUsageEnabled);
		
		// Set how much medical kits can heal
		if (InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent)) && m_fMaxHealScaled != settings.m_fMedicalKitMaxHealScaled)
		{
			m_fMaxHealScaled = settings.m_fMedicalKitMaxHealScaled;
			Rpc(RpcDo_ACE_Medical_SetMaxHealScaledBroadcast, settings.m_fMedicalKitMaxHealScaled);
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_ACE_Medical_SetMaxHealScaledBroadcast(float maxHealScaled)
	{
		m_fMaxHealScaled = maxHealScaled;
	}
}
