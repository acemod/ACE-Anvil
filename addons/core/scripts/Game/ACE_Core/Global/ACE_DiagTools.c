#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
class ACE_DiagTools
{
	//------------------------------------------------------------------------------------------------
	//! Get target for diag menu and string for printing which target it is
	//! Return true if a target was found
	static bool GetDiagTargetCharacter(out IEntity target, out string targetType)
	{
		CameraBase camera = GetGame().GetCameraManager().CurrentCamera();
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
}
#endif
