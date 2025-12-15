//------------------------------------------------------------------------------------------------
modded class SCR_InventoryMenuUI : ChimeraMenuBase
{
	protected SCR_InventoryHitZonePointUI m_ACE_Medical_PneumothoraxHandler;

	//------------------------------------------------------------------------------------------------
	//! Initialize pneumothorax widget and attach event handler
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		if (!m_widget || !m_Player)
			return;
		
		Widget pneumothorax = m_widget.FindAnyWidget("ACE_Medical_PneumothoraxInfo");
		if (pneumothorax)
			m_ACE_Medical_PneumothoraxHandler = SCR_InventoryHitZonePointUI.Cast(pneumothorax.FindHandler(SCR_InventoryHitZonePointUI));
		
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_Player.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return;
		
		vitals.GetOnPneumothoraxStateChanged().Insert(ACE_Medical_OnPneumothoraxStateChange);
		ACE_Medical_OnPneumothoraxStateChange(vitals.GetPneumothoraxScale(), vitals.HasTensionPneumothorax());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Detach event handler
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		if (!m_Player)
			return;
		
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_Player.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return;
		
		vitals.GetOnPneumothoraxStateChanged().Remove(ACE_Medical_OnPneumothoraxStateChange);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnPneumothoraxStateChange(float scale, bool hasTension)
	{
		if (m_ACE_Medical_PneumothoraxHandler)
			m_ACE_Medical_PneumothoraxHandler.GetRootWidget().SetVisible(scale > 0 || hasTension);
	}
}
