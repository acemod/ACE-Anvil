//------------------------------------------------------------------------------------------------
class ACE_Captives_TogglePrisonerUserAction : ScriptedUserAction
{
	[Attribute(defvalue: "#ACE_Captives-UserName_TakeAsPrisoner", desc: "String shown for taking as prisoner")]
	protected LocalizedString m_sTakePrisoner;
	
	[Attribute(defvalue: "#ACE_Captives-UserName_ReleasePrisoner", desc: "String shown for releasing prisoner")]
	protected LocalizedString m_sReleasePrisoner;
	
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
		
		if (m_pOwnerCharCtrl.ACE_IsCarried())
			return false;
		
		if (m_pOwnerCharCtrl.ACE_Captives_IsCaptive())
			return true;
		
		return m_pOwnerCharCtrl.ACE_Captives_HasSurrendered();
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		if (m_pOwnerCharCtrl.ACE_Captives_IsCaptive())
		{
			ACE_AnimationHelperCompartment helper = ACE_AnimationHelperCompartment.Cast(pOwnerEntity.GetParent());
			if (helper)
				helper.Terminate(EGetOutType.ANIMATED);
		}
		else
		{
			ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.TIED, pOwnerEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_pOwnerCharCtrl.ACE_Captives_IsCaptive())
			outName = m_sReleasePrisoner;
		else
			outName = m_sTakePrisoner;
		
		return true;
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
