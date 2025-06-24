//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_ExtendedDamageManagerComponent
{
	[Attribute(defvalue: "20", desc: "Threshold for severe bleeding rate")]
	protected float m_fACE_SevereBleedingRateThreshold;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Return the health code color based on bleeding rate
	SCR_ENearbyInteractionContextColors ACE_GetGroupBleedingStatus(ECharacterHitZoneGroup hitZoneGroup)
	{
		float bleedingRate = GetGroupDamageOverTime(hitZoneGroup, EDamageType.BLEEDING);
		if (float.AlmostEqual(bleedingRate, 0))
			return SCR_ENearbyInteractionContextColors.DEFAULT;
		
		if (GetGroupTourniquetted(hitZoneGroup))
			return SCR_ENearbyInteractionContextColors.STABLE;
		
		if (bleedingRate > m_fACE_SevereBleedingRateThreshold)
			return SCR_ENearbyInteractionContextColors.SEVERE;
		
		return SCR_ENearbyInteractionContextColors.MEDIUM;
	}
}
