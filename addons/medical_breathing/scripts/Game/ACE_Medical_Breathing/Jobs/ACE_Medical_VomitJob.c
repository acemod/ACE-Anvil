//------------------------------------------------------------------------------------------------
class ACE_Medical_VomitJob : ACE_IFrameJob
{
	protected ref ACE_Medical_CharacterContext m_pContext;
	
	protected static ACE_Medical_Breathing_Settings s_Settings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_VomitJob()
	{
		if (!s_Settings)
			s_Settings = ACE_SettingsHelperT<ACE_Medical_Breathing_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		
		if (!m_pContext.IsValid())
			return;
		
		if (m_pContext.m_fNextVomitUpdateTime > GetGame().GetWorld().GetWorldTime())
			return;
		
		if (Math.RandomFloat(0, 1) > s_Settings.m_fVomitChancePerMinute)
			return;
		
		HandleVomit();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HandleVomit()
	{
		// Check if blocked by King LT
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(m_pContext.m_pObject.FindComponent(ACE_EquipmentStorageComponent));
		if (storageComponent && storageComponent.IsSlotOccupied(ACE_EEquipementSlot.LARYNGEAL_TUBE))
			return;
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pContext.m_pObject.GetCharacterController());
		if (!charController)
			return;
		
		// Check for recovery positions
		ACE_Medical_EUnconsciousPose pose = charController.ACE_Medical_GetUnconsciousPose();
		if (pose == ACE_Medical_EUnconsciousPose.LEFT_SIDE || pose == ACE_Medical_EUnconsciousPose.RIGHT_SIDE)
			return;
		
		m_pContext.m_pVitals.SetIsAirwayOccluded(true);
		m_pContext.m_fNextVomitUpdateTime = GetGame().GetWorld().GetWorldTime() + 60000 * s_Settings.m_fVomitCooldown;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetContext(ACE_Medical_CharacterContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_CharacterContext GetContext()
	{
		return m_pContext;
	}
}
