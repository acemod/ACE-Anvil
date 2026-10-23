//------------------------------------------------------------------------------------------------
class ACE_Captives_SurrenderHelperCompartmentClass : ACE_AnimationHelperCompartmentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Captives_SurrenderHelperCompartment : ACE_AnimationHelperCompartment
{
	protected Faction m_pTrueFaction;
	
	//------------------------------------------------------------------------------------------------
	override void Init(SCR_ChimeraCharacter performer)
	{
		super.Init(performer);
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(performer.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_Captives_SetHasSurrendered(true);
		
		SCR_CharacterFactionAffiliationComponent factionAffiliation = SCR_CharacterFactionAffiliationComponent.Cast(performer.FindComponent(SCR_CharacterFactionAffiliationComponent));
		if (factionAffiliation)
			factionAffiliation.SetAffiliatedFactionByKey("CIV");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnCompartmentLeft()
	{
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_Captives_SetHasSurrendered(false);
		
		// Do not reset faction when we transition to captive state
		SCR_CharacterFactionAffiliationComponent factionAffiliation = SCR_CharacterFactionAffiliationComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterFactionAffiliationComponent));
		if (factionAffiliation && !charCtrl.ACE_Captives_IsCaptive())
			factionAffiliation.SetAffiliatedFaction(factionAffiliation.GetDefaultAffiliatedFaction());
		
		super.OnCompartmentLeft();
	}
}
