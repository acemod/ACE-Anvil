//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_DefibStatesSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Medical_Defibrillation_DefibStates_JobScheduler m_pScheduler;
	
	protected ACE_Medical_Defibrillation_Defibrillation_Settings m_pSettings;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_Defibrillation_DefibStatesSystem GetInstance(ChimeraWorld world)
	{
		return ACE_Medical_Defibrillation_DefibStatesSystem.Cast(world.FindSystem(ACE_Medical_Defibrillation_DefibStatesSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			   .SetUnique(true)
			   .SetLocation(WorldSystemLocation.Server)
			   .AddPoint(WorldSystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		m_pSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Defibrillation_Settings>.GetModSettings();
		m_pScheduler.OnInit(CreateTemplateJob());
		
		ACE_FrameJobSystem jobManager = ACE_FrameJobSystem.GetInstance();
		if (jobManager)
			jobManager.Register(m_pScheduler, ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_Defibrillation_DefibStateMachine CreateTemplateJob()
	{
		ACE_Medical_Defibrillation_DefibStateMachine machine = new ACE_Medical_Defibrillation_DefibStateMachine();
		
		machine.AddState(new ACE_Medical_Defibrillation_DisconnectedState(ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED));
		machine.AddState(new ACE_Medical_Defibrillation_ConnectedState(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED));
		machine.AddState(new ACE_Medical_Defibrillation_AnalysingState(ACE_Medical_Defibrillation_EDefibStateID.ANALYSING));
		machine.AddState(new ACE_Medical_Defibrillation_AnalysedState(ACE_Medical_Defibrillation_EDefibStateID.ANALYSED));
		machine.AddState(new ACE_Medical_Defibrillation_ChargingState(ACE_Medical_Defibrillation_EDefibStateID.CHARGING));
		machine.AddState(new ACE_Medical_Defibrillation_ChargedState(ACE_Medical_Defibrillation_EDefibStateID.CHARGED));
		
		machine.AddTransition(new ACE_Medical_Defibrillation_AnalysedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.ANALYSING,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.ANALYSED
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_AnalysingStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.CONNECTED,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.ANALYSING
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_ChargedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.CHARGING,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.CHARGED
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_ChargingStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.ANALYSED,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.CHARGING
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_ConnectedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.CONNECTED
		));
		
		machine.AddTransition(new ACE_Medical_Defibrillation_DisconnectedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.CONNECTED,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_DisconnectedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.ANALYSING,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_DisconnectedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.ANALYSED,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_DisconnectedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.CHARGING,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED
		));
		machine.AddTransition(new ACE_Medical_Defibrillation_DisconnectedStateTransition(
			fromStateIDs: ACE_Medical_Defibrillation_EDefibStateID.CHARGED,
			toStateID: ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED
		));
		
		machine.UpdateTransitionTable();		
		return machine;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull IEntity defib)
	{
		m_pScheduler.Register(defib);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(IEntity defib)
	{
		m_pScheduler.Unregister(defib);
	}
	
#ifdef ENABLE_DIAG
	// TODO: Add DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		DbgUI.Begin(string.Format("ACE_Medical_Defibrillation_DefibStatesSystem"));
		DbgUI.Text("Total Active Defibrillators: " + m_pScheduler.GetJobs().Count());
		
		IEntity target;
		ACE_Medical_Defibrillation_DefibComponent defibComponent;
		string targetType = "Camera Target";
		
		if (!ACE_Medical_Defibrillation_DiagTools.GetDiagTargetDefib(target, defibComponent))
		{
			float distance = 3;
			ACE_Medical_Defibrillation_DiagTools.GetDiagNearestDefib(target, defibComponent, distance);
			targetType = string.Format("Nearest Defib (%1m)", distance);
		}
		
		if (!defibComponent)
			return;
		
		DbgUI.Spacer(10);
		DbgUI.Text(string.Format("Target Type: %1", targetType));
		DbgUI.Text(string.Format("Connected Patient: %1", defibComponent.GetPatient()));
		DbgUI.Text(string.Format("Current State: %1", SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_EDefibStateID, defibComponent.GetDefibStateID())));
		DbgUI.Text(string.Format("Time In State: %1", defibComponent.GetDefibProgressData().GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.StateTimeElapsed) / 1000));
		DbgUI.Text(string.Format("Analysis Percent: %1", defibComponent.GetDefibProgressData().GetPercentComplete(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis)));
		DbgUI.Text(string.Format("Charge Percent: %1", defibComponent.GetDefibProgressData().GetPercentComplete(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge)));
		DbgUI.End();
	}
#endif
}