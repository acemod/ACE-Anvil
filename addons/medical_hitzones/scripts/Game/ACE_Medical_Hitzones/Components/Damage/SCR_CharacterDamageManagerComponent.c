//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize organ hit zones
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		if (!m_pACE_Medical_Settings || !m_pACE_Medical_Settings.m_bOrganHitZonesEnabled)
			return;
		
		array<HitZone> hitZones = {};
		GetPhysicalHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (charHitZone)
				charHitZone.ACE_Medical_InitOrganHitZones(this);
		}
	}
}
