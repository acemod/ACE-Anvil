//------------------------------------------------------------------------------------------------
class ACE_CarriableAnimationHelperComponentClass : ACE_CarriableEntityComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_CarriableAnimationHelperComponent : ACE_CarriableEntityComponent
{
	[Attribute(defvalue: "false", desc: "Terminates animation helper compartment when released")]
	protected bool m_bTerminateWhenReleased;
	
	protected ACE_AnimationHelperCompartment m_pOwner;
	protected static EPhysicsLayerPresets m_iPhysicsLayerPreset = -1;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pOwner = ACE_AnimationHelperCompartment.Cast(GetOwner());
		m_pOwner.GetOnTerminated().Insert(Release);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void AttachHandlersAuthority()
	{
		super.AttachHandlersAuthority();
		
		IEntity carried = m_pOwner.GetPerformer();
		if (!carried)
			return;
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(carried.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_SetIsCarried(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DetachHandlersAuthority()
	{
		super.DetachHandlersAuthority();
		
		IEntity carried = m_pOwner.GetPerformer();
		if (!carried)
			return;
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(carried.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			charCtrl.ACE_SetIsCarried(false);
		
		if (m_bTerminateWhenReleased)
			m_pOwner.Terminate(EGetOutType.ANIMATED);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void AttachHandlersOwner()
	{
		super.AttachHandlersOwner();
		
		Physics carriedPhys = GetOwner().GetChildren().GetPhysics();
		
		if (m_iPhysicsLayerPreset < 0)
			m_iPhysicsLayerPreset = carriedPhys.GetInteractionLayer();
		
		carriedPhys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DetachHandlersOwner()
	{
		super.DetachHandlersOwner();
		
		IEntity carried = GetOwner().GetChildren();
		if (carried)
			carried.GetPhysics().SetInteractionLayer(m_iPhysicsLayerPreset);
	}
}
