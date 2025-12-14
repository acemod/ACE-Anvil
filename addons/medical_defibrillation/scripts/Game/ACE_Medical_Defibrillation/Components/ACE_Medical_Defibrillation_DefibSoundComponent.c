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
	void PlayDefibSoundServer(string soundName, bool terminatePrevious = false, bool isLoop = false)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_PlaySound(soundName, terminatePrevious, isLoop);
		Rpc(RPC_PlaySound, soundName, terminatePrevious, isLoop);
	}
	
	//------------------------------------------------------------------------------------------------
	void TerminateDefibSound(bool terminateAll = false)
	{
		if (!Replication.IsServer())
			return;
		
		RPC_TerminateSound(terminateAll);
		Rpc(TerminateDefibSound, terminateAll);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_PlaySound(string soundName, bool terminatePrevious, bool isLoop)
	{		
		if (isLoop && m_pCurrentSound)
			return;
		
		if (terminatePrevious)
			TerminateDefibSound();
		
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
}