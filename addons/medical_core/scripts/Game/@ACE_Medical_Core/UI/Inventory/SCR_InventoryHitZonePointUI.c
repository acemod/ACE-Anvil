//------------------------------------------------------------------------------------------------
//! Add more extensible implementation of ShowVirtualHZInfo for tooltips of new global stats
modded class SCR_InventoryHitZonePointUI : ScriptedWidgetComponent
{
	[Attribute(desc: "Medical info type for tooltip", uiwidget: UIWidgets.Flags, enumType: ACE_Medical_EUIInfoType)]
	protected ACE_Medical_EUIInfoType m_eACE_Medical_TooltipInfoType;
	
	//------------------------------------------------------------------------------------------------
	//! Create tooltip
	//! Override this to add specific info to m_DamageInfo associated with type
	protected void ACE_Medical_ShowGlobalStatTooltip(Widget w, ACE_Medical_EUIInfoType type)
	{
		Widget localInfoWidget;
		if (!m_DamageInfo)
		{
			SCR_InventoryMenuUI inventoryMenuUI = GetInventoryMenuUI();
			if (!inventoryMenuUI)
						return;
			
			localInfoWidget = GetGame().GetWorkspace().CreateWidgets(DAMAGE_INFO, inventoryMenuUI.GetRootWidget());
			if (!localInfoWidget)
				return;
			
			localInfoWidget.SetVisible(true);
			localInfoWidget.SetOpacity(0);
			
			localInfoWidget.AddHandler(new SCR_InventoryDamageInfoUI());
			m_DamageInfo = SCR_InventoryDamageInfoUI.Cast(localInfoWidget.FindHandler(SCR_InventoryDamageInfoUI));
		}
		
		if (!m_DamageInfo)
			return;
		
		m_DamageInfo.SetName("");
		
		//callLater is here because the widget needs to be created before size can be determined and location can be set
		GetGame().GetCallqueue().CallLater(EnableVirtualHZInfoWidget, 300, false, localInfoWidget, w);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Show tooltip
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		
		if (m_eACE_Medical_TooltipInfoType > 0)
			ACE_Medical_ShowGlobalStatTooltip(w, m_eACE_Medical_TooltipInfoType);

		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Hide tooltip
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		
		if (m_eACE_Medical_TooltipInfoType > 0)
			HideVirtualHZInfo();
		
		return false;
	}
}
