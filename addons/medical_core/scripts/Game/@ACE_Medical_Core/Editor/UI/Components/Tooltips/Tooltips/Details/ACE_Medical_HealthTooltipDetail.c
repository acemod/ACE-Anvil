[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class ACE_Medical_HealthTooltipDetail : SCR_HealthTooltipDetail
{
	[Attribute(defvalue: "Health", desc: "Name for the hit zone that should represent the overall health")]
	protected string m_sRepresentativeHitZoneName;
	protected HitZone m_pRepresentativeHitZone;
	
	//------------------------------------------------------------------------------------------------
	override void UpdateDetail(SCR_EditableEntityComponent entity)
	{
		super.UpdateDetail(entity);
		
		if (m_pRepresentativeHitZone)
			SetBarAndPercentageValue(m_pRepresentativeHitZone.GetHealthScaled());
	}

	//------------------------------------------------------------------------------------------------
	override bool InitDetail(SCR_EditableEntityComponent entity, Widget widget)
	{
		if (!super.InitDetail(entity, widget))
			return false;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManager);
		if (charDamageManager)
			m_pRepresentativeHitZone = charDamageManager.GetHitZoneByName(m_sRepresentativeHitZoneName);
		
		return true;
	}
}
