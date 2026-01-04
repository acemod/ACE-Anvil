class ACE_Medical_Defibrillation_DefibSoundComponentClass : SoundComponentClass
{
}

class ACE_Medical_Defibrillation_DefibSoundComponent : SoundComponent
{
	AudioHandle m_pCurrentSound;
	
	//------------------------------------------------------------------------------------------------
	AudioHandle GetCurrentSound()
	{
		return m_pCurrentSound;
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
		Rpc(TerminateSound, terminateAll);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySound(string soundName, bool terminatePrevious, bool isLoop)
	{		
		if (isLoop && m_pCurrentSound)
			return;
		
		if (terminatePrevious)
			TerminateSound();
		
		m_pCurrentSound = SoundEvent(soundName);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_TerminateSound(bool terminateAll)
	{
		if (terminateAll)
			TerminateAll();
		else
			Terminate(m_pCurrentSound);
		
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
		ACE_Medical_Defibrillation_DefibComponent defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibComponent));
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
}