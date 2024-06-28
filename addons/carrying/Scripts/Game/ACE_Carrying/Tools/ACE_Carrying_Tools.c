//------------------------------------------------------------------------------------------------
//! Helper methods for carrying
class ACE_Carrying_Tools
{
	protected static const ResourceName HELPER_CARRY_PREFAB_NAME = "{FF78613C1DAFF28F}Prefabs/Helpers/ACE_Carrying_HelperCompartment.et";
	protected static const ResourceName HELPER_STRECHER_PREFAB_NAME = "{F9861A601F934DE7}Prefabs/Helpers/ACE_Carrying_StrecherHelperCompartment.et";
	
	//------------------------------------------------------------------------------------------------
	//! Start <carrier> to carry the specified <carried>
	static void Carry(notnull IEntity carrier, notnull IEntity carried)
	{
		ACE_Carrying_HelperCompartment helper = ACE_Carrying_HelperCompartment.Cast(GetGame().SpawnEntityPrefab(Resource.Load(HELPER_CARRY_PREFAB_NAME), null, EntitySpawnParams()));
		helper.Init(carrier, carried);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Release the carried player by passing the carrier
	//! Calls Release method on helper compartment entity
	static void ReleaseFromCarrier(notnull IEntity carrier)
	{
		ACE_Carrying_HelperCompartment helper = GetHelperCompartmentFromCarrier(carrier);
		
		if (!helper)
			return;
		
		helper.Terminate();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Release the carried player
	//! Calls Release method on helper compartment entity
	static void ReleaseCarried(notnull IEntity carried)
	{
		ACE_Carrying_HelperCompartment helper = GetHelperCompartmentFromCarried(carried);
		
		if (!helper)
			return;
		
		helper.Terminate();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Place the carried player on entity
	static void PlaceOnEntity(notnull IEntity entity, notnull IEntity carried, notnull PointInfo placementInfo)
	{
		ACE_Carrying_StrecherHelperCompartment helper = ACE_Carrying_StrecherHelperCompartment.Cast(GetGame().SpawnEntityPrefab(Resource.Load(HELPER_STRECHER_PREFAB_NAME), null, EntitySpawnParams()));
		helper.Init(entity, carried);
		helper.SetTransformPoint(placementInfo);
	}
	
	//------------------------------------------------------------------------------------------------
	//! True if the given player is currently a carrier
	static bool IsCarrier(IEntity carrier)
	{
		if (!carrier)
			return false;
		
		return GetHelperCompartmentFromCarrier(carrier);
	}
	
	//------------------------------------------------------------------------------------------------
	//! True if the given player is currently carried by another player
	static bool IsCarried(IEntity carried)
	{
		if (!carried)
			return false;
		
		ACE_Carrying_HelperCompartment compartment = GetHelperCompartmentFromCarried(carried);
		if (!compartment)
			return false;
		
		return compartment.IsCarrierPlayer();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the carrier that carries the given player
	static IEntity GetCarrier(notnull IEntity carried)
	{
		ACE_Carrying_HelperCompartment helper = GetHelperCompartmentFromCarried(carried);
		if (!helper)
			return null;
		
		return helper.GetCarrier();
	}

	//------------------------------------------------------------------------------------------------
	//! Get the player that is carried by the given carrier
	static IEntity GetCarried(notnull IEntity carrier)
	{
		ACE_Carrying_HelperCompartment helper = GetHelperCompartmentFromCarrier(carrier);
		if (!helper)
			return null;
		
		return helper.GetCarried();
	}

	//------------------------------------------------------------------------------------------------
	//! Get the instance of the helper compartment entity for the given carrier
	protected static ACE_Carrying_HelperCompartment GetHelperCompartmentFromCarrier(notnull IEntity carrier)
	{
		ACE_Carrying_HelperCompartment helper;
		IEntity child = carrier.GetChildren();
		
		while (child)
		{
			helper = ACE_Carrying_HelperCompartment.Cast(child);
			
			if (helper)
				break;
			
			child = child.GetSibling();
		}
		
		return helper;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the instance of the helper compartment entity for the given carried player
	protected static ACE_Carrying_HelperCompartment GetHelperCompartmentFromCarried(notnull IEntity carried)
	{
		return ACE_Carrying_HelperCompartment.Cast(carried.GetParent());
	}
}
