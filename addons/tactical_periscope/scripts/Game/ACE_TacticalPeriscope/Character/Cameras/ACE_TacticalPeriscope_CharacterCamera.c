//-----------------------------------------------------------------------------
//! Extends binocular camera by accouting for the offset of the scope
class ACE_TacticalPeriscope_CharacterCamera : CharacterCameraBinoculars
{
	protected IEntity m_pGadgetEntity;
	
	//-----------------------------------------------------------------------------
	override void OnActivate(ScriptedCameraItem pPrevCamera, ScriptedCameraItemResult pPrevCameraResult)
	{
		super.OnActivate(pPrevCamera, pPrevCameraResult);
		
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.Cast(m_ePlayer.FindComponent(SCR_GadgetManagerComponent));
		if (!gadgetManager)
			return;
		
		m_pGadgetEntity = gadgetManager.GetHeldGadget();
	}
	
	//-----------------------------------------------------------------------------
	//! Account for the offset of the scope
	override void OnUpdate(float pDt, out ScriptedCameraItemResult pOutResult)
	{
		super.OnUpdate(pDt, pOutResult);
		
		if (!m_Optics)
			return;
		
		vector gadgetPosInPlayerSpace = m_ePlayer.CoordToLocal(m_pGadgetEntity.GetOrigin());
		vector sightsOffsetInWorldSpace = m_pGadgetEntity.VectorToParent(m_Optics.GetSightsOffset());
		vector sightsOffsetInPlayerSpace = m_ePlayer.VectorToLocal(sightsOffsetInWorldSpace);
		
		pOutResult.m_CameraTM[3] = gadgetPosInPlayerSpace + sightsOffsetInPlayerSpace;
		pOutResult.m_iDirectBone = -1; // Offset is caclulated from the root of the character
	}
}
