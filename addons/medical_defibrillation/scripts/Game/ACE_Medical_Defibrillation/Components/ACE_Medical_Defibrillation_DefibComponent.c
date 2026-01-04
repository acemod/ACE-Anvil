class ACE_Medical_Defibrillation_DefibComponentClass : ScriptComponentClass
{
}

class ACE_Medical_Defibrillation_DefibComponent : ScriptComponent
{
	[Attribute("1", UIWidgets.ComboBox, "Defibrillator Emulation Type", "", ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibEmulation))]
	protected ACE_Medical_Defibrillation_EDefibEmulation m_eDefibrillatorEmulation;
	
	protected float m_fAnalysisDuration = 3;
	protected float m_fChargeDuration = 5.5;
	
	[Attribute(defvalue: "120", params: "0 inf 1", desc: "Time (s) between analysis events where players should perform CPR.")]
	protected float m_fCPRCooldownDuration;
	
	[Attribute(defvalue: "10", params: "0 inf 0.1", desc: "Stun duration (s) for a character that touches the patient while a shock is delivered.")]
	protected float m_fContactShockStunDuration;
	
	[RplProp(onRplName: "OnPatientReplicated")]
	protected int m_iPatientRplId;
	protected IEntity m_pPatient;
	
	[RplProp(onRplName: "OnDefibStateChanged")]
	protected ACE_Medical_Defibrillation_EDefibStateID m_eDefibrillatorStateID;
	
	[RplProp(onRplName: "OnDefibProgressChanged"), RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	protected ref ACE_Medical_Defibrillation_DefibProgressData m_pProgressData;
	
	ref ACE_Medical_Defibrillation_DefibSounds m_pSounds;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{	
		super.EOnInit(owner);
		
		PrintFormat("%1::OnPostInit | Starting defibrillator init...", this.ClassName());
		
		ACE_Medical_Defibrillation_Defibrillation_Settings settings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Defibrillation_Settings>.GetModSettings();
		
		// Convert to milliseconds and make data
		m_pProgressData = new ACE_Medical_Defibrillation_DefibProgressData(this,
																		   m_fAnalysisDuration * 1000,
																		   m_fChargeDuration * 1000,
																		   m_fCPRCooldownDuration * 1000);
		
		// Create sound data
		m_pSounds = new ACE_Medical_Defibrillation_DefibSounds();
		
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
		
		// Subscribe to data change events for replication
		m_pProgressData.m_OnDataChanged.Insert(OnDefibProgressChanged);
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
	void Reset()
	{
		SetPatient(null);
		
		// Convert to milliseconds and make data
		m_pProgressData = new ACE_Medical_Defibrillation_DefibProgressData(this,
																		   m_fAnalysisDuration * 1000,
																		   m_fChargeDuration * 1000,
																		   m_fCPRCooldownDuration * 1000);
		
		// Create sound data
		m_pSounds = new ACE_Medical_Defibrillation_DefibSounds();
		m_eDefibrillatorStateID = ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED;
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
		if (!patient)
		{
			m_iPatientRplId = ACE_Medical_Defibrillation_ReplicationHelper.GetRplIdByEntity(null);
			m_pPatient = null;
			Replication.BumpMe();
			return;
		}
		
		ACE_Medical_VitalsComponent component = ACE_Medical_VitalsComponent.Cast(patient.FindComponent(ACE_Medical_VitalsComponent));
		if (!component)
			return;
		
		m_iPatientRplId = ACE_Medical_Defibrillation_ReplicationHelper.GetRplIdByEntity(patient);
		m_pPatient = patient;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPatient()
	{
		m_iPatientRplId = -1;
		m_pPatient = null;
		Replication.BumpMe();
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
		
		array<ACE_Medical_Defibrillation_ECardiacRhythm> shockableRhythms = { ACE_Medical_Defibrillation_ECardiacRhythm.VF };
		if (shockableRhythms.Contains(vitals.GetCardiacRhythm()))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	void ShockPatient()
	{
		if (!m_pPatient)
			return;
		
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(m_pPatient.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitals)
			return;
		
		vitals.ModifyShocksDelivered(1);
		GetSoundComponent().PlaySoundOnPatient(ACE_Medical_Defibrillation_DefibSounds.SOUNDSHOCKTHUMP);
		
		SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
		
		float cprCooldown = m_pProgressData.GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
		m_pProgressData.SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprCooldown);
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnDefibProgressChanged()
	{
		PrintFormat("%1::OnDefibDataChanged | Data updated to client...", this.ClassName());
		Replication.BumpMe();
	}
	
	
	//------------------------------------------------------------------------------------------------
	private void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		ACE_Medical_Defibrillation_DefibStatesSystem system = GetDefibStatesSystem();
		if (!system)
			return;
		
		if (!newSlot)
		{
			system.Register(GetOwner());
		}
		else
		{
			system.Unregister(GetOwner());
			Reset();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnDefibStateChanged()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPatientReplicated()
	{
		m_pPatient = ACE_Medical_Defibrillation_ReplicationHelper.GetEntityByRplId(m_iPatientRplId);
	}
}