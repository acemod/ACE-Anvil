//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
modded class ACE_SettingsConfig
{
	protected static const int ACE_FINGER_SNAPSHOT_SIZE = 2 * 4;
	
	//------------------------------------------------------------------------------------------------
	override static bool Extract(ACE_SettingsConfig instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		if (!super.Extract(instance, ctx, snapshot))
			return false;
		
		ACE_Finger_Settings settings = ACE_Finger_Settings.Cast(instance.GetModSettings(ACE_Finger_Settings));
		if (!settings)
			return false;
		
		snapshot.SerializeFloat(settings.m_fMaxPointingDistanceM);
		snapshot.SerializeFloat(settings.m_fPingRangeM);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, ACE_SettingsConfig instance)
	{
		if (!super.Inject(snapshot, ctx, instance))
			return false;
		
		ACE_Finger_Settings settings = ACE_Finger_Settings.Cast(instance.GetModSettings(ACE_Finger_Settings));
		if (!settings)
			return false;
		
		snapshot.SerializeFloat(settings.m_fMaxPointingDistanceM);
		snapshot.SerializeFloat(settings.m_fPingRangeM);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		super.Encode(snapshot, ctx, packet);
		snapshot.Serialize(packet, ACE_FINGER_SNAPSHOT_SIZE);
	}

	//------------------------------------------------------------------------------------------------
	override static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		if (!super.Decode(packet, ctx, snapshot))
			return false;
		
		snapshot.Serialize(packet, ACE_FINGER_SNAPSHOT_SIZE);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs , ScriptCtx ctx)
	{
		if (!super.SnapCompare(lhs, rhs, ctx))
			return false;
		
		return lhs.CompareSnapshots(rhs, ACE_FINGER_SNAPSHOT_SIZE);
	}

	//------------------------------------------------------------------------------------------------
	override static bool PropCompare(ACE_SettingsConfig instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		if (!super.PropCompare(instance, snapshot, ctx))
			return false;
		
		ACE_Finger_Settings settings = ACE_Finger_Settings.Cast(instance.GetModSettings(ACE_Finger_Settings));
		if (!settings)
			return false;
		
		return snapshot.CompareFloat(settings.m_fMaxPointingDistanceM)
			&& snapshot.CompareFloat(settings.m_fPingRangeM);
	}
}
