//------------------------------------------------------------------------------------------------
//! Add SpO2
modded class ACE_Medical_VitalsComponent : ACE_BaseComponent
{
	protected float m_fRespiratoryRate = 14.573233653074396; // 1/min
	protected float m_fTidalVolume = 500*2/3; // ml
	protected float m_fCapacityVolume = 6000*2/3; // ml
	protected float m_fSpO2 = 97.14862728279128; // %
	protected float m_fPalvO2 = 13.3; // kPa
	protected float m_fPalvCO2 = 5.2101385454100555; // kPa
	protected float m_fCvenCO2 = 0.0012360371385742145; // kPa
	
	[RplProp()]
	protected bool m_bIsAirwayObstructed = false;
	[RplProp()]
	protected bool m_bIsAirwayOccluded = false;
	[RplProp()]
	protected float m_fPneumothoraxScale = 0;
	[RplProp()]
	protected bool m_bHasTensionPneumothorax = false;
	
	static const float PALVO2_MAX = 19.925; // kPa
	static const float CARBONATE_DEHYDRATION_RATE_SCALE = 126.93385;
	
	//------------------------------------------------------------------------------------------------
	//! Sets respiratory rate in 1/min
	void SetRespiratoryRate(float rate)
	{
		m_fRespiratoryRate = rate;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns respiratory rate in 1/min
	float GetRespiratoryRate()
	{
		return m_fRespiratoryRate;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns tidal volume of lung in ml
	float GetTidalVolume()
	{
		return m_fTidalVolume * (1 - m_fPneumothoraxScale);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns capacity volume of lung in ml
	float GetCapacityVolume()
	{
		return m_fCapacityVolume * (1 - m_fPneumothoraxScale);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets peripheral oxygen saturation
	void SetSpO2(float spo2)
	{
		m_fSpO2 = spo2;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns peripheral oxygen saturation
	float GetSpO2()
	{
		return m_fSpO2;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets alveolar oxygen pressure in kPa
	void SetPalvO2(float p)
	{
		m_fPalvO2 = p;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns alveolar oxygen pressure in kPa
	float GetPalvO2()
	{
		return m_fPalvO2;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets alveolar carbon dioxide pressure in kPa
	void SetPalvCO2(float p)
	{
		m_fPalvCO2 = p;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns alveolar carbon dioxide pressure in kPa
	float GetPalvCO2()
	{
		return m_fPalvCO2;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets venous carbon dioxide concentration in nM
	void SetCvenCO2(float c)
	{
		m_fCvenCO2 = c;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns venous carbon dioxide concentration in nM
	float GetCvenCO2()
	{
		return m_fCvenCO2;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns body temperature in K
	float GetTemperature()
	{
		return 310.15;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets extend of pneumothorax
	void SetPneumothoraxScale(float scale)
	{
		m_fPneumothoraxScale = scale;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns extend of pneumothorax
	float GetPneumothoraxScale()
	{
		return m_fPneumothoraxScale;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set tongue-based obstruction of airway
	void SetIsAirwayObstructed(bool isObstructed)
	{
		m_bIsAirwayObstructed = isObstructed;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Is airway obstructed by tongue
	bool IsAirwayObstructed()
	{
		return m_bIsAirwayObstructed;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set vomit-based occlusion of airway
	void SetIsAirwayOccluded(bool isOccluded)
	{
		m_bIsAirwayOccluded = isOccluded;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Is airway occluded by vomit
	bool IsAirwayOccluded()
	{
		return m_bIsAirwayOccluded;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHasTensionPneumothorax(bool hasTPTX)
	{
		m_bHasTensionPneumothorax = hasTPTX;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasTensionPneumothorax()
	{
		return m_bHasTensionPneumothorax;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanBreath()
	{
		return !m_bIsAirwayObstructed && !m_bIsAirwayOccluded && !m_bHasTensionPneumothorax;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Resets vitals to defaults
	override void Reset()
	{
		super.Reset();
		SetRespiratoryRate(14.573233653074396);
		SetSpO2(97.14862728279128);
		SetPalvO2(13.3);
		SetPalvCO2(5.2101385454100555);
		SetCvenCO2(0.0012360371385742145);
		SetPneumothoraxScale(0);
		SetIsAirwayObstructed(false);
		SetIsAirwayOccluded(false);
		SetHasTensionPneumothorax(false);
	}
}
