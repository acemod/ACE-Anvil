//------------------------------------------------------------------------------------------------
class ACE_Finger_MapPointerClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! TO DO: Replace with public WorldController when new systems are available
class ACE_Finger_MapPointer : GenericEntity
{
	[RplProp(), Attribute(defvalue: "10", desc: "Range of the pointer in meters. Only players in range will see it. Anyone can see it if negative.")]
	protected float m_fPointerRangeM;
	
	[RplProp(onRplName: "OnOwnerSet")]
	protected int m_iOwnerPlayerID;
	
	[RplProp(onRplName: "OnToggleActive")]
	protected bool m_bState;
	
	[RplProp()]
	protected vector m_vPos;
	
	protected static ACE_Finger_MapPointer s_pLocalInstance;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Finger_MapPointer GetLocalInstance()
	{
		return s_pLocalInstance;
	}
	
	//------------------------------------------------------------------------------------------------
	void InitServer(int ownerPlayerID)
	{
		ACE_Finger_Settings settings = ACE_SettingsHelperT<ACE_Finger_Settings>.GetModSettings();
		if (settings)
			m_fPointerRangeM = settings.m_fMapPointingRangeM;
		
		m_iOwnerPlayerID = ownerPlayerID;
		OnOwnerSet();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnOwnerSet()
	{
		if (m_iOwnerPlayerID == SCR_PlayerController.GetLocalPlayerId())
			s_pLocalInstance = this;
	}
	
	//------------------------------------------------------------------------------------------------
	void ToggleActive(bool state)
	{
		Rpc(RpcAsk_ToggleActive, state);
	}
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_ToggleActive(bool state)
    {
		m_bState = state;
		OnToggleActive();
		Replication.BumpMe();
    }
	
	//------------------------------------------------------------------------------------------------
	bool IsToggleOn()
	{
		return m_bState;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register/Unregister pointer
	protected void OnToggleActive()
	{
		ACE_Finger_MapPointingSystem manager = ACE_Finger_MapPointingSystem.GetInstance();
		if (!manager)
			return;
		
		if (m_bState)
			manager.RegisterActivePointer(this);
		else
			manager.UnregisterActivePointer(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPos(vector pos)
	{
		Rpc(RpcAsk_SetPos, pos);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_SetPos(vector pos)
	{
		m_vPos = pos;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetPos()
	{
		return m_vPos;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerPlayerID()
	{
		return m_iOwnerPlayerID;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPointerRange()
	{
		return m_fPointerRangeM;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_Finger_MapPointer()
	{
		ACE_Finger_MapPointingSystem manager = ACE_Finger_MapPointingSystem.GetInstance();
		if (manager)
			manager.UnregisterActivePointer(this);
	}
}
