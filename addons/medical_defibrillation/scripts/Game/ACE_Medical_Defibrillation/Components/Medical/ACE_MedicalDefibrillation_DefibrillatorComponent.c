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
	protected ACE_MedicalDefibrillation_EDefibrillatorEmulation m_eDefibrillatorEmulation;
	
	[Attribute(defvalue: "1", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to analyse cardiac rhythm.")]
	protected float m_fAnalysisTime;
	
	[Attribute(defvalue: "5.5", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to fully charge.")]
	protected float m_fChargeTime;
	
	protected float m_fAnalysisAmount = 0.0;
	protected float m_fAnalysisTimeOffset = 1;
	protected float m_fChargeAmount = 0.0;
	protected float m_fChargeTimeOffset = 0.0;
	protected AudioHandle m_CurrentSound;
	protected IEntity m_Patient;
	
	[RplProp()]
	protected bool m_bIsAnalysed = false;
	
	[RplProp()]
	protected bool m_bIsAnalysing = false;
	
	[RplProp()]
	protected bool m_bIsCharged = false;
	
	[RplProp()]
	protected bool m_bIsCharging = false;
	
	[RplProp(onRplName: "OnPatientReplicated")]
	protected RplId m_iPatientRplId;
	
	static const ref array<ACE_MedicalDefibrillation_ECardiacRhythm> s_aShockableRhythms = { ACE_MedicalDefibrillation_ECardiacRhythm.VF };
	
	const static string SOUNDANALYSING = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_Analysing";
	const static string SOUNDCHARGED = "ACE_MedicalDefibrillationSound_Charged";
	const static string SOUNDCHARGING = "ACE_MedicalDefibrillationSound_Charging";
	const static string SOUNDCONNECTED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_Connected";
	const static string SOUNDDISCONNECTED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_Disconnected";
	const static string SOUNDNOSHOCKADVISED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_NoShockAdvised";
	const static string SOUNDSHOCKADVISED = "ACE_MedicalDefibrillation_DefibrillatorVoicePrompt_ShockAdvised";
	const static string SOUNDSHOCKTHUMP = "ACE_MedicalDefibrillationSound_Thump";
	
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
	bool IsCharging() 
	{
	    return m_bIsCharging;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCharging(bool state) 
	{
	    m_bIsCharging = state;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_EDefibrillatorEmulation GetDefibrillationEmulation()
	{
		return m_eDefibrillatorEmulation;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSoundComponent(out SoundComponent componentOut)
	{
		componentOut = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSignalsComponent(out SignalsManagerComponent componentOut)
	{
		componentOut = SignalsManagerComponent.Cast(GetOwner().FindComponent(SignalsManagerComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetConnectedPatient()
	{
		return m_Patient;
	}
	
	bool GetConnectedPatient(out IEntity patient, out ACE_Medical_CardiovascularComponent componentOut)
	{
		patient = GetConnectedPatient();
		componentOut = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		
		if (!patient || !componentOut)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPatientConnected()
	{
		if (!GetConnectedPatient())
			return false;
		
		IEntity patient;
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatient(patient, cardiovascularComponent))
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsShockableRhythm()
	{
		IEntity patient;
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatient(patient, cardiovascularComponent))
			return false;
		
		ACE_MedicalDefibrillation_ECardiacRhythm rhythm = cardiovascularComponent.GetCardiacRhythm();
		if (s_aShockableRhythms.Contains(rhythm))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReadyToShock()
	{
		if (!GetConnectedPatient())
			return false;
		
		IEntity patient;
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatient(patient, cardiovascularComponent))
			return false;
		
		if (!IsCharged())
			return false;
	
		return true;
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
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_TerminateSound(bool terminateAll)
	{
		SoundComponent soundComponent;
		if (!GetSoundComponent(soundComponent))
			return;
		
		if (terminateAll)
			soundComponent.TerminateAll();
		else
			soundComponent.Terminate(m_CurrentSound);
		
		m_CurrentSound = null;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySound(string soundName, bool terminatePrevious, bool isLoop)
	{
		SoundComponent soundComponent;
		if (!GetSoundComponent(soundComponent))
			return;
		
		if (isLoop && m_CurrentSound)
			return;
		
		if (terminatePrevious)
			TerminateSound();
		
		m_CurrentSound = soundComponent.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySoundOnPatient(string soundName)
	{
		if (!m_Patient)
			return;
		
		SoundComponent soundComponent = SoundComponent.Cast(m_Patient.FindComponent(SoundComponent));
		if (!soundComponent)
			return;
		
		soundComponent.SoundEvent(soundName);
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
	void Analyse()
	{
		Print("ACE_MedicalDefibrillation_DefibrillatorComponent::AnalyzeRhythm | Starting rhythm analysis...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetAnalysing(true);
		
		PlaySound(SOUNDANALYSING, true);
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
		if (m_Patient)
			PlaySound(SOUNDDISCONNECTED, true);
		
		m_iPatientRplId = RplId.Invalid();
		m_Patient = null;
		Replication.BumpMe();
		
		ResetAnalysisAndCharge();
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
		m_Patient = patient;
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
		
		IEntity patient;
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatient(patient, cardiovascularComponent))
			return false;
	
		cardiovascularComponent.AddShocksDelivered(1);
		cardiovascularComponent.SetShockCooldown(cardiovascularComponent.GetShockCooldownTime());
		
		return true;
	}
	
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
	protected void OnPatientReplicated()
	{
		m_Patient = ACE_MedicalDefibrillation_ReplicationHelper.GetEntityByRplId(m_iPatientRplId);
	}
}