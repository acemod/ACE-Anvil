//-----------------------------------------------------------------------------------------------------------
modded class ACE_Medical_HipsHitZone : SCR_CharacterHitZone
{
	protected ref array<ACE_Medical_FemoralArteryHitZone> m_aFemoralArteries = {};
	
	//-----------------------------------------------------------------------------------------------------------
	//! Add bleeding from femoral arteries
	override float ACE_Medical_CalculateBleedingRate()
	{
		float rate = super.ACE_Medical_CalculateBleedingRate();
		
		array<HitZone> hitZones = {};
		GetHitZoneContainer().GetAllHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			ACE_Medical_FemoralArteryHitZone femoralArtery = ACE_Medical_FemoralArteryHitZone.Cast(hitZone);
			if (femoralArtery)
				rate += femoralArtery.CalculateBleedingRate();
		}
		
		return rate;
	}
}
