//------------------------------------------------------------------------------------------------
class ACE_TacticalLadderEntityClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_TacticalLadderEntity : GenericEntity
{
	[Attribute(defvalue: "6", desc: "Signal value of the first ladder component")]
	protected int m_iFirstLadderComponentSignalValue;
	
	// TO DO: Use event-based approach instead once one exists
	[Attribute(defvalue: "750", desc: "Update delay in ms (should match with proc anim time)")]
	protected int m_iUpdateDelayMS;
	
	protected SignalsManagerComponent m_pSignalsManager;
	protected ref array<Managed> m_aLadderComponents = {};
	protected int m_iExtendSignalID;
	protected int m_iCurrentLadderComponentIdx = -1;
	
	//------------------------------------------------------------------------------------------------
	void ACE_TacticalLadderEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_pSignalsManager = SignalsManagerComponent.Cast(FindComponent(SignalsManagerComponent));
		m_iExtendSignalID = m_pSignalsManager.FindSignal("extend");
		
		FindComponents(LadderComponent, m_aLadderComponents);
		
		if (Replication.IsServer())
		{
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(FindComponent(InventoryItemComponent));
			if (itemComponent)
				itemComponent.m_OnParentSlotChangedInvoker.Insert(OnParentSlotChanged);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		// Placed on ground
		if (!newSlot)
		{
			GetGame().GetCallqueue().CallLater(OnItemPlacedOnGroundDelayed, 100);
		}
		// Picked up from ground
		else if (!oldSlot)
		{
			RpcDo_OnItemPickedFromGroundBroadcast();
			Rpc(RpcDo_OnItemPickedFromGroundBroadcast);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Workaround: Replace instance, since procedural animation gets broken when placing the ladder
	protected void OnItemPlacedOnGroundDelayed()
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetWorldTransform(params.Transform);
		GetGame().SpawnEntityPrefab(Resource.Load(GetPrefabData().GetPrefabName()), GetWorld(), params);
		SCR_EntityHelper.DeleteEntityAndChildren(this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Retract ladder when picking up from ground
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_OnItemPickedFromGroundBroadcast()
	{
		// Retract ladder
		m_pSignalsManager.SetSignalValue(m_iExtendSignalID, 0);
		UpdateScriptedServer();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handles all required updates after ladder was extended or retracted
	void UpdateScriptedServer()
	{
		UpdateScriptedLocal();
		Rpc(UpdateScriptedLocal);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handles all required updates after ladder was extended or retracted
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void UpdateScriptedLocal()
	{
		// Update colliders
		GetGame().GetCallqueue().CallLater(Update, m_iUpdateDelayMS);
		
		// Update active ladder component and drop all players that are climbing it
		m_iCurrentLadderComponentIdx = Math.Round(m_pSignalsManager.GetSignalValue(m_iExtendSignalID) - m_iFirstLadderComponentSignalValue);
		
		ChimeraCharacter player = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!player)
			return;
		
		SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(player.GetAnimationComponent().GetCommandHandler());
		if (!handler)
			return;
		
		LadderComponent ladder = handler.GetTargetLadder();
		if (!ladder)
			return;
				
		int ladderIdx = m_aLadderComponents.Find(ladder);
		if (ladderIdx < 0 || ladderIdx == m_iCurrentLadderComponentIdx)
			return;
		
		// Drop player
		handler.StartCommand_Ladder(null);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the index of the currently active ladder component
	int GetCurrentLadderComponentIdx()
	{
		return m_iCurrentLadderComponentIdx;
	}
}
