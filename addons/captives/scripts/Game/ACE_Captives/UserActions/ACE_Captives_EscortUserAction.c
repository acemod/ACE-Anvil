//------------------------------------------------------------------------------------------------
class ACE_Captives_EscortCaptiveUserAction : ScriptedUserAction
{
	protected SCR_CharacterControllerComponent m_pOwnerCharCtrl;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		m_pOwnerCharCtrl = SCR_CharacterControllerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterControllerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		if (!m_pOwnerCharCtrl.ACE_Captives_IsCaptive())
			return false;
		
		if (m_pOwnerCharCtrl.ACE_IsCarried())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		m_pOwnerCharCtrl.ACE_Captives_EscortCaptive(pUserEntity);
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
