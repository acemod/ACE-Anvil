//------------------------------------------------------------------------------------------------
//! Explosive disarm user action
class ACE_Explosives_DisarmUserAction : ACE_WrenchUserAction
{	
	protected SCR_ExplosiveChargeComponent m_pChargeComponent;
	protected SCR_PressureTriggerComponent m_pTriggerComponent;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_pChargeComponent = SCR_ExplosiveChargeComponent.Cast(pOwnerEntity.FindComponent(SCR_ExplosiveChargeComponent));
		m_pTriggerComponent = SCR_PressureTriggerComponent.Cast(pOwnerEntity.FindComponent(SCR_PressureTriggerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Disarm the explosive
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		if (m_pChargeComponent)
		{
			// Disarm explosives
			m_pChargeComponent.DisarmCharge();
		}
		else
		{
			// Delete mines
			SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		if (m_pChargeComponent && m_pChargeComponent.GetUsedFuzeType() == SCR_EFuzeType.NONE)
			return false;
		
		if (m_pTriggerComponent && !m_pTriggerComponent.IsActivated())
			return false;

		return true;
	}
}
