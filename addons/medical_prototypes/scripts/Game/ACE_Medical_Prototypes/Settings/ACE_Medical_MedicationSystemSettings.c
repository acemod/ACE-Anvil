//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationSystemSettings : ScriptAndConfig
{
	[Attribute(desc: "Configurations for all drugs")]
	ref array<ref ACE_Medical_PharmacokineticsConfig> m_aPharmacokineticsConfigs;
	
	[Attribute(desc: "Drug effect configs")]
	ref array<ref ACE_Medical_DrugEffectConfig> m_aPharmacodynamicsConfigs;
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_PharmacokineticsConfig GetPharmacokineticsConfig(ACE_Medical_EDrugType type)
	{
		foreach (ACE_Medical_PharmacokineticsConfig config : m_aPharmacokineticsConfigs)
		{
			if (config.m_eType == type)
				return config;
		}
		
		return null;
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EDrugType, "m_eType")]
class ACE_Medical_PharmacokineticsConfig
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the medication", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	ACE_Medical_EDrugType m_eType;
	
	[Attribute(desc: "Rate constant for the activation of the drug [1/s]")]
	float m_fActivationRateConstant;
	
	[Attribute(desc: "Rate constant for the deactivation of the drug [1/s]")]
	float m_fDeactivationRateConstant;
		
	// We need to slightly offset the curve so the exponential tail will pass through zero
	// TO DO: Determine an appropriate value for this
	static const float CONCENTRATION_OFFSET_NM = -0.01;
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_DrugEffectConfig : ScriptAndConfig
{
	[Attribute(desc: "Pharmacodynamics config for all drugs with this effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aDrugConfigs;
	
	//------------------------------------------------------------------------------------------------
	void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations);

	//------------------------------------------------------------------------------------------------
	float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations);
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_LinearDrugEffectConfig : ACE_Medical_DrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	//! Sum of chi
	override float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations)
	{
		float chiSum = 0;
		
		foreach (ACE_Medical_PharmacodynamicsConfig config : m_aDrugConfigs)
		{
			chiSum += config.ComputeChi(concentrations);
		}
		
		return chiSum;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_HillDrugEffectConfig : ACE_Medical_LinearDrugEffectConfig
{
	[Attribute(defvalue: "1", desc: "Maximum possible effect of this drug")]
	protected float m_fMaxEffect;
	
	//------------------------------------------------------------------------------------------------
	//! Apply hill to sum of chi
	override float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations)
	{
		return m_fMaxEffect * ACE_Math.Hill(super.ComputeEffect(concentrations));
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_PainSuppressionDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
			damageManager.ACE_Medical_SetPainSuppression(ComputeEffect(concentrations));
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_HeartRateEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
			cardiovascularComponent.SetHeartRateMedicationAdjustment(ComputeEffect(concentrations));
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_SystemicVascularResistencDrugEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (cardiovascularComponent)
			cardiovascularComponent.SetSystemicVascularResistenceMedicationAdjustment(ComputeEffect(concentrations));
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_BloodFlowScaleDrugEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
			damageManager.ACE_Medical_SetBloodFlowScale(ComputeEffect(concentrations));
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EDrugType, "m_eType")]
class ACE_Medical_PharmacodynamicsConfig
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the medication", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	ACE_Medical_EDrugType m_eType;
	
	[Attribute(desc: "Concentration at which the half maximum effect is reached [nM]")]
	float m_fEC50;
	
	[Attribute(desc: "Configs for synergistic effects on this drug effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aSynergistConfigs;
	
	[Attribute(desc: "Configs for antagonistic effects on this drug effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aAntagonistConfigs;
	
	//------------------------------------------------------------------------------------------------
	float ComputeChi(map<ACE_Medical_EDrugType, float> concentrations)
	{
		float concentration
		if (!concentrations.Find(m_eType, concentration))
			return 0;
		
		float chi = concentration / m_fEC50;
		
		if (m_aSynergistConfigs)
		{
			foreach (ACE_Medical_PharmacodynamicsConfig synergistConfig : m_aSynergistConfigs)
			{
				chi *= synergistConfig.ComputeChi(concentrations);
			}
		}

		if (m_aAntagonistConfigs)
		{
			foreach (ACE_Medical_PharmacodynamicsConfig antagonistConfig : m_aAntagonistConfigs)
			{
				chi /= antagonistConfig.ComputeChi(concentrations);
			}
		}

		return chi;
	}
}


/*
//------------------------------------------------------------------------------------------------
class ACE_Medical_HillTypeDrugEffect : ACE_Medical_DrugEffect
{
	[Attribute(defvalue: "1", desc: "Maximum possible effect of this drug")]
	protected float m_fMaxEffect;
	
	[Attribute(desc: "Concentration at which the half maximum effect is reached [nM]")]
	protected float m_fEC50;
	
	//------------------------------------------------------------------------------------------------
	float ComputeEfficency(float concentration)
	{
		return m_fMaxEffect * ACE_Math.Hill(concentration / m_fEC50);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_AnalgesicDrugEffect : ACE_Medical_HillTypeDrugEffect
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(float concentration, inout ACE_Medical_MedicationAdjustments adjustments)
	{
		adjustments.m_fPain += ComputeEfficency(concentration);
	}
}
*/