class ACE_Medical_AEDBaseUserAction : ScriptedUserAction
{
	protected ACE_Medical_AEDComponent m_AEDComp;
	
	//------------------------------------------------------------------------------------------------	
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_AEDComp = ACE_Medical_AEDComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_AEDComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		PrintFormat("%1.PerformAction:: NOT IMPLIMENTED", this, level: LogLevel.ERROR);
	}
}