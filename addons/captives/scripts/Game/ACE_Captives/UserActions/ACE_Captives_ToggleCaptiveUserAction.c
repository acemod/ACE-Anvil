//------------------------------------------------------------------------------------------------
class ACE_Captives_ReleaseCaptiveUserAction : ScriptedUserAction
{
	[Attribute(defvalue: "#ACE_Captives-UserAction_TakeAsPrisoner", desc: "String shown for taking as captive")]
	protected LocalizedString m_sTakeCaptive;
	
	[Attribute(defvalue: "#ACE_Captives-UserAction_ReleasePrisoner", desc: "String shown for releasing captive")]
	protected LocalizedString m_sReleaseCaptive;

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar == user)
			return false;
		
		if (ownerChar.IsInVehicle() && !ACE_AnimationTools.GetHelperCompartment(ownerChar))
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return false;
		
		if (ownerCharController.ACE_IsCarried())
			return false;
		
		return ownerCharController.ACE_Captives_IsCaptive();
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!ownerChar)
			return;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return;
		
		ownerCharController.ACE_Captives_SetCaptive(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}
