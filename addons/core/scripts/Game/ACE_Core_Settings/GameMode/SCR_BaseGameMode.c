//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	protected ref map<ACE_ESettings, ref ACE_Setting> m_mACE_Settings = new map<ACE_ESettings, ref ACE_Setting>();
	
	// Maps cannot be sent over net, so we stick to arrays and store the index in ACE_Setting
	protected ref array<ref ACE_BaseVar> m_aACE_ClientSettingValues;
	protected ref array<ref ACE_BaseVar> m_aACE_MissionSettingValues;
	[RplProp()]
	protected ref array<ref ACE_BaseVar> m_aACE_ServerSettingValues;
	protected ref array<int> m_aACE_MissionSettingPriorities;
	[RplProp()]
	protected ref array<int> m_aACE_ServerSettingPriorities;
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_OnServerSettingsInitialized()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	//! Implementation-specific => use ACE_SettingsHelper instead
	array<ref ACE_BaseVar> ACE_GetRuntimeSettingValues(ACE_ESettingsSource source)
	{
		switch (source)
		{
			case ACE_ESettingsSource.CLIENT: { return m_aACE_ClientSettingValues; }
			case ACE_ESettingsSource.MISSION: { return m_aACE_MissionSettingValues; }
			case ACE_ESettingsSource.SERVER: { return m_aACE_ServerSettingValues; }
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	array<int> ACE_GetSettingPriorities(ACE_ESettingsSource source)
	{
		switch (source)
		{
			case ACE_ESettingsSource.MISSION: { return m_aACE_MissionSettingPriorities; }
			case ACE_ESettingsSource.SERVER: { return m_aACE_ServerSettingPriorities; }
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_BroadcastSetting(ACE_ESettings id, ACE_ESettingsSource source, bool triggerOnChanged)
	{
		ACE_Setting setting = m_mACE_Settings[id];
		if (!setting)
			return;
		
		array<ref ACE_BaseVar> values = ACE_GetRuntimeSettingValues(source);
		if (!values)
			return;
		
		array<int> priorities = ACE_GetSettingPriorities(source);
		if (!priorities)
			return;
		
		int valueIdx = setting.GetValueIdx();
		Rpc(ACE_RpcDo_Broadcast_Setting, id, source, values[valueIdx], priorities[valueIdx], triggerOnChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void ACE_RpcDo_Broadcast_Setting(ACE_ESettings id, ACE_ESettingsSource source, ACE_BaseVar value, int priority, bool triggerOnChanged)
	{
		ACE_Setting setting = m_mACE_Settings[id];
		if (!setting)
			return;
		
		array<ref ACE_BaseVar> values = ACE_GetRuntimeSettingValues(source);
		if (!values)
			return;
		
		array<int> priorities = ACE_GetSettingPriorities(source);
		if (!priorities)
			return;
				
		int valueIdx = setting.GetValueIdx();
		values[valueIdx] = value;
		priorities[valueIdx] = priority;
		
		if (!triggerOnChanged)
			return;
		
		ACE_ScriptInvokerBaseVar onChanged = setting.GetOnChanged();
		if (onChanged)
			onChanged.Invoke(value);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Implementation-specific => use ACE_SettingsHelper instead
	ACE_Setting ACE_GetSetting(ACE_ESettings settingId)
	{
		return m_mACE_Settings[settingId];
	}
	
	//------------------------------------------------------------------------------------------------
	//! Implementation-specific => use ACE_SettingsHelper instead
	array<ACE_Setting> ACE_GetAllSettings()
	{
		array<ACE_Setting> settings = {};
		
		foreach (ACE_Setting setting : m_mACE_Settings)
		{
			settings.Insert(setting);
		}
		
		return settings;
	}
}
