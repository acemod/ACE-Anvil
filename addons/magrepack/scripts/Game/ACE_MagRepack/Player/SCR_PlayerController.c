//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController
{
	//------------------------------------------------------------------------------------------------
	void ACE_MagRepack_Repack(MagazineComponent fromMag, MagazineComponent toMag, SCR_InventoryStorageManagerComponent managerComp, BaseInventoryStorageComponent storageTo, BaseInventoryStorageComponent storageFrom)
	{
		if (!fromMag || !toMag)
		{
			Print("Could not locate magazine components", LogLevel.ERROR);
			return;
		}
		
		if (!managerComp)
		{
			Print("Was unable to locate storage or manager components", LogLevel.ERROR);
			return;
		}
		
		int fromCount = fromMag.GetAmmoCount();
		int toCount = toMag.GetAmmoCount();
		int maxCount = toMag.GetMaxAmmoCount();
		
		IEntity fromEntity = fromMag.GetOwner();
		IEntity toEntity = toMag.GetOwner();

		Print("fromMag holds: " + fromCount + " rounds.");
		Print("toMag holds: " + toCount + " rounds.");
		
			
		if (fromCount + toCount <= maxCount)
		{
			toMag.SetAmmoCount(fromCount + toCount);
			fromMag.SetAmmoCount(0);
			Print("Repacking exhausted fromMag.");
			Print("Added " + (fromCount) + " rounds to toMag. It now holds " + toMag.GetAmmoCount() + ". The remainder in fromMag is " + fromMag.GetAmmoCount() + ".");
						
			// Reinsert toMag in optimal slot so things stack correctly
			if (managerComp.TryRemoveItemFromInventory(toEntity, storageTo))
			{
				if (!managerComp.TryInsertItemInStorage(toEntity, managerComp.FindStorageForItem(toEntity, EStoragePurpose.PURPOSE_ANY)))
				{
					Print("Was unable to successfully add toMag back into storage. If it could not be dropped on the ground, it was deleted.", LogLevel.WARNING);
					if (!managerComp.TryRemoveItemFromInventory(toEntity)) {SCR_EntityHelper.DeleteEntityAndChildren(toEntity);}
				}
			}

			// fromMag is exhausted. It it cannot be dropped on the ground it is deleted.
			if (!managerComp.TryRemoveItemFromInventory(fromEntity)) 
			{
				Print("Was unable to drop fromMag on the ground. It was deleted.");
				SCR_EntityHelper.DeleteEntityAndChildren(fromEntity);
			}
				
		}
		else
		{		
			
			Print("Repacking did not exhaust fromMag.");
			int remainder = (fromCount + toCount) % maxCount;
			Print("Added " + (maxCount - toCount) + " rounds to toMag. It is now full. The remainder in fromMag is " + remainder);
			
			toMag.SetAmmoCount(maxCount);
			
			// Reinsert toMag in optimal slot to help stacking and updating
			if (managerComp.TryRemoveItemFromInventory(toEntity, storageTo))
			{
				if (!managerComp.TryInsertItemInStorage(toEntity, managerComp.FindStorageForItem(toEntity, EStoragePurpose.PURPOSE_ANY)))
				{
					Print("Was unable to successfully toMag back into storage. If it could not be droppeed on the ground, it was deleted.", LogLevel.WARNING);
					if (!managerComp.TryRemoveItemFromInventory(toEntity)) {SCR_EntityHelper.DeleteEntityAndChildren(toEntity);}
				}
			}

			if (remainder > 0)
			{
				fromMag.SetAmmoCount(remainder);
				
				// Reinsert fromMag in optimal slot to help stacking and updating
				if (managerComp.TryRemoveItemFromInventory(fromEntity, storageFrom))
				{
					if (!managerComp.TryInsertItemInStorage(fromEntity, managerComp.FindStorageForItem(fromEntity, EStoragePurpose.PURPOSE_ANY)))
					{
						Print("Was unable to successfully fromMag back into storage. If it could not be droppeed on the ground, it was deleted.", LogLevel.WARNING);
						if (!managerComp.TryRemoveItemFromInventory(fromEntity)) {SCR_EntityHelper.DeleteEntityAndChildren(fromEntity);}
					}
				}
		
			}				
			else
			{
				SCR_EntityHelper.DeleteEntityAndChildren(fromEntity);
			}
		}	
	}		
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void ACE_MagRepack_RpcAsk_Repack(RplId fromMag, RplId toMag, RplId managerComp, RplId storageTo, RplId storageFrom)
	{
  		// Make sure we have everything
		if (!fromMag.IsValid())
		{
			Print("Invalid from magazine", LogLevel.ERROR);
			return;
		}
		
		if (!toMag.IsValid())
		{
			Print("Invalid to magazine", LogLevel.ERROR);
			return;
		}
		
		if (!managerComp.IsValid())
		{
			Print("Invalid manager", LogLevel.ERROR);
			return;
		}
		
		if (!storageTo.IsValid())
		{
			Print("Invalid storageTo", LogLevel.ERROR);
		}
		
		if (!storageFrom.IsValid())
		{
			Print("Invalid storageFrom", LogLevel.ERROR);
		}
		
		ACE_MagRepack_Repack(
			MagazineComponent.Cast(Replication.FindItem(fromMag)),
			MagazineComponent.Cast(Replication.FindItem(toMag)),
			SCR_InventoryStorageManagerComponent.Cast(Replication.FindItem(managerComp)),
			BaseInventoryStorageComponent.Cast(Replication.FindItem(storageTo)),
			BaseInventoryStorageComponent.Cast(Replication.FindItem(storageFrom))
		);
	}	
}
