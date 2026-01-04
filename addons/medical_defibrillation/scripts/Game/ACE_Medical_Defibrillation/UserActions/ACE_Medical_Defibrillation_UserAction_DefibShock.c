class ACE_Medical_Defibrillation_UserActions_DefibShock : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		super.CanBeShownScript(user);
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return false;
		
		if (!defibComponent.GetPatient())
			return false;
		
		if (defibComponent.GetDefibStateID() != ACE_Medical_Defibrillation_EDefibStateID.CHARGED)
			return false;
		
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
		
		defibComponent.ShockPatient();
		return;
	}
}