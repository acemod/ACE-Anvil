//------------------------------------------------------------------------------------------------
class ACE_CookOff_DetonateAmmoJob : ACE_IFrameJob
{
	protected ref ACE_CookOff_ObjectContext m_pContext = null;
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		
		if (!m_pContext.IsValid())
			return;
		
		m_pContext.m_fDetonationCountdown -= timeSlice;
		if (m_pContext.m_fDetonationCountdown > 0)
			return;
		
		m_pContext.m_fDetonationCountdown = Math.RandomFloat(0, 10) / Math.Sqrt(m_pContext.m_iTotalAmmoCount);
		
		SCR_WeightedArray<int> weights = new SCR_WeightedArray<int>();
		weights.Insert(0, m_pContext.m_aMagazines.Count());
		weights.Insert(1, m_pContext.m_aExplosives.Count());
		int option;
		weights.GetRandomValue(option);
		
		switch (option)
		{
			case 0: { DetonateAmmoInMagazine(m_pContext.m_aMagazines.GetRandomElement()); return; }
			case 1: { DetonateExplosive(m_pContext.m_aExplosives.GetRandomElement()); return; }
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetonateAmmoInMagazine(BaseMagazineComponent magazine)
	{
		if (!magazine)
		{
			m_pContext.m_aMagazines.RemoveItem(magazine);
			return;
		}
		
		m_pContext.m_pHelperEntity.DetonateAmmoInMagazine(magazine, m_pContext.m_pObject);
		
		float prevAmmoCount = magazine.GetAmmoCount();
		float detonatedAmmoCount = Math.Min(prevAmmoCount, Math.RandomIntInclusive(1, 5));
		float newAmmoCount = prevAmmoCount - detonatedAmmoCount;
		m_pContext.m_iTotalAmmoCount -= detonatedAmmoCount;
		magazine.SetAmmoCount(newAmmoCount);
		
		if (newAmmoCount == 0)
		{
			m_pContext.m_aMagazines.RemoveItem(magazine);
			m_pContext.m_pStorageManager.TryDeleteItem(magazine.GetOwner(), null);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetonateExplosive(BaseTriggerComponent explosive)
	{
		m_pContext.m_aExplosives.RemoveItem(explosive);
		--m_pContext.m_iTotalAmmoCount;
		
		if (!explosive)
			return;
		
		if (Math.RandomFloat(0, 1) < 0.5)
		{
			m_pContext.m_pStorageManager.TryDeleteItem(explosive.GetOwner(), null);
			return;
		}

		m_pContext.m_pHelperEntity.SpawnExplosion(explosive.GetOwner().GetPrefabData().GetPrefabName(), m_pContext.m_pObject);
		m_pContext.m_pStorageManager.TryDeleteItem(explosive.GetOwner(), null);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetContext(ACE_CookOff_ObjectContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_CookOff_ObjectContext GetContext()
	{
		return m_pContext;
	}
}
