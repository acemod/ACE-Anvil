class ACE_MedicalDefibrillator_DefibrillatorBaseUserAction : ScriptedUserAction
{
	protected ACE_MedicalDefibrillation_DefibrillatorComponent m_defibrillatorComponent;
	
	//------------------------------------------------------------------------------------------------	
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(pOwnerEntity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// keep action on server for replication
		if (!Replication.IsServer())
			return;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool GetDefibrillatorNetworkComponent(IEntity user, out ACE_MedicalDefibrillation_NetworkComponent networkComponent)
	{
		if (!user)
			return false;
		
		PlayerManager playerMgr = GetGame().GetPlayerManager();
		if (!playerMgr)
			return false;
		
		int userId = playerMgr.GetPlayerIdFromControlledEntity(user);
		if (userId == 0)
			return false;
		
		SCR_PlayerController userController = SCR_PlayerController.Cast(playerMgr.GetPlayerController(userId));
		if (!userController)
			return false;
		
		networkComponent = ACE_MedicalDefibrillation_NetworkComponent.Cast(userController.FindComponent(ACE_MedicalDefibrillation_NetworkComponent));
		if (!networkComponent)
			return false;
		
		return true;
	}
}