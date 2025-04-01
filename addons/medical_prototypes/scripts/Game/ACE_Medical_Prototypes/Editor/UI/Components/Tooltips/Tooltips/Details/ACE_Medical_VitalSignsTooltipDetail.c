[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class ACE_Medical_VitalSignsTooltipDetail : SCR_EntityTooltipDetail
{
	[Attribute(defvalue: "1", desc: "Update frequency [s]")]
	protected float m_fUpdateTimeout;
	protected float m_fUpdateTimer = 0;
	
	protected TextWidget m_Text;
	protected ACE_Medical_CardiovascularComponent m_pCardiovascularComponent;
	protected ACE_Medical_NetworkComponent m_pNetworkComponent;
	
	//------------------------------------------------------------------------------------------------
	override bool NeedUpdate()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateDetail(SCR_EditableEntityComponent entity)
	{
		m_fUpdateTimer += GetGame().GetWorld().GetTimeSlice();
		
		if (m_fUpdateTimer < m_fUpdateTimeout)
			return;
		
		m_fUpdateTimer = 0;
		ForceUpdateDetail(entity);
	}
	
	//------------------------------------------------------------------------------------------------
	void ForceUpdateDetail(SCR_EditableEntityComponent entity)
	{
		m_pNetworkComponent.RequestUpdateVitalsData(SCR_ChimeraCharacter.Cast(entity.GetOwner()));
		Tuple2<float, float> pressures = m_pCardiovascularComponent.GetBloodPressures();
		m_Text.SetTextFormat("%1 bpm\n%2/%3 mmHg",
			Math.Round(m_pCardiovascularComponent.GetHeartRate()),
			Math.Round(pressures.param2 * ACE_Medical_CardiovascularComponent.KPA2MMHG),
			Math.Round(pressures.param1 * ACE_Medical_CardiovascularComponent.KPA2MMHG)
		);
	}

	//------------------------------------------------------------------------------------------------
	override bool InitDetail(SCR_EditableEntityComponent entity, Widget widget)
	{
		m_Text = TextWidget.Cast(widget);
		if (!m_Text)
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(entity.GetOwner());
		if (!ownerChar)
			return false;
		
		m_pCardiovascularComponent = ownerChar.ACE_Medical_GetCardiovascularComponent();
		if (!m_pCardiovascularComponent)
			return false;
		
		m_pNetworkComponent = ACE_Medical_NetworkComponent.Cast(GetGame().GetPlayerController().FindComponent(ACE_Medical_NetworkComponent));
		if (!m_pNetworkComponent)
			return false;
		
		ForceUpdateDetail(entity);
		return true;
	}
}
