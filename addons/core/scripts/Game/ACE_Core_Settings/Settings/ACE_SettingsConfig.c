//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(configRoot: true)]
class ACE_SettingsConfig
{
	[Attribute()]
	protected ref array<ref ACE_BaseSettingConfigEntry> m_aSettings;
}

//------------------------------------------------------------------------------------------------ 
class ACE_SettingConfigTitle: BaseContainerCustomTitle
{
	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		ACE_ESettings settingId;
		if (!source.Get("m_eSettingId", settingId))
			return false;
		
		title = typename.EnumToString(ACE_ESettings, settingId);
		
		bool isGlobal;
		if (!source.Get("m_bIsGlobal", isGlobal))
			return false;
		
		if (isGlobal)
			title += " (GLOBAL)";
		
		return true;
	}
}


//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_BaseSettingConfigEntry
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_ESettings))]
	protected ACE_ESettings m_eSettingId;
	
	[Attribute(defvalue: "false")]
	protected bool m_bIsGlobal;
		
	bool IsGlobal() { return m_bIsGlobal; }
	
	ACE_ESettings GetId() { return m_eSettingId; }
	
	ACE_BaseVar GetDefaultValue() { return null; }
}

//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_CheckboxSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	[Attribute()]
	protected bool m_bEnabledByDefault;
	
	override ACE_BaseVar GetDefaultValue() { return ACE_BaseVar.CreateBool(m_bEnabledByDefault); }
}

//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_IntSliderSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	[Attribute()]
	protected float m_iDefaultValue;
	
	[Attribute()]
	protected float m_iMinValue;

	[Attribute()]
	protected float m_iMaxValue;
	
	[Attribute()]
	protected float m_iStepSize;
	
	override ACE_BaseVar GetDefaultValue() { return ACE_BaseVar.CreateInt(m_iDefaultValue); }
}


//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_FloatSliderSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	[Attribute()]
	protected float m_fDefaultValue;
	
	[Attribute()]
	protected float m_fMinValue;

	[Attribute()]
	protected float m_fMaxValue;
	
	[Attribute()]
	protected float m_iStepSize;
	
	override ACE_BaseVar GetDefaultValue() { return ACE_BaseVar.CreateFloat(m_fDefaultValue); }
}

//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_ComboBoxSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_ESettingOption))]
	protected ACE_ESettingOption m_eDefaultOption;
	
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_ESettingOption))]
	protected ref array<ACE_ESettingOption> m_Options;
	
	override ACE_BaseVar GetDefaultValue() { return ACE_BaseVar.CreateInt(m_eDefaultOption); }
	
	array<ACE_ESettingOption> GetOptions() { return m_Options; }
}

//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_IntEditBoxSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	[Attribute()]
	protected int m_iDefaultValue;
	
	override ACE_BaseVar GetDefaultValue() { return ACE_BaseVar.CreateInt(m_iDefaultValue); }
}

//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), ACE_SettingConfigTitle()]
class ACE_FloatEditBoxSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	[Attribute()]
	protected float m_fDefaultValue;
	
	override ACE_BaseVar GetDefaultValue() { return ACE_BaseVar.CreateFloat(m_fDefaultValue); }
}
