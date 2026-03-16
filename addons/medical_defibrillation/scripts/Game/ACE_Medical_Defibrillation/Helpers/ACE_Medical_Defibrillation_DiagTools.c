#ifdef ENABLE_DIAG
class ACE_Medical_Defibrillation_DiagTools
{
	//------------------------------------------------------------------------------------------------
	static bool GetDiagTargetDefib(out IEntity target, out ACE_Medical_Defibrillation_DefibComponent defibComponent)
	{
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (!cameraManager)
			return false;
		
		CameraBase camera = cameraManager.CurrentCamera();
		if (!camera)
			return false;
		
		target = IEntity.Cast(camera.GetCursorTarget());
		if (!target)
			return false;
		
		defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(target.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool GetDiagNearestDefib(out IEntity target, out ACE_Medical_Defibrillation_DefibComponent defibComponent, float distance = 3)
	{
		IEntity player = GetGame().GetPlayerController().GetControlledEntity();
		if (!player)
			return false;
		
		vector playerPos = player.GetOrigin();
		
		ACE_Medical_Defibrillation_QueryNearestDefib query = new ACE_Medical_Defibrillation_QueryNearestDefib(distance);
		target = query.GetEntity(playerPos);
		if (!target)
			return false;
		
		defibComponent = ACE_Medical_Defibrillation_DefibComponent.Cast(target.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!defibComponent)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static float GetReviveChanceShockBonus(IEntity target)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(target);
		if (!char)
			return -1;
		
		ACE_Medical_CharacterContext context = new ACE_Medical_CharacterContext(char);
		ACE_Medical_ReviveTransition transition = new ACE_Medical_ReviveTransition(ACE_Medical_EVitalStateID.ANY, ACE_Medical_EVitalStateID.ANY);
		
		return transition.ComputeReviveChanceShockBonus(context);
	}
	
	//------------------------------------------------------------------------------------------------
	static float GetReviveChance(IEntity target)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(target);
		if (!char)
			return -1;
		
		ACE_Medical_CharacterContext context = new ACE_Medical_CharacterContext(char);
		ACE_Medical_ReviveTransition transition = new ACE_Medical_ReviveTransition(ACE_Medical_EVitalStateID.ANY, ACE_Medical_EVitalStateID.ANY);
		
		return transition.ComputeReviveChance(context);
	}
}
#endif