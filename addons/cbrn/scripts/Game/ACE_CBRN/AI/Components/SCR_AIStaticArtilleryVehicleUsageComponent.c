//----------------------------------------------------------------------------------------
modded enum SCR_EAIArtilleryAmmoType
{
	ACE_CBRN_POISON = 42
}

//----------------------------------------------------------------------------------------
modded class SCR_AIStaticArtilleryVehicleUsageComponentClass : SCR_AIVehicleUsageComponentClass
{
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", category: "Ammo Prefabs")]
	protected ResourceName m_sACE_CBRN_AmmoPoison;
	
	//----------------------------------------------------------------------------------------
	override ResourceName GetAmmoResourceName(SCR_EAIArtilleryAmmoType ammoType)
	{
		ResourceName resName = super.GetAmmoResourceName(ammoType);
		if (!resName.IsEmpty())
			return resName;
		
		switch (ammoType)
		{
			case SCR_EAIArtilleryAmmoType.ACE_CBRN_POISON:
				return m_sACE_CBRN_AmmoPoison;
		}
		
		return string.Empty;
	}
}
