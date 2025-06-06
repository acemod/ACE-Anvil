//------------------------------------------------------------------------------------------------
//! Central effect responsible for applying blood loss from all registered SCR_BleedingDamageEffect
class ACE_Medical_BloodLossDamageEffect : SCR_DotDamageEffect
{
	protected ref array<SCR_BleedingDamageEffect> m_aDamageEffects = {};
	
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDamageManager)
			SetAffectedHitZone(charDamageManager.GetBloodHitZone());
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!charDamageManager)
			return;
		
		HandleBloodLoss(timeSlice, charDamageManager, ComputeTotalBloodLoss(timeSlice, charDamageManager));
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTotalBloodLoss(float timeSlice, SCR_CharacterDamageManagerComponent damageManager)
	{
		float bloodLoss = 0;
		
		foreach (SCR_BleedingDamageEffect damageEffect : m_aDamageEffects)
		{
			bloodLoss += ComputeBloodLossForDamageEffect(timeSlice, damageManager, damageEffect);
		}
		
		return bloodLoss;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeBloodLossForDamageEffect(float timeSlice, SCR_CharacterDamageManagerComponent damageManager, SCR_BleedingDamageEffect damageEffect)
	{
		float bloodLoss = damageEffect.GetDPS() * damageEffect.GetAccurateTimeSlice(timeSlice);
		
		// Check if bleeding hitzone also has a tourniquet on it
		SCR_CharacterHitZone affectedHitZone = SCR_CharacterHitZone.Cast(damageEffect.GetAffectedHitZone());
		if (affectedHitZone && damageManager.GetGroupTourniquetted(affectedHitZone.GetHitZoneGroup()))
			bloodLoss *= damageManager.GetTourniquetStrengthMultiplier();
		
		return bloodLoss;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HandleBloodLoss(float timeSlice, SCR_CharacterDamageManagerComponent damageManager, float loss)
	{
		DotDamageEffectTimerToken token = UpdateTimer(timeSlice, damageManager);
		DealCustomDot(GetAffectedHitZone(), loss, token, damageManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.BLEEDING;
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterDamageEffect(SCR_ExtendedDamageManagerComponent damageManager, SCR_BleedingDamageEffect damageEffect)
	{
		m_aDamageEffects.Insert(damageEffect);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate blood loss damage effect when no more bleeding damage effects are registered
	void UnregisterDamageEffect(SCR_ExtendedDamageManagerComponent damageManager, SCR_BleedingDamageEffect damageEffect)
	{
		m_aDamageEffects.RemoveItem(damageEffect);
		if (!m_aDamageEffects.IsEmpty())
			return;
		
		damageManager.TerminateDamageEffect(this);
	}
}
