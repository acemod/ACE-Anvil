/*
	Will need to extend/modify this system if manual defibs are added. Will need a manual defib component.
*/

class ACE_MedicalDefibrillation_DefibrillationSystem : ACE_Medical_BaseSystem3
{
	protected ACE_MedicalDefibrillation_DefibrillationSystemSettings m_Settings;
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_DefibrillationSystem)
			m_Settings = settings.m_DefibrillationSystem;
		
		Print("ACE_MedicalDefibrillation_DefibrillationSystem.OnInit:: Defib system starting...");
		super.OnInit();
	}

	//------------------------------------------------------------------------------------------------
	protected override void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		ACE_MedicalDefibrillation_DefibrillatorState_Base defibrillatorState = defibrillatorComponent.GetState();
		if (!defibrillatorState)
			return;
		
		if (defibrillatorComponent.m_bIsChangingState)
			return;
		
		// Disconnect any patients that are too far
		CheckPatientPosition(entity, timeSlice);
		
		defibrillatorState.Update(timeSlice, m_fMinEntityUpdateTimeoutMS);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Register(notnull IEntity entity)
	{
		super.Register(entity);
		
		PrintFormat("ACE_MedicalDefibrillation_DefibrillationSystem.Register:: Entity <%1> Registered", entity.ClassName());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CheckPatientPosition(IEntity entity, float timeSlice)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient());
		if (!char)
			return;
		
		vector charPos = char.GetOrigin();
		vector AEDPos = entity.GetOrigin();
		
		float distance = vector.Distance(charPos, AEDPos);
		if (distance >= 3)
			defibrillatorComponent.ResetPatient();
	}

#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		DbgUI.Begin("ACE_MedicalDefibrillation_DefibrillationSystem");
		
		DbgUI.Text("Defibrillators: " + m_aQueue.Count());
		
		for (int index = 0; index < m_aQueue.Count(); index++)
		{
			IEntity defib = m_aQueue.Get(index);
			ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(defib.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
			if (!defibrillatorComponent)
				continue;
			
			DbgUI.Text(string.Format("Defib_%1::CurrentState | %2", index, defibrillatorComponent.GetState().ClassName()));
		}
		
		DbgUI.End();
	}
#endif
}