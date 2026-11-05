//------------------------------------------------------------------------------------------------
class ACE_Overheating_CoolBarrelInWaterUserAction : ACE_Overheating_CoolBarrelBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_CharacterBuoyancyComponent buoyancy = SCR_CharacterBuoyancyComponent.Cast(user.FindComponent(SCR_CharacterBuoyancyComponent));
		if (!buoyancy)
			return false;

		return buoyancy.IsInWater();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPreformCoolingStep()
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (playerController)
			playerController.ACE_Overheating_RequestCoolBarrel(m_WeaponComponent);
	}
}
