//------------------------------------------------------------------------------------------------
class ACE_Medical_SplintDamageEffect: SCR_DotDamageEffect
{
	protected ref array<HitZone> m_aAffectedHitZones = {};

	[Attribute(defvalue: "0.667", desc: "Maximum health ratio that the splint can heal limbs up to.")]
	protected float m_fACE_SplintHealTarget;

	[Attribute(defvalue: "0.5", desc: "Healing speed multiplier for the splint effect.")]
	protected float m_fACE_SplintHealMultiplier;
	
	//------------------------------------------------------------------------------------------------
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!charDamageManager)
			return;
		
		SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(GetAffectedHitZone());
		if (!charHitZone)
			return;
		
		charDamageManager.GetHitZonesOfGroup(charHitZone.GetHitZoneGroup(), m_aAffectedHitZones);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		float accurateTimeSlice = GetAccurateTimeSlice(timeSlice);
		float damage = GetDPS() * m_fACE_SplintHealMultiplier * accurateTimeSlice;
		DotDamageEffectTimerToken token = UpdateTimer(accurateTimeSlice, dmgManager);
		
		foreach (HitZone hitZone : m_aAffectedHitZones)
		{
			if (hitZone.GetHealthScaled() < m_fACE_SplintHealTarget)
				DealCustomDot(hitZone, damage, token, dmgManager);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}