//------------------------------------------------------------------------------------------------
class ACE_Finger_MapPointerClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Temporary workaround until WorldController works
class ACE_Finger_MapPointer : ScriptComponent
{
	[RplProp(onRplName: "OnToggleActive", condition: RplCondition.None)]
	protected bool m_bState;
	
	[RplProp()]
	protected vector m_vPos;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Finger_MapPointer GetLocalInstance()
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller)
			return null;
		
		return ACE_Finger_MapPointer.Cast(controller.FindComponent(ACE_Finger_MapPointer));
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
}
