//------------------------------------------------------------------------------------------------
class ACE_Finger_MapPointerController : WorldController
{
	[RplProp(), Attribute(defvalue: "10", desc: "Range of the pointer in meters. Only players in range will see it. Anyone can see it if negative.")]
	protected float m_fPointerRangeM;
	
	[RplProp()]
	protected int m_iOwnerPlayerId;
	
	[RplProp(onRplName: "OnToggleActive")]
	protected bool m_bState;
	
	[RplProp()]
	protected vector m_vPos;
	
	protected static const int ATTEMPT_TIMEOUT_MS = 500;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Finger_MapPointer GetLocalInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Finger_MapPointer.Cast(world.GetSystems().FindMyController(ACE_Finger_MapPointer));
	}
	
	override static void InitInfo(WorldControllerInfo outInfo)
	{
		outInfo.SetPublic(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnAuthorityReady()
	{
		ACE_Finger_Settings settings = ACE_SettingsHelperT<ACE_Finger_Settings>.GetModSettings();
		if (settings)
			m_fPointerRangeM = settings.m_fMapPointingRangeM;
		
		Replication.BumpMe();
		Rpc(RpcAsk_InitOwnerPlayerIdOwner);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcAsk_InitOwnerPlayerIdOwner()
	{
		int playerId = SCR_PlayerController.GetLocalPlayerId();
		if (playerId <= 0)
		{
			// Try later if player controller wasn't ready yet
			GetGame().GetCallqueue().CallLater(RpcAsk_InitOwnerPlayerIdOwner, false, ATTEMPT_TIMEOUT_MS);
			return;
		}
		
		Rpc(RpcDo_InitOwnerPlayerIdServer, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_InitOwnerPlayerIdServer(int playerId)
	{
		m_iOwnerPlayerId = playerId;
		Replication.BumpMe();
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
	//! Temporary workaround until WorldController:GetOwnerPlayerId works
	int ACE_GetOwnerPlayerId()
	{
		return m_iOwnerPlayerId;
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
