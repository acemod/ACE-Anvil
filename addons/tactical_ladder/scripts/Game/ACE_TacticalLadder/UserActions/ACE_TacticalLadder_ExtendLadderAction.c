//------------------------------------------------------------------------------------------------
class ACE_TacticalLadder_ExtendLadderAction : SCR_AdjustSignalAction
{
	protected ACE_TacticalLadderEntity m_pOwner;
	
	//------------------------------------------------------------------------------------------------
	protected override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		m_pOwner = ACE_TacticalLadderEntity.Cast(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Force update of entity when signal was adjusted
	override protected bool OnSaveActionData(ScriptBitWriter writer)
	{
		bool result = super.OnSaveActionData(writer);
		m_pOwner.UpdateScriptedLocal();
		return result;
	}

	//------------------------------------------------------------------------------------------------
	//! Force update of entity when signal was adjusted
	override protected bool OnLoadActionData(ScriptBitReader reader)
	{
		bool result = super.OnLoadActionData(reader);
		m_pOwner.UpdateScriptedLocal();
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handle streaming out of signal
	override protected bool OnRplSave(ScriptBitWriter writer)
	{
		super.OnRplSave(writer);
		writer.WriteFloat(m_fTargetValue);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handle streaming in of signal
	override protected bool OnRplLoad(ScriptBitReader reader)
	{
		super.OnRplLoad(reader);
		reader.ReadFloat(m_fTargetValue);
		SetSignalValue(m_fTargetValue);
		m_pOwner.UpdateScriptedLocal();
		return true;
	}
}
