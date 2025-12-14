class ACE_Medical_Defibrillation_DefibComponentClass : ScriptComponentClass
{
}

class ACE_Medical_Defibrillation_DefibComponent : ScriptComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibEmulation))]
	protected ACE_Medical_Defibrillation_EDefibEmulation m_eDefibrillatorEmulation;
	
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
	protected ACE_Medical_Defibrillation_EDefibStateID m_eDefibrillatorStateID;
	
	[RplProp(onRplName: "OnDefibDataChanged")]
	protected ref ACE_Medical_Defibrillation_DefibProgressData m_pProgressData;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{		
		PrintFormat("%1::OnPostInit | Starting defibrillator init...", this.ClassName());
		
		// TODO: Settings
		
		// Convert to milliseconds and make data
		m_pProgressData = new ACE_Medical_Defibrillation_DefibProgressData(m_fAnalysisDuration * 1000,
																		  m_fChargeDuration * 1000,
																		  m_fCPRCooldownDuration * 1000);
		
		// Subscribe to the InventoryItemComponent OnParentSlotChanged
		// Determines if defib is already on the ground to add it to the system
		InventoryItemComponent invComp = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		if (invComp)
		{
			invComp.m_OnParentSlotChangedInvoker.Insert(OnParentSlotChanged);
			if (!invComp.GetParentSlot())
			{
				ACE_Medical_Defibrillation_DefibStatesSystem system = GetDefibStatesSystem();
				if (system)
					system.Register(owner);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_DefibSoundComponent GetSoundComponent()
	{
		ACE_Medical_Defibrillation_DefibSoundComponent component = ACE_Medical_Defibrillation_DefibSoundComponent.Cast(GetOwner().FindComponent(ACE_Medical_Defibrillation_DefibSoundComponent));
		if (!component)
			return null;
		
		return component;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_DefibStatesSystem GetDefibStatesSystem()
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return null;
		
		ACE_Medical_Defibrillation_DefibStatesSystem system = ACE_Medical_Defibrillation_DefibStatesSystem.Cast(world.FindSystem(ACE_Medical_Defibrillation_DefibStatesSystem));
		if (!system)
			return null;
		
		return system;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_EDefibEmulation GetDefibrillatorEmulation()
	{
		return m_eDefibrillatorEmulation;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_EDefibStateID GetDefibStateID()
	{
		return m_eDefibrillatorStateID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID state)
	{
		m_eDefibrillatorStateID = state;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_DefibProgressData GetDefibProgressData()
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
	bool IsShockableRhythm()
	{
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_pPatient.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return false;
		
		array<ACE_Medical_ECardiacRhythm> shockableRhythms = { ACE_Medical_ECardiacRhythm.VF };
		if (shockableRhythms.Contains(vitals.GetCardiacRhythm()))
			return true;
		
		return false;
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
		PrintFormat("%1::OnDefibDataChanged | Data updated to client...", this.ClassName());
		// TODO: Impliment Defib UI Changes
	}
}