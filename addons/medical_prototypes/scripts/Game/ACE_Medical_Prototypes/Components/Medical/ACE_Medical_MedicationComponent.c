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
	
	[RplProp()]
	protected ref array<float> m_aLogMessageTimes = {};
	[RplProp()]
	protected ref array<string> m_aLogMessages = {};
	[RplProp()]
	protected ref array<string> m_aLogMessageAuthors = {};
	protected const int MAX_NUM_LOG_ENTRIES = 8;

	
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
	void AddMedication(ACE_Medical_Dose dose)
	{
		ACE_Medical_EDrugType drug = dose.GetDrugType();
		dose.SetAdministrationTime();
		
		ACE_Medical_BaseSystem2 system = ACE_Medical_BaseSystem2.GetInstance(ACE_Medical_MedicationSystem);
		if (system && m_aDrugs.IsEmpty())
			system.Register(GetOwner());
		
		int idx = m_aDrugs.Find(drug);
		if (idx >= 0)
		{
			m_aDoses[idx].Insert(dose);
		}
		else
		{
			m_aDrugs.Insert(drug);
			m_aDoses.Insert({dose});
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void GetMedications(out array<ACE_Medical_EDrugType> drugs, out array<ref array<ref ACE_Medical_Dose>> doses)
	{
		drugs = m_aDrugs;
		doses = m_aDoses;
	}
	
	//------------------------------------------------------------------------------------------------
	void AddLogEntry(string message, int authorID = -1)
	{
		float timeOfDay = 24 * GetGame().GetClock().GetTimeOfDay();
		RplDo_AddLogEntryBroadcast(timeOfDay, message, authorID);
		Rpc(RplDo_AddLogEntryBroadcast, timeOfDay, message, authorID);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RplDo_AddLogEntryBroadcast(float time, string message, int authorID)
	{
		if (m_aLogMessages.Count() >= MAX_NUM_LOG_ENTRIES)
		{
			m_aLogMessageTimes.RemoveOrdered(0);
			m_aLogMessages.RemoveOrdered(0);
			m_aLogMessageAuthors.RemoveOrdered(0);
		}
		
		m_aLogMessageTimes.Insert(time);
		m_aLogMessages.Insert(message);
		
		string authorName = GetGame().GetPlayerManager().GetPlayerName(authorID);
		if (authorName.IsEmpty())
			authorName = "N/A";
		
		m_aLogMessageAuthors.Insert(authorName);
	}
	
	//------------------------------------------------------------------------------------------------
	void GetLogData(out array<float> times, out array<string> messages, out array<string> authors)
	{
		times = m_aLogMessageTimes;
		messages = m_aLogMessages;
		authors = m_aLogMessageAuthors
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clears the body of all drugs and effects
	void Clear()
	{
		m_aDrugs.Clear();
		m_aDoses.Clear();
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings || !settings.m_MedicationSystem)
			return;
		
		foreach (ACE_Medical_DrugEffectConfig effect : settings.m_MedicationSystem.m_aPharmacodynamicsConfigs)
		{
			effect.ClearEffect(GetOwner());
		}
	}
}
