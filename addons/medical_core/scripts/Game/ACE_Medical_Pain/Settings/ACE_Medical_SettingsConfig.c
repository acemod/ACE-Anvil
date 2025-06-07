//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	
	[Attribute(defvalue: "WHITE_FLASH", desc: "What kind of pain screen effect is used", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ACE_Medical_EPainEffectType))]
	ACE_Medical_EPainEffectType m_ePainScreenEffectType;
}

