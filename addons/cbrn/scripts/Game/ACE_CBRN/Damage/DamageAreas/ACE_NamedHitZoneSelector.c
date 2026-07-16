//------------------------------------------------------------------------------------------------
class ACE_NamedHitZoneSelector : SCR_BaseHitZoneSelector
{
	[Attribute(desc: "Name of the hit zone to select")]
	protected string m_sName;
	
	//------------------------------------------------------------------------------------------------
	override HitZone SelectHitZone(SCR_DamageManagerComponent dmgMgr, SCR_EHitZoneSelectionMode hitZoneSelectionMode)
	{
		return dmgMgr.GetHitZoneByName(m_sName);
	}
}
