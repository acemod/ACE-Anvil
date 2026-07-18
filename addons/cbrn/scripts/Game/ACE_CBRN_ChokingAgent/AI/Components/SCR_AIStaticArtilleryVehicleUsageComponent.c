//----------------------------------------------------------------------------------------
modded enum SCR_EAIArtilleryAmmoType
{
	ACE_CBRN_CHEMICAL_WEAPON = 42
}

//----------------------------------------------------------------------------------------
modded class SCR_AIStaticArtilleryVehicleUsageComponentClass : SCR_AIVehicleUsageComponentClass
{
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", category: "Ammo Prefabs")]
	protected ResourceName m_sACE_CBRN_ChemicalWeapon;
	
	//----------------------------------------------------------------------------------------
	override ResourceName GetAmmoResourceName(SCR_EAIArtilleryAmmoType ammoType)
	{
		ResourceName resName = super.GetAmmoResourceName(ammoType);
		if (!resName.IsEmpty())
			return resName;
		
		switch (ammoType)
		{
			case SCR_EAIArtilleryAmmoType.ACE_CBRN_CHEMICAL_WEAPON:
				return m_sACE_CBRN_ChemicalWeapon;
		}
		
		return string.Empty;
	}
}
