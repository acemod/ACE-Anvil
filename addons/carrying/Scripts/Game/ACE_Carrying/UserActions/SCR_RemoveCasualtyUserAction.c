//------------------------------------------------------------------------------------------------
modded class SCR_RemoveCasualtyUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter casualty = SCR_ChimeraCharacter.Cast(ACE_Carrying_Tools.GetCarried(pUserEntity));
		if (casualty)
		{
			PerformLoadCasualtyAction(pUserEntity, casualty);
		}
		else
		{
			super.PerformAction(pOwnerEntity, pUserEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PerformLoadCasualtyAction(IEntity user, IEntity casualty)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return;
		
		if (compartment.IsOccupied() || !compartment.IsCompartmentAccessible() || compartment.IsReserved())
			return;

		SCR_CompartmentAccessComponent casualtyCompartmentAccess = SCR_CompartmentAccessComponent.Cast(casualty.FindComponent(SCR_CompartmentAccessComponent));
		if (!casualtyCompartmentAccess)
			return;
		
		ACE_Carrying_Tools.LoadCarriedInVehicle(casualty, compartment, GetRelevantDoorIndex(user));
	}
	
	/*
	//------------------------------------------------------------------------------------------------	
	protected void PerformRemoveCasualtyAction(IEntity user)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return;
		
		IEntity casualty = SCR_ChimeraCharacter.Cast(compartment.GetOccupant());
		if (!casualty)
			return;
		
		ACE_Carrying_Tools.Carry(user, casualty, GetRelevantDoorIndex(user));
	}
	*/
		
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		IEntity user = SCR_PlayerController.GetLocalControlledEntity();
		if (!user || !CanLoadCasualtyBeShown(user))
			return super.GetActionNameScript(outName);
		
		return GetLoadCasualtyActionName(outName);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool GetLoadCasualtyActionName(out string outName)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		UIInfo compartmentInfo = compartment.GetUIInfo();
		if (!compartmentInfo)
			return false;
				
		outName = "#ACE_Carrying-UserAction_LoadCasualty" + "%CTX_HACK%" + compartmentInfo.GetName();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		return CanLoadCasualtyBePerformed(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanLoadCasualtyBePerformed(IEntity user)
	{
		if (!ACE_Carrying_Tools.IsCarrier(user))
			return true;
		
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

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (super.CanBeShownScript(user))
			return true;
		
		return CanLoadCasualtyBeShown(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanLoadCasualtyBeShown(IEntity user)
	{
		return ACE_Carrying_Tools.IsCarrier(user);
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}
