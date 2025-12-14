class ACE_MedicalDefibrillation_DefibrillatorComponentClass : ScriptComponentClass
{
}

class ACE_MedicalDefibrillation_DefibrillatorComponent : ScriptComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_MedicalDefibrillation_EDefibrillatorEmulation))]
	protected ACE_MedicalDefibrillation_EDefibrillatorEmulation m_eDefibrillatorEmulation;
	
	[Attribute(defvalue: "1", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to analyse cardiac rhythm.")]
	protected float m_fAnalysisDuration;
	
	[Attribute(defvalue: "5.5", params: "0 inf 0.1", desc: "Time (s) it takes for the defibrillator to fully charge.")]
	protected float m_fChargeDuration;
	
	[Attribute(defvalue: "120", params: "0 inf 1", desc: "Time (s) between analysis events where players should perform CPR.")]
	protected float m_fCPRCooldownDuration;
	
	[Attribute(defvalue: "10", params: "0 inf 0.1", desc: "Stun duration (s) for a character that touches the patient while a shock is delivered.")]
	protected float m_fContactShockStunDuration;
	
	protected IEntity m_pPatient;
	
	[RplProp(onRplName: "OnDefibStateChanged")]
	protected ACE_MedicalDefibrillation_EDefibStateID m_eDefibrillatorStateID;
	
	[RplProp(onRplName: "OnDefibDataChanged")]
	protected ref ACE_MedicalDefibrillation_DefibProgressData m_pProgressData;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		PrintFormat("%1::OnPostInit | Starting defibrillator init...", this.ClassName());
		
		// TODO: Settings
		
		
		m_pProgressData = new ACE_MedicalDefibrillation_DefibProgressData(m_fAnalysisDuration,
																		  m_fChargeDuration,
																		  m_fCPRCooldownDuration);
		
		// Subscribe to the InventoryItemComponent OnParentSlotChanged
		// Determines if defib is already on the ground to add it to the system
		InventoryItemComponent invComp = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		if (invComp)
		{
			invComp.m_OnParentSlotChangedInvoker.Insert(OnParentSlotChanged);
			if (!invComp.GetParentSlot())
			{
				ACE_MedicalDefibrillation_DefibStatesSystem system = GetDefibStatesSystem();
				if (system)
					system.Register(owner);
				
				SetDefibStateID(ACE_MedicalDefibrillation_EDefibStateID.DISCONNECTED);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_DefibStatesSystem GetDefibStatesSystem()
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return null;
		
		ACE_MedicalDefibrillation_DefibStatesSystem system = ACE_MedicalDefibrillation_DefibStatesSystem.Cast(world.FindSystem(ACE_MedicalDefibrillation_DefibStatesSystem));
		if (!system)
			return null;
		
		return system;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_EDefibrillatorEmulation GetDefibrillatorEmulation()
	{
		return m_eDefibrillatorEmulation;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_EDefibStateID GetDefibStateID()
	{
		return m_eDefibrillatorStateID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefibStateID(ACE_MedicalDefibrillation_EDefibStateID state)
	{
		m_eDefibrillatorStateID = state;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_DefibProgressData GetDefibProgressData()
	{
		return m_pProgressData;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPatient(IEntity patient)
	{
		m_pPatient = patient;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetPatient()
	{
		return m_pPatient;
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnParentSlotChanged()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnDefibStateChanged()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnDefibDataChanged()
	{
		// TODO: Impliment Defib UI Changes
	}
}