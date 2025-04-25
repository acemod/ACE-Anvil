enum ACE_MedicalDefibrillation_EDefibrillatorEmulation
{
	Manual_NotImplimented,
	Automated
}

class ACE_MedicalDefibrillation_DefibrillatorComponentClass : ACE_Medical_BaseComponentClass
{
}

class ACE_MedicalDefibrillation_DefibrillatorComponent : ACE_Medical_BaseComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_MedicalDefibrillation_EDefibrillatorEmulation))]
	protected ACE_MedicalDefibrillation_EDefibrillatorEmulation m_DefibrillatorEmulation;
	
	static const ref array<ACE_MedicalDefibrillation_ECardiacRhythm> shockableRhythms = { ACE_MedicalDefibrillation_ECardiacRhythm.VF };
	
	// Offset variables
	protected float m_fAnalysisTimeOffset = 1;
	protected float m_fChargeTimeOffset = 0.0;
	
	// Analysis variables
	[Attribute(defvalue: "1", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to analyse cardiac rhythm.")]
	protected float m_fAnalysisTime;
	protected float m_fAnalysisAmount = 0.0;
	[RplProp()]
	protected bool m_bIsAnalysing = false;
	[RplProp()]
	protected bool m_bIsAnalysed = false;
	
	// Charge variables
	[Attribute(defvalue: "5.5", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to fully charge.")]
	protected float m_fChargeTime;
	protected float m_fChargeAmount = 0.0;
	[RplProp()]
	protected bool m_bIsCharging = false;
	[RplProp()]
	protected bool m_bIsCharged = false;

	protected AudioHandle m_currentSound;
	
	protected IEntity m_patient;
	[RplProp(onRplName : "OnPatientReplicated")]
	protected RplId m_iPatientRplId;
	
	const static string SOUNDCHARGED = "ACE_MedicalDefibrillationSound_Charged";
	const static string SOUNDCHARGING = "ACE_MedicalDefibrillationSound_Charging";
	const static string SOUNDSHOCKTHUMP = "ACE_MedicalDefibrillationSound_Thump";
	
	const static string SOUNDSHOCKADVISED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_ShockAdvised";
	const static string SOUNDNOSHOCKADVISED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_NoShockAdvised";
	const static string SOUNDANALYSING = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_Analysing";
	const static string SOUNDCONNECTED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_Connected";
	const static string SOUNDDISCONNECTED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_Disconnected";
	
	//------------------------------------------------------------------------------------------------	
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_DefibrillationSystem)
		{
		}
		// TODO: Do something with settings
		
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
		
		// get signals component and register the charge time for audio modulation
		SignalsManagerComponent signalsManagerComponent;
		if (GetSignalsComponent(signalsManagerComponent))
		{
			signalsManagerComponent.AddOrFindSignal("ACE_MedicalDefibrillation_ChargeSoundModulator", GetChargeTime());
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
		return ACE_MedicalDefibrillation_DefibrillationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldRegisterAtSystemOnInit()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_DefibrillationSystem GetDefibrillationSystem()
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return null;
		
		ACE_MedicalDefibrillation_DefibrillationSystem system = ACE_MedicalDefibrillation_DefibrillationSystem.Cast(world.FindSystem(ACE_MedicalDefibrillation_DefibrillationSystem));
		
		if (!system)
		{
			Print("ACE_MedicalDefibrillation_DefibrillatorComponent::GetDefibrillationSystem | No system found.", level: LogLevel.ERROR);
			return null;
		}
		
		return system;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterToSystem(IEntity entity)
	{
		ACE_MedicalDefibrillation_DefibrillationSystem system = GetDefibrillationSystem();
		if (system)
			system.Register(entity);
		else
			Print("ACE_MedicalDefibrillation_DefibrillatorComponent::RegisterToSystem | Cannot register to defibrillation system", level: LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterFromSystem(IEntity entity)
	{
		ACE_MedicalDefibrillation_DefibrillationSystem system = GetDefibrillationSystem();
		if (system)
			system.Unregister(entity);
		else
			Print("ACE_MedicalDefibrillation_DefibrillatorComponent::RegisterToSystem | Cannot unregister from defibrillation system", level: LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAnalysisAndCharge()
	{
		m_fAnalysisAmount = 0.0;
		m_fChargeAmount = 0.0;
		
		SetIsAnalysed(false);
		SetIsCharged(false);
		
		PrintFormat("ACE_MedicalDefibrillation_DefibrillatorComponent::ResetAnalysisAndCharge | Reset Defibrillator [Analysis = %1, Charge = %1]", m_fAnalysisAmount, m_fChargeAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPatient()
	{
		if (m_patient)
			PlaySound(SOUNDDISCONNECTED, true);
		
		m_iPatientRplId = RplId.Invalid();
		m_patient = null;
		Replication.BumpMe();
		
		ResetAnalysisAndCharge();
	}
		
	//------------------------------------------------------------------------------------------------
	void AnalyzeRhythm()
	{
		Print("ACE_MedicalDefibrillation_DefibrillatorComponent::AnalyzeRhythm | Starting rhythm analysis...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetAnalysing(true);
		
		PlaySound(SOUNDANALYSING, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Charge()
	{		
		Print("ACE_MedicalDefibrillation_DefibrillatorComponent::Charge | Starting charging sequence...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetCharging(true);
		
		PlaySound(SOUNDCHARGING, true);
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
		
		PlaySound(SOUNDCONNECTED, true);
		
		m_iPatientRplId = ACE_MedicalDefibrillation_ReplicationHelper.GetRplIdByEntity(patient);
		m_patient = patient;
		Replication.BumpMe();
		
		PrintFormat("ACE_MedicalDefibrillation_DefibrillatorComponent::ConnectPatient | Connected Patient: %1", patient.ClassName());
		PrintFormat("ACE_MedicalDefibrillation_DefibrillatorComponent::ConnectPatient | Connected PatientID: %1", m_iPatientRplId);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool DeliverShock()
	{
		if (!IsReadyToShock())
			return false;
		
		TerminateSound();
		
		ResetAnalysisAndCharge();
		
		// Shock thump sound effect - played on patient
		RPC_PlaySoundOnPatient(ACE_MedicalDefibrillation_DefibrillatorComponent.SOUNDSHOCKTHUMP);
		Rpc(RPC_PlaySoundOnPatient, ACE_MedicalDefibrillation_DefibrillatorComponent.SOUNDSHOCKTHUMP);
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatientCardiovascularComponent(cardiovascularComponent))
			return false;

		cardiovascularComponent.AddShocksDelivered(1);
		cardiovascularComponent.SetShockCooldown(cardiovascularComponent.GetShockCooldownTime());
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReadyToShock()
	{
		if (!GetConnectedPatient())
			return false;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatientCardiovascularComponent(cardiovascularComponent))
			return false;
		
		if (!IsCharged())
			return false;
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsShockableRhythm()
	{
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatientCardiovascularComponent(cardiovascularComponent))
			return false;
		
		ACE_MedicalDefibrillation_ECardiacRhythm rhythm = cardiovascularComponent.GetCardiacRhythm();
		if (shockableRhythms.Contains(rhythm))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPatientConnected()
	{
		if (!GetConnectedPatient())
			return false;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatientCardiovascularComponent(cardiovascularComponent))
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void PlaySound(string soundName, bool terminatePrevious = false, bool isLoop = false)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_PlaySound(soundName, terminatePrevious, isLoop); // Play on authority
		Rpc(RPC_PlaySound, soundName, terminatePrevious, isLoop); // Broadcast clients
	}
	
	//------------------------------------------------------------------------------------------------
	void TerminateSound(bool terminateAll = false)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_TerminateSound(terminateAll); // Terminate on authority
		Rpc(RPC_TerminateSound, terminateAll); // Broadcast clients
	}
	
	//------------------------------------------------------------------------------------------------
	// Replication
	//------------------------------------------------------------------------------------------------
	protected void OnPatientReplicated()
	{
		m_patient = ACE_MedicalDefibrillation_ReplicationHelper.GetEntityByRplId(m_iPatientRplId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_TerminateSound(bool terminateAll)
	{
		SoundComponent soundComponent;
		if (!GetSoundComponent(soundComponent))
			return;
		
		if (terminateAll)
			soundComponent.TerminateAll();
		else
			soundComponent.Terminate(m_currentSound);
		
		m_currentSound = null;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySound(string soundName, bool terminatePrevious, bool isLoop)
	{
		SoundComponent soundComponent;
		if (!GetSoundComponent(soundComponent))
			return;
		
		if (isLoop && m_currentSound)
			return;
		
		if (terminatePrevious)
			TerminateSound();
		
		m_currentSound = soundComponent.SoundEvent(soundName);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySoundOnPatient(string soundName)
	{
		if (!m_patient)
			return;
		
		SoundComponent soundComponent = SoundComponent.Cast(m_patient.FindComponent(SoundComponent));
		if (!soundComponent)
			return;
		
		soundComponent.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	// GETTERS AND SETTERS
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_EDefibrillatorEmulation GetDefibrillationEmulation()
	{
		return m_DefibrillatorEmulation;
	}
	
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
	bool GetConnectedPatientCardiovascularComponent(out ACE_Medical_CardiovascularComponent componentOut)
	{
		if (!m_patient)
			return false;
		
		componentOut = null;
		componentOut = ACE_Medical_CardiovascularComponent.Cast(m_patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSoundComponent(out SoundComponent componentOut)
	{
		componentOut = null;
		componentOut = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSignalsComponent(out SignalsManagerComponent componentOut)
	{
		componentOut = null;
		componentOut = SignalsManagerComponent.Cast(GetOwner().FindComponent(SignalsManagerComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
}