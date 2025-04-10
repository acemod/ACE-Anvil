//------------------------------------------------------------------------------------------------
class ACE_Medical_Dose : ScriptAndConfig
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of administered drug", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	protected ACE_Medical_EDrugType m_eDrugType;
	
	protected float m_fAdministrationTimeMS;
	protected bool m_bIsExpired = false;
	
	protected static const float EXPIRED_DOSE_PERCENTAGE = 0.01; // Concentration at which dose is considered expired [percentage of initial]
	
	//------------------------------------------------------------------------------------------------
	//! Computes the current concentration of this dose based on the administration time and the
	//! passed config for the pharamockinetics
	float ComputeConcentration(ACE_Medical_PharmacokineticsConfig config);
	
	//------------------------------------------------------------------------------------------------
	//! Get the type of the drug administered by this dose
	ACE_Medical_EDrugType GetDrugType()
	{
		return m_eDrugType;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets administration time to current time
	void SetAdministrationTime()
	{
		m_fAdministrationTimeMS = GetGame().GetWorld().GetWorldTime();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Time passed since dose was administered [s]
	float GetElapsedTime()
	{
		return (GetGame().GetWorld().GetWorldTime() - m_fAdministrationTimeMS) / 1000;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return true when the dose's concentration has dropped so low that it should be considered expired
	bool IsExpired()
	{
		return m_bIsExpired;
	}
}
