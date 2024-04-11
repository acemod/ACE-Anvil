//------------------------------------------------------------------------------------------------
void ACE_ScriptInvokerBaseVarMethod(ACE_BaseVar var);
typedef func ACE_ScriptInvokerBaseVarMethod;
typedef ScriptInvokerBase<ACE_ScriptInvokerBaseVarMethod> ACE_ScriptInvokerBaseVar;

//------------------------------------------------------------------------------------------------
//! Similar to SCR_BaseEditorAttributeVar, but smaller, managed and immutable
class ACE_BaseVar : Managed
{
	protected static const int SIZE_IN_BYTES = 4;
	protected float m_fValue;
	
	//------------------------------------------------------------------------------------------------
	static ACE_BaseVar CreateInt(int value)
	{
		ACE_BaseVar var = new ACE_BaseVar();
		var.m_fValue = value;
		return var;
	}

	//------------------------------------------------------------------------------------------------
	int GetInt()
	{
		return m_fValue;
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_BaseVar CreateFloat(float value)
	{
		ACE_BaseVar var = new ACE_BaseVar();
		var.m_fValue = value;
		return var;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetFloat()
	{
		return m_fValue;
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_BaseVar CreateBool(bool value)
	{
		ACE_BaseVar var = new ACE_BaseVar();
		var.m_fValue = value;
		return var;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetBool()
	{
		return m_fValue;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx hint, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, SIZE_IN_BYTES);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		return snapshot.Serialize(packet, SIZE_IN_BYTES);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx hint) 
	{
		return lhs.CompareSnapshots(rhs, SIZE_IN_BYTES);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(ACE_BaseVar prop, SSnapSerializerBase snapshot, ScriptCtx hint) 
	{
		return snapshot.Compare(prop.m_fValue, SIZE_IN_BYTES);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(ACE_BaseVar prop, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		snapshot.SerializeBytes(prop.m_fValue, SIZE_IN_BYTES);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx hint, ACE_BaseVar prop) 
	{
		return Extract(prop, hint, snapshot);
	}
}
