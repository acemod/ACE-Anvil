//------------------------------------------------------------------------------------------------
modded class SCR_BleedingScreenEffect : SCR_BaseScreenEffect
{
	[Attribute(defvalue: "0.85", uiwidget: UIWidgets.Slider, params: "0 1", desc: "Opacity for the blackout effect when in cardiac arrest")]
	protected float m_fACE_Medical_CardiacArrestBlackoutOpacity;
	
	protected ACE_Medical_CardiovascularComponent m_pACE_Medical_CardiovascularComponent;

	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		super.DisplayControlledEntityChanged(from, to);
		
		if (to)
			m_pACE_Medical_CardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(to.FindComponent(ACE_Medical_CardiovascularComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Make the screen completely black when in cardiac arrest
	override void BlackoutEffect(float effectStrength)
	{
		bool inCardiacArrest = m_pACE_Medical_CardiovascularComponent.IsInCardiacArrest();
		if (inCardiacArrest)
			effectStrength = m_fACE_Medical_CardiacArrestBlackoutOpacity / BLACKOUT_OPACITY_MULTIPLIER;
			
		SCR_UISoundEntity.SetSignalValueStr("ACE_Medical_CardiacArrest", inCardiacArrest);
		super.BlackoutEffect(effectStrength);
	}
}
