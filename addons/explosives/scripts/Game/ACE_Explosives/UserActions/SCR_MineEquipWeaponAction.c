//------------------------------------------------------------------------------------------------
modded class SCR_MineEquipWeaponAction : SCR_EquipWeaponAction
{
	//------------------------------------------------------------------------------------------------
	//! Mine cannot be picked up while buried
	override bool CanBeShownScript(IEntity user)
	{
		SCR_MineWeaponComponent mineWeaponComponent = SCR_MineWeaponComponent.Cast(GetOwner().FindComponent(SCR_MineWeaponComponent));
		if (mineWeaponComponent && mineWeaponComponent.ACE_Exposives_IsBuried())
			return false;
		
		return super.CanBeShownScript(user);
	}
}
