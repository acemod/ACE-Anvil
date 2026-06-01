class ACE_Temperature_HeatpackDamageEffect : SCR_DotDamageEffect {
	
	
//	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
//	{
//		return false;
//	}
	override event void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{	
		super.OnEffectAdded(dmgManager);
		
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(player.FindComponent(ACE_Medical_VitalsComponent));
		vitals.m_iHeatPackCount=vitals.m_iHeatPackCount+1;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override event void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(player.FindComponent(ACE_Medical_VitalsComponent));
 		vitals.m_iHeatPackCount=vitals.m_iHeatPackCount-1;
	}
	
	bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out int reason){
		return true;
	
	}
	
	//------------------------------------------------------------------------------------------------

}