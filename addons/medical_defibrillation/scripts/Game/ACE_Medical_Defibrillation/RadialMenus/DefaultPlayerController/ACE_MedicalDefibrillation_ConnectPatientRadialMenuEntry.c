//------------------------------------------------------------------------------------------------

//! ACE_MedicalDefibrillation_ConnectPatientRadialMenuEntry
//! Bind to ACE_RadialMenuComponent on DefaultPlayerController.et
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_Medical_ConnectPatientMenuEntry : ACE_SelectionMenuEntry
{
	protected float m_fSearchDistancePerform = 3;
	protected float m_fShortestDistancePerform;
	protected IEntity m_NearestDefibPerform;
	protected vector m_vSearchPosPerform;

	
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{			
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!player)
			return;
		
		PlayerCamera camera = PlayerCamera.Cast(GetGame().GetCameraManager().CurrentCamera());
		if (!camera)
			return;
		
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
		if (!targetChar)
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(targetChar.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		QueryForDefibPerform(targetChar.GetOrigin(), 3);
		if (!m_NearestDefibPerform)
			return;
		
		ACE_Medical_NetworkComponent networkComponent = ACE_Medical_NetworkComponent.GetACEMedicalNetworkComponent(player);
		if (!networkComponent)
			return;
		
		networkComponent.RequestDefibrillatorConnectPatient(GameEntity.Cast(m_NearestDefibPerform), targetChar, player);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{		
		PlayerCamera camera = GetGame().GetPlayerController().GetPlayerCamera();
		if (!camera)
			return false;
		
		return SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
	}
	
	//------------------------------------------------------------------------------------------------
	// QUERY SYSTEM	
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	protected void QueryForDefibPerform(vector searchPos, float searchDistance)
	{
		m_NearestDefibPerform = null;
		m_vSearchPosPerform = searchPos;
		m_fSearchDistancePerform = searchDistance;
		
		m_fShortestDistancePerform = m_fSearchDistancePerform;
		
		GetGame().GetWorld().QueryEntitiesBySphere(m_vSearchPosPerform, m_fSearchDistancePerform, QueryForDefibPerformCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryForDefibPerformCallback(IEntity entity)
	{
		if (!entity)
			return true;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return true;

		if (defibrillatorComponent.GetConnectedPatient())
			return true;
		
		float distance = vector.Distance(m_vSearchPosPerform, entity.GetOrigin());
		
		if (m_fShortestDistancePerform > distance)
		{
			m_fShortestDistancePerform = distance;
			m_NearestDefibPerform = entity;
		}
		
		return (m_fShortestDistancePerform != 0);
	}
}
