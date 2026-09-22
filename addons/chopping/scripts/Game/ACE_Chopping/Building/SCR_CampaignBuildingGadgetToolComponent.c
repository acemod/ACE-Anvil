//------------------------------------------------------------------------------------------------
//! Adding dynamic spawning/despawning of chopping helper entities to building preview handlers
modded class SCR_CampaignBuildingGadgetToolComponent : SCR_GadgetComponent
{
	protected static ref ACE_Chopping_Config s_ACE_Chopping_Config;	
	protected ref array<IEntity> m_aACE_Chopping_PlantsInRange = {};
	protected ref array<IEntity> m_aACE_Chopping_PlantsInRangeOld = {};
	protected ref array<IEntity> m_aACE_Chopping_HelperEntities = {};
	
	protected static const ResourceName ACE_CHOPPING_CONFIG_NAME = "{B205BD6D8069137B}Config/ACE/Chopping.conf";
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		if (!s_ACE_Chopping_Config)
			s_ACE_Chopping_Config = SCR_ConfigHelperT<ACE_Chopping_Config>.GetConfigObject(ACE_CHOPPING_CONFIG_NAME);
	}
	
	//------------------------------------------------------------------------------------------------
	override void ToolToInventory()
	{
		super.ToolToInventory();
		
		if (System.IsConsoleApp())
			return;
		
		ACE_Chopping_RemoveHelpers();
	}

	//------------------------------------------------------------------------------------------------
	// Remove all helpers when E-tool is put back in inventory
	protected void ACE_Chopping_RemoveHelpers()
	{
		foreach (IEntity helper : m_aACE_Chopping_HelperEntities)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(helper);
		}
		
		m_aACE_Chopping_PlantsInRange.Clear();
		m_aACE_Chopping_PlantsInRangeOld.Clear();
		m_aACE_Chopping_HelperEntities.Clear();
	}

	//------------------------------------------------------------------------------------------------
	//! Trace the trees in range
	override protected void FoliageDetection()
	{
		super.FoliageDetection();
		
		// Delete helper if the associated plant is no longer in range
		for (int i = m_aACE_Chopping_PlantsInRangeOld.Count() - 1; i >= 0; i--)
		{
			IEntity plant = m_aACE_Chopping_PlantsInRangeOld[i];
			if (!plant || !m_aACE_Chopping_PlantsInRange.Contains(plant))
			{
				SCR_EntityHelper.DeleteEntityAndChildren(m_aACE_Chopping_HelperEntities[i]);
				m_aACE_Chopping_PlantsInRangeOld.Remove(i);
				m_aACE_Chopping_HelperEntities.Remove(i);
			}	
		}
		
		// Create helpers for new plants in range
		foreach (IEntity plant : m_aACE_Chopping_PlantsInRange)
		{
			if (plant && !m_aACE_Chopping_PlantsInRangeOld.Contains(plant))
			{
				m_aACE_Chopping_HelperEntities.Insert(s_ACE_Chopping_Config.SpawnHelper(plant));
				m_aACE_Chopping_PlantsInRangeOld.Insert(plant);
			}
		}
		
		m_aACE_Chopping_PlantsInRange.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool FilterFoliage(IEntity ent)
	{
		return Tree.Cast(ent);
	}

	//------------------------------------------------------------------------------------------------
	override protected bool EvaluateBush(IEntity ent)
	{
		m_aACE_Chopping_PlantsInRange.Insert(ent);
		return true;
	}
}
