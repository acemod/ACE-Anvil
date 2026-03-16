enum ACE_Medical_Defibrillation_EDefibProgressCategory
{
	StateTimeElapsed,
	Analysis,
	Charge,
	CPRCooldown
}

class ACE_Medical_Defibrillation_DefibProgressData : Managed
{
	ACE_Medical_Defibrillation_DefibComponent m_pDefibComponent;
	
	protected ref map<ACE_Medical_Defibrillation_EDefibProgressCategory, float> m_mDurations;
	protected ref map<ACE_Medical_Defibrillation_EDefibProgressCategory, float> m_mTimers;
	
	ref ScriptInvoker m_OnDataChanged = new ScriptInvoker();
	
	protected float m_fAnalysisDuration;
	protected float m_fChargeDuration;
	protected float m_fCPRCooldownDuration;
	
	void ACE_Medical_Defibrillation_DefibProgressData(ACE_Medical_Defibrillation_DefibComponent defibComponent, float analysisDuration, float chargeDuration, float cprCooldownDuration)
	{
		PrintFormat("%1::Constructor | Building new defib progress data...", this.ClassName());
		
		m_pDefibComponent = defibComponent;
		
		m_mDurations = new map<ACE_Medical_Defibrillation_EDefibProgressCategory, float>();
		m_mTimers = new map<ACE_Medical_Defibrillation_EDefibProgressCategory, float>();
		
		SetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis, analysisDuration);
		SetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge, chargeDuration);
		SetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprCooldownDuration);
		
		SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis, 0);
		SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge, 0);
		SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, 0);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory durationName)
	{
		if (m_mDurations.Contains(durationName))
			return m_mDurations[durationName];
		return -1;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory durationName, float value, bool replicate = true)
	{
		m_mDurations[durationName] = value;
		if (replicate)
			m_OnDataChanged.Invoke();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory timerName)
	{
		if (m_mTimers.Contains(timerName))
			return m_mTimers[timerName];
		return -1;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory timerName, float value, bool replicate = true)
	{
		m_mTimers[timerName] = value;
		if (replicate)
			m_OnDataChanged.Invoke();
	}
	
	//------------------------------------------------------------------------------------------------
	void ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory timerName, float value, bool replicate = true)
	{
		if (m_mTimers.Contains(timerName))
		{
			m_mTimers[timerName] = Math.Max(m_mTimers[timerName] + value, 0);
			if (replicate)
				m_OnDataChanged.Invoke();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory timerName, bool replicate = true)
	{
		m_mTimers[timerName] = 0;
		if (replicate)
			m_OnDataChanged.Invoke();
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetAll(bool replicate = true)
	{
		ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge);
		ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis);
		ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
		ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.StateTimeElapsed);
		if (replicate)
			m_OnDataChanged.Invoke();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPercentComplete(ACE_Medical_Defibrillation_EDefibProgressCategory timerName)
	{
		if (!m_mDurations.Contains(timerName) || !m_mTimers.Contains(timerName))
			return -1;
		
		return Math.Min(m_mTimers[timerName] / m_mDurations[timerName], 1);
	}
}

