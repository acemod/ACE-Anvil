//------------------------------------------------------------------------------------------------
//! Get encryption key for radio radial menu
modded class SCR_VONEntryRadio : SCR_VONEntry
{
	protected string m_sACE_Radio_EncryptionKeyText;
	
	//------------------------------------------------------------------------------------------------
	override void InitEntry()
	{
		super.InitEntry();
		
		m_sACE_Radio_EncryptionKeyText = "";
		string key = m_RadioTransceiver.GetRadio().GetEncryptionKey();
		array<string> tokens = {};
		key.Split("-", tokens, true);
		
		if (tokens.Count() < 3)
			return;
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return;
		
		SCR_Faction faction = SCR_Faction.Cast(factionManager.GetFactionByIndex(tokens[0].ToInt()));
		if (!faction)
			return;
		
		SCR_FactionCallsignInfo info = faction.GetCallsignInfo();
		if (!info)
			return;
		
		string groupName = WidgetManager.Translate(info.GetCompanyCallsignName(tokens[1].ToInt()));
		m_sACE_Radio_EncryptionKeyText = string.Format("%1-%2", groupName, tokens[2].ToInt() + 1)
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update entry visuals
	override void Update()
	{
		super.Update();
		
		SCR_VONEntryComponent entryComp = SCR_VONEntryComponent.Cast(m_EntryComponent);
		if (entryComp)
			entryComp.ACE_Radio_SetEncryptionKeyText(m_sACE_Radio_EncryptionKeyText);
	}
}
