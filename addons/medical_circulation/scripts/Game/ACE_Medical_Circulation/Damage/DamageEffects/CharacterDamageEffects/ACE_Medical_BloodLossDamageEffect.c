//------------------------------------------------------------------------------------------------
//! Central effect responsible for applying all blood loss to SCR_CharacterBloodHitZone
modded class ACE_Medical_BloodLossDamageEffect : SCR_DotDamageEffect
{
	protected static ACE_Medical_Circulation_Settings s_pCirculationSettings;
	protected ACE_Medical_VitalsComponent m_pVitalsComponent;
	protected ref array<SCR_BleedingDamageEffect> m_aDamageEffects = {};
	
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		if (super.HijackDamageEffect(dmgManager))
			return true;
		
		if (!s_pCirculationSettings)
			s_pCirculationSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
		
		IEntity char = dmgManager.GetOwner();
		if (!char)
			return true;
		
		m_pVitalsComponent = ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Limit total blood loss by cardiac output and scale by blood flow scale
	override protected float ComputeEffectiveTotalBloodLoss(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		float totalBloodLoss = super.ComputeEffectiveTotalBloodLoss(timeSlice, dmgManager);
		float maxBleedingRate = Math.Max(s_pCirculationSettings.m_fCardiacArrestMaxTotalBleedingRate, m_pVitalsComponent.GetCardiacOutput() / 60);
		totalBloodLoss = Math.Min(totalBloodLoss, maxBleedingRate * timeSlice);
		
		SCR_CharacterDamageManagerComponent charDmgManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (dmgManager)
			totalBloodLoss *= charDmgManager.ACE_Medical_GetBloodFlowScale();
		
		return totalBloodLoss;
	}
}
