//------------------------------------------------------------------------------------------------
//! Decides which next medic action the patient requires
class ACE_Medical_AIDecideMedicAction : AITaskScripted
{
	// Inputs
	protected const string PORT_PATIENT_ENTITY = "PatientEntity";
	
	// Outputs
	protected static const string PORT_CONSUMABLE_TYPE_OUT = "ConsumableType";
	
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aVarsIn = { PORT_PATIENT_ENTITY };
	override TStringArray GetVariablesIn() { return s_aVarsIn; }
	
	protected static ref TStringArray s_aVarsOut = { PORT_CONSUMABLE_TYPE_OUT };
	override TStringArray GetVariablesOut() { return s_aVarsOut; }

	// -----------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		ChimeraCharacter patientEntity
		GetVariableIn(PORT_PATIENT_ENTITY, patientEntity);
		
		if (!patientEntity)
			return ENodeResult.FAIL;
		
		SetVariableOut(PORT_CONSUMABLE_TYPE_OUT, SelectConsumableTypeToApply(patientEntity));
		return ENodeResult.SUCCESS;
	}
	
	//-----------------------------------------------------------------------------------------------
	//! Select consumable type to apply; Return NONE if patient is fine.
	//! 1. Fix blood
	//! 2. Fix resilience
	SCR_EConsumableType SelectConsumableTypeToApply(ChimeraCharacter patient)
	{
		if (!patient)
			return SCR_EConsumableType.NONE;
		
		if (patient.GetCharacterController().GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return SCR_EConsumableType.NONE;
		
		SCR_EConsumableType type = SelectFixBlood(patient);
		if (type != SCR_EConsumableType.NONE)
			return type;
		
		type = SelectFixResilience(patient);
		if (type != SCR_EConsumableType.NONE)
			return type;
		
		return SCR_EConsumableType.NONE;
	}
	
	//-----------------------------------------------------------------------------------------------
	//! Select consumable type for fixing bleeding/hypovolemia
	//! 1. Apply bandages
	//! 2. Apply saline bags
	protected SCR_EConsumableType SelectFixBlood(ChimeraCharacter patient)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(patient.GetDamageManager());
		if (!damageManager)
			return SCR_EConsumableType.NONE;
		
		if (damageManager.IsBleeding())
			return SCR_EConsumableType.BANDAGE;
		
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		if (bloodHZ.GetDamageStateThreshold(ECharacterBloodState.FAINTING) > bloodHZ.GetHealthScaled() && !damageManager.FindDamageEffectOfType(SCR_SalineDamageEffect))
			return SCR_EConsumableType.SALINE;
		
		return SCR_EConsumableType.NONE;
	}
	
	//-----------------------------------------------------------------------------------------------
	//! Select consumable type for waking up
	//! 1. Heal with medical kit
	//! 2. Apply epinephrine
	protected SCR_EConsumableType SelectFixResilience(ChimeraCharacter patient)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(patient.GetDamageManager());
		if (!damageManager || !damageManager.ACE_Medical_WasSecondChanceGranted())
			return SCR_EConsumableType.NONE;
		
		SCR_EConsumableFailReason reason;
		if (damageManager.ACE_Medical_CanApplyEpinephrine(reason))
			return SCR_EConsumableType.ACE_MEDICAL_EPINEPHRINE;
		
		if (reason == SCR_EConsumableFailReason.ACE_MEDICAL_TOO_DAMAGED)
			return SCR_EConsumableType.MED_KIT;
		
		return SCR_EConsumableType.NONE;
	}
		
	// -----------------------------------------------------------------------------------------------
	protected override static bool VisibleInPalette()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------
	protected override static bool CanReturnRunning()
	{
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------
	protected override static string GetOnHoverDescription()
	{
		return "Decides which next medic action the patient requires and returns the corresponding consumable type.";
	}
}
