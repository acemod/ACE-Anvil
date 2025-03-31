class ACE_Medical_AEDComponentClass : ScriptComponentClass
{
}

class ACE_Medical_AEDComponent : ScriptComponent
{
	[Attribute(defvalue: "5", desc: "AED Charge Time [s]", category: "ACE AED Settings")]
	float m_fChargeTime;
	
	[Attribute(defvalue: "1", desc: "AED Analysis Time [s]", category: "ACE AED Settings")]
	protected float m_fAnalysisTime;
	
	static const ref array<ACE_Medical_ECardiacRhythm> shockableRhythms = { ACE_Medical_ECardiacRhythm.VF };
	
	float m_fCharge = 0.0;
	protected bool m_bIsCharged = false;
	protected bool m_bIsCharging = false;
	
	float m_fAnalysis = 0.0;
	protected bool m_bIsAnalysing = false;
	protected bool m_bIsAnalysed = false;
	
	IEntity m_patient;
	ACE_Medical_CardiovascularComponent m_component;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		RegisterToSystem(this);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterToSystem(ACE_Medical_AEDComponent component)
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return;
		
		ACE_Medical_AEDSystem system = ACE_Medical_AEDSystem.Cast(world.FindSystem(ACE_Medical_AEDSystem));
		
		if (!system)
			return;
		
		system.Register(component);
	}
		
	//------------------------------------------------------------------------------------------------
	void AnalyzeRhythm()
	{
		ResetAnalysis();
		Print("ACE_AED.AnalyzeRhythm:: Starting rhythm analysis...");
		m_bIsAnalysing = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void Charge()
	{
		ResetCharge();
		Print("ACE_AED.Charge:: Starting charging sequence...");
		m_bIsCharging = true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ConnectPatient(IEntity patient)
	{
		ResetPatient();
		
		if (!patient)
			return false;
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return false;	
		
		m_patient = patient;
		m_component = component;
				
		return true; 
	}
	
	//------------------------------------------------------------------------------------------------	
	void ResetPatient()
	{
		m_patient = null;
		m_component = null;
	}
	
	//------------------------------------------------------------------------------------------------
	void DeliverShock()
	{
		if (!IsReadyToShock())
			return;
		
		m_component.AddShocksDelivered(1);
	}
	
	//------------------------------------------------------------------------------------------------
	void EnableCharging(bool state)
	{
		m_bIsCharging = state;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCharging()
	{
		return m_bIsCharging;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAnalysis()
	{
		return m_fAnalysis;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAnalysisTime()
	{
		return m_fAnalysisTime;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsAnalysing()
	{
		return m_bIsAnalysing;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetCharge()
	{
		return m_fCharge;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetChargePercent()
	{
		return m_fCharge;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetChargeTime()
	{
		return m_fChargeTime;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetConnectedPatient()
	{
		return m_patient;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_CardiovascularComponent GetPatientCardiovascularComponent()
	{
		return m_component;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCharged()
	{
		if (m_fCharge >= 1.0)
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReadyToShock()
	{
		if (PatientConnected() && IsCharged())
		{
			return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsShockableRhythm()
	{
		if (!m_component)
			return false;
		
		ACE_Medical_ECardiacRhythm rhythm = m_component.GetCardiacRhythm();
		if (shockableRhythms.Contains(rhythm))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool PatientConnected()
	{
		if (!m_patient || !m_component)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAnalysis()
	{
		m_fAnalysis = 0.0;
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetCharge()
	{
		m_fCharge = 0.0;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAnalysis(float value)
	{
		m_fAnalysis = value;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCharge(float charge)
	{
		m_fCharge = charge;
	}
	
	//------------------------------------------------------------------------------------------------
	void EnableAnalysis(bool state)
	{
		m_bIsAnalysing = state;
	}
}