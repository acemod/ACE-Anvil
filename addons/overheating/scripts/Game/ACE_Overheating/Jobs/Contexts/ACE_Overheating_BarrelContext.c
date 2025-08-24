//------------------------------------------------------------------------------------------------
class ACE_Overheating_BarrelContext : ACE_FrameJobScheduler_IObjectContext<ACE_Overheating_BarrelComponent>
{
	SCR_ChimeraCharacter m_pOwnerChar;
	ACE_Overheating_BarrelGlowComponent m_pGlowComponent;
	bool m_bIsChamberingPossible;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_BarrelContext(ACE_Overheating_BarrelComponent object)
	{
		m_pOwnerChar = SCR_ChimeraCharacter.Cast(object.GetOwner().GetParent());
		MuzzleComponent muzzle = object.GetMuzzle();
		m_pGlowComponent = ACE_Overheating_BarrelGlowComponent.FromMuzzle(muzzle);
		m_bIsChamberingPossible = muzzle.IsChamberingPossible();
	}
}
