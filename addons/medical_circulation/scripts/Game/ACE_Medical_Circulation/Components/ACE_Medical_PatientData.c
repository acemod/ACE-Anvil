//------------------------------------------------------------------------------------------------
//! Data to be synchronized for medic
class ACE_Medical_PatientData : Managed
{
	protected float m_fHeartRate;
	protected float m_fMeanArterialPressure;
	protected float m_fPulsePressure;
	
	protected static const int SNAPSHOT_SIZE = 12;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_PatientData Create(notnull SCR_ChimeraCharacter char)
	{
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent)
			return null;
		
		ACE_Medical_PatientData instance = new ACE_Medical_PatientData();
		instance.m_fHeartRate = vitalsComponent.GetHeartRate();
		instance.m_fMeanArterialPressure = vitalsComponent.GetMeanArterialPressure();
		instance.m_fPulsePressure = vitalsComponent.GetPulsePressure();
		return instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void Apply(notnull SCR_ChimeraCharacter char)
	{
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(char.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent)
			return;
		
		vitalsComponent.SetHeartRate(m_fHeartRate);
		vitalsComponent.SetMeanArterialPressure(m_fMeanArterialPressure);
		vitalsComponent.SetPulsePressure(m_fPulsePressure);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(ACE_Medical_PatientData instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeFloat(instance.m_fHeartRate);
		snapshot.SerializeFloat(instance.m_fMeanArterialPressure);
		snapshot.SerializeFloat(instance.m_fPulsePressure);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, ACE_Medical_PatientData instance)
	{
		snapshot.SerializeFloat(instance.m_fHeartRate);
		snapshot.SerializeFloat(instance.m_fMeanArterialPressure);
		snapshot.SerializeFloat(instance.m_fPulsePressure);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		snapshot.Serialize(packet, SNAPSHOT_SIZE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		return snapshot.Serialize(packet, SNAPSHOT_SIZE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
	{
		return lhs.CompareSnapshots(rhs, SNAPSHOT_SIZE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(ACE_Medical_PatientData instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		return snapshot.CompareFloat(instance.m_fHeartRate)
			&& snapshot.CompareFloat(instance.m_fMeanArterialPressure)
			&& snapshot.CompareFloat(instance.m_fPulsePressure);
	}
}
