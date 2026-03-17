//------------------------------------------------------------------------------------------------
modded class SCR_MineEquipHolsterAction : SCR_EquipWeaponHolsterAction
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
