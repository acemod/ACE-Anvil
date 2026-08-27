//------------------------------------------------------------------------------------------------
class ACE_Overheating_CoolBarrelInWaterUserAction : SCR_InspectionUserAction
{
	protected float m_fTimer;
	
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
	override protected void OnActionStart(IEntity pUserEntity)
	{
		SCR_UISoundEntity.SoundEvent("ACE_OVERHEATING_SOUND_COOL_BARREL");
	}
	
	//------------------------------------------------------------------------------------------------
	override float GetActionProgressScript(float fProgress, float timeSlice)
	{
		fProgress = super.GetActionProgressScript(fProgress, timeSlice);
		float overProgress = fProgress - GetActionDuration();
		
		if (overProgress < 0)
			return fProgress;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (playerController)
			playerController.ACE_Overheating_RequestCoolBarrel(m_WeaponComponent);
		
		return overProgress;
	}
}
