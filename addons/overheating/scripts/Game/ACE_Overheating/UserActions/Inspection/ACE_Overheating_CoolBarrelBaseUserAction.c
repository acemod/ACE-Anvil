//------------------------------------------------------------------------------------------------
class ACE_Overheating_CoolBarrelBaseUserAction : SCR_InspectionUserAction
{
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
		
		OnPreformCoolingStep();
		return overProgress;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnPreformCoolingStep();
}
