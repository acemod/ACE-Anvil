//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class SCR_ResourceContainer : SCR_ResourceActor
{
	//------------------------------------------------------------------------------------------------
	override void CopyFromContainer(notnull SCR_ResourceContainer container)
	{
		super.CopyFromContainer(container);
		m_fResourceDecayTimeout = container.m_fResourceDecayTimeout;
	}
}
