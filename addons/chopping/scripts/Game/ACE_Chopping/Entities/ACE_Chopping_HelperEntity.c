//----------------------------------------------------------------------------------------
class ACE_Chopping_HelperEntityClass : GenericEntityClass
{
}

//----------------------------------------------------------------------------------------
class ACE_Chopping_HelperEntity : GenericEntity
{
	protected IEntity m_pAssociatedPlant;
	
	//----------------------------------------------------------------------------------------
	void SetAssociatedPlant(IEntity plant)
	{
		m_pAssociatedPlant = plant;
	}
	
	//----------------------------------------------------------------------------------------
	IEntity GetAssociatedPlant()
	{
		return m_pAssociatedPlant;
	}
}
