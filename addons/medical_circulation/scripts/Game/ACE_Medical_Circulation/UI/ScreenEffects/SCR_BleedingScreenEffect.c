//------------------------------------------------------------------------------------------------
modded class SCR_BleedingScreenEffect : SCR_BaseScreenEffect
{
	[Attribute(defvalue: "0.85", uiwidget: UIWidgets.Slider, params: "0 1", desc: "Opacity for the blackout effect when in cardiac arrest")]
	protected float m_fACE_Medical_CardiacArrestBlackoutOpacity;
	
	protected ACE_Medical_VitalsComponent m_pACE_Medical_VitalsComponent;

	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		super.DisplayControlledEntityChanged(from, to);
		
		if (to)
			m_pACE_Medical_VitalsComponent = ACE_Medical_VitalsComponent.Cast(to.FindComponent(ACE_Medical_VitalsComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Make the screen completely black when in cardiac arrest
	override void BlackoutEffect(float effectStrength)
	{
		bool inCardiacArrest = (m_pACE_Medical_VitalsComponent.GetVitalStateID() & (ACE_Medical_EVitalStateID.CARDIAC_ARREST | ACE_Medical_EVitalStateID.RESUSCITATION));
		if (inCardiacArrest)
			effectStrength = m_fACE_Medical_CardiacArrestBlackoutOpacity / BLACKOUT_OPACITY_MULTIPLIER;
			
		SCR_UISoundEntity.SetSignalValueStr("ACE_Medical_CardiacArrest", inCardiacArrest);
		super.BlackoutEffect(effectStrength);
	}
}
