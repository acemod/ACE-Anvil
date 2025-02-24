//------------------------------------------------------------------------------------------------
//! Add action for loading carried casualties
modded class SCR_RemoveCasualtyUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (CanLoadCarriedBeShown(pUserEntity) && CanLoadCarriedBePerformed(pUserEntity))
		{
			PerformLoadCarriedAction(pOwnerEntity, pUserEntity);
			return;
		}
		
		PerformRemoveCasualtyAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PerformRemoveCasualtyAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PerformLoadCarriedAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return;
		
		if (compartment.IsOccupied() || !compartment.IsCompartmentAccessible() || compartment.IsReserved())
			return;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(pUserEntity.FindComponent(SCR_CharacterControllerComponent));
		if (!userCharController)
			return;
		
		ACE_AnimationHelperCompartment helper = ACE_AnimationHelperCompartment.Cast(userCharController.ACE_GetCarried());
		if (!helper)
			return;
		
		IEntity carried = helper.GetPerformer();
		if (!carried)
			return;

		SCR_CompartmentAccessComponent carriedCompartmentAccess = SCR_CompartmentAccessComponent.Cast(carried.FindComponent(SCR_CompartmentAccessComponent));
		if (!carriedCompartmentAccess)
			return;
		
		carriedCompartmentAccess.MoveInVehicle(compartment.GetVehicle(), compartment.GetType(), false, compartment);
	}
		
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		IEntity user = SCR_PlayerController.GetLocalControlledEntity();
		if (!user || !CanLoadCarriedBeShown(user))
			return super.GetActionNameScript(outName);
		
		return GetLoadCarriedActionName(outName);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool GetLoadCarriedActionName(out string outName)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		UIInfo compartmentInfo = compartment.GetUIInfo();
		if (!compartmentInfo)
			return false;
				
		outName = "#ACE-UserAction_LoadCarried" + "%CTX_HACK%" + compartmentInfo.GetName();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (super.CanBeShownScript(user))
			return true;
		
		return CanLoadCarriedBeShown(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanLoadCarriedBeShown(IEntity user)
	{
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
		if (!userCharController)
			return false;
		
		//! SCR_CharacterControllerComponent::ACE_IsCarrier would also be true if we carried other things than characters
		return ACE_AnimationHelperCompartment.Cast(userCharController.ACE_GetCarried());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (CanRemoveCasualtyBePerformed(user))
			return true;
		
		return CanLoadCarriedBePerformed(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanRemoveCasualtyBePerformed(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
		if (!userCharController)
			return false;
		
		return !userCharController.ACE_IsCarrier();
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanLoadCarriedBePerformed(IEntity user)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		if (compartment.IsOccupied() || compartment.IsReserved())
		{
			SetCannotPerformReason("#AR-UserAction_SeatOccupied");
			return false;
		}
		
		return true;
	}
}
