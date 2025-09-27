//------------------------------------------------------------------------------------------------
class ACE_Radio_FillDeviceComponentClass : SCR_GadgetComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Radio_FillDeviceComponent : SCR_GadgetComponent
{
	[RplProp()]
	protected int m_iFactionID = -1;
	
	[RplProp()]
	protected int m_iGroupID = -1;
	
	protected SignalsManagerComponent m_pSignalsManager;
	protected int m_iAddressSignal;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Radio_FillDeviceComponent GetHeldInstance(IEntity user)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.Cast(user.FindComponent(SCR_GadgetManagerComponent));
		if (!gadgetManager)
			return null;
		
		return ACE_Radio_FillDeviceComponent.Cast(gadgetManager.GetHeldGadgetComponent());
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_pSignalsManager = SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent));
		m_iAddressSignal = m_pSignalsManager.FindSignal("address");
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadKeys(int factionID, int groupID)
	{
		m_iFactionID = factionID;
		m_iGroupID = groupID;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearKeys()
	{
		LoadKeys(-1, -1);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsLoaded()
	{
		return (m_iFactionID >= 0) && (m_iGroupID >= 0);
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAddressIndex()
	{
		return m_pSignalsManager.GetSignalValue(m_iAddressSignal);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetKeyGroupName()
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return "";
		
		SCR_Faction faction = SCR_Faction.Cast(factionManager.GetFactionByIndex(m_iFactionID));
		if (!faction)
			return "";
		
		SCR_FactionCallsignInfo info = faction.GetCallsignInfo();
		if (!info)
			return "";
		
		return  WidgetManager.Translate(info.GetCompanyCallsignName(m_iGroupID));
	}
	
	//------------------------------------------------------------------------------------------------
	string GetCurrentKeyName()
	{
		string address = (1 + GetAddressIndex()).ToString();
		
		string group = GetKeyGroupName();
		if (group.IsEmpty())
			return address;
		
		return string.Format("%1-%2", group, address);
	}
	
	//------------------------------------------------------------------------------------------------
	void CopyCurrentKey(BaseRadioComponent radio)
	{
		string key;
		if (m_iFactionID >= 0 && m_iGroupID >= 0)
			key = string.Format("%1-%2-%3", m_iFactionID, m_iGroupID, GetAddressIndex());
		
		radio.SetEncryptionKey(key);
	}
	
	//------------------------------------------------------------------------------------------------
	void CopyFrom(ACE_Radio_FillDeviceComponent from)
	{
		LoadKeys(from.m_iFactionID, from.m_iGroupID);		
	}
}
