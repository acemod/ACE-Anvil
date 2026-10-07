//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent
{
	[RplProp()]
	protected bool m_bACE_Medical_LeftArmSplinted;

	[RplProp()]
	protected bool m_bACE_Medical_RightArmSplinted;

	[RplProp()]
	protected bool m_bACE_Medical_LeftLegSplinted;

	[RplProp()]
	protected bool m_bACE_Medical_RightLegSplinted;

	//------------------------------------------------------------------------------------------------
	bool ACE_Medical_GetGroupSplinted(ECharacterHitZoneGroup group)
	{
		if (group == ECharacterHitZoneGroup.LEFTARM)
			return m_bACE_Medical_LeftArmSplinted;

		if (group == ECharacterHitZoneGroup.RIGHTARM)
			return m_bACE_Medical_RightArmSplinted;

		if (group == ECharacterHitZoneGroup.LEFTLEG)
			return m_bACE_Medical_LeftLegSplinted;

		if (group == ECharacterHitZoneGroup.RIGHTLEG)
			return m_bACE_Medical_RightLegSplinted;

		return false;
	}

	//------------------------------------------------------------------------------------------------
	bool ACE_Medical_IsGroupSplinted(ECharacterHitZoneGroup group)
	{
		return ACE_Medical_GetGroupSplinted(group);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Medical_SetGroupSplinted(ECharacterHitZoneGroup group, bool state)
	{
		if (group == ECharacterHitZoneGroup.LEFTARM)
			m_bACE_Medical_LeftArmSplinted = state;
		else if (group == ECharacterHitZoneGroup.RIGHTARM)
			m_bACE_Medical_RightArmSplinted = state;
		else if (group == ECharacterHitZoneGroup.LEFTLEG)
			m_bACE_Medical_LeftLegSplinted = state;
		else if (group == ECharacterHitZoneGroup.RIGHTLEG)
			m_bACE_Medical_RightLegSplinted = state;

		Replication.BumpMe();
	}
}