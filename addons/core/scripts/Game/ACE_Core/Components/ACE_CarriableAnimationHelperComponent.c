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
	protected ref array<int> m_aCarriedGeomLayers;
	
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
		
		// The carried entity might not be known yet, as the helper replicates it independently of the carrier
		m_pOwner.GetOnPerformerChanged().Insert(DisableCarriedCollision);
		DisableCarriedCollision(m_pOwner.GetPerformer());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DetachHandlersOwner()
	{
		super.DetachHandlersOwner();
		
		m_pOwner.GetOnPerformerChanged().Remove(DisableCarriedCollision);
		RestoreCarriedCollision();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stop the carried entity from colliding with the carrier
	//! Has to be done on the carrier's machine, since that is where the carrier's movement gets simulated
	protected void DisableCarriedCollision(IEntity carried)
	{
		// Nothing to do if the carried entity is not known yet or its collision is already disabled
		if (!carried || m_aCarriedGeomLayers)
			return;
		
		Physics carriedPhys = carried.GetPhysics();
		if (!carriedPhys)
			return;
		
		// Remember the layer of each geometry, since SetInteractionLayer overwrites all of them
		m_aCarriedGeomLayers = {};
		for (int i, count = carriedPhys.GetNumGeoms(); i < count; i++)
		{
			m_aCarriedGeomLayers.Insert(carriedPhys.GetGeomInteractionLayer(i));
		}
		
		carriedPhys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Restore the layers stored by DisableCarriedCollision
	protected void RestoreCarriedCollision()
	{
		if (!m_aCarriedGeomLayers)
			return;
		
		Physics carriedPhys;
		IEntity carried = m_pOwner.GetPerformer();
		if (carried)
			carriedPhys = carried.GetPhysics();
		
		if (carriedPhys)
		{
			int geomCount = carriedPhys.GetNumGeoms();
			foreach (int i, int layer : m_aCarriedGeomLayers)
			{
				if (i >= geomCount)
					break;
				
				carriedPhys.SetGeomInteractionLayer(i, layer);
			}
		}
		
		m_aCarriedGeomLayers = null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnParentTerminated()
	{
		m_bHasParentTerminated = true;
		Release();
	}
}
