[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class ACE_Medical_BrainTooltipDetail : SCR_DpsConditionBarBaseTooltipDetail
{
	protected ACE_Medical_BrainHitZone m_pBrainHitZone;
	
	//------------------------------------------------------------------------------------------------
	override void UpdateDetail(SCR_EditableEntityComponent entity)
	{
		super.UpdateDetail(entity);
		
		if (m_pBrainHitZone)
			SetBarAndPercentageValue(m_pBrainHitZone.GetHealthScaled());
	}

	//------------------------------------------------------------------------------------------------
	override bool InitDetail(SCR_EditableEntityComponent entity, Widget widget)
	{
		if (!super.InitDetail(entity, widget))
			return false;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_DamageManager);
		if (!charDamageManager || charDamageManager.GetState() == EDamageState.DESTROYED || !charDamageManager.IsDamageHandlingEnabled())
			return false;

		m_pBrainHitZone = charDamageManager.ACE_Medical_GetBrainHitZone();
		return true;
	}
}
