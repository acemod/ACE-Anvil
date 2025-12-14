#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
class ACE_DiagTools
{
	//------------------------------------------------------------------------------------------------
	//! Get target for diag menu and string for printing which target it is
	//! Return true if a target was found
	static bool GetDiagTargetCharacter(out IEntity target, out string targetType)
	{
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (!cameraManager)
			return false;
		
		CameraBase camera = cameraManager.CurrentCamera();
		if (!camera)
			return false;
		
		target = SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
		if (target)
		{
			targetType = "Target";
			return true;
		}
		
		target = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (target)
		{
			targetType = "Self";
			return true;
		}
		
		return false;
	}
	
	static bool GetDiagTargetDefibrillator(out IEntity target, out string targetType)
	{
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (!cameraManager)
			return false;
		
		CameraBase camera = cameraManager.CurrentCamera();
		if (!camera)
			return false;
		
		target = IEntity.Cast(camera.GetCursorTarget());
		if (target)
		{
			if (ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(target.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent)))
				return false;
			
			targetType = "Target";
			return true;
		}
		
		return false;
	}
}
#endif
