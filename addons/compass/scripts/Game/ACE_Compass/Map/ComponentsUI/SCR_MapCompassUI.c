//------------------------------------------------------------------------------------------------
modded class SCR_MapCompassUI : SCR_MapRTWBaseUI
{		
	protected SCR_MapToolEntry m_ACE_DeclinationEntry;
	protected Widget m_wACE_DeclinationInfo;
	protected bool m_bACE_Declination_Visible;
	
	protected static const string ACE_DECLINATION_INFO_LAYOUT_NAME = "{584E24A7094D7F1A}UI/layouts/Map/ACE_MapDeclinationInfo.layout";
	protected const ResourceName ACE_DECLINATION_ICON = "{694C2E8773F36418}UI/Textures/Map/ACE_DeclinationIcon.edds";
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		super.Init();
		
		SCR_MapToolMenuUI toolMenu = SCR_MapToolMenuUI.Cast(m_MapEntity.GetMapUIComponent(SCR_MapToolMenuUI));
		if (!toolMenu)
			return;
		
		m_ACE_DeclinationEntry = toolMenu.RegisterToolMenuEntry(ACE_DECLINATION_ICON, "", 11, m_bIsExclusive);
		m_ACE_DeclinationEntry.m_OnClick.Insert(ACE_Declination_ToggleVisible);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Declination_ToggleVisible(SCR_ButtonBaseComponent button = null)
	{
		ACE_Declination_SetVisible(!m_bACE_Declination_Visible);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Declination_SetVisible(bool visible)
	{
		if (m_bACE_Declination_Visible == visible)
			return;
		
		if (visible)
			m_wACE_DeclinationInfo = GetGame().GetWorkspace().CreateWidgets(ACE_DECLINATION_INFO_LAYOUT_NAME, m_MapEntity.GetMapWidget().GetParent());
		else if (m_wACE_DeclinationInfo)
			m_wACE_DeclinationInfo.RemoveFromHierarchy();
	
		m_ACE_DeclinationEntry.SetActive(visible);
		m_bACE_Declination_Visible = visible;
	}
}
