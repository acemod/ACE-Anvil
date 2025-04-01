//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRHelperCompartmentClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! TO DO: Refactor common code with ACE_Carrying_HelperCompartment to a ACE Core parent class
class ACE_Medical_CPRHelperCompartment : GenericEntity
{
	protected SCR_ChimeraCharacter m_pPerformer;
	protected SCR_ChimeraCharacter m_pPatient;

	protected static const int SEARCH_POS_RADIUS_M = 5; // Search radius for safe position for dropping carried player
	protected static const float HELPER_DELETION_DELAY_MS = 1000; // Delay for helper to get deleted after release

	
	//------------------------------------------------------------------------------------------------
	//! Move performer into compartment and attach handlers
	void Init(SCR_ChimeraCharacter performer, SCR_ChimeraCharacter patient)
	{
		m_pPerformer = performer;
		m_pPatient = patient;
		
		AttachHandlers();

		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(performer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		SCR_BaseCompartmentManagerComponent compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(FindComponent(SCR_BaseCompartmentManagerComponent));
		if (!compartmentManager)
			return;
		
		compartmentAccess.GetInVehicle(this, compartmentManager.FindCompartment(0), false, 0, ECloseDoorAfterActions.INVALID, false);
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
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.GetCharacterController());
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnPerformerLifeStateChanged);
		
		charController = SCR_CharacterControllerComponent.Cast(m_pPatient.GetCharacterController());
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnPatientLifeStateChanged);
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.GetCompartmentAccessComponent());
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
		
		if (m_pPatient)
		{
			ACE_Medical_CardiovascularComponent cardiovascularComponent = m_pPatient.ACE_Medical_GetCardiovascularComponent();
			if (cardiovascularComponent)
				cardiovascularComponent.SetIsCPRPerformed(true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlers()
	{
		if (m_pPerformer)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.GetCharacterController());
			if (charController)
				charController.m_OnLifeStateChanged.Remove(OnPerformerLifeStateChanged);
		}
		
		if (m_pPatient)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPatient.GetCharacterController());
			if (charController)
				charController.m_OnLifeStateChanged.Remove(OnPatientLifeStateChanged);
		}
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Remove(OnPlayerDisconnected);
		
		// Clean-up when carried has left the compartment
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.GetCompartmentAccessComponent());
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
		
		if (m_pPatient)
		{
			ACE_Medical_CardiovascularComponent cardiovascularComponent = m_pPatient.ACE_Medical_GetCardiovascularComponent();
			if (cardiovascularComponent)
				cardiovascularComponent.SetIsCPRPerformed(false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates CPR
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Terminate(EGetOutType getOutType)
	{
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		if (getOutType == EGetOutType.ANIMATED)
			compartmentAccess.GetOutVehicle(EGetOutType.ANIMATED, -1, ECloseDoorAfterActions.INVALID, false);
		else
			EjectPerformer();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Eject the performer out of the helper compartment
	protected void EjectPerformer()
	{
		if (!m_pPerformer)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
				
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
		DetachHandlers();
		// Deletion of helper has to be delayed or released players stay visibly prone for other players on dedicated
		GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, HELPER_DELETION_DELAY_MS, false, this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer gets incapacitated or dies
	protected void OnPerformerLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		Terminate(EGetOutType.TELEPORT);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when patient wakes up
	protected void OnPatientLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		if (newLifeState == ECharacterLifeState.ALIVE)
			Terminate(EGetOutType.ANIMATED);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer or patient disconnects
	protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (player == m_pPerformer)
			Terminate(EGetOutType.TELEPORT);
		else if (player == m_pPatient)
			Terminate(EGetOutType.ANIMATED);
	}
}
