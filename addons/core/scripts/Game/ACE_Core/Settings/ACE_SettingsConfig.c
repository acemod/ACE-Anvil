//------------------------------------------------------------------------------------------------
//! Base container for all settings
[BaseContainerProps(configRoot: true)]
class ACE_SettingsConfig
{
	[Attribute(desc: "Definitions of settings for all mods")]
	protected ref array<ref ACE_ModSettings> m_aInitialModSettings;
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for all categories
	array<ref ACE_ModSettings> GetAllModSettings()
	{
		return m_aInitialModSettings;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for a mod
	ACE_ModSettings GetModSettings(typename modSettingsType)
	{
		foreach (ACE_ModSettings entry : m_aInitialModSettings)
		{
			if (entry.Type() == modSettingsType)
				return entry;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set settings for a mod
	void SetModSettings(notnull ACE_ModSettings settings)
	{
		for (int i, count = m_aInitialModSettings.Count(); i < count; i++)
		{
			if (m_aInitialModSettings[i].Type() == settings.Type())
			{
				m_aInitialModSettings[i] = settings;
				return;
			}
		}
		
		m_aInitialModSettings.Insert(settings);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(ACE_SettingsConfig instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, ACE_SettingsConfig instance)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet);

	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs , ScriptCtx ctx)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(ACE_SettingsConfig instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		return true;
	}
}
