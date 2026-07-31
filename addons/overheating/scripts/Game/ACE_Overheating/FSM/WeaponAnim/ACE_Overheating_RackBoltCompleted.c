//------------------------------------------------------------------------------------------------
class ACE_Overheating_RackBoltCompleted : ACE_FSM_ITransition<ACE_Overheating_WeaponAnimContext>
{
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Overheating_WeaponAnimContext context, float timeSlice)
	{
		return !context.IsReloading();
	}
}
