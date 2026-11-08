//------------------------------------------------------------------------------------------------
//! Match items connected to m_DetonatorComponent
class ACE_NotConnectedToDetonatorItemPredicate : ACE_InventorySearchPredicate
{
	SCR_DetonatorGadgetComponent m_DetonatorComponent;
	
	//------------------------------------------------------------------------------------------------
	void ACE_NotConnectedToDetonatorItemPredicate()
	{
		QueryComponentTypes.Insert(SCR_ExplosiveChargeComponent);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SCR_ExplosiveChargeComponent component = SCR_ExplosiveChargeComponent.Cast(queriedComponents[0]);
		return !m_DetonatorComponent.IsAttachedToTheDetonator(component);
	}
}
