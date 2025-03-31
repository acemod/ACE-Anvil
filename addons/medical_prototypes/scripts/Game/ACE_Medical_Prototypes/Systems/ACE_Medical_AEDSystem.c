class ACE_Medical_AEDSystem : GameSystem
{
	[Attribute(defvalue: "1000", desc: "Minimum update timeout for a particular AED [ms]")]
	protected int m_iMinEntityUpdateTimeoutMS;
	
	[Attribute(defvalue: "10", desc: "Maximum number of entities to process in an update")]
	protected int m_iMaxIterationsPerUpdate;
	
	protected ref array<ACE_Medical_AEDComponent> m_Components = {};
	protected ref array<ACE_Medical_AEDComponent> m_DeletedComponents = {};
	
	protected bool m_bUpdating = false;
	protected float m_fDelayAfterAnalysis = 2;
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		
		outInfo.SetAbstract(false)
			.SetLocation(ESystemLocation.Server)
			.AddPoint(ESystemPoint.Frame);
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnUpdate(ESystemPoint point)
	{
		float timeSlice = GetWorld().GetTimeSlice();
		
		m_bUpdating = true;
		
		foreach (ACE_Medical_AEDComponent comp: m_Components)
		{
			UpdateComponent(comp, timeSlice);
		}
		
		m_bUpdating = false;
		
		foreach (ACE_Medical_AEDComponent comp : m_DeletedComponents)
		{
			Unregister(comp);
			comp.ResetCharge();
			comp.ResetAnalysis();
			comp.ResetPatient();
		}
		m_DeletedComponents.Clear();
	}
	
	//------------------------------------------------------------------------------------------------	
	void Register(ACE_Medical_AEDComponent component)
	{
		// About to be deleted
		if (component.GetOwner().IsDeleted() ||
			(component.GetOwner().GetFlags() & EntityFlags.USER5))
			return;
		
		if (m_Components.Find(component) != -1)
			return;
		
		m_Components.Insert(component);
		PrintFormat("ACE_Medical_AEDSystem.Register:: Registered component <%1> to system.", component.ClassName());
		PrintFormat("ACE_Medical_AEDSystem.Register:: Total components registered: %1", m_Components.Count());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Unregister(ACE_Medical_AEDComponent component)
	{
		int idx = m_Components.Find(component);
		if (idx == -1)
			return;
		
		if (m_bUpdating)
			m_DeletedComponents.Insert(component);
		else
			m_Components.Remove(idx);
	}
		
	//------------------------------------------------------------------------------------------------
	protected void UpdateComponent(ACE_Medical_AEDComponent component, float timeSlice)
	{
		if (!component)
			return;
		
		UpdateCharge(component, timeSlice);
		UpdateAnalysis(component, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCharge(ACE_Medical_AEDComponent component, float timeSlice)
	{
		float charge = component.GetCharge();
		float chargeTime = component.GetChargeTime();
		
		if (component.IsCharging())
		{
			charge += timeSlice / chargeTime;
			component.SetCharge(charge);
			
			PrintFormat("ACE_Medical_AEDSystem.UpdateAnalysis:: Current charge percent = %1", component.GetCharge());
			
			if (charge >= 1.0)
			{
				component.SetCharge(1.0);
				component.EnableCharging(false);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateAnalysis(ACE_Medical_AEDComponent component, float timeSlice)
	{
		
		float analysis = component.GetAnalysis();
		float analysisTime = component.GetAnalysisTime();
		
		if (component.IsAnalysing())
		{
			analysis += timeSlice / analysisTime;
			component.SetAnalysis(analysis);

			PrintFormat("ACE_Medical_AEDSystem.UpdateAnalysis:: Current analysis percent = %1", component.GetAnalysis());
			
			if (analysis >= 1.0)
			{
				component.SetAnalysis(1.0);
				component.EnableAnalysis(false);
			}
		}
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override protected void OnDiag(float timeSlice)
	{
		DbgUI.Begin("ACE_Medical_AEDSystem");
		
		DbgUI.Text("AEDs: " + m_Components.Count());
		
		if (DbgUI.Button("Dump active components"))
		{
			foreach (ACE_Medical_AEDComponent comp : m_Components)
			{
				Print(comp.GetOwner(), LogLevel.ERROR);
			}
		}
		
		DbgUI.End();
	}
#endif
}