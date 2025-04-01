//------------------------------------------------------------------------------------------------
modded class SCR_MineInventoryItemComponent : SCR_PlaceableInventoryItemComponent
{
	//------------------------------------------------------------------------------------------------
	//! Mine should not be shown in inventory menu when buried
	override bool ShouldHideInVicinity()
	{
		if (super.ShouldHideInVicinity())
			return true;
		
		SCR_MineWeaponComponent weaponComponent = SCR_MineWeaponComponent.Cast(GetOwner().FindComponent(SCR_MineWeaponComponent));
		if (!weaponComponent)
			return false;
		
		return weaponComponent.ACE_Exposives_IsBuried();
	}
}
