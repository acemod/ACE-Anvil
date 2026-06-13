class ACE_Medical_Defibrillation_UserAction_DefibDisconnect : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		World world = GetGame().GetWorld();
		if (!world)
			return;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		super.CanBeShownScript(user);
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return false;
		
		IEntity patient = defibComponent.GetPatient();
		if (!patient)
		{
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return;
		
		defibComponent.ResetPatient();
		
		ACE_Medical_NetworkComponent networkComponent = ACE_Medical_Defibrillation_GlobalHelpers.GetMedicalNetworkComponent(SCR_ChimeraCharacter.Cast(pUserEntity));
		if (!networkComponent)
			return;

		networkComponent.RequestDefibNotification(ENotification.ACE_MEDICAL_DEFIBRILLATION_DISCONNECTED, SCR_ChimeraCharacter.Cast(pOwnerEntity));
	}
}