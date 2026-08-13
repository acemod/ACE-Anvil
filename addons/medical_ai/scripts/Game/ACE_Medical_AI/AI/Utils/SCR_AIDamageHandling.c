//---------------------------------------------------------------------------------------------------------
modded class SCR_AIDamageHandling
{
	//---------------------------------------------------------------------------------------------------------
	//! Also consider as wounded when a consumable is applicable
	override static bool IsCharacterWounded(IEntity entity)
	{
		if (super.IsCharacterWounded(entity))
			return true;
		
		ChimeraCharacter char = ChimeraCharacter.Cast(entity);
		if (!char)
			return false;
		
		ACE_Medical_AIDecideMedicAction task = new ACE_Medical_AIDecideMedicAction();
		return task.SelectConsumableTypeToApply(char) != SCR_EConsumableType.NONE;
	}
}
