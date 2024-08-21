//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRHelperCompartmentClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! TO DO: Refactor common code with ACE_Carrying_HelperCompartment to a ACE Core parent class
class ACE_Medical_CPRHelperCompartment : GenericEntity
{
	protected IEntity m_pPerformer;
	protected IEntity m_pPatient;

	protected static const int SEARCH_POS_RADIUS_M = 5; // Search radius for safe position for dropping carried player
	protected static const float HELPER_DELETION_DELAY_MS = 1000; // Delay for helper to get deleted after release

	
	//------------------------------------------------------------------------------------------------
	//! Move performer into compartment and attach handlers
	void Init(IEntity performer, IEntity patient)
	{
		m_pPerformer = performer;
		m_pPatient = patient;
		
		AttachHandlers();

		SCR_CompartmentAccessComponent performerCompartmentAccess = SCR_CompartmentAccessComponent.Cast(performer.FindComponent(SCR_CompartmentAccessComponent));
		if (!performerCompartmentAccess)
			return;
		
		performerCompartmentAccess.MoveInVehicle(this, ECompartmentType.CARGO);
		
		if (patient)
		{
			ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
			if (cardiovascularComponent)
				cardiovascularComponent.SetIsCPRPerformed(true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AttachHandlers()
	{
		// Set owner to performer
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController performerPlayerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerManager.GetPlayerIdFromControlledEntity(m_pPerformer)));
		
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (performerPlayerController && rpl)
			rpl.Give(performerPlayerController.GetRplIdentity());
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnLifeStateChanged);
		
		charController = SCR_CharacterControllerComponent.Cast(m_pPatient.FindComponent(SCR_CharacterControllerComponent));
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnLifeStateChanged);
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
		
		Rpc(RpcDo_AttachHandlersOwner);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_AttachHandlersOwner()
	{
		GetGame().GetInputManager().AddActionListener("GetOut", EActionTrigger.DOWN, ActionTerminateCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlers()
	{
		if (m_pPerformer)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
			if (charController)
				charController.m_OnLifeStateChanged.Remove(OnLifeStateChanged);
		}
		
		if (m_pPatient)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPatient.FindComponent(SCR_CharacterControllerComponent));
			if (charController)
				charController.m_OnLifeStateChanged.Remove(OnLifeStateChanged);
		}
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Remove(OnPlayerDisconnected);
		
		Rpc(RpcDo_DetachHandlersOwner);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_DetachHandlersOwner()
	{
		GetGame().GetInputManager().RemoveActionListener("GetOut", EActionTrigger.DOWN, ActionTerminateCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates CPR
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Terminate()
	{
		MoveOutPerformer();
		
		if (m_pPatient)
		{
			ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(m_pPatient.FindComponent(ACE_Medical_CardiovascularComponent));
			if (cardiovascularComponent)
				cardiovascularComponent.SetIsCPRPerformed(false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Moves the performer out of the helper compartment and schedule clean-up
	protected void MoveOutPerformer()
	{
		if (!m_pPerformer)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		// Clean-up when carried has left the comparment
		compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
		
		vector transform[4];
		GetWorldTransform(transform);
		vector pos;
		SCR_WorldTools.FindEmptyTerrainPosition(pos, transform[3], SEARCH_POS_RADIUS_M);
		transform[3] = pos;
		compartmentAccess.ACE_MoveOutVehicle(transform);
		
		// Broadcast teleport on network
		RplComponent performerRpl = RplComponent.Cast(m_pPerformer.FindComponent(RplComponent));
		if (performerRpl)
			performerRpl.ForceNodeMovement(GetOrigin());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean-up when the peformer has left the compartment
	protected void OnCompartmentLeft()
	{
		if (m_pPerformer)
		{
			SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
			if (compartmentAccess)
				compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
		}
		
		DetachHandlers();
		// Deletion of helper has to be delayed or released players stay visibly prone for other players on dedicated
		GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, HELPER_DELETION_DELAY_MS, false, this);
	}
	
	//------------------------------------------------------------------------------------------------
	// Release from performer when they get incapacitated or die
	protected void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		Terminate();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (player == m_pPerformer)
			Terminate();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for the release keybind
	protected void ActionTerminateCallback()
	{
		Rpc(Terminate);
	}
}
