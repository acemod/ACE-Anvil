class ACE_Medical_AEDComponentClass : ACE_Medical_BaseComponentClass
{
}

class ACE_Medical_AEDComponent : ACE_Medical_BaseComponent
{
	static const ref array<ACE_Medical_ECardiacRhythm> shockableRhythms = { ACE_Medical_ECardiacRhythm.VF };
	
	protected ACE_Medical_DefibrillationSystemSettings m_Settings;
	
	// Offset variables
	protected float m_fAnalysisTimeOffset = 1;
	protected float m_fChargeTimeOffset = 0.0;
	
	// Charge variables
	protected float m_fChargeTime = 5.5;
	protected float m_fChargeAmount = 0.0;
	
	// Needs replication
	[RplProp()]
	protected bool m_bIsCharging = false;
	// Needs replication
	[RplProp()]
	protected bool m_bIsCharged = false;
	
	// Analysis variables
	[Attribute(defvalue: "1", desc: "AED Analysis Time [s]", category: "ACE AED Settings")]
	protected float m_fAnalysisTime;
	protected float m_fAnalysisAmount = 0.0;
	// Needs replication
	[RplProp()]
	protected bool m_bIsAnalysing = false;
	// Needs replication
	[RplProp()]
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
	//[RplProp()]
	IEntity m_patient;
	
	SoundComponent m_soundComp;
	
	//------------------------------------------------------------------------------------------------	
	override protected void EOnInit(IEntity owner)
	{
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_DefibrillationSystem)
			m_Settings = settings.m_DefibrillationSystem;

		m_soundComp = SoundComponent.Cast(owner.FindComponent(SoundComponent));
		
		// import settings
		if (m_Settings)
		{
			if (m_Settings.m_bAEDAnalysisTimeOverride)
				m_fAnalysisTime = m_Settings.m_fAEDAnalysisTimeOverrideValue;
			
			if (m_Settings.m_bAEDChargeTimeOverride)
				m_fChargeTime = m_Settings.m_fAEDChargeTimeOverrideValue;
		}
		
		// add offsets to charge and analysis times to sync with sounds
		m_fAnalysisTime += m_fAnalysisTimeOffset;
		m_fChargeTime += m_fChargeTimeOffset;
		
		// subscribe to inventoryitemcomponent OnParentSlotChanged
		InventoryItemComponent invComp = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		if (invComp)
		{
			invComp.m_OnParentSlotChangedInvoker.Insert(OnParentSlotChanged);
			if (!invComp.GetParentSlot())
				RegisterToSystem(owner);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		if (!newSlot)
			RegisterToSystem(this.GetOwner());
		else
			UnregisterFromSystem(this.GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	override typename GetAssociatedSystemType()
	{
		return ACE_Medical_DefibrillationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_DefibrillationSystem GetDefibrillationSystem()
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return null;
		
		ACE_Medical_DefibrillationSystem system = ACE_Medical_DefibrillationSystem.Cast(world.FindSystem(ACE_Medical_DefibrillationSystem));
		
		if (!system)
		{
			Print("ACE_Medical_AEDComponent.GetDefibrillationSystem:: No system found.", level: LogLevel.ERROR);
			return null;
		}
		
		return system;
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
		ACE_Medical_DefibrillationSystem system = GetDefibrillationSystem();
		if (system)
			system.Register(entity);
		else
			Print("ACE_Medical_AEDComponent.RegisterToSystem:: Cannot register to defibrillation system", level: LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterFromSystem(IEntity entity)
	{
		ACE_Medical_DefibrillationSystem system = GetDefibrillationSystem();
		if (system)
			system.Unregister(entity);
		else
			Print("ACE_Medical_AEDComponent.RegisterToSystem:: Cannot unregister from defibrillation system", level: LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAnalysisAndCharge()
	{
		m_fAnalysisAmount = 0.0;
		m_fChargeAmount = 0.0;
		
		SetIsAnalysed(false);
		SetIsCharged(false);
		
		Replication.BumpMe();
		
		PrintFormat("ACE_Medical_AEDComponent.ResetAnalysisAndCharge:: Reset AED [Analysis = %1, Charge = %1]", m_fAnalysisAmount, m_fChargeAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPatient()
	{
		if (m_patient)
			PlaySound(SOUNDDISCONNECTED, true);
		
		m_patient = null;
		ResetAnalysisAndCharge();
	}
		
	//------------------------------------------------------------------------------------------------
	void AnalyzeRhythm()
	{
		Print("ACE_AED.AnalyzeRhythm:: Starting rhythm analysis...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetAnalysing(true);
		PlaySound(SOUNDANALYSING, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Charge()
	{		
		Print("ACE_AED.Charge:: Starting charging sequence...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetCharging(true);
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
	void ConnectPatient(IEntity patient)
	{
		ResetPatient();
		
		if (!patient)
			return;
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;	
		
		m_patient = patient;
		return;
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
			soundComp.SoundEvent(SOUNDSHOCKTHUMP);
		
		ACE_Medical_CardiovascularComponent comp = GetConnectedPatientCardiovascularComponent();
		comp.AddShocksDelivered(1);
		comp.SetShockCooldown(comp.GetShockCooldownTime());
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReadyToShock()
	{
		if (!GetConnectedPatient() ||
			!GetConnectedPatientCardiovascularComponent() ||
			!IsCharged())
			return false;
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsShockableRhythm()
	{
		if (!GetConnectedPatientCardiovascularComponent())
			return false;
		
		ACE_Medical_ECardiacRhythm rhythm = GetConnectedPatientCardiovascularComponent().GetCardiacRhythm();
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
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCharging(bool state) 
	{
	    m_bIsCharging = state;
		Replication.BumpMe();
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
		Replication.BumpMe();
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
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetConnectedPatient()
	{
		return m_patient;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_CardiovascularComponent GetConnectedPatientCardiovascularComponent()
	{
		return ACE_Medical_CardiovascularComponent.Cast(m_patient.FindComponent(ACE_Medical_CardiovascularComponent));
	}
}