[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class ACE_Medical_HealthTooltipDetail : SCR_HealthTooltipDetail
{
	//------------------------------------------------------------------------------------------------
	override void UpdateDetail(SCR_EditableEntityComponent entity)
	{
		super.UpdateDetail(entity);
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManager);
		if (charDamageManager)
			SetBarAndPercentageValue(charDamageManager.ACE_Medical_GetHealthScaled());
	}
}
