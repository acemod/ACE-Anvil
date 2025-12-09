//------------------------------------------------------------------------------------------------
//! Add handling for cook-off
modded class SCR_FlammableHitZone : SCR_VehicleHitZone
{
	//------------------------------------------------------------------------------------------------
	override void SetFireState(SCR_EBurningState fireState, float weight = -1, bool changeRate = true)
	{
		if (!IsProxy())
			ACE_CookOff_HandleCookOff(fireState);
		
		super.SetFireState(fireState, weight, changeRate);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_CookOff_HandleCookOff(SCR_EBurningState fireState)
	{
		bool isBurning = (m_eFireState == SCR_EBurningState.BURNING);
		bool shouldBeBurning = (fireState == SCR_EBurningState.BURNING);
		
		if (isBurning == shouldBeBurning)
			return;
		
		ACE_CookOff_DetonateAmmoSystem system = ACE_CookOff_DetonateAmmoSystem.GetInstance();
		if (!system)
			return;
		
		if (shouldBeBurning)
			system.Register(GetOwner());
		else
			system.Unregister(GetOwner());
	}
}
