class ACE_Temperature_HeatPackDamageEffect : SCR_DotDamageEffect {
	
	
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		return false;
	}
	override event void OnEffectApplied(SCR_ExtendedDamageManagerComponent dmgManager)
	{	
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(player.FindComponent(ACE_Medical_VitalsComponent));
		vitals.m_iHeatPackCount+=1;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override event void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(player.FindComponent(ACE_Medical_VitalsComponent));
 		vitals.m_iHeatPackCount-=1;
	}
	
	bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out int reason){
		return true;
	
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}

}