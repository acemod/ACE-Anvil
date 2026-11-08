//------------------------------------------------------------------------------------------------
//! Add speedup for packing between belt-based magazines
//! TODO: Find better way to detect belts than ammo capacity
modded class SCR_MagazineRepackingSystem : GameSystem
{
	protected ACE_MagRepack_Settings m_ACE_Settings;
	
	protected static const int ACE_MAGREPACK_SPEEDUP_AMMO_CAPACITY_THRESHOLD = 50;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		
		if (!m_bIsServer)
			return;
		
		m_ACE_Settings = ACE_SettingsHelperT<ACE_MagRepack_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void StartRepackingMagazines(notnull SCR_ChimeraCharacter character, SCR_MagazineComponent sourceMag, SCR_MagazineComponent targetMag, float transferTime = -1)
	{
		super.StartRepackingMagazines(character, sourceMag, targetMag, transferTime);
		
		if (!m_bIsServer)
			return;
		
		float timeScale = m_ACE_Settings.m_fPackingTimeScale;
		
		if (ACE_CanLinkBelts(sourceMag, targetMag))
			timeScale = m_ACE_Settings.m_fBeltPackingTimeScale;
		
		if (float.AlmostEqual(1.0, timeScale))
			return;
		
		SCR_MagRepackingData data = m_mRepackingData.Get(character);
		if (!data)
			return;
		
		transferTime = data.GetTimePerRound() * timeScale;
		data.SetTimePerRound(transferTime);
		character.SetRepackingState_S(true, transferTime);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ACE_CanLinkBelts(SCR_MagazineComponent sourceMag, SCR_MagazineComponent targetMag)
	{
		if (!sourceMag || sourceMag.GetMaxAmmoCount() < ACE_MAGREPACK_SPEEDUP_AMMO_CAPACITY_THRESHOLD)
			return false;
		
		if (!targetMag || targetMag.GetMaxAmmoCount() < ACE_MAGREPACK_SPEEDUP_AMMO_CAPACITY_THRESHOLD)
			return false;
		
		return true;
	}
}
