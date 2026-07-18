//------------------------------------------------------------------------------------------------
modded class ACE_Medical_EpinephrineUserAction : SCR_MorphineUserAction
{
	[Attribute(defvalue: "#ACE_CBRN-FailReason_Paralyzed", desc: "String for when target is paralyzed")]
	protected LocalizedString m_sACE_CBRN_Paralyzed;
	
	//------------------------------------------------------------------------------------------------
	override bool SetCannotPerformReason(SCR_EConsumableFailReason reason)
	{
		if (super.SetCannotPerformReason(reason))
			return true;
		
		switch (reason)
		{
			case SCR_EConsumableFailReason.ACE_CBRN_PARALYZED:
			{
				SetCannotPerformReason(m_sACE_CBRN_Paralyzed);
				return true;
			}
		}
		
		return false;
	}
}
