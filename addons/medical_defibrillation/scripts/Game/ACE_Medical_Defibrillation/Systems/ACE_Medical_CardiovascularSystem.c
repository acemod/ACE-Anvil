modded class ACE_Medical_CardiovascularSystem
{
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);

		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		UpdateCardiacRhythm(cardiovascularComponent, damageManager, timeSlice);
		UpdateShockAmount(cardiovascularComponent, damageManager, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCardiacRhythm(ACE_Medical_CardiovascularComponent cardiovascularComponent, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		ACE_Medical_EVitalState vitalState = cardiovascularComponent.GetVitalState();
		
		if (cardiovascularComponent.IsInCardiacArrest())
		{
			cardiovascularComponent.SetCardiacRhythm(ACE_MedicalDefibrillation_ECardiacRhythm.VF);
			// TODO: impliment system for deterioration to PEA - we need two cardiac arrest states?
			if (damageManager.GetState() == EDamageState.DESTROYED)
				cardiovascularComponent.SetCardiacRhythm(ACE_MedicalDefibrillation_ECardiacRhythm.Asystole);
		}
		else if (ACE_MedicalDefibrillation_DefibrillationConnectPatientUserAction.IsAlive(cardiovascularComponent.GetOwner()))
		{
			cardiovascularComponent.SetCardiacRhythm(ACE_MedicalDefibrillation_ECardiacRhythm.Sinus);
		}
		else
		{
			cardiovascularComponent.SetCardiacRhythm(ACE_MedicalDefibrillation_ECardiacRhythm.Asystole);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateShockAmount(ACE_Medical_CardiovascularComponent cardiovascularComponent, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(cardiovascularComponent.GetOwner());
		if (!ownerChar)
			return;
		
		CharacterControllerComponent ownerController = ownerChar.GetCharacterController();
		if (!ownerController)
			return;
		
		if (!ownerController.IsUnconscious())
		{
			if (cardiovascularComponent.GetShocksDelivered() > 0)
				cardiovascularComponent.ResetShocksDelivered();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ComputeCPRSuccess(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		
		float minScale = bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float maxScale = bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float scale = Math.Clamp(bloodHZ.GetHealthScaled(), minScale, maxScale);
		float chance = Math.Map(scale, minScale, maxScale, m_Settings.m_fCPRSuccessChanceMin, m_Settings.m_fCPRSuccessChanceMax);
		float diceRoll = Math.RandomFloat01();
		
		return diceRoll < chance * (1 + (component.GetShockChanceIncrease() * component.GetShocksDelivered()));
	}

#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!GetDiagTarget(target, targetType))
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;

		ACE_MedicalDefibrillation_ECardiacRhythm rhythm = cardiovascularComponent.GetCardiacRhythm();
		string rhythmName = SCR_Enum.GetEnumName(ACE_MedicalDefibrillation_ECardiacRhythm, rhythm);
		
		DbgUI.Begin(string.Format("ACE_MedicalDefibrillation_DefibrillationSystem (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Cardiac rhythm:                  %1", rhythmName));
		DbgUI.Text(string.Format("# of Shocks:                  %1", cardiovascularComponent.GetShocksDelivered()));
		DbgUI.End();
	}
#endif
}