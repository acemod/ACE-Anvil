enum ACE_MedicalDefibrillation_EDefibrillatorEmulation
{
	Manual_NotImplimented,
	Automated
}

class ACE_MedicalDefibrillation_DefibrillatorComponentClass : ACE_Medical_BaseComponentClass
{
}

enum ACE_MedicalDefibrillation_EDefibrillatorState
{
	Invalid = -1,
	Disconnected,
	Connected,
	Analysing,
	Analysed,
	Charging,
	Charged
}

class ACE_MedicalDefibrillation_DefibrillatorComponent : ACE_Medical_BaseComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_MedicalDefibrillation_EDefibrillatorEmulation))]
	protected ACE_MedicalDefibrillation_EDefibrillatorEmulation m_eDefibrillatorEmulation;
	
	[Attribute(defvalue: "1", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to analyse cardiac rhythm.")]
	protected float m_fAnalysisTime;
	
	[Attribute(defvalue: "5.5", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to fully charge.")]
	protected float m_fChargeTime;
	
	[Attribute(defvalue: "120", params: "0 inf 1", desc: "Time (s) between analysis events where players should perform CPR.")]
	protected float m_fCPRCooldownTime;
	
	[Attribute(defvalue: "10", params: "0 inf 0.1", desc: "Stun duration (s) for a character that touches the patient while a shock is delivered.")]
	protected float m_fContactShockStunDuration;
	
	// Percent from 0 to 1 of analysis completion
	// Prop'd for UI in future
	[RplProp()]
	protected float m_fAnalysisPercent;
	
	// Percent from 0 to 1 of charge completion
	// Prop'd for UI in future
	[RplProp()]
	protected float m_fChargePercent;
	
	// Time left between next analysis event.
	// Prop'd for UI in future
	[RplProp()]
	protected float m_fCPRCooldown;	
	
	protected AudioHandle m_CurrentSound;
	protected IEntity m_Patient;
	
	// Current enum equivalent of the current state on server. Use for UI or actions on client
	// Can replace if someone wants to build a RplProp encoder for ACE_MedicalDefibrillation_DefibrillatorState_Base
	[RplProp()]
	ACE_MedicalDefibrillation_EDefibrillatorState m_eDefibrillatorState;
	
	protected ref ACE_MedicalDefibrillation_DefibrillatorState_Base m_DefibrillatorState;
	bool m_fInitialAnalysisPerformed;
	bool m_bIsChangingState = false;
	
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
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_DefibrillationSystem)
		{
		}
		// TODO: Do something with settings
		
		// Set properties
		SetAnalysisPercent(0);
		SetChargePercent(0);
		SetCPRCooldown(m_fCPRCooldownTime);
		ResetPatient();
		
		// Manual changing of state - avoids null errors if using internal method
		m_DefibrillatorState = new ACE_MedicalDefibrillation_DefibrillatorState_Disconnected(GetOwner());
		m_DefibrillatorState.Enter();
		ChangeStateEnum(m_DefibrillatorState);
		Replication.BumpMe();
		
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
			signalsManagerComponent.AddOrFindSignal("ACE_MedicalDefibrillation_ChargeSoundModulator", m_fChargeTime);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_DefibrillatorState_Base GetState() { return m_DefibrillatorState; }
	
	//------------------------------------------------------------------------------------------------
	float GetChargePercent() { return m_fChargePercent; }
	
	void SetChargePercent(float value)
	{
		if (m_fChargePercent == value)
			return;
		
		m_fChargePercent = value;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAnalysisPercent() { return m_fAnalysisPercent; }
	
	void SetAnalysisPercent(float value)
	{
		if (m_fAnalysisPercent == value)
			return;
		
		m_fAnalysisPercent = value;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetCPRCooldown() { return m_fCPRCooldown; }
	float GetCPRCooldownTime() { return m_fCPRCooldownTime; }
	
	void SetCPRCooldown(float value)
	{
		if (m_fCPRCooldown == value)
			return;
		
		m_fCPRCooldown = value;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAnalysisTime() { return m_fAnalysisTime; }
	float GetChargeTime() { return m_fChargeTime; }
	
	//------------------------------------------------------------------------------------------------
	bool GetInitialAnalysisPerformed() { return m_fInitialAnalysisPerformed; }
	
	void SetInitialAnalysisPerformed(bool value)
	{
		m_fInitialAnalysisPerformed = value;
	}
	
	//------------------------------------------------------------------------------------------------
	void ChangeState(ACE_MedicalDefibrillation_DefibrillatorState_Base newState)
	{
		if (m_DefibrillatorState.Type() == newState.Type())
			return;
		
		m_bIsChangingState = true;
		
		m_DefibrillatorState.Exit();
		m_DefibrillatorState = newState;
		m_eDefibrillatorState = ChangeStateEnum(newState);
		Replication.BumpMe();
		m_DefibrillatorState.Enter();
		
		m_bIsChangingState = false;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_EDefibrillatorState GetStateEnum()
	{
		return m_eDefibrillatorState;
	}
	
	//------------------------------------------------------------------------------------------------
	private ACE_MedicalDefibrillation_EDefibrillatorState ChangeStateEnum(ACE_MedicalDefibrillation_DefibrillatorState_Base newState)
	{
		switch (newState.ClassName())
		{
			case "ACE_MedicalDefibrillation_DefibrillatorState_Disconnected":
				return ACE_MedicalDefibrillation_EDefibrillatorState.Disconnected;
			case "ACE_MedicalDefibrillation_DefibrillatorState_Connected":
				return ACE_MedicalDefibrillation_EDefibrillatorState.Connected;
			case "ACE_MedicalDefibrillation_DefibrillatorState_Analysing":
				return ACE_MedicalDefibrillation_EDefibrillatorState.Analysing;
			case "ACE_MedicalDefibrillation_DefibrillatorState_Analysed":
				return ACE_MedicalDefibrillation_EDefibrillatorState.Analysed;
			case "ACE_MedicalDefibrillation_DefibrillatorState_Charging":
				return ACE_MedicalDefibrillation_EDefibrillatorState.Charging;
			case "ACE_MedicalDefibrillation_DefibrillatorState_Charged":
				return ACE_MedicalDefibrillation_EDefibrillatorState.Charged;				
		}
		
		return ACE_MedicalDefibrillation_EDefibrillatorState.Invalid;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCharged()
	{
		return m_fChargePercent >= 1;
	}
	
	//------------------------------------------------------------------------------------------------	
	bool IsAnalysed()
	{
		return m_fAnalysisPercent >= 1;
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
	
	//------------------------------------------------------------------------------------------------
	bool GetConnectedPatient(out IEntity patient, out ACE_Medical_CardiovascularComponent componentOut)
	{
		patient = GetConnectedPatient();
		if (!patient)
			return false;
		
		componentOut = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		
		if (!componentOut)
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
		
		PrintFormat("RPC_Playsound | Attempting to play sound: %1",soundName);
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
	void ResetPatient()
	{
		if (m_Patient)
			PlaySound(SOUNDDISCONNECTED, true);
		
		m_iPatientRplId = RplId.Invalid();
		m_Patient = null;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetDefibrillator()
	{
		m_fAnalysisPercent = 0;
		m_fChargePercent = 0;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestConnectPatient(SCR_ChimeraCharacter patient, SCR_ChimeraCharacter medic)
	{
		Rpc(Rpc_ConnectPatient, Replication.FindId(patient), Replication.FindId(medic));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_ConnectPatient(RplId patientId, RplId medicId)
	{
		ResetPatient();
		
		SCR_ChimeraCharacter patient = SCR_ChimeraCharacter.Cast(ACE_MedicalDefibrillation_ReplicationHelper.GetEntityByRplId(patientId));
		SCR_ChimeraCharacter medic = SCR_ChimeraCharacter.Cast(ACE_MedicalDefibrillation_ReplicationHelper.GetEntityByRplId(medicId));
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(patient.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		PlaySound(SOUNDCONNECTED, true);
		
		m_iPatientRplId = ACE_MedicalDefibrillation_ReplicationHelper.GetRplIdByEntity(patient);
		m_Patient = patient;
		Replication.BumpMe();
		
		// Notify
		if (!m_Patient)
			return;
		
		ACE_MedicalDefibrillation_NetworkComponent networkComponent;
		if (!ACE_MedicalDefibrillator_DefibrillatorBaseUserAction.GetDefibrillatorNetworkComponent(medic, networkComponent))
			return;
		
		ENotification notification;
		if (EntityUtils.IsPlayer(m_Patient))
			notification = ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED;
		else
			notification = ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTCONNECTED_AI;
		
		networkComponent.RequestDefibrillatorNotification(
			notification,
			GetOwner(),
			SCR_ChimeraCharacter.Cast(m_Patient)
		);
	}
	
	//------------------------------------------------------------------------------------------------
	bool DeliverShock()
	{
		if (m_eDefibrillatorState != ACE_MedicalDefibrillation_EDefibrillatorState.Charged)
			return false;
		
		TerminateSound();
		
		// Shock thump sound effect - played on patient
		RPC_PlaySoundOnPatient(ACE_MedicalDefibrillation_DefibrillatorComponent.SOUNDSHOCKTHUMP);
		Rpc(RPC_PlaySoundOnPatient, ACE_MedicalDefibrillation_DefibrillatorComponent.SOUNDSHOCKTHUMP);
		
		IEntity patient;
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!GetConnectedPatient(patient, cardiovascularComponent))
			return false;
	
		cardiovascularComponent.AddShocksDelivered(1);
		HandleContactShockStun(cardiovascularComponent);
		
		// Set defib back to connected state
		ChangeState(new ACE_MedicalDefibrillation_DefibrillatorState_Connected(GetOwner()));
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stun characters that touch the patient while a shock is delivered
	protected void HandleContactShockStun(ACE_Medical_CardiovascularComponent patientCardiovascularComponent)
	{
		SCR_ChimeraCharacter performerChar = patientCardiovascularComponent.GetCPRPreformer();
		if (!performerChar)
			return;
		
		SCR_CharacterControllerComponent performerCharController = SCR_CharacterControllerComponent.Cast(performerChar.GetCharacterController());
		if (!performerCharController)
			return;
		
		performerCharController.ACE_ForceRagdoll(m_fContactShockStunDuration);
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
	void RplState()
	{
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPatientReplicated()
	{
		m_Patient = ACE_MedicalDefibrillation_ReplicationHelper.GetEntityByRplId(m_iPatientRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool IsAlive(IEntity entity)
	{
		if (!entity)
			return false;
		
		DamageManagerComponent damageManager;
		ChimeraCharacter character = ChimeraCharacter.Cast(entity);
		if (character)
			damageManager = character.GetDamageManager();
		else
			damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		
		if (!damageManager)
			return true;
		
		return damageManager.GetState() != EDamageState.DESTROYED;
	}
}