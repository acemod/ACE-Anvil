//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_SelectTurretMagazineRadialMenuCategoryEntry : ACE_SelectionMenuCategoryEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		m_aEntries.Clear();
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!char)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(char.GetCompartmentAccessComponent());
		if (!compartmentAccess)
			return;
		
		TurretCompartmentSlot turretCompartment = TurretCompartmentSlot.Cast(compartmentAccess.GetCompartment());
		if (!turretCompartment)
			return;
		
		BaseWeaponManagerComponent weaponManager = BaseWeaponManagerComponent.Cast(turretCompartment.GetOwner().FindComponent(BaseWeaponManagerComponent));
		if (!weaponManager)
			return;
		
		BaseWeaponComponent weapon = weaponManager.GetCurrentWeapon();
		if (!weapon)
			return;
		
		BaseMuzzleComponent muzzle = weapon.GetCurrentMuzzle();
		if (!muzzle)
			return;
		
		IEntity vehicle = turretCompartment.GetVehicle();
		if (!vehicle)
			return;
		
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(vehicle.FindComponent(InventoryStorageManagerComponent));
		if (!storageManager)
			return;
		
		SCR_MagazinePredicate searchPredicate = new SCR_MagazinePredicate();
		searchPredicate.magWellType = muzzle.GetMagazineWell().Type();
		
		array<IEntity> items = {};
		storageManager.FindItems(items, searchPredicate);
		SCR_Sorting<IEntity, ACE_CompareItemName>.HeapSort(items, false);
		
		// Make sure we only add each
		set<EntityPrefabData> uniquePrefabDataSet = new set<EntityPrefabData>();
		
		foreach (IEntity item : items)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			if (!prefabData)
				continue;
			
			if (uniquePrefabDataSet.Find(prefabData) >= 0)
				continue;
			
			m_aEntries.Insert(new ACE_SelectTurretMagazineRadialMenuEntry(item));
			uniquePrefabDataSet.Insert(prefabData);
		}
	}
}
