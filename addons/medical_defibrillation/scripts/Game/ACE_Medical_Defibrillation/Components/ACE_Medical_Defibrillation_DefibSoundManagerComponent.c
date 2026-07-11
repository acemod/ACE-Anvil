class ACE_Medical_Defibrillation_DefibSoundManagerComponentClass : ScriptComponentClass
{
}

class ACE_Medical_Defibrillation_DefibSoundManagerComponent : ScriptComponent
{
	ref ACE_Medical_Defibrillation_DefibSoundTimers m_pSoundTimers;
	
	[RplProp()]
	protected bool m_bCPRBeepLoop = false;
	[RplProp()]
	protected bool m_bChargedBeepLoop = false;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SetEventMask(owner, EntityEvent.FRAME);
		
		m_pSoundTimers = new ACE_Medical_Defibrillation_DefibSoundTimers();
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		timeSlice *= 1000;
		UpdateCPRBeeps(timeSlice);
		UpdateChargedBeeps(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	SoundComponent GetSoundComponent()
	{
		return SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	void Reset()
	{
		m_pSoundTimers = new ACE_Medical_Defibrillation_DefibSoundTimers();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Plays a sound on the defibrillator's sound component.
	void PlaySoundGlobal(string soundName)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_PlaySound(soundName);
		Rpc(RPC_PlaySound, soundName);
		return;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates all sounds from the defibrillator.
	void TerminateAllSoundsGlobal()
	{
		if (!Replication.IsServer())
			return;
		
		RPC_TerminateAllSounds();
		Rpc(RPC_TerminateAllSounds);
		return;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Plays a sound on the patient's sound component.
	void PlaySoundOnPatientGlobal(string soundName)
	{
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_ComponentManager.GetDefibComponent(GetOwner());
		if (!defibComponent)
			return;
		
		IEntity patient = defibComponent.GetPatient();
		if (!patient)
			return;
		
		RPC_PlaySoundOnPatient(soundName);
		Rpc(RPC_PlaySoundOnPatient, soundName);
		return;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RPC_PlaySound(string soundName)
	{
		SoundComponent sndComponent = GetSoundComponent();
		if (!sndComponent)
			return;
		
		sndComponent.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RPC_TerminateAllSounds()
	{
		SoundComponent sndComponent = GetSoundComponent();
		if (!sndComponent)
			return;
		
		sndComponent.TerminateAll();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RPC_PlaySoundOnPatient(string soundName)
	{
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_ComponentManager.GetDefibComponent(GetOwner());
		if (!defibComponent)
			return;
		
		IEntity patient = defibComponent.GetPatient();
		if (!patient)
			return;
		
		CharacterSoundComponent sndComponent = CharacterSoundComponent.Cast(patient.FindComponent(CharacterSoundComponent));
		if (!sndComponent)
			return;
		
		sndComponent.SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCPRBeepLoop(bool enabled)
	{
		m_bCPRBeepLoop = enabled;
		Replication.BumpMe();
	}
	
	void SetChargedBeepLoop(bool enabled)
	{
		m_bChargedBeepLoop = enabled;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Plays a local cpr beep at appropriate interval
	protected void UpdateCPRBeeps(float timeSlice)
	{
		if (!m_bCPRBeepLoop)
			return;
		
		SoundComponent sndComponent = GetSoundComponent();
		if (!sndComponent)
			return;
		
		m_pSoundTimers.m_fLastCPRPaceTimer += timeSlice;
		
		const float CPR_PACING_BEATS_BPM = 105;
		if (m_pSoundTimers.m_fLastCPRPaceTimer >= ACE_Medical_Defibrillation_ConversionHelper.BpmToMs(CPR_PACING_BEATS_BPM))
		{
			sndComponent.SoundEvent(ACE_Medical_Defibrillation_SharedSounds.SOUNDCPRBEEP);
			m_pSoundTimers.m_fLastCPRPaceTimer = 0;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Plays a local charged beep at appropriate interval
	protected void UpdateChargedBeeps(float timeSlice)
	{
		if (!m_bChargedBeepLoop)
			return;
		
		SoundComponent sndComponent = GetSoundComponent();
		if (!sndComponent)
			return;
		
		const float CHARGED_BEEP_INTERVAL_MS = 250;
		m_pSoundTimers.m_fChargedBeepTimer += timeSlice;
		
		if (m_pSoundTimers.m_fChargedBeepTimer >= CHARGED_BEEP_INTERVAL_MS)
		{
			if (Math.Mod(m_pSoundTimers.m_iChargedBeepPhase, 2) == 0)
			{
				sndComponent.SoundEvent(ACE_Medical_Defibrillation_SharedSounds.SOUNDCHARGEDBEEPLOW);
			}
			else
			{
				sndComponent.SoundEvent(ACE_Medical_Defibrillation_SharedSounds.SOUNDCHARGEDBEEPHIGH);
			}
			
			m_pSoundTimers.m_iChargedBeepPhase++;
			m_pSoundTimers.m_fChargedBeepTimer = 0;
		}
	}
}