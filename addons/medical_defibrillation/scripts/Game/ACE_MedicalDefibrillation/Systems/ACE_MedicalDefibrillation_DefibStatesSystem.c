//------------------------------------------------------------------------------------------------
class ACE_MedicalDefibrillation_DefibStatesSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_MedicalDefibrillation_DefibStates_JobScheduler m_pScheduler;
	
	protected ACE_MedicalDefibrillation_Defibrillation_Settings m_pSettings;
	
	//------------------------------------------------------------------------------------------------
	static ACE_MedicalDefibrillation_DefibStatesSystem GetInstance(ChimeraWorld world)
	{
		return ACE_MedicalDefibrillation_DefibStatesSystem.Cast(world.FindSystem(ACE_MedicalDefibrillation_DefibStatesSystem));
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
		m_pSettings = ACE_SettingsHelperT<ACE_MedicalDefibrillation_Defibrillation_Settings>.GetModSettings();
		m_pScheduler.OnInit(CreateTemplateJob());
		
		ACE_FrameJobSystem jobManager = ACE_FrameJobSystem.GetInstance();
		if (jobManager)
			jobManager.Register(m_pScheduler, ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_MedicalDefibrillation_DefibStateMachine CreateTemplateJob()
	{
		ACE_MedicalDefibrillation_DefibStateMachine machine = new ACE_MedicalDefibrillation_DefibStateMachine();
		
		machine.AddState(new ACE_MedicalDefibrillation_AnalysedState(ACE_MedicalDefibrillation_EDefibStateID.ANALYSED));
		machine.AddState(new ACE_MedicalDefibrillation_AnalysingState(ACE_MedicalDefibrillation_EDefibStateID.ANALYSING));
		machine.AddState(new ACE_MedicalDefibrillation_ChargedState(ACE_MedicalDefibrillation_EDefibStateID.CHARGED));
		machine.AddState(new ACE_MedicalDefibrillation_ChargingState(ACE_MedicalDefibrillation_EDefibStateID.CHARGING));
		machine.AddState(new ACE_MedicalDefibrillation_ConnectedState(ACE_MedicalDefibrillation_EDefibStateID.CONNECTED));
		machine.AddState(new ACE_MedicalDefibrillation_DisconnectedState(ACE_MedicalDefibrillation_EDefibStateID.DISCONNECTED));
		
		machine.AddTransition(new ACE_MedicalDefibrillation_AnalysedStateTransition(
			fromStateIDs: ACE_MedicalDefibrillation_EDefibStateID.ANALYSING,
			toStateID: ACE_MedicalDefibrillation_EDefibStateID.ANALYSED
		));
		machine.AddTransition(new ACE_MedicalDefibrillation_AnalysingStateTransition(
			fromStateIDs: ACE_MedicalDefibrillation_EDefibStateID.CONNECTED,
			toStateID: ACE_MedicalDefibrillation_EDefibStateID.ANALYSING
		));
		machine.AddTransition(new ACE_MedicalDefibrillation_ChargedStateTransition(
			fromStateIDs: ACE_MedicalDefibrillation_EDefibStateID.CHARGING,
			toStateID: ACE_MedicalDefibrillation_EDefibStateID.CHARGED
		));
		machine.AddTransition(new ACE_MedicalDefibrillation_ChargingStateTransition(
			fromStateIDs: ACE_MedicalDefibrillation_EDefibStateID.ANALYSED,
			toStateID: ACE_MedicalDefibrillation_EDefibStateID.CHARGING
		));
		machine.AddTransition(new ACE_MedicalDefibrillation_ConnectedStateTransition(
			fromStateIDs: ACE_MedicalDefibrillation_EDefibStateID.DISCONNECTED,
			toStateID: ACE_MedicalDefibrillation_EDefibStateID.CONNECTED
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
		
		DbgUI.Begin(string.Format("ACE_MedicalDefibrillation_DefibStatesSystem"));
		
		DbgUI.Text("Defibrillators: " + m_pScheduler.GetJobs().Count());
		
		for (int index = 0; index < m_pScheduler.GetJobs().Count(); index++)
		{
			ACE_MedicalDefibrillation_DefibStateMachine defibMachine = m_pScheduler.GetJobs().Get(index);
			ACE_MedicalDefibrillation_DefibrillatorContext context = defibMachine.GetContext();
			ACE_MedicalDefibrillation_DefibrillatorComponent component = context.m_pDefibrillator;
			if (!component)
				continue;
			
			DbgUI.Text(string.Format("Defib_%1::CurrentState | %2", index, SCR_Enum.GetEnumName(ACE_MedicalDefibrillation_EDefibStateID, component.GetDefibStateID())));
		}
		DbgUI.End();
	}
#endif
}