[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class ACE_Medical_ResilienceTooltipDetail : SCR_DpsConditionBarBaseTooltipDetail
{
	protected SCR_CharacterResilienceHitZone m_ResilienceHitZone;
	
	//------------------------------------------------------------------------------------------------
	override void UpdateDetail(SCR_EditableEntityComponent entity)
	{
		super.UpdateDetail(entity);
		
		if (m_ResilienceHitZone)
			SetBarAndPercentageValue(m_ResilienceHitZone.GetHealthScaled());
	}

	//------------------------------------------------------------------------------------------------
	override bool InitDetail(SCR_EditableEntityComponent entity, Widget widget)
	{
		if (!super.InitDetail(entity, widget))
			return false;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManager);
		if (!charDamageManager || charDamageManager.GetState() == EDamageState.DESTROYED || !charDamageManager.IsDamageHandlingEnabled())
			return false;

		m_ResilienceHitZone = charDamageManager.GetResilienceHitZone();
		return true;
	}
}
