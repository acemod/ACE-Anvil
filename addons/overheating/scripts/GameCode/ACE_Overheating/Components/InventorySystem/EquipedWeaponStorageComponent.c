//------------------------------------------------------------------------------------------------
modded class EquipedWeaponStorageComponent : BaseEquipedWeaponStorageComponent
{
	protected bool m_bACE_Overheating_ShouldRegisterWeapons = false;
	
	//------------------------------------------------------------------------------------------------
	//! Whether equipped weapons should be registered at temperature system
	void ACE_Overheating_ToggleRegisterWeapons(bool enable)
	{
		ACE_Overheating_BarrelTemperatureSystem system = ACE_Overheating_BarrelTemperatureSystem.GetInstance();
		if (!system)
			return;
		
		array<IEntity> weapons = {};
		GetAll(weapons);
		
		foreach (IEntity weapon : weapons)
		{
			ACE_Overheating_MuzzleJamComponent jamComponent = ACE_Overheating_MuzzleJamComponent.FromWeapon(weapon);
			if (!jamComponent)
				continue;
			
			if (enable)
				system.Register(jamComponent);
			else
				system.Unregister(jamComponent);
		}
		
		m_bACE_Overheating_ShouldRegisterWeapons = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register picked up weapon at temperature system
	override protected void OnAddedToSlot(IEntity item, int slotID)
	{
		super.OnAddedToSlot(item, slotID);
		
		if (!m_bACE_Overheating_ShouldRegisterWeapons)
			return;
		
		ACE_Overheating_MuzzleJamComponent jamComponent = ACE_Overheating_MuzzleJamComponent.FromWeapon(item);
		if (!jamComponent)
			return;
		
		ACE_Overheating_BarrelTemperatureSystem system = ACE_Overheating_BarrelTemperatureSystem.GetInstance();
		if (system)
			system.Register(jamComponent);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister dropped weapon from temperature system
	override protected void OnRemovedFromSlot(IEntity item, int slotID)
	{
		super.OnRemovedFromSlot(item, slotID);
		
		if (!m_bACE_Overheating_ShouldRegisterWeapons)
			return;
		
		ACE_Overheating_MuzzleJamComponent jamComponent = ACE_Overheating_MuzzleJamComponent.FromWeapon(item);
		if (!jamComponent)
			return;
		
		ACE_Overheating_BarrelTemperatureSystem system = ACE_Overheating_BarrelTemperatureSystem.GetInstance();
		if (system)
			system.Unregister(jamComponent);
	}
}
