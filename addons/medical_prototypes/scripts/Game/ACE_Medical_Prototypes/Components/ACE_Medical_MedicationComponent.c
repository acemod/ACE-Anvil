//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationManagerComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationManagerComponent : ScriptComponent
{
	[Attribute(defvalue: "1", desc: "Timeout between updates of effects [s]")]
	protected float m_fEffectUpdateTimeoutS;
	protected float m_fEffectUpdateTimerS = 0;
	
	protected float m_fHeartRateAdjustments;
	protected float m_fSystemicVascularResistenceAdjustments;
	protected float m_fPainSuppression;
	
	protected ACE_Medical_CardiovascularSystemComponent m_pCardiovascularSystem;
	
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
		m_pCardiovascularSystem = ACE_Medical_CardiovascularSystemComponent.Cast(owner.FindComponent(ACE_Medical_CardiovascularSystemComponent));	
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
	//! Clears the body of all medication
	void Clear()
	{
		// TO DO: Implement
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
class ACE_Medical_Medication : Managed
{
	protected ref array<ref ACE_Medical_Dose> m_aDoses;
	
	//------------------------------------------------------------------------------------------------
	void Administer(ACE_Medical_Dose dose)
	{
		m_aDoses.Insert(dose);
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeConcentration()
	{
		float time = System.GetTickCount();
		float concentration = 0;
		
		for (int i = m_aDoses.Count() - 1; i >= 0; i--)
		{
			ACE_Medical_Dose dose = m_aDoses[i];
			
			
		}
		
		return concentration;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_Dose : Managed
{
	protected float m_fConcentrationNM;
	protected float m_fTimeS;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Dose(float concentration)
	{
		m_fConcentrationNM = concentration;
		m_fTimeS = System.GetTickCount() / 1000;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Administered concentration
	float GetConcentration()
	{
		return m_fConcentrationNM;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Time when the dose was given
	float GetTime()
	{
		return m_fTimeS;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_Bolus : ACE_Medical_Dose
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_Infusion : ACE_Medical_Dose
{
	protected float m_fDurationS;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Infusion(float concentration, float duration)
	{
		ACE_Medical_Dose(concentration);
		m_fDurationS = duration;
	}
}
