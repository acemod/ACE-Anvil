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
	protected bool m_bHasParentTerminated = false;
	protected static EPhysicsLayerPresets m_iPhysicsLayerPreset = -1;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pOwner = ACE_AnimationHelperCompartment.Cast(GetOwner());
		m_pOwner.GetOnTerminated().Insert(OnParentTerminated);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void AttachHandlersAuthority()
	{
		super.AttachHandlersAuthority();
		
		SCR_ChimeraCharacter carried = m_pOwner.GetPerformer();
		if (!carried)
			return;
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(carried.GetCharacterController());
		if (charCtrl)
			charCtrl.ACE_SetCarrier(m_pCarrier);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DetachHandlersAuthority()
	{
		super.DetachHandlersAuthority();
		
		SCR_ChimeraCharacter carried = m_pOwner.GetPerformer();
		if (!carried)
			return;
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(carried.GetCharacterController());
		if (charCtrl)
			charCtrl.ACE_SetCarrier(null);
		
		if (m_bTerminateWhenReleased && !m_bHasParentTerminated)
			m_pOwner.Terminate();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void AttachHandlersOwner()
	{
		super.AttachHandlersOwner();
		
		IEntity carried = m_pOwner.GetPerformer();
		if (!carried)
			return;
		
		Physics carriedPhys = carried.GetPhysics();
		if (!carriedPhys)
			return;
		
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
	
	//------------------------------------------------------------------------------------------------
	protected void OnParentTerminated()
	{
		m_bHasParentTerminated = true;
		Release();
	}
}
