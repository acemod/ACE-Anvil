//----------------------------------------------------------------------------------------
modded enum SCR_EAIArtilleryAmmoType
{
	ACE_CBRN_SARIN = 43
}

//----------------------------------------------------------------------------------------
modded class SCR_AIStaticArtilleryVehicleUsageComponentClass : SCR_AIVehicleUsageComponentClass
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "et", category: "Ammo Prefabs")]
	protected ResourceName m_sACE_CBRN_Sarin;
	
	//----------------------------------------------------------------------------------------
	override ResourceName GetAmmoResourceName(SCR_EAIArtilleryAmmoType ammoType)
	{
		ResourceName resName = super.GetAmmoResourceName(ammoType);
		if (!resName.IsEmpty())
			return resName;
		
		switch (ammoType)
		{
			case SCR_EAIArtilleryAmmoType.ACE_CBRN_SARIN:
				return m_sACE_CBRN_Sarin;
		}
		
		return string.Empty;
	}
}
