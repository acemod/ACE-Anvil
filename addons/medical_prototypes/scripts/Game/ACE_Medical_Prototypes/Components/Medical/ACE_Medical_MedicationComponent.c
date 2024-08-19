//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationComponentClass : ACE_Medical_BaseComponent2Class
{
}

//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
//! TO DO: Inherit from ACE_Medical_BaseComponent once systems support inheritance
class ACE_Medical_MedicationComponent : ACE_Medical_BaseComponent2
{
	protected ref array<ACE_Medical_EDrugType> m_aDrugs = {};
	protected ref array<ref array<ref ACE_Medical_Dose>> m_aDoses = {};
	
	//------------------------------------------------------------------------------------------------
	//! Register this component at ACE_Medical_CardiovascularSystem
	override typename GetAssociatedSystemType()
	{
		return ACE_Medical_MedicationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	//! We only register at the system once medication was added
	override protected bool ShouldRegisterAtSystemOnInit()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	void AddMedication(ACE_Medical_EDrugType drug, ACE_Medical_Dose dose)
	{
		dose.SetAdministrationTime();
		
		ACE_Medical_BaseSystem2 system = ACE_Medical_BaseSystem2.GetInstance(ACE_Medical_MedicationSystem);
		if (system && m_aDrugs.IsEmpty())
			system.Register(GetOwner());
		
		int idx = m_aDrugs.Find(drug);
		if (idx >= 0)
		{
			m_aDoses[idx].Insert(dose);
			return;
		}
		
		m_aDrugs.Insert(drug);
		m_aDoses.Insert({dose});			
	}
	
	//------------------------------------------------------------------------------------------------
	void GetMedications(out array<ACE_Medical_EDrugType> drugs, out array<ref array<ref ACE_Medical_Dose>> doses)
	{
		drugs = m_aDrugs;
		doses = m_aDoses;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clears the body of all drugs
	void Clear()
	{
		m_aDrugs.Clear();
		m_aDoses.Clear();
	}
}
