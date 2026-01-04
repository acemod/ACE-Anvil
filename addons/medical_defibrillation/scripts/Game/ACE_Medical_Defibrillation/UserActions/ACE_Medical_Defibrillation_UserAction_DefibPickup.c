class ACE_Medical_Defibrillation_UserAction_DefibPickup : SCR_PickUpItemAction
{	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (Replication.IsServer())
		{
			ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibComponent));
			if (!defibComponent)
				return;
			
			defibComponent.ResetPatient();
			// Unsubscribe from system
			defibComponent.GetDefibStatesSystem().Unregister(pOwnerEntity);
		}
		
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
}