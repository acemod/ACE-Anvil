class ACE_Medical_AEDComponent : ScriptComponent
{
	static const array<ACE_Medical_ECardiacRhythm> shockableRhythms = { ACE_Medical_ECardiacRhythm.VF };
	
	float m_fChargeTime;
	float m_fCharge = 0.0;
	protected bool m_bIsCharged = false;
	protected bool m_bIsCharging = false;
	protected float m_fChargeTimer = 0.0;
	
	IEntity m_AED;
	IEntity m_patient;
	ACE_Medical_CardiovascularComponent m_component;
		
	//------------------------------------------------------------------------------------------------
	bool ConnectPatient(IEntity patient)
	{
		// reset patient and component
		m_patient = null; m_component = null;
		
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
	bool PatientConnected()
	{
		if (!m_patient || !m_component)
			return false;
		
		return true;
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
	void DeliverShock()
	{
		if (!IsReadyToShock())
			return;
		
		m_component.AddShocksDelivered(1);
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
	override void EOnInit(IEntity AED)
	{
		m_AED = AED;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity AED, float timeSlice)
	{
		super.EOnFrame(AED, timeSlice);
		
		// Charging system 
		if (m_bIsCharging)
		{
			m_fCharge += timeSlice / m_fChargeTime;
			
			if (m_fCharge >= 1.0)
			{
				m_fCharge = 1.0;
				m_bIsCharging = false;
				m_bIsCharged = true;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnActivate(IEntity AED)
	{
		super.EOnActivate(AED);
		
		SetEventMask(AED, EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDeactivate(IEntity AED)
	{
		super.EOnDeactivate(AED);
		
		ClearEventMask(AED, EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Charge()
	{
		ResetCharge();
		m_bIsCharging = true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCharged()
	{
		return m_bIsCharged;
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetCharge()
	{
		m_fCharge = 0.0;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetChargePercent()
	{
		return m_fCharge;
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
}