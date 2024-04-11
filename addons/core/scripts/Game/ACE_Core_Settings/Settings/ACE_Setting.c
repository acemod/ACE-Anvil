//------------------------------------------------------------------------------------------------
class ACE_Setting : Managed
{
	protected SCR_BaseGameMode m_pGameMode;
	protected ACE_ESettings m_eId;
	protected ACE_BaseSettingConfigEntry m_Config;
	protected int m_iValueIdx;
	protected int m_iMissionSettingsPriority;
	protected ref ACE_ScriptInvokerBaseVar m_OnChanged;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Setting(SCR_BaseGameMode gameMode, ACE_ESettings id, ACE_BaseSettingConfigEntry config)
	{
		m_pGameMode = gameMode;
		m_eId = id;
		m_Config = config;
		
		ACE_BaseVar defaultValue = m_Config.GetDefaultValue();
		array<ref ACE_BaseVar> serverValues = m_pGameMode.ACE_GetRuntimeSettingValues(ACE_ESettingsSource.SERVER);
		m_iValueIdx = serverValues.Count();
		serverValues.Insert(defaultValue);
		m_pGameMode.ACE_GetRuntimeSettingValues(ACE_ESettingsSource.CLIENT).Insert(defaultValue);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetValue(ACE_BaseVar value, ACE_ESettingsSource source = ACE_ESettingsSource.CLIENT)
	{
		array<ref ACE_BaseVar> values = m_pGameMode.ACE_GetRuntimeSettingValues(source);
		if (!values)
			return;
		
		ACE_BaseVar prevValue = values[m_iValueIdx];
		values[m_iValueIdx] = value;
				
		// Trigger OnChanged when value has changed on priority source
		bool triggerOnChanged = (source == GetPrioritySource() && prevValue != value);
		if (triggerOnChanged && m_OnChanged)
			m_OnChanged.Invoke(value);
		
		// Broadcast changes in server settings
		if (source == ACE_ESettingsSource.SERVER)
			m_pGameMode.ACE_BroadcastSetting(m_eId, source, triggerOnChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_BaseVar GetValue(ACE_ESettingsSource source = ACE_ESettingsSource.PRIORITY)
	{
		if (source == ACE_ESettingsSource.PRIORITY)
		{
			source = GetPrioritySource();
		}
		
		if (source == ACE_ESettingsSource.DEFAULT)
			return m_Config.GetDefaultValue();

		array<ref ACE_BaseVar> settingValues = m_pGameMode.ACE_GetRuntimeSettingValues(source);
		if (!settingValues)
			return null;
		
		return settingValues[m_iValueIdx];
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsGlobal()
	{
		return m_Config.IsGlobal();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSourcePriority(ACE_ESettingsSource source, int priority)
	{
		array<int> priorities = m_pGameMode.ACE_GetSettingPriorities(source);
		if (!priorities)
			return;
		
		ACE_ESettingsSource prevPrioritySource = GetPrioritySource();
		
		priorities[m_iValueIdx] = priority;
		
		// Trigger OnChanged when priority source has changed
		bool triggerOnChanged = (GetPrioritySource() != prevPrioritySource);
		if (triggerOnChanged && m_OnChanged)
		{
			array<ref ACE_BaseVar> values = m_pGameMode.ACE_GetRuntimeSettingValues(source);
			if (values)
				m_OnChanged.Invoke(values[m_iValueIdx]);
		}
		
		// Broadcast changes in server settings
		if (source == ACE_ESettingsSource.SERVER)
			m_pGameMode.ACE_BroadcastSetting(m_eId, source, triggerOnChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	int GetSourcePriority(ACE_ESettingsSource source)
	{
		array<int> priorities = m_pGameMode.ACE_GetSettingPriorities(source);
		if (!priorities)
			return 0;
		
		return priorities[m_iValueIdx];
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_ESettingsSource GetPrioritySource()
	{
		bool isGlobal = m_Config.IsGlobal();
		bool isSetOnClient = m_pGameMode.ACE_GetRuntimeSettingValues(ACE_ESettingsSource.CLIENT)[m_iValueIdx];
		bool isSetInMission = m_pGameMode.ACE_GetRuntimeSettingValues(ACE_ESettingsSource.MISSION)[m_iValueIdx];
		bool isSetOnServer = m_pGameMode.ACE_GetRuntimeSettingValues(ACE_ESettingsSource.SERVER)[m_iValueIdx];
		int missionPriority =  m_pGameMode.ACE_GetSettingPriorities(ACE_ESettingsSource.MISSION)[m_iValueIdx];
		int serverPriority = m_pGameMode.ACE_GetSettingPriorities(ACE_ESettingsSource.SERVER)[m_iValueIdx];
		
		// Order depending on depending on set priorities
		// | C | M | S |
		// +---+---+---+
		// | * | 0 | 0 | => C > M > S > D
		// | * | 1 | 0 | => M > C > S > D
		// | * | 0 | 1 | => S > C > M > D
		// | * | 1 | 1 | => M > S > C > D
		// | * | 1 | 2 | => S > M > C > D
		if (serverPriority == 0 && missionPriority == 0)
		{
			// Client is skipped if setting is global
			if (isSetOnClient && !isGlobal)
				return ACE_ESettingsSource.CLIENT;
			
			if (isSetInMission)
				return ACE_ESettingsSource.MISSION;
			
			if (isSetOnServer)
				return ACE_ESettingsSource.SERVER;
		}
		else if (missionPriority == 0)
		{
			if (isSetOnServer)
				return ACE_ESettingsSource.SERVER;
			
			if (isSetOnClient && !isGlobal)
				return ACE_ESettingsSource.CLIENT;
			
			if (isSetInMission)
				return ACE_ESettingsSource.MISSION;
		}
		else if (serverPriority == 0)
		{
			if (isSetInMission)
				return ACE_ESettingsSource.MISSION;
			
			if (isSetOnClient && !isGlobal)
				return ACE_ESettingsSource.CLIENT;
			
			if (isSetOnServer)
				return ACE_ESettingsSource.SERVER;
		}
		else if (serverPriority > missionPriority)
		{
			if (isSetOnServer)
				return ACE_ESettingsSource.SERVER;
			
			if (isSetInMission)
				return ACE_ESettingsSource.MISSION;
			
			if (isSetOnClient && !isGlobal)
				return ACE_ESettingsSource.CLIENT;
		}
		else
		{
			if (isSetInMission)
				return ACE_ESettingsSource.MISSION;
			
			if (isSetOnServer)
				return ACE_ESettingsSource.SERVER;
			
			if (isSetOnClient && !isGlobal)
				return ACE_ESettingsSource.CLIENT;
		}
		
		return ACE_ESettingsSource.DEFAULT;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetValueIdx()
	{
		return m_iValueIdx;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_ScriptInvokerBaseVar GetOnChanged(bool createNew = true)
	{
		if (!m_OnChanged && createNew)
			m_OnChanged = new ACE_ScriptInvokerBaseVar();
		
		return m_OnChanged;
	}
}
