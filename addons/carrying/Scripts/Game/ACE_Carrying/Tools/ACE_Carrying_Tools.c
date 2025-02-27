/*
//------------------------------------------------------------------------------------------------
//! Helper methods for carrying
class ACE_Carrying_Tools
{
	//------------------------------------------------------------------------------------------------
	//! Start <carrier> to carry the specified <carried>
	static void Carry(notnull SCR_ChimeraCharacter carrier, notnull SCR_ChimeraCharacter carried, ACE_EAnimationHelperID type = ACE_EAnimationHelperID.CARRIED)
	{
		ACE_AnimationHelperCompartment helper = ACE_AnimationTools.AnimateWithHelperCompartment(type, carried);
		if (!helper)
			return;
		
		ACE_CarriableAnimationHelperComponent carriable = ACE_CarriableAnimationHelperComponent.Cast(helper.FindComponent(ACE_CarriableAnimationHelperComponent));
		if (!carriable)
			return;
		
		carriable.Carry(carrier);
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
	//! True if the given player is currently a carrier
	static bool IsCarrier(IEntity carrier)
	{
		if (!carrier)
			return false;
		
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(carrier.FindComponent(SCR_CharacterControllerComponent));
		if (!controller)
			return false;
		
		return controller.ACE_IsCarrier();
	}
	
	//------------------------------------------------------------------------------------------------
	//! True if the given player is currently carried by another player
	static bool IsCarried(IEntity carried)
	{
		if (!carried)
			return false;
		
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(carried.FindComponent(SCR_CharacterControllerComponent));
		if (!controller)
			return false;
		
		return controller.ACE_IsCarried();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the carrier that carries the given player
	static IEntity GetCarrier(notnull IEntity carried)
	{
		ACE_Carrying_HelperCompartment helper = GetHelperCompartmentFromCarried(carried);
		if (!helper)
			return null;
		
		ACE_CarriableAnimationHelperComponent carriable = ACE_CarriableAnimationHelperComponent.Cast(helper.FindComponent(ACE_CarriableAnimationHelperComponent));
		if (!carriable)
			return null;
		
		return carriable.GetCarrier();
	}

	//------------------------------------------------------------------------------------------------
	//! Get the player that is carried by the given carrier
	static IEntity GetCarried(notnull IEntity carrier)
	{
		ACE_Carrying_HelperCompartment helper = GetHelperCompartmentFromCarrier(carrier);
		if (!helper)
			return null;
		
		return helper.GetPerformer();
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
		return ACE_Carrying_HelperCompartment.Cast(ACE_AnimationTools.GetHelperCompartment(carried));
	}
}
*/
