enum ACE_Medical_EDefibrillatorEmulation
{
	Manual,
	Automated
}

class ACE_Medical_DefibrillatorComponentClass : ACE_Medical_BaseComponentClass
{
}

class ACE_Medical_DefibrillatorComponent : ACE_Medical_BaseComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_Medical_EDefibrillatorEmulation))]
	protected ACE_Medical_EDefibrillatorEmulation m_EDefibrillatorEmulation;
	
	ACE_Medical_DefibrillationSystemSettings m_Settings;
	static const ref array<ACE_Medical_ECardiacRhythm> shockableRhythms = { ACE_Medical_ECardiacRhythm.VF };
	
	// Offset variables
	protected float m_fAnalysisTimeOffset = 1;
	protected float m_fChargeTimeOffset = 0.0;
	
	// Analysis variables
	protected float m_fAnalysisTime = 1;
	protected float m_fAnalysisAmount = 0.0;
	[RplProp()]
	protected bool m_bIsAnalysing = false;
	[RplProp()]
	protected bool m_bIsAnalysed = false;
	
	// Charge variables
	protected float m_fChargeTime = 5.5;
	protected float m_fChargeAmount = 0.0;
	[RplProp()]
	protected bool m_bIsCharging = false;
	[RplProp()]
	protected bool m_bIsCharged = false;

	AudioHandle m_currentSound;
	
	protected IEntity m_patient;
	[RplProp(onRplName : "OnPatientReplicated")]
	protected RplId m_iPatientRplId;
	
	const static string SOUNDCHARGED = "ACE_Medical_DefibrillatorSound_Charged";
	const static string SOUNDCHARGING = "ACE_Medical_DefibrillatorSound_Charging";
	const static string SOUNDSHOCKTHUMP = "ACE_Medical_DefibrillatorSound_Thump";
	
	const static string SOUNDSHOCKADVISED = "ACE_DefibrillatorVoicePrompt_ShockAdvised";
	const static string SOUNDNOSHOCKADVISED = "ACE_DefibrillatorVoicePrompt_NoShockAdvised";
	const static string SOUNDANALYSING = "ACE_DefibrillatorVoicePrompt_Analysing";
	const static string SOUNDCONNECTED = "ACE_DefibrillatorVoicePrompt_Connected";
	const static string SOUNDDISCONNECTED = "ACE_DefibrillatorVoicePrompt_Disconnected";
	
	//------------------------------------------------------------------------------------------------	
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_DefibrillationSystem)
			m_Settings = settings.m_DefibrillationSystem;
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
	override bool ShouldRegisterAtSystemOnInit()
	{
		return false;
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
			Print("ACE_Medical_DefibrillatorComponent::GetDefibrillationSystem | No system found.", level: LogLevel.ERROR);
			return null;
		}
		
		return system;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterToSystem(IEntity entity)
	{
		ACE_Medical_DefibrillationSystem system = GetDefibrillationSystem();
		if (system)
			system.Register(entity);
		else
			Print("ACE_Medical_DefibrillatorComponent::RegisterToSystem | Cannot register to defibrillation system", level: LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	void UnregisterFromSystem(IEntity entity)
	{
		ACE_Medical_DefibrillationSystem system = GetDefibrillationSystem();
		if (system)
			system.Unregister(entity);
		else
			Print("ACE_Medical_DefibrillatorComponent::RegisterToSystem | Cannot unregister from defibrillation system", level: LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAnalysisAndCharge()
	{
		m_fAnalysisAmount = 0.0;
		m_fChargeAmount = 0.0;
		
		SetIsAnalysed(false);
		SetIsCharged(false);
		
		PrintFormat("ACE_Medical_DefibrillatorComponent::ResetAnalysisAndCharge | Reset Defibrillator [Analysis = %1, Charge = %1]", m_fAnalysisAmount, m_fChargeAmount);
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
		Print("ACE_Medical_DefibrillatorComponent::AnalyzeRhythm | Starting rhythm analysis...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetAnalysing(true);
		
		PlaySound(SOUNDANALYSING, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Charge()
	{		
		Print("ACE_Medical_DefibrillatorComponent::Charge | Starting charging sequence...", level: LogLevel.DEBUG);
		
		ResetAnalysisAndCharge();
		SetCharging(true);
		
		PlaySound(SOUNDCHARGING, true);
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
		
		PlaySound(SOUNDCONNECTED, true);
		
		m_iPatientRplId = ACE_Medical_DefibrillationReplicationHelper.GetRplIdByEntity(patient);
		m_patient = patient;
		Replication.BumpMe();
		
		PrintFormat("ACE_Medical_DefibrillatorComponent::ConnectPatient | Connected Patient: %1", patient.ClassName());
		PrintFormat("ACE_Medical_DefibrillatorComponent::ConnectPatient | Connected PatientID: %1", m_iPatientRplId);
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
		RPC_PlaySoundOnPatient(ACE_Medical_DefibrillatorComponent.SOUNDSHOCKTHUMP);
		Rpc(RPC_PlaySoundOnPatient, ACE_Medical_DefibrillatorComponent.SOUNDSHOCKTHUMP);
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatientCardiovascularComponent(cardiovascularComponent))
			return;

		cardiovascularComponent.AddShocksDelivered(1);
		cardiovascularComponent.SetShockCooldown(cardiovascularComponent.GetShockCooldownTime());
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
		
		ACE_Medical_ECardiacRhythm rhythm = cardiovascularComponent.GetCardiacRhythm();
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
		m_patient = ACE_Medical_DefibrillationReplicationHelper.GetEntityByRplId(m_iPatientRplId);
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
	ACE_Medical_EDefibrillatorEmulation GetDefibrillationEmulation()
	{
		return m_EDefibrillatorEmulation;
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
		
		componentOut = ACE_Medical_CardiovascularComponent.Cast(m_patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSoundComponent(out SoundComponent componentOut)
	{
		componentOut = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		if (!componentOut)
			return false;
		
		return true;
	}
}