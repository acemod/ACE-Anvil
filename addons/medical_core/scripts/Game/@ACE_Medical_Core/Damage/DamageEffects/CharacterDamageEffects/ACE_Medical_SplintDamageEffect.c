//------------------------------------------------------------------------------------------------
class ACE_Medical_SplintDamageEffect: SCR_DotDamageEffect
{
	protected ref array<HitZone> m_aAffectedHitZones = {};
	protected float m_fMaxHealScaled;
	
	//------------------------------------------------------------------------------------------------
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!charDamageManager)
			return;
		
		m_fMaxHealScaled = charDamageManager.ACE_Medical_GetSplintMaxHealScaled();
		
		SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(GetAffectedHitZone());
		if (!charHitZone)
			return;
		
		charDamageManager.GetHitZonesOfGroup(charHitZone.GetHitZoneGroup(), m_aAffectedHitZones);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		float accurateTimeSlice = GetAccurateTimeSlice(timeSlice);
		float damage = GetDPS() * accurateTimeSlice;
		DotDamageEffectTimerToken token = UpdateTimer(accurateTimeSlice, dmgManager);
		
		bool isHealing = false;
		
		foreach(HitZone hitZone : m_aAffectedHitZones)
		{
			float missingHealth = (m_fMaxHealScaled - hitZone.GetHealthScaled()) * hitZone.GetMaxHealth();
			if (missingHealth <= 0)
				continue;
			
			DealCustomDot(hitZone, Math.Max(damage, -missingHealth), token, dmgManager);
			isHealing = true;
		}
		
		if (!isHealing && !IsProxy())
			dmgManager.TerminateDamageEffect(this);
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}
