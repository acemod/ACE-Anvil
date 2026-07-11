class ACE_Medical_Defibrillation_ScriptedUserAction_DefibShock : ScriptedUserAction
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
		
		if (defibComponent.TryShockPatient())
		{
			ACE_Medical_NetworkComponent networkComponent = ACE_Medical_Defibrillation_ComponentManager.GetMedicalNetworkComponent(SCR_ChimeraCharacter.Cast(pUserEntity));
			if (!networkComponent)
				return;
			
			networkComponent.RequestDefibNotification(ENotification.ACE_MEDICAL_DEFIBRILLATION_SHOCKDELIVERED, SCR_ChimeraCharacter.Cast(pOwnerEntity));
		}
		
		return;
	}
}