//------------------------------------------------------------------------------------------------
//! Prevent captive and surrendered players from seeing name tags
[BaseContainerProps()]
modded class SCR_NameTagRulesetBase : Managed
{
	protected bool m_bACE_IsLocalPlayerCaptiveOrSurrendered = false;
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_UpdateIsLocalPlayerCaptiveOrSurrendered()
	{
		m_bACE_IsLocalPlayerCaptiveOrSurrendered = false;
		
		SCR_ChimeraCharacter localPlayer = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		
		SCR_CharacterControllerComponent localCharController = SCR_CharacterControllerComponent.Cast(localPlayer.GetCharacterController());
		if (!localCharController)
			return;
		
		m_bACE_IsLocalPlayerCaptiveOrSurrendered = (localCharController.ACE_Captives_IsCaptive() || localCharController.ACE_Captives_HasSurrendered())
	}
	
	//------------------------------------------------------------------------------------------------
	override void DetermineVisibility(float timeSlice)
	{
		ACE_UpdateIsLocalPlayerCaptiveOrSurrendered();
		super.DetermineVisibility(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool TestVisibility(SCR_NameTagData data, float timeSlice)
	{
		if (m_bACE_IsLocalPlayerCaptiveOrSurrendered)
			return false;
		
		return super.TestVisibility(data, timeSlice);
	}
}
