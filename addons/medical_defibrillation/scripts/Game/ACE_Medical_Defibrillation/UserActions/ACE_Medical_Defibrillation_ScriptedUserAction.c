class ACE_Medical_Defibrillation_ScriptedUserAction : ScriptedUserAction
{
	protected ACE_Medical_Defibrillation_Settings m_pSettings;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		World world = GetGame().GetWorld();
		if (!world)
			return;
		
		m_pSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Settings>.GetModSettings();
	}
}