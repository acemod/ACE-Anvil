modded class ACE_Medical_CardiovascularSystem
{
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);

		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		UpdateCardiacRhythm(component, damageManager, timeSlice);
		UpdateShockCooldown(component, damageManager, timeSlice);
		UpdateShockAmount(component, damageManager, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCardiacRhythm(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		ACE_Medical_EVitalState vitalState = component.GetVitalState();
		
		if (component.IsInCardiacArrest())
		{
			component.SetCardiacRhythm(ACE_Medical_ECardiacRhythm.VF);
			// TODO: impliment system for deterioration to PEA - we need two cardiac arrest states?
			if (damageManager.GetState() == EDamageState.DESTROYED)
				component.SetCardiacRhythm(ACE_Medical_ECardiacRhythm.Asystole);
		}
		else
		{
			component.SetCardiacRhythm(ACE_Medical_ECardiacRhythm.Sinus);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateShockCooldown(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		float current = component.GetShockCooldown();
		
		if (current <= 0)
			return;
		
		float newCooldown = current - (m_fMinEntityUpdateTimeoutMS / 1000);
		
		if (newCooldown <= 0)
		{
			component.SetShockCooldown(0, true);
		}
		else
		{
			component.SetShockCooldown(newCooldown);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateShockAmount(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(component.GetOwner());
		if (!ownerChar)
			return;
		
		CharacterControllerComponent ownerController = ownerChar.GetCharacterController();
		if (!ownerController)
			return;
		
		if (!ownerController.IsUnconscious())
		{
			if (component.GetShocksDelivered() > 0)
				component.ResetShocksDelivered();
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
		
		return diceRoll < chance + (component.GetShockChanceIncrease() * component.GetShocksDelivered());
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
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;

		ACE_Medical_ECardiacRhythm rhythm = component.GetCardiacRhythm();
		string rhythmName = SCR_Enum.GetEnumName(ACE_Medical_ECardiacRhythm, rhythm);
		
		DbgUI.Begin(string.Format("ACE_Medical_DefibrillationSystem (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Cardiac rhythm:                  %1", rhythmName));
		DbgUI.Text(string.Format("# of Shocks:                  %1", component.GetShocksDelivered()));
		DbgUI.Text(string.Format("Shock cooldown:                  %1", component.GetShockCooldown()));
		DbgUI.End();
	}
#endif
}