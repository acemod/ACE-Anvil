class ACE_Medical_DefibrillatorBaseUserAction : ScriptedUserAction
{
	protected ACE_Medical_DefibrillatorComponent m_defibrillatorComponent;
	
	//------------------------------------------------------------------------------------------------	
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_DefibrillatorComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// keep action on server for replication
		if (!Replication.IsServer())
			return;
	}
}