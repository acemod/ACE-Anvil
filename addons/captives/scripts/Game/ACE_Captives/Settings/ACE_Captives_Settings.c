//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Captives_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "SURRENDER", desc: "Requirements for taking a player captive.", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ACE_Captives_ETakeCaptiveRequirement))]
	ACE_Captives_ETakeCaptiveRequirement m_eTakePlayerCaptiveRequirements;
	
	[Attribute(defvalue: "SURRENDER", desc: "Requirements for taking AI captive.", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ACE_Captives_ETakeCaptiveRequirement))]
	ACE_Captives_ETakeCaptiveRequirement m_eTakeAICaptiveRequirements;
}
