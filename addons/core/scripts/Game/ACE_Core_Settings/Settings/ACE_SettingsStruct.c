//------------------------------------------------------------------------------------------------ 
class ACE_SettingsStruct : SCR_JsonApiStruct
{
	protected ref array<ref ACE_SettingEntryStruct> m_aSettings = {};
	
	void ACE_SettingsStruct()
	{
		RegV("m_aSettings");
	}
	
	//------------------------------------------------------------------------------------------------ 
	static ACE_SettingsStruct CreateFromSettings(ACE_ESettingsSource source)
	{
		ACE_SettingsStruct struct = new ACE_SettingsStruct();
		array<ACE_ESettings> ids = {};
		SCR_Enum.GetEnumValues(ACE_ESettings, ids);
		
		foreach (ACE_ESettings id : ids)
		{
			struct.m_aSettings.Insert(ACE_SettingEntryStruct.CreateFromSetting(id, source));
		}
		
		return struct;
	}
	
	//------------------------------------------------------------------------------------------------ 
	void ApplyToSettings(ACE_ESettingsSource source)
	{
		foreach (ACE_SettingEntryStruct struct : m_aSettings)
		{
			struct.ApplyToSetting(source);
		}
	}
	
	//------------------------------------------------------------------------------------------------ 
	override void ClearCache()
	{
		m_aSettings.Clear();
	}
}

//------------------------------------------------------------------------------------------------ 
class ACE_SettingEntryStruct : SCR_JsonApiStruct
{
	// -- TO DO: Check if needed. Maybe mission header can already set enum from strings
	protected string m_sName;
	protected int m_iPriority;
	//! Check if we could write the value with its original type and read as string
	protected float m_Value;
	
	//------------------------------------------------------------------------------------------------ 
	void ACE_SettingEntryStruct()
	{
		RegV("m_sName");
		RegV("m_iPriority");
		RegV("m_Value");
	}

	//------------------------------------------------------------------------------------------------ 
	static ACE_SettingEntryStruct CreateFromSetting(ACE_ESettings settingId, ACE_ESettingsSource source)
	{
		ACE_Setting setting = ACE_SettingsHelper.Get(settingId);
		if (!setting)
			return null;
		
		ACE_SettingEntryStruct struct = new ACE_SettingEntryStruct();
		
		// Client settings have no priority modifier
		if (source == ACE_ESettingsSource.CLIENT)
		{
			struct.UnregV("m_iPriority");
		}
		else
		{
			struct.m_iPriority = setting.GetSourcePriority(source);
		}
		
		struct.m_sName = typename.EnumToString(ACE_ESettings, settingId);
		
		ACE_BaseVar value = setting.GetValue(source);
		if (!value)
			return null;
		
		struct.m_Value = value.GetFloat();
		return struct;
	}
	
	//------------------------------------------------------------------------------------------------ 
	void ApplyToSetting(ACE_ESettingsSource source)
	{
		ACE_Setting setting = ACE_SettingsHelper.Get(typename.StringToEnum(ACE_ESettings, m_sName));
		if (!setting)
			return;
		
		setting.SetValue(ACE_BaseVar.CreateFloat(m_Value), source);
		
		// Client settings have no priority modifier
		if (source != ACE_ESettingsSource.CLIENT)
			setting.SetSourcePriority(source, m_iPriority);
	}
	
	//------------------------------------------------------------------------------------------------ 
	override void ClearCache()
	{
		m_sName = "";
	}
}
