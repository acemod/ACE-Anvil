class ACE_Medical_AEDComponentClass : ACE_Medical_BaseComponentClass
{
}

class ACE_Medical_AEDComponent : ACE_Medical_BaseComponent
{
	static const ref array<ACE_Medical_ECardiacRhythm> shockableRhythms = { ACE_Medical_ECardiacRhythm.VF };
	
	// Offset variables
	protected float m_fAnalysisTimeOffset = 1;
	protected float m_fChargeTimeOffset = 0.0;
	
	// Charge variables
	protected float m_fChargeTime = 5.5;
	protected float m_fChargeAmount = 0.0;
	
	// Needs replication
	protected bool m_bIsCharging = false;
	// Needs replication
	protected bool m_bIsCharged = false;
	
	// Analysis variables
	[Attribute(defvalue: "1", desc: "AED Analysis Time [s]", category: "ACE AED Settings")]
	protected float m_fAnalysisTime;
	protected float m_fAnalysisAmount = 0.0;
	// Needs replication
	protected bool m_bIsAnalysing = false;
	// Needs replication
	protected bool m_bIsAnalysed = false;
	
	const static string SOUNDCHARGED = "ACE_MEDICAL_AED_SOUNDCHARGED";
	const static string SOUNDCHARGING = "ACE_MEDICAL_AED_SOUNDCHARGING";
	const static string SOUNDSHOCKADVISED = "ACE_MEDICAL_AED_SOUNDSHOCKADVISED";
	const static string SOUNDNOSHOCKADVISED = "ACE_MEDICAL_AED_SOUNDNOSHOCKADVISED";
	const static string SOUNDANALYSING = "ACE_MEDICAL_AED_SOUNDANALYSING";
	const static string SOUNDSHOCKTHUMP = "ACE_MEDICAL_AED_SOUNDSHOCKTHUMP";
	const static string SOUNDCONNECTED = "ACE_MEDICAL_AED_SOUNDCONNECTED";
	const static string SOUNDDISCONNECTED = "ACE_MEDICAL_AED_SOUNDDISCONNECTED";

	protected AudioHandle m_currentSound;
	
	// Needs replication
	IEntity m_patient;
	// Needs replication
	ACE_Medical_CardiovascularComponent m_patientCardiovascularComponent;
	
	SoundComponent m_soundComp;
	
	//------------------------------------------------------------------------------------------------	
	override protected void EOnInit(IEntity owner)
	{
		RegisterToSystem(owner);
		m_soundComp = SoundComponent.Cast(owner.FindComponent(SoundComponent));
		
		// add offsets to charge and analysis times to sync with sounds
		m_fAnalysisTime += m_fAnalysisTimeOffset;
		m_fChargeTime += m_fChargeTimeOffset;
	}
	