//------------------------------------------------------------------------------------------------
//! Replication System
modded class ACE_Medical_Defibrillation_DefibProgressData
{
	const static float REPLICATION_TIMER_THRESHOLD = 0.05; // 50ms
		
	//------------------------------------------------------------------------------------------------
	static bool Extract(ACE_Medical_Defibrillation_DefibProgressData instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		float analysisTimer = instance.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis);
		float chargeTimer = instance.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge);
		float cprCooldownTimer = instance.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
		
		snapshot.SerializeFloat(analysisTimer);
		snapshot.SerializeFloat(chargeTimer);
		snapshot.SerializeFloat(cprCooldownTimer);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, ACE_Medical_Defibrillation_DefibProgressData instance)
	{
		float analysisTimer, chargeTimer, cprCooldownTimer;
		
		snapshot.SerializeFloat(analysisTimer);
		snapshot.SerializeFloat(chargeTimer);
		snapshot.SerializeFloat(cprCooldownTimer);
		
		instance.SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis, analysisTimer);
		instance.SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge, chargeTimer);
		instance.SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprCooldownTimer);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		// Read values from snapshot and encode them efficiently
		float analysisTimer, chargeTimer, cprCooldownTimer;
		
		snapshot.SerializeFloat(analysisTimer);
		snapshot.SerializeFloat(chargeTimer);
		snapshot.SerializeFloat(cprCooldownTimer);
		
		// Encode timers - using full 32-bit precision since medical timing is critical
		packet.Serialize(analysisTimer, 32);
		packet.Serialize(chargeTimer, 32);
		packet.Serialize(cprCooldownTimer, 32);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		float analysisTimer, chargeTimer, cprTimer;
		
		// Decode values from packet
		packet.Serialize(analysisTimer, 32);
		packet.Serialize(chargeTimer, 32);
		packet.Serialize(cprTimer, 32);
		
		// Write decoded values to snapshot
		snapshot.SerializeFloat(analysisTimer);
		snapshot.SerializeFloat(chargeTimer);
		snapshot.SerializeFloat(cprTimer);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
	{
		// Compare all 6 float values
		return lhs.CompareSnapshots(rhs, 3 * 4); // 3 floats * 4 bytes each
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(ACE_Medical_Defibrillation_DefibProgressData instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		// Read values from snapshot
		float snapshotAnalysisTimer, snapshotChargeTimer, snapshotCprTimer;
		
		snapshot.SerializeFloat(snapshotAnalysisTimer);
		snapshot.SerializeFloat(snapshotChargeTimer);
		snapshot.SerializeFloat(snapshotCprTimer);
		
		// Compare with instance values using thresholds for timers		
		return Math.AbsFloat(instance.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis) - snapshotAnalysisTimer) < REPLICATION_TIMER_THRESHOLD
			&& Math.AbsFloat(instance.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge) - snapshotChargeTimer) < REPLICATION_TIMER_THRESHOLD
			&& Math.AbsFloat(instance.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown) - snapshotCprTimer) < REPLICATION_TIMER_THRESHOLD;
	}
	
	static void EncodeDelta(SSnapSerializerBase oldSnapshot, SSnapSerializerBase newSnapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		// Read values from both snapshots
		float oldAnalysisTimer, oldChargeTimer, oldCprTimer;
		float newAnalysisTimer, newChargeTimer, newCprTimer;
		
		oldSnapshot.SerializeFloat(oldAnalysisTimer);
		oldSnapshot.SerializeFloat(oldChargeTimer);
		oldSnapshot.SerializeFloat(oldCprTimer);
		
		newSnapshot.SerializeFloat(newAnalysisTimer);
		newSnapshot.SerializeFloat(newChargeTimer);
		newSnapshot.SerializeFloat(newCprTimer);
		
		// Encode which values changed
		bool analysisTimerChanged = Math.AbsFloat(newAnalysisTimer - oldAnalysisTimer) >= REPLICATION_TIMER_THRESHOLD;
		bool chargeTimerChanged = Math.AbsFloat(newChargeTimer - oldChargeTimer) >= REPLICATION_TIMER_THRESHOLD;
		bool cprTimerChanged = Math.AbsFloat(newCprTimer - oldCprTimer) >= REPLICATION_TIMER_THRESHOLD;
		
		// Encode change flags (6 bits)
		packet.Serialize(analysisTimerChanged, 1);
		packet.Serialize(chargeTimerChanged, 1);
		packet.Serialize(cprTimerChanged, 1);
		
		// Encode changed values
		if (analysisTimerChanged)
			packet.Serialize(newAnalysisTimer, 32);
		if (chargeTimerChanged)
			packet.Serialize(newChargeTimer, 32);
		if (cprTimerChanged)
			packet.Serialize(newCprTimer, 32);
	}
	
	static void DecodeDelta(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase oldSnapshot, SSnapSerializerBase newSnapshot)
	{
		// Read old values
		float oldAnalysisTimer, oldChargeTimer, oldCprTimer;
		
		oldSnapshot.SerializeFloat(oldAnalysisTimer);
		oldSnapshot.SerializeFloat(oldChargeTimer);
		oldSnapshot.SerializeFloat(oldCprTimer);
		
		// Read change flags
		bool analysisTimerChanged, chargeTimerChanged, cprTimerChanged;
		
		packet.Serialize(analysisTimerChanged, 1);
		packet.Serialize(chargeTimerChanged, 1);
		packet.Serialize(cprTimerChanged, 1);
		
		// Initialize new values with old values
		float newAnalysisTimer = oldAnalysisTimer;
		float newChargeTimer = oldChargeTimer;
		float newCprTimer = oldCprTimer;
		
		// Override with new values if they changed
		if (analysisTimerChanged)
		{
			packet.Serialize(newAnalysisTimer, 32);
		}
		
		if (chargeTimerChanged)
		{
			packet.Serialize(newChargeTimer, 32);
		}
		
		if (cprTimerChanged)
		{
			packet.Serialize(newCprTimer, 32);
		}
		
		// Write to new snapshot
		newSnapshot.SerializeFloat(newAnalysisTimer);
		newSnapshot.SerializeFloat(newChargeTimer);
		newSnapshot.SerializeFloat(newCprTimer);
	}
}