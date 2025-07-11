typedef ACE_FSM_Machine<ACE_Medical_VitalStates_CharacterContext> ACE_Medical_VitalStates_TMachine;

//------------------------------------------------------------------------------------------------
//! Current workaround, as template classes are not directly supported as attributes: https://feedback.bistudio.com/T193122
[BaseContainerProps()]
class ACE_Medical_VitalStates_JobScheduler : ACE_TemplateFrameJobScheduler<SCR_ChimeraCharacter, ACE_Medical_VitalStates_CharacterContext, ACE_Medical_VitalStates_TMachine>
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_VitalStatesSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Medical_VitalStates_JobScheduler m_pScheduler;
	
	protected ACE_Medical_Circulation_Settings m_pSettings;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_VitalStatesSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Medical_VitalStatesSystem.Cast(world.FindSystem(ACE_Medical_VitalStatesSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(ESystemLocation.Server)
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		m_pSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
		m_pScheduler.OnInit(CreateTemplateJob());
		
		ACE_FrameJobSystem jobManager = ACE_FrameJobSystem.GetInstance();
		if (jobManager)
			jobManager.Register(m_pScheduler, ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_VitalStates_TMachine CreateTemplateJob()
	{
		ACE_Medical_VitalStates_TMachine machine = new ACE_Medical_VitalStates_TMachine();
		
		machine.AddState(new ACE_Medical_StableState(ACE_Medical_EVitalStateID.STABLE));
		machine.AddState(new ACE_Medical_StableState(ACE_Medical_EVitalStateID.UNSTABLE));
		machine.AddState(new ACE_Medical_CriticalState(ACE_Medical_EVitalStateID.CRITICAL));
		machine.AddState(new ACE_Medical_CardiacArrestState(ACE_Medical_EVitalStateID.CARDIAC_ARREST));
		machine.AddState(new ACE_Medical_ResuscitationState(ACE_Medical_EVitalStateID.RESUSCITATION));
		
		machine.AddTransition(new ACE_Medical_StartResuscitationTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.CARDIAC_ARREST,
			toStateID: ACE_Medical_EVitalStateID.RESUSCITATION
		));
		machine.AddTransition(new ACE_Medical_ReviveTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.RESUSCITATION,
			toStateID: ACE_Medical_EVitalStateID.CRITICAL
		));
		machine.AddTransition(new ACE_Medical_StopResuscitationTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.RESUSCITATION,
			toStateID: ACE_Medical_EVitalStateID.CARDIAC_ARREST
		));
		machine.AddTransition(new ACE_Medical_VitalStateThresholdTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.STABLE | ACE_Medical_EVitalStateID.UNSTABLE | ACE_Medical_EVitalStateID.CRITICAL,
			toStateID: ACE_Medical_EVitalStateID.CARDIAC_ARREST,
			thresholds: m_pSettings.m_CardiacArrestThresholds
		));
		machine.AddTransition(new ACE_Medical_VitalStateThresholdTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.STABLE | ACE_Medical_EVitalStateID.UNSTABLE,
			toStateID: ACE_Medical_EVitalStateID.CRITICAL,
			thresholds: m_pSettings.m_CriticalThresholds
		));
		machine.AddTransition(new ACE_Medical_VitalStateThresholdTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.STABLE,
			toStateID: ACE_Medical_EVitalStateID.UNSTABLE,
			thresholds: m_pSettings.m_UnstableThresholds
		));
		machine.AddTransition(new ACE_Medical_VitalStateInverseThresholdTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.UNSTABLE,
			toStateID: ACE_Medical_EVitalStateID.STABLE,
			thresholds: m_pSettings.m_UnstableThresholds
		));
		machine.AddTransition(new ACE_Medical_VitalStateInverseThresholdTransition(
			fromStateIDs: ACE_Medical_EVitalStateID.CRITICAL,
			toStateID: ACE_Medical_EVitalStateID.UNSTABLE,
			thresholds: m_pSettings.m_CriticalThresholds
		));
		
		machine.UpdateTransitionTable();
		return machine;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull SCR_ChimeraCharacter char)
	{
		m_pScheduler.Register(char);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(SCR_ChimeraCharacter char)
	{
		m_pScheduler.Unregister(char);
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	//! Show vitals for target entity
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		/***** if (!GetDiagTarget(target, targetType))
			return; *****/
		
		ACE_Medical_VitalStateComponent component = ACE_Medical_VitalStateComponent.Cast(target.FindComponent(ACE_Medical_VitalStateComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;

		DbgUI.Begin(string.Format("ACE_Medical_VitalStatesSystem (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Vital state:                  %1", SCR_Enum.GetEnumName(ACE_Medical_EVitalStateID, component.GetVitalState())));
		DbgUI.Text(string.Format("Heart rate:                   %1 bpm", Math.Round(component.GetHeartRate())));
		Tuple2<float, float> pressures = component.GetBloodPressures();
/*****  DbgUI.Text(string.Format("Blood pressure:               %1/%2 mmHg", Math.Round(pressures.param2 * KPA2MMHG), Math.Round(pressures.param1 * KPA2MMHG))); *****/
		DbgUI.Text(string.Format("Blood state:                  %1", SCR_Enum.GetEnumName(ACE_Medical_EBloodState, damageManager.GetBloodHitZone().GetDamageState())));
/*****	DbgUI.Text(string.Format("Resilience recovery scale:    %1", component.GetResilienceRecoveryScale())); *****/
		DbgUI.End();
	}
#endif
}
