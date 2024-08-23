//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationManagerComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationManagerComponent : ScriptComponent
{
	[Attribute(desc: "Types of drugs that can be administered to this character")]
	protected ref array<ref ACE_Medical_DrugHandler> m_aDrugHandlers;
	
	[Attribute(defvalue: "1", desc: "Timeout between updates of effects [s]")]
	protected float m_fEffectUpdateTimeoutS;
	protected float m_fEffectUpdateTimerS = 0;
	
	protected float m_fHeartRateAdjustments;
	protected float m_fSystemicVascularResistenceAdjustments;
	protected float m_fPainSuppression;
	
	protected ACE_Medical_CardiovascularComponent m_pCardiovascularComponent;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		m_pCardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(owner.FindComponent(ACE_Medical_CardiovascularComponent));	
		SetEventMask(owner, EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		m_fEffectUpdateTimerS += timeSlice;
		
		if (m_fEffectUpdateTimerS < m_fEffectUpdateTimeoutS)
			return;
		
		UpdateEffects(m_fEffectUpdateTimerS);
		m_fEffectUpdateTimerS = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateEffects(float timeSlice)
	{
		
	}
	
	//------------------------------------------------------------------------------------------------
	void AddMedication(ACE_Medical_EDrugType type, ACE_Medical_Dose dose)
	{
		foreach (ACE_Medical_DrugHandler drugHandler : m_aDrugHandlers)
		{
			if (type != drugHandler.GetType())
				continue;
			
			drugHandler.AddDose(dose);
			return;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clears the body of all medication
	void Clear()
	{
		foreach (ACE_Medical_DrugHandler drugHandler : m_aDrugHandlers)
		{
			drugHandler.Clear();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTargetHeartRateAdjustment()
	{
		return m_fHeartRateAdjustments;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetSystemicVascularResistenceAdjustment()
	{
		return m_fSystemicVascularResistenceAdjustments;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPainSuppression()
	{
		return m_fPainSuppression;
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EDrugType, "m_eType")]
class ACE_Medical_DrugHandler : Managed
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the medication", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	protected ACE_Medical_EDrugType m_eType;
	
	[Attribute(desc: "Rate constant for the activation of the drug [1/s]")]
	protected float m_fActivationRateConstant;
	
	[Attribute(desc: "Rate constant for the deactivation of the drug [1/s]")]
	protected float m_fDeactivationRateConstant;
	
	[Attribute(desc: "Effects this drug has")]
	protected ref array<ref ACE_Medical_DrugEffectConfig> m_aEffects;
	
	protected ref array<ref ACE_Medical_Dose> m_aDoses;
	protected bool m_bActive;
	
	// We need to slightly offset the curve so the exponential tail will pass through zero
	// TO DO: Determine an appropriate value for this
	protected const float CONCENTRATION_OFFSET_NM = -0.01;
	
	//------------------------------------------------------------------------------------------------
	void AddDose(ACE_Medical_Dose dose)
	{
		m_aDoses.Insert(dose);
		m_bActive = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clears the body of this drug
	void Clear()
	{
		m_aDoses.Clear();
		m_bActive = false;
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeConcentration()
	{
		if (!m_bActive)
			return 0;
		
		float totalConcentration = 0;
				
		for (int i = m_aDoses.Count() - 1; i >= 0; i--)
		{
			ACE_Medical_Dose dose = m_aDoses[i];
			
			float initialConcentration = dose.GetConcentration();
			float time = dose.GetElapsedTime();
			float concentration = m_fActivationRateConstant / (m_fActivationRateConstant - m_fDeactivationRateConstant) * (ACE_Math.Exp(-m_fDeactivationRateConstant * time) - ACE_Math.Exp(-m_fActivationRateConstant * time)) *	initialConcentration + CONCENTRATION_OFFSET_NM;
			
			// Remove doses that have expired
			if (concentration <= 0)
			{
				m_aDoses.Remove(i);
				continue;
			}
			
			totalConcentration += concentration;
		}
		
		if (m_aDoses.IsEmpty())
			m_bActive = false;
		
		return totalConcentration;
	}
	
	//------------------------------------------------------------------------------------------------
	void ApplyEffects(inout ACE_Medical_MedicationAdjustments adjustments)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true when drug is in the system
	bool IsActive()
	{
		return m_bActive;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_EDrugType GetType()
	{
		return m_eType;
	}
	
	
}
