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
			PrintFormat("%1::CanBeShownScript | Defib has no patient!", this.ClassName());
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("%1::PerformAction | Server Execution: %2", this.ClassName(), Replication.IsServer());
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return;
		
		defibComponent.ResetPatient();
	}
}