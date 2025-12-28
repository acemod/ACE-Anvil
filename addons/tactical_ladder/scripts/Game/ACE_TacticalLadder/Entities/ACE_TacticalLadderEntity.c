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
	protected ref array<ref PointInfo> m_aLadderTopPoints = {};
	protected int m_iExtendSignalID;
	protected int m_iCurrentLadderComponentIdx = -1;
	protected bool m_bIsTopExitObstructed = false;
	
#ifdef WORKBENCH
	protected ref Shape m_pDebugLine;
#endif
	
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
		
		array<IEntityComponentSource> ladderComponentSources = {};
		SCR_BaseContainerTools.FindComponentSourcesOfClass(GetPrefabData().GetPrefab().ToEntitySource(), LadderComponent, false, ladderComponentSources);
		foreach (IEntityComponentSource ladderComponentSource : ladderComponentSources)
		{
			PointInfo point = new PointInfo();
			ladderComponentSource.Get("TopFrontPositionInfo", point);
			m_aLadderTopPoints.Insert(point);
		}
		
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
		// Picked up from ground
		if (!oldSlot)
		{
			RpcDo_OnItemPickedFromGroundBroadcast();
			Rpc(RpcDo_OnItemPickedFromGroundBroadcast);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_OnItemDroppedOnGround()
	{
		vector transform[4];
		GetWorldTransform(transform);
		TraceParam traceParams = new TraceParam();
		traceParams.Exclude = this;
		traceParams.TargetLayers = EPhysicsLayerPresets.Building;
		SCR_TerrainHelper.SnapToTerrain(transform, GetWorld(), false, traceParams);
		SetWorldTransform(transform);
	}
		
	//------------------------------------------------------------------------------------------------
	//! Retract ladder when picking up from ground
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_OnItemPickedFromGroundBroadcast()
	{
		// Retract ladder
		m_pSignalsManager.SetSignalValue(m_iExtendSignalID, 0);
		UpdateScriptedLocal();
		
		ActionsManagerComponent actionsManager = ActionsManagerComponent.Cast(FindComponent(ActionsManagerComponent));
		if (!actionsManager)
			return;
		
		array<BaseUserAction> actions = {};
		actionsManager.GetActionsList(actions);
		
		foreach (BaseUserAction action : actions)
		{
			ACE_TacticalLadder_ExtendLadderAction extendAction = ACE_TacticalLadder_ExtendLadderAction.Cast(action);
			if (extendAction)
			{
				// Reset target value
				extendAction.Init(this, actionsManager);
				break;
			}
		}
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
		
		// Update active ladder component
		int newLadderComponentIdx = Math.Round(m_pSignalsManager.GetSignalValue(m_iExtendSignalID) - m_iFirstLadderComponentSignalValue);
		if (newLadderComponentIdx == m_iCurrentLadderComponentIdx)
			return;
		
		m_iCurrentLadderComponentIdx = newLadderComponentIdx;
		CheckAndDropLocalPlayer();
		UpdateIsTopExitObstructed();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Drop local player from the ladder if they are climbing and the active ladder component has changed
	void CheckAndDropLocalPlayer()
	{
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
		
		// Drop local player
		handler.StartCommand_Ladder(null);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Considered obstructed when top entry of ladder doesn't have enough space
	void UpdateIsTopExitObstructed()
	{
		if (m_iCurrentLadderComponentIdx < 0)
			return;
		
		PointInfo currentTopPoint = m_aLadderTopPoints[m_iCurrentLadderComponentIdx];
		currentTopPoint.Init(this);
		TraceParam params = new TraceParam();
		params.Flags = TraceFlags.ENTS;
		params.Exclude = SCR_PlayerController.GetLocalControlledEntity();
		
		// Trace for ceiling that block the top exit
		params.Start = currentTopPoint.GetWorldTransformAxis(3) + 0.25 * currentTopPoint.GetWorldTransformAxis(2);
		params.End = params.Start + 2 * currentTopPoint.GetWorldTransformAxis(1);
		m_bIsTopExitObstructed = (GetWorld().TraceMove(params, null) < 1) && Building.Cast(params.TraceEnt);
		
	#ifdef WORKBENCH
		vector debugPoints[4];
		debugPoints[0] = params.Start;
		debugPoints[1] = params.End;
	#endif
			
		if (m_bIsTopExitObstructed)
		{
		#ifdef WORKBENCH
			m_pDebugLine = Shape.CreateLines(0xff0000, ShapeFlags.DEFAULT, debugPoints, 2);
		#endif
			
			return;
		}
		
		// Trace for walls that block the top exit
		params.Start += currentTopPoint.GetWorldTransformAxis(1);
		params.End = params.Start - 1.25 * currentTopPoint.GetWorldTransformAxis(2);
		m_bIsTopExitObstructed = (GetWorld().TraceMove(params, null) < 1) && Building.Cast(params.TraceEnt);
		
	#ifdef WORKBENCH
		debugPoints[2] = params.Start;
		debugPoints[3] = params.End;
		
		int color = 0x00ff00;
		if (m_bIsTopExitObstructed)
		    color = 0xff0000;; // Red if true
		
		m_pDebugLine = Shape.CreateLines(color, ShapeFlags.DEFAULT, debugPoints, 4);
	#endif
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the index of the currently active ladder component
	int GetCurrentLadderComponentIdx()
	{
		return m_iCurrentLadderComponentIdx;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsTopExitObstructed()
	{
		return m_bIsTopExitObstructed;
	}
}
