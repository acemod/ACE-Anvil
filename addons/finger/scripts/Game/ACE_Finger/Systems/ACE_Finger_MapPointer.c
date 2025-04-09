//------------------------------------------------------------------------------------------------
/*
class ACE_Finger_MapPointer : WorldController
{
	[RplProp(onRplName: "OnToggleActive")]
	protected bool m_bState;
	
	[RplProp()]
	protected vector m_vPos;
	
	[RplProp()]
	protected int m_iPlayerID;
	
	//------------------------------------------------------------------------------------------------
    override static void InitInfo(WorldControllerInfo outInfo)
    {
		super.InitInfo(outInfo);
		Print(1313);
        outInfo.SetPublic(true);
    }
	
	//------------------------------------------------------------------------------------------------
	static ACE_Finger_MapPointer GetLocalInstance()
	{
		return ACE_Finger_MapPointer.Cast(WorldController.FindMyController(GetGame().GetWorld(), ACE_Finger_MapPointer));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnAuthorityReady()
	{
		super.OnAuthorityReady();
		Print(11111);
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
	protected void OnToggleActive()
	{
		ACE_Finger_MapPointingSystem manager = ACE_Finger_MapPointingSystem.GetInstance();
		if (!manager)
			return;
		
		if (m_bState)
			manager.RegisterActivePointerLocal(this);
		else
			manager.UnregisterActivePointerLocal(this);
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
}
*/