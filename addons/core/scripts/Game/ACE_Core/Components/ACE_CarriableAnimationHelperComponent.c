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
	protected IEntity m_pCollisionDisabledEntity;
	protected ref array<int> m_aCollisionRestoreLayers;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pOwner = ACE_AnimationHelperCompartment.Cast(GetOwner());
		m_pOwner.GetOnTerminated().Insert(OnParentTerminated);
		m_pOwner.GetOnPerformerChanged().Insert(OnParentPerformerChanged);
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
		DisableCarriedCollision();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DetachHandlersOwner()
	{
		super.DetachHandlersOwner();
		RestoreCarriedCollision();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stop the carried entity from colliding with the carrier.
	//! Has to be done on the carrier's machine, since that is where the carrier's movement gets simulated.
	protected void DisableCarriedCollision()
	{
		if (m_pCollisionDisabledEntity)
			return;
		
		IEntity carried = m_pOwner.GetPerformer();
		if (!carried)
			return;
		
		Physics carriedPhys = carried.GetPhysics();
		if (!carriedPhys)
			return;
		
		// Remember the layer of each geometry, since SetInteractionLayer overwrites all of them
		m_aCollisionRestoreLayers = {};
		int geomCount = carriedPhys.GetNumGeoms();
		for (int i = 0; i < geomCount; i++)
		{
			m_aCollisionRestoreLayers.Insert(carriedPhys.GetGeomInteractionLayer(i));
		}
		
		carriedPhys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
		m_pCollisionDisabledEntity = carried;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Restore the layers stored by DisableCarriedCollision. Does nothing if they were never changed.
	protected void RestoreCarriedCollision()
	{
		if (!m_pCollisionDisabledEntity)
			return;
		
		Physics carriedPhys = m_pCollisionDisabledEntity.GetPhysics();
		
		// Skip when the geometries are no longer the ones we stored the layers for
		if (carriedPhys && carriedPhys.GetNumGeoms() == m_aCollisionRestoreLayers.Count())
		{
			foreach (int i, int layer : m_aCollisionRestoreLayers)
			{
				carriedPhys.SetGeomInteractionLayer(i, layer);
			}
		}
		
		m_pCollisionDisabledEntity = null;
		m_aCollisionRestoreLayers = null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! The carried entity gets replicated by the helper compartment and can become known on the carrier's
	//! machine after the handlers got attached, in which case its collision has to be disabled here
	protected void OnParentPerformerChanged()
	{
		if (!m_pCarrier)
			return;
		
		RplComponent carrierRpl = m_pCarrier.GetRplComponent();
		if (carrierRpl && carrierRpl.IsOwner())
			DisableCarriedCollision();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnParentTerminated()
	{
		m_bHasParentTerminated = true;
		Release();
	}
}
