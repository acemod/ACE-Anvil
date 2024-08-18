class ACE_Medical_GadgetAEDClass : GenericEntityClass
{
}

class ACE_Medical_GadgetAED : GenericEntity
{
	// Constants - Defibrillation 
	protected const int m_iAEDChargeTime = 3000;		// ms 
	
	// Properties - Defibrillation 
	protected int m_iAEDUsesLeft = 999;
	protected bool m_bAEDIsCharged = false;
	protected float m_fAEDChargeAmount = 0;
	
	//------------------------------------------------------------------------------------------------
	void ChargeAED()
	{
		if (m_fAEDChargeAmount < 1)
		{
			m_fAEDChargeAmount++;
			GetGame().GetCallqueue().CallLater(ChargeAED, 1000, true)
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsAEDCharged()
	{
		if (m_fAEDChargeAmount < 1)
		{
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAEDChargeAmount()
	{
		return m_fAEDChargeAmount;
	}
	
	//------------------------------------------------------------------------------------------------
	bool Defibrillate(ACE_Medical_CardiovascularComponent component)
	{		
		if (IsAEDCharged() &&
			IsShockableRhythm(component))
		{
			component.AddShocksDelivered();
			return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsShockableRhythm(ACE_Medical_CardiovascularComponent component)
	{
		if (component.GetCardiacRhythmState() != ACE_Medical_ECardiacRhythmState.VF)
		{
			return false;
		}
		
		return true;
	}
}