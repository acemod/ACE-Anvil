//------------------------------------------------------------------------------------------------
class ACE_Captives_SurrenderHelperCompartmentClass : ACE_AnimationHelperCompartmentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Captives_SurrenderHelperCompartment : ACE_AnimationHelperCompartment
{
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity performer)
	{
		super.Init(performer);
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_Captives_SetHasSurrendered(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnCompartmentLeft()
	{
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_Captives_SetHasSurrendered(false);
		
		super.OnCompartmentLeft();
	}
}
