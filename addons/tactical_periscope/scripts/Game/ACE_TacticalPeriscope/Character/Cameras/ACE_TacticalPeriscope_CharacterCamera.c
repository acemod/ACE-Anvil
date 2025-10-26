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
		
		vector playerTransform[4];
		m_ePlayer.GetWorldTransform(playerTransform);
		vector playerTransformT[4];
		Math3D.MatrixGetInverse4(playerTransform, playerTransformT);
		
		vector gadgetTransform[4];
		m_pGadgetEntity.GetWorldTransform(gadgetTransform);
		
		vector gadgetToPlayerTransform[4];
		Math3D.MatrixMultiply4(playerTransformT, gadgetTransform, gadgetToPlayerTransform);
		vector offset = m_Optics.GetSightsOffset();
		pOutResult.m_CameraTM[3] = offset.Multiply4(gadgetToPlayerTransform);
		pOutResult.m_iDirectBone = -1; // Offset is caclulated from the root of the character
	}
}
