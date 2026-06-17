//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_DamagePassRuleContainerTitle()]
modded class SCR_LimbDamagePassRule : SCR_BaseDamagePassRule
{
	//------------------------------------------------------------------------------------------------
	//! Turn off spillovers to other hit zone groups
	override void HandlePassedDamage(inout notnull BaseDamageContext damageContext, notnull SCR_HitZone srcHitZone, notnull SCR_DamageManagerComponent dmgManager)
	{
		m_eSpilloverHitZoneGroup = -1;
		super.HandlePassedDamage(damageContext, srcHitZone, dmgManager);
	}
}
