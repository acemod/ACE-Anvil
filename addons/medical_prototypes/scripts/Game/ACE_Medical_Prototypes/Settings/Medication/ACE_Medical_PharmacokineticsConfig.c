//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EDrugType, "m_eType")]
class ACE_Medical_PharmacokineticsConfig
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the medication", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	ACE_Medical_EDrugType m_eType;
	
	[Attribute(desc: "Rate constant for the activation of the drug [1/s]", precision: 8)]
	float m_fActivationRateConstant;
	
	[Attribute(desc: "Rate constant for the deactivation of the drug [1/s]", precision: 8)]
	float m_fDeactivationRateConstant;
		
	// We need to slightly offset the curve so the exponential tail will pass through zero
	// TO DO: Determine an appropriate value for this
	static const float CONCENTRATION_OFFSET_NM = -0.5;
}
