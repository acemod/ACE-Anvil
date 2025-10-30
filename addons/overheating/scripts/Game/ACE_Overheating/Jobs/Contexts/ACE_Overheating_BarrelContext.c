//------------------------------------------------------------------------------------------------
class ACE_Overheating_BarrelContext : ACE_FrameJobScheduler_IObjectContext<ACE_Overheating_BarrelComponent>
{
	SCR_ChimeraCharacter m_pOwnerChar;
	ACE_Overheating_BarrelGlowEffectComponent m_pGlowEffect;
	ACE_Overheating_SmokeEffectComponent m_pSmokeEffect;
	ACE_Overheating_HelperAttachmentComponent m_pHelperAttachment;
	bool m_bIsChamberingPossible;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_BarrelContext(ACE_Overheating_BarrelComponent object)
	{
		m_pOwnerChar = SCR_ChimeraCharacter.Cast(object.GetOwner().GetParent());
		MuzzleComponent muzzle = object.GetMuzzle();
		m_bIsChamberingPossible = muzzle.IsChamberingPossible();
		m_pHelperAttachment = ACE_Overheating_HelperAttachmentComponent.Cast(muzzle.FindComponent(ACE_Overheating_HelperAttachmentComponent));
		m_pSmokeEffect = ACE_Overheating_SmokeEffectComponent.FromMuzzle(muzzle);
		m_pGlowEffect = ACE_Overheating_BarrelGlowEffectComponent.FromMuzzle(muzzle);
	}
}
