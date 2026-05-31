[BaseContainerProps()]
class ACE_Temperature_ConsumableHeatpack : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	//! Can be applied when patient is in pain and no morphine is in the system
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{	
		ChimeraCharacter targetChar = ChimeraCharacter.Cast(target);
		if (!targetChar)
			return false;
		
		SCR_CharacterDamageManagerComponent targetDamageManager = SCR_CharacterDamageManagerComponent.Cast(targetChar.GetDamageManager());
		if (!targetDamageManager)
			return false;
		
		return true;
	}
	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableHeatpack()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_HEATPACK;
	}
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
}