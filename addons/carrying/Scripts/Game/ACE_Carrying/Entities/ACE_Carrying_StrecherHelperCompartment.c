//------------------------------------------------------------------------------------------------
class ACE_Carrying_StrecherHelperCompartmentClass : ACE_Carrying_HelperCompartmentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Carrying_StrecherHelperCompartment : ACE_Carrying_HelperCompartment
{	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity carrier, IEntity carried)
	{
		m_pCarrier = carrier;
		m_pCarried = carried;
		
		carrier.AddChild(this, -1);

		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (!rpl)
			return;
		
		SCR_CharacterControllerComponent carriedController = SCR_CharacterControllerComponent.Cast(carried.FindComponent(SCR_CharacterControllerComponent));
		if (!carriedController)
			return;
		
		carriedController.m_OnLifeStateChanged.Insert(OnCarriedLifeStateChanged);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(carried.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		// Clean-up when carried has left the comparment
		compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
		
		compartmentAccess.MoveInVehicle(this, ECompartmentType.Cargo);
		
	}
	
	override void RpcDo_Owner_CleanUp(RplId carriedId)
	{
		// nothing to cleanup
	}
	
	void SetTransformPoint(notnull PointInfo transformPoint)
	{
		vector transform[4];
		transformPoint.GetLocalTransform(transform);
		SetLocalTransform(transform);
	}
}
