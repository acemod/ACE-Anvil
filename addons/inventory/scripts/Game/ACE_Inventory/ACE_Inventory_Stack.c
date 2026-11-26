class ACE_Inventory_Stack
{
	//------------------------------------------------------------------------------------------------
	static bool IsSameGroup(IEntity representative, IEntity candidate)
	{
		if (!representative || !candidate)
			return false;
		if (representative == candidate)
			return true;
		if (!representative.GetPrefabData() || representative.GetPrefabData() != candidate.GetPrefabData())
			return false;

		InventoryItemComponent referenceItem = InventoryItemComponent.Cast(representative.FindComponent(InventoryItemComponent));
		InventoryItemComponent candidateItem = InventoryItemComponent.Cast(candidate.FindComponent(InventoryItemComponent));
		if (!referenceItem || !candidateItem || referenceItem.GetTotalWeight() != candidateItem.GetTotalWeight())
			return false;

		MagazineComponent referenceMagazine = MagazineComponent.Cast(representative.FindComponent(MagazineComponent));
		MagazineComponent candidateMagazine = MagazineComponent.Cast(candidate.FindComponent(MagazineComponent));
		if (referenceMagazine || candidateMagazine)
			return referenceMagazine && candidateMagazine && referenceMagazine.GetAmmoCount() == candidateMagazine.GetAmmoCount();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static void GetGroup(BaseInventoryStorageComponent source, IEntity representative, out array<IEntity> group)
	{
		group = {};
		if (!source || !representative)
			return;

		array<IEntity> items = {};
		if (ClothNodeStorageComponent.Cast(source))
		{
			// Clothing UI roots expose their pocket contents through owned storages.
			array<BaseInventoryStorageComponent> storages = {};
			source.GetOwnedStorages(storages, 1, false);
			foreach (BaseInventoryStorageComponent storage : storages)
			{
				if (!storage)
					continue;
				array<IEntity> pocketItems = {};
				storage.GetAll(pocketItems);
				foreach (IEntity pocketItem : pocketItems)
				{
					if (items.Find(pocketItem) < 0)
						items.Insert(pocketItem);
				}
			}
		}
		else
			source.GetAll(items);
		foreach (IEntity item : items)
		{
			if (IsSameGroup(representative, item))
				group.Insert(item);
		}
	}
}
