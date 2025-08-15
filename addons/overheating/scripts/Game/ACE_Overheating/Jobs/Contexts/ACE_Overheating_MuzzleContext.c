//------------------------------------------------------------------------------------------------
class ACE_Overheating_MuzzleContext : ACE_FrameJobScheduler_IObjectContext<ACE_Overheating_MuzzleJamComponent>
{
	SCR_ChimeraCharacter m_pOwnerChar;
	bool m_bIsChamberingPossible;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_MuzzleContext(ACE_Overheating_MuzzleJamComponent object)
	{
		m_pOwnerChar = SCR_ChimeraCharacter.Cast(object.GetOwner().GetParent());
		m_bIsChamberingPossible = object.GetMuzzle().IsChamberingPossible();
	}
}
