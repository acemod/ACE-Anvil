//------------------------------------------------------------------------------------------------
class ACE_Captives_TakePrisonerUserAction : ScriptedUserAction
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
		
		if (!m_pOwnerCharCtrl.ACE_Captives_HasSurrendered())
			return false;
		
		if (m_pOwnerCharCtrl.ACE_Captives_IsCaptive())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		ACE_AnimationTools.AnimateWithHelperCompartment(pOwnerEntity, "{5E23E43ED54785D0}Prefabs/Helpers/ACE_Captives_TiedHelperCompartment.et");
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
