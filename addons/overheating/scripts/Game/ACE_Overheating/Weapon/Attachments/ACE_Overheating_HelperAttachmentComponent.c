//------------------------------------------------------------------------------------------------
class ACE_Overheating_HelperAttachmentComponentClass : AttachmentSlotComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Helper attachment slot for modifying weapon stats from overheating
class ACE_Overheating_HelperAttachmentComponent : AttachmentSlotComponent
{
	protected bool m_bInitDone = false;
	protected ACE_Overheating_BarrelComponent m_pBarrel;
	protected IEntity m_pAttachedEntity;
	protected SCR_WeaponStatsManagerComponent m_pStatsManager;
	
	//------------------------------------------------------------------------------------------------
	void Init()
	{
		m_pAttachedEntity = GetAttachedEntity();
		m_pStatsManager = SCR_WeaponStatsManagerComponent.Cast(m_pAttachedEntity.GetParent().FindComponent(SCR_WeaponStatsManagerComponent));
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateStats(float muzzleDispersionFactor)
	{
		if (!m_bInitDone)
			Init();
		
		if (!m_pStatsManager || !m_pAttachedEntity)
			return;
		
		m_pStatsManager.SetMuzzleDispersionFactor(m_pAttachedEntity, 0, muzzleDispersionFactor);
		// Only known way to force sync in MP is through reattaching the entity
		SetAttachment(null);
		SetAttachment(m_pAttachedEntity);
		
	#ifdef ENABLE_DIAG
		m_fMuzzleDispersionFactor = muzzleDispersionFactor;
	#endif
	}
	
#ifdef ENABLE_DIAG
	protected float m_fMuzzleDispersionFactor;

	//------------------------------------------------------------------------------------------------
	float GetMuzzleDispersionFactor()
	{
		float factor;
		
		if (m_pStatsManager)
			m_pStatsManager.GetMuzzleDispersionFactor(0, factor);
		
		return factor;
	}
#endif
}
