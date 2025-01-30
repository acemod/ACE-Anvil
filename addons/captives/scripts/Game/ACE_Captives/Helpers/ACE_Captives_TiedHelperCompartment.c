//------------------------------------------------------------------------------------------------
class ACE_Captives_TiedHelperCompartmentClass : ACE_AnimationHelperCompartmentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Captives_TiedHelperCompartment : ACE_AnimationHelperCompartment
{
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity performer)
	{
		super.Init(performer);
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_Captives_SetIsCaptive(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnCompartmentLeft()
	{
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_Captives_SetIsCaptive(false);
		
		super.OnCompartmentLeft();
	}
}
