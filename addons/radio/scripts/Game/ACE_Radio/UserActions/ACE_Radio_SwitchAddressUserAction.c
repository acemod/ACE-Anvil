//------------------------------------------------------------------------------------------------
class ACE_Radio_SwitchAddressUserAction : SCR_AdjustSignalAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter userChar = ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		CharacterControllerComponent userCharController = userChar.GetCharacterController();
		if (!userCharController)
			return false;
		
		return userCharController.GetInspect();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		ACE_Radio_FillDeviceComponent device = ACE_Radio_FillDeviceComponent.Cast(GetOwner().FindComponent(ACE_Radio_FillDeviceComponent));
		if (!device)
			return false;
		
		outName = string.Format("Address: %1", device.GetCurrentKeyName());
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool OnSaveActionData(ScriptBitWriter writer)
	{
		super.OnSaveActionData(writer);
		UpdateAnimation();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool OnLoadActionData(ScriptBitReader reader)
	{
		super.OnLoadActionData(reader);
		UpdateAnimation();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateAnimation()
	{
		Animation anim = GetOwner().GetAnimation();
		TNodeId pivot = anim.GetBoneIndex("i_address_switch");
		vector transform[4];
		Math3D.AnglesToMatrix(Vector(28.4 * m_fTargetValue, 0, 0), transform);
		anim.SetBoneMatrix(GetOwner(), pivot, transform);
		
		SCR_SoundManagerEntity soundManager = GetGame().GetSoundManagerEntity();
		if (soundManager)
			soundManager.CreateAndPlayAudioSource(GetOwner(), SCR_SoundEvent.SOUND_ITEM_RADIO_TUNE_UP);
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
		return true;
	}
}
