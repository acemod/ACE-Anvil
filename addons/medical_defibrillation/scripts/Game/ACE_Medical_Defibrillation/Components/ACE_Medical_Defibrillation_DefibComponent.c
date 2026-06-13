class ACE_Medical_Defibrillation_DefibComponentClass : ScriptComponentClass
{
}

class ACE_Medical_Defibrillation_DefibComponent : ScriptComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibEmulation), category: "General Settings")]
	protected ACE_Medical_Defibrillation_EDefibEmulation m_eDefibrillatorEmulation;
	
	protected float m_fAnalysisDuration = 3;
	protected float m_fChargeDuration = 5.5;
	
	[Attribute(defvalue: "120", params: "0 inf 1", desc: "Time (s) between analysis events where players should perform CPR.", category: "AED Settings")]
	protected float m_fCPRCooldownDuration;
	
	[Attribute(defvalue: "true", desc: "Plays pacing beats when players should be performing CPR.", category: "AED Settings")]
	protected bool m_bPlayCPRPacingBeats;
	
	[Attribute(defvalue: "false", desc: "Always sets the AED to detect shockable rhythm. Use to quickly test shock systems.", category: "DEBUG")]
	protected bool m_bDebugAlwaysShockableRhythm;
	
	[RplProp(onRplName: "OnPatientReplicated")]
	protected int m_iPatientRplId;
	protected IEntity m_pPatient;
	
	[RplProp(onRplName: "OnDefibStateChanged")]
	protected ACE_Medical_Defibrillation_EDefibStateID m_eDefibrillatorStateID;
	
	[RplProp(onRplName: "OnDefibProgressChanged"), RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	protected ref ACE_Medical_Defibrillation_DefibProgressData m_pProgressData;
	
	ref ACE_Medical_Defibrillation_DefibSounds m_pSounds;
	
	protected bool m_bCPRBeepLoop = false;
	protected bool m_bChargedBeepLoop = false;
	
	protected AudioHandle m_pCurrentSound;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{	
		super.EOnInit(owner);
		
		SetEventMask(owner, EntityEvent.FRAME);
		
		// Convert to milliseconds and make data
		m_pProgressData = new ACE_Medical_Defibrillation_DefibProgressData(this,
																		   m_fAnalysisDuration * 1000,
																		   m_fChargeDuration * 1000,
																		   m_fCPRCooldownDuration * 1000);
		
		// Create sound data
		m_pSounds = new ACE_Medical_Defibrillation_DefibSounds();
		
		// Subscribe to the InventoryItemComponent OnParentSlotChanged
		// Determines if defib is already on the ground to add it to the system
		InventoryItemComponent invComp = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		if (invComp)
		{
			invComp.m_OnParentSlotChangedInvoker.Insert(OnParentSlotChanged);
			if (!invComp.GetParentSlot())
			{
				ACE_Medical_Defibrillation_DefibStatesSystem system = GetDefibStatesSystem();
				if (system)
					system.Register(owner);
			}
		}
		
		// Subscribe to data change events for replication
		m_pProgressData.m_OnDataChanged.Insert(OnDefibProgressChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_Defibrillation_Settings GetDefibSystemSettings()
	{
		ACE_Medical_Defibrillation_Settings settings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Settings>.GetModSettings();
		if (!settings)
			return null;
		
		return settings;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Temporary until a client side sound manager system is possibly implimented
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		timeSlice *= 1000;
		
		// CPR Beep
		if (m_bCPRBeepLoop)
		{
			m_pSounds.m_fLastCPRPaceTimer += timeSlice;
			
			if (m_pSounds.m_fLastCPRPaceTimer >= ACE_Medical_Defibrillation_GlobalHelpers.BpmToMs(102))
			{
				PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCPRBEEP);
				m_pSounds.m_fLastCPRPaceTimer = 0;
			}
		}
		
		// Charged Sound
		if (m_bChargedBeepLoop)
		{
			const float BEEP_INTERVAL = 250;
			
			if (m_pSounds.m_fChargedBeepTimer >= BEEP_INTERVAL)
			{
				if (Math.Mod(m_pSounds.m_iChargedBeepPhase, 2) == 0)
				{
					PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCHARGEDBEEPLOW);
				}
				else
				{
					PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCHARGEDBEEPHIGH);
				}
				
				// Advance to next phase and reset timer
				m_pSounds.m_iChargedBeepPhase++;
				m_pSounds.m_fChargedBeepTimer = 0;
			}
			
			m_pSounds.m_fChargedBeepTimer += timeSlice;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	SoundComponent GetSoundComponent()
	{
		SoundComponent component = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		if (!component)
			return null;
		
		return component;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_DefibStatesSystem GetDefibStatesSystem()
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return null;
		
		ACE_Medical_Defibrillation_DefibStatesSystem system = ACE_Medical_Defibrillation_DefibStatesSystem.Cast(world.FindSystem(ACE_Medical_Defibrillation_DefibStatesSystem));
		if (!system)
			return null;
		
		return system;
	}
	
	//------------------------------------------------------------------------------------------------
	void Reset()
	{
		SetPatient(null);
		
		// Convert to milliseconds and make data
		m_pProgressData = new ACE_Medical_Defibrillation_DefibProgressData(this,
																		   m_fAnalysisDuration * 1000,
																		   m_fChargeDuration * 1000,
																		   m_fCPRCooldownDuration * 1000);
		
		// Create sound data
		m_pSounds = new ACE_Medical_Defibrillation_DefibSounds();
		m_eDefibrillatorStateID = ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_EDefibEmulation GetDefibrillatorEmulation()
	{
		return m_eDefibrillatorEmulation;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_EDefibStateID GetDefibStateID()
	{
		return m_eDefibrillatorStateID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID state)
	{
		m_eDefibrillatorStateID = state;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_DefibProgressData GetDefibProgressData()
	{
		return m_pProgressData;
	}
	
	//------------------------------------------------------------------------------------------------
	bool SetPatient(IEntity patient)
	{
		if (!patient)
		{
			m_iPatientRplId = ACE_Medical_Defibrillation_ReplicationHelper.GetRplIdByEntity(null);
			m_pPatient = null;
			Replication.BumpMe();
			return false;
		}
		
		ACE_Medical_VitalsComponent component = ACE_Medical_VitalsComponent.Cast(patient.FindComponent(ACE_Medical_VitalsComponent));
		if (!component)
			return false;
		
		m_iPatientRplId = ACE_Medical_Defibrillation_ReplicationHelper.GetRplIdByEntity(patient);
		m_pPatient = patient;
		Replication.BumpMe();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPatient()
	{
		m_iPatientRplId = -1;
		m_pPatient = null;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetPatient()
	{
		return m_pPatient;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsShockableRhythm()
	{
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_pPatient.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return false;
		
		array<ACE_Medical_Defibrillation_ECardiacRhythm> shockableRhythms = { ACE_Medical_Defibrillation_ECardiacRhythm.VF };
		if (shockableRhythms.Contains(vitals.GetCardiacRhythm()))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ShockPatient()
	{
	    if (!m_pPatient)
	        return false;
	    
	    ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_pPatient.FindComponent(ACE_Medical_VitalsComponent));
	    if (!vitals)
	        return false;

	    ACE_Medical_Defibrillation_Settings settings = GetDefibSystemSettings();
	    
	    // Calculate shock success chance
	    float shockSuccessChance = CalculateShockSuccessChance(vitals);
	    
	    // Roll for shock success
	    float randomRoll = Math.RandomFloat01();
	    bool shockSuccessful = randomRoll < shockSuccessChance;
	    
	    // Only increment shock count if shock was successful
	    if (shockSuccessful)
	    {
	        vitals.ModifyShocksDelivered(1);
			vitals.ResetTimeSinceLastShock();
	    }
		
		PlaySoundOnPatient(ACE_Medical_Defibrillation_DefibSounds.SOUNDSHOCKTHUMP);	    
	    SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
	    
	    float cprCooldown = m_pProgressData.GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
	    m_pProgressData.SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprCooldown);
	    
	    return shockSuccessful;
	}
	
	//------------------------------------------------------------------------------------------------
	static float CalculateShockSuccessChance(ACE_Medical_VitalsComponent vitals)
	{
	    int shocks = vitals.GetShocksDelivered();
	    float timeSinceLastShock = vitals.GetTimeSinceLastShock();
	    ACE_Medical_Defibrillation_Settings settings = GetDefibSystemSettings();
	    
	    float spamPenalty = ACE_Medical_Defibrillation_DecayCalculator.CalculateSpamPenalty(vitals);
	    float shockChance = ACE_Medical_Defibrillation_DecayCalculator.CalculateShockChance(vitals);
	    
	    float finalChance = shockChance * (1.0 - spamPenalty);
	    
	    return Math.Clamp(finalChance, 0.0, settings.m_fMaxShockSuccessChance);
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnDefibProgressChanged()
	{
		Replication.BumpMe();
	}
	
	
	//------------------------------------------------------------------------------------------------
	private void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		ACE_Medical_Defibrillation_DefibStatesSystem system = GetDefibStatesSystem();
		if (!system)
			return;
		
		if (!newSlot)
		{
			system.Register(GetOwner());
		}
		else
		{
			system.Unregister(GetOwner());
			Reset();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnDefibStateChanged()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPatientReplicated()
	{
		GetGame().GetCallqueue().Call(OnPatientReplicatedDelayed);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPatientReplicatedDelayed()
	{
		m_pPatient = ACE_Medical_Defibrillation_ReplicationHelper.GetEntityByRplId(m_iPatientRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	AudioHandle GetCurrentSound()
	{
		return m_pCurrentSound;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCPRBeepLoop(bool state = false)
	{
		m_bCPRBeepLoop = state;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetChargedBeepLoop(bool state = false)
	{
		m_bChargedBeepLoop = state;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void PlaySound(string soundName, bool terminatePrevious = false, bool isLoop = false)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_PlaySound(soundName, terminatePrevious, isLoop);
		Rpc(RPC_PlaySound, soundName, terminatePrevious, isLoop);
	}
	
	//------------------------------------------------------------------------------------------------
	void TerminateSound(bool terminateAll = false)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_TerminateSound(terminateAll);
		Rpc(RPC_TerminateSound, terminateAll);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySound(string soundName, bool terminatePrevious, bool isLoop)
	{		
		if (isLoop && m_pCurrentSound)
			return;
		
		if (terminatePrevious)
			TerminateSound();
		
		SoundComponent sndComp = GetSoundComponent();
		if (sndComp)
			m_pCurrentSound = sndComp.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_TerminateSound(bool terminateAll)
	{
		SoundComponent sndComp = GetSoundComponent();
		if (!sndComp)
			return;
		
		if (terminateAll)
			sndComp.TerminateAll();
		else
			sndComp.Terminate(m_pCurrentSound);
		
		m_pCurrentSound = null;
	}
	
	//------------------------------------------------------------------------------------------------
	void PlaySoundOnPatient(string soundName)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_PlaySoundOnPatient(ACE_Medical_Defibrillation_DefibSounds.SOUNDSHOCKTHUMP);
		Rpc(RPC_PlaySoundOnPatient, ACE_Medical_Defibrillation_DefibSounds.SOUNDSHOCKTHUMP);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySoundOnPatient(string soundName)
	{
		IEntity patient = GetPatient();
		if (!patient)
			return;
		
		CharacterSoundComponent sndComponent = CharacterSoundComponent.Cast(patient.FindComponent(CharacterSoundComponent));
		if (!sndComponent)
			return;
		
		sndComponent.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetDebugAlwaysShockableRhythm()
	{
		return m_bDebugAlwaysShockableRhythm;
	}
}