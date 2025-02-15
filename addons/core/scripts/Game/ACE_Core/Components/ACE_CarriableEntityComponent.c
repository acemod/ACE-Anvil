//------------------------------------------------------------------------------------------------
class ACE_CarriableEntityComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_CarriableEntityComponent : ScriptComponent
{
	[Attribute(desc: "Info for the attachment point on the carrier")]
	protected ref PointInfo m_AttachmentPoint;
	
	[RplProp(onRplName: "OnCarrierChanged")]
	protected RplId m_iCarrierID;
	protected IEntity m_pCarrier;
	
	//------------------------------------------------------------------------------------------------
	//! Compartment gets attached to the carrier
	void Carry(notnull IEntity carrier)
	{
		if (m_pCarrier)
			return;	
				
		RplComponent carrierRpl = RplComponent.Cast(carrier.FindComponent(RplComponent));
		if (!carrierRpl)
			return;
		
		m_iCarrierID = carrierRpl.Id();
		OnCarrierChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Release()
	{
		if (!m_pCarrier)
			return;
		
		m_iCarrierID = RplId.Invalid();
		OnCarrierChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnCarrierChanged()
	{
		IEntity prevCarrier = GetOwner().GetParent();
		if (prevCarrier)
		{
			
			if (!prevCarrier)
				return;
			
			RplComponent prevCarrierRpl = RplComponent.Cast(prevCarrier.FindComponent(RplComponent));
			if (!prevCarrierRpl)
				return;
			
			prevCarrier.RemoveChild(GetOwner(), true);
			
			if (!prevCarrierRpl.IsProxy())
				DetachHandlersAuthority();
			
			if (prevCarrierRpl.IsOwner())
				DetachHandlersOwner();
			
			m_pCarrier = null;
		}
		
		RplComponent newCarrierRpl = RplComponent.Cast(Replication.FindItem(m_iCarrierID));
		if (newCarrierRpl)
		{
			m_pCarrier = newCarrierRpl.GetEntity();
			m_AttachmentPoint.Init(m_pCarrier);
			m_pCarrier.AddChild(GetOwner(), m_AttachmentPoint.GetNodeId());
			vector transform[4];
			m_AttachmentPoint.GetLocalTransform(transform);
			GetOwner().SetLocalTransform(transform);
			
			if (!newCarrierRpl.IsProxy())
				AttachHandlersAuthority();
			
			if (newCarrierRpl.IsOwner())
				AttachHandlersOwner();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AttachHandlersAuthority()
	{
		// Set carrier as owner
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController performerPlayerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerManager.GetPlayerIdFromControlledEntity(m_pCarrier)));
		
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (performerPlayerController && rpl)
			rpl.Give(performerPlayerController.GetRplIdentity());
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCarrier.FindComponent(SCR_CharacterControllerComponent));
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnCarrierLifeStateChanged);
				
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnCarrierDisconnected);
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pCarrier.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_SetIsCarrier(true);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlersAuthority()
	{
		// Set server as owner
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (rpl)
			rpl.Give(RplIdentity.Local());
		
		if (m_pCarrier)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCarrier.FindComponent(SCR_CharacterControllerComponent));
			if (charController)
				charController.m_OnLifeStateChanged.Remove(OnCarrierLifeStateChanged);
		}
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Remove(OnCarrierDisconnected);
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pCarrier.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_SetIsCarrier(false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AttachHandlersOwner()
	{
		GetGame().GetInputManager().AddActionListener("ACE_StopCarrying", EActionTrigger.DOWN, ActionReleaseCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlersOwner()
	{
		GetGame().GetInputManager().RemoveActionListener("ACE_StopCarrying", EActionTrigger.DOWN, ActionReleaseCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer gets incapacitated or dies
	protected void OnCarrierLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		Release();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer or patient disconnects
	protected void OnCarrierDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (player == m_pCarrier)
			Release();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for the release keybind
	protected void ActionReleaseCallback()
	{
		Rpc(Release);
	}
	
}