	//------------------------------------------------------------------------------------------------
	override typename GetAssociatedSystemType()
	{
		return ACE_Medical_DefibrillationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetDefibrillationSystem(out ACE_Medical_DefibrillationSystem systemOut)
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return false;
		
		ACE_Medical_DefibrillationSystem system = ACE_Medical_DefibrillationSystem.Cast(world.FindSystem(ACE_Medical_DefibrillationSystem));
		
		if (!system)
		{
			Print("ACE_Medical_AEDComponent.GetDefibrillationSystem:: No system found.", level: LogLevel.ERROR);
			return false;
		}
		
		systemOut = system;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Manually register using EOnInit
	override protected bool ShouldRegisterAtSystemOnInit()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterToSystem(IEntity entity)
	{
		ACE_Medical_DefibrillationSystem system;
		if (GetDefibrillationSystem(system))
		{
			system.Register(entity);
		}
		else
		{
			Print("ACE_Medical_AEDComponent.RegisterToSystem:: Cannot register to defibrillation system", level: LogLevel.ERROR);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAnalysisAndCharge()
	{
		m_fAnalysisAmount = 0.0;
		SetIsAnalysed(false);
		m_fChargeAmount = 0.0;
		SetIsCharged(false);
		
		PrintFormat("ACE_Medical_AEDComponent.ResetAnalysisAndCharge:: Reset AED [Analysis = %1, Charge = %1]", m_fAnalysisAmount, m_fChargeAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPatient()
	{
		m_patient = null;
		m_patientCardiovascularComponent = null;
	}
		
	//------------------------------------------------------------------------------------------------
	void AnalyzeRhythm()
	{
		ResetAnalysisAndCharge();
		
		Print("ACE_AED.AnalyzeRhythm:: Starting rhythm analysis...");
		
		// Register to defib system for analysis rate frame handler
		PlaySound(SOUNDANALYSING, true);
		SetAnalysing(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Charge()
	{
		ResetAnalysisAndCharge();
		
		Print("ACE_AED.Charge:: Starting charging sequence...");
		SetCharging(true);
		
		if (!m_soundComp)
			return;
		
		PlaySound(SOUNDCHARGING, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void PlaySound(string soundName, bool terminatePrevious = false, bool isLoop = false)
	{
		if (!m_soundComp)
			return;
		
		// skip starting a new sound if it is a looping sound
		if (isLoop && m_currentSound)
			return;
		
		if (terminatePrevious)
			TerminateSound();
		
		m_currentSound = m_soundComp.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	void TerminateSound()
	{
		if (!m_soundComp)
			return;
		
		m_soundComp.Terminate(m_currentSound);
		m_currentSound = null;
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
		m_patientCardiovascularComponent = component;
		Replication.BumpMe();
				
		return true; 
	}
	
	//------------------------------------------------------------------------------------------------
	void DeliverShock()
	{
		if (!IsReadyToShock())
			return;
		
		TerminateSound();		
		ResetAnalysisAndCharge();
		
		// Shock thump sound effect - played on patient
		CharacterSoundComponent soundComp = CharacterSoundComponent.Cast(m_patient.FindComponent(CharacterSoundComponent));
		if (soundComp)
			PlaySound(SOUNDSHOCKTHUMP, true);
		
		m_patientCardiovascularComponent.AddShocksDelivered(1);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReadyToShock()
	{
		if (!GetConnectedPatient() || !GetConnectedPatientCardiovascularComponent())
			return false;
		
		if (GetChargeAmount() < 1.0)
			return false;
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsShockableRhythm()
	{
		if (!m_patientCardiovascularComponent)
			return false;
		
		ACE_Medical_ECardiacRhythm rhythm = m_patientCardiovascularComponent.GetCardiacRhythm();
		if (shockableRhythms.Contains(rhythm))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPatientConnected()
	{
		if (!GetConnectedPatient() || !GetConnectedPatientCardiovascularComponent())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	// GETTERS AND SETTERS
	//------------------------------------------------------------------------------------------------
	float GetChargeTime()
	{
		return m_fChargeTime;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetChargeAmount() 
	{
	    return m_fChargeAmount;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetChargeAmount(float chargeAmount) 
	{
	    m_fChargeAmount = chargeAmount;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCharging() 
	{
	    return m_bIsCharging;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCharged()
	{
		return m_bIsCharged;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsCharged(bool state)
	{
		m_bIsCharged = state;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCharging(bool state) 
	{
	    m_bIsCharging = state;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAnalysisTime()
	{
		return m_fAnalysisTime;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAnalysisAmount() 
	{
	    return m_fAnalysisAmount;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAnalysisAmount(float analysisAmount) 
	{
	    m_fAnalysisAmount = analysisAmount;
	}
	
	//------------------------------------------------------------------------------------------------	
	bool IsAnalysed()
	{
		return m_bIsAnalysed;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsAnalysed(bool state)
	{
		m_bIsAnalysed = state;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsAnalysing() 
	{
	    return m_bIsAnalysing;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAnalysing(bool state) 
	{
	    m_bIsAnalysing = state;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetConnectedPatient()
	{
		return m_patient;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_CardiovascularComponent GetConnectedPatientCardiovascularComponent()
	{
		return m_patientCardiovascularComponent;
	}
}