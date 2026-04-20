[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class ACE_Medical_SpO2TooltipDetail : SCR_DpsConditionBarBaseTooltipDetail
{
	protected SCR_CharacterResilienceHitZone m_pResilienceHitZone;
	
	//------------------------------------------------------------------------------------------------
	override void UpdateDetail(SCR_EditableEntityComponent entity)
	{
		super.UpdateDetail(entity);
		
		if (m_pResilienceHitZone)
			SetBarAndPercentageValue(m_pResilienceHitZone.GetHealthScaled());
	}

	//------------------------------------------------------------------------------------------------
	/*****
	override bool InitDetail(SCR_EditableEntityComponent entity, Widget widget)
	{
		m_Text = TextWidget.Cast(widget);
		if (!m_Text)
			return false;
		
		IEntity owner = entity.GetOwner();
		if (!owner)
			false;
		
		m_pACE_Medical_VitalsComponent = ACE_Medical_VitalsComponent.Cast(owner.FindComponent(ACE_Medical_VitalsComponent));
		if (!m_pACE_Medical_VitalsComponent)
			return false;
		
		m_pNetworkComponent = ACE_Medical_NetworkComponent.Cast(GetGame().GetPlayerController().FindComponent(ACE_Medical_NetworkComponent));
		if (!m_pNetworkComponent)
			return false;
		
		ForceUpdateDetail(entity);
		return true;
	}
	*****/
}
