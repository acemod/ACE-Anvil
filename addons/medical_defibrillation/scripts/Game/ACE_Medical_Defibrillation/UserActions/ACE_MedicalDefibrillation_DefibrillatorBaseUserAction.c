class ACE_MedicalDefibrillator_DefibrillatorBaseUserAction : ScriptedUserAction
{	
	//------------------------------------------------------------------------------------------------	
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// keep action on server for replication
		if (!Replication.IsServer())
			return;
	}
}