//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController
{
	//------------------------------------------------------------------------------------------------
	void ACE_MagRepack_Repack(MagazineComponent fromMag, MagazineComponent toMag, SCR_InventoryStorageManagerComponent managerComp, BaseInventoryStorageComponent storageTo, BaseInventoryStorageComponent storageFrom)
	{
		if(!fromMag || !toMag)
		{
			Print("Could not locate magazine components", LogLevel.ERROR);
			return;
		}
		
		if(!managerComp)
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
		
			
		if(fromCount + toCount <= maxCount)
		{
			toMag.SetAmmoCount(fromCount + toCount);
			Print("Added " + fromCount + " rounds to toMag. It now holds " + (fromCount + toCount) + " rounds. fromMag was fully exhausted.");
			
			
			// Reinsert mag in optimal slot to help stacking and updating
			if(managerComp.TryRemoveItemFromInventory(toEntity, storageTo))
			{
				Print("Underflow situation. Mag removed from inventory. Inserting.");
				// Just print something if for whatever reason we can't add the magazine back
				if(!managerComp.TryInsertItemInStorage(toEntity, managerComp.FindStorageForItem(toEntity, EStoragePurpose.PURPOSE_ANY)))
				//if(!managerComp.TryInsertItemInStorage(toEntity, storageTo))
				{
					Print("Was unable to successfully add packed magazine back into storage. Trying to find new storage for item.", LogLevel.WARNING);
				}
			}

			// fromMag is exhausted and deleted	
			//SCR_EntityHelper.DeleteEntityAndChildren(fromEntity);
			
			// fromMag is exhausted and dropped on the ground
			managerComp.TryRemoveItemFromInventory(fromEntity);
				
		}
		else
		{		
			
			int remainder = (fromCount + toCount) % maxCount;
			Print("Added " + (maxCount - toCount) + " rounds to toMag. It is now full. The remainder in fromMag is " + remainder);
			
			toMag.SetAmmoCount(maxCount);
			
			// Reinsert mag in optimal slot to help stacking and updating
			if(managerComp.TryRemoveItemFromInventory(toEntity, storageTo))
			{
				if(!managerComp.TryInsertItemInStorage(toEntity, managerComp.FindStorageForItem(toEntity, EStoragePurpose.PURPOSE_ANY)))
				{
					Print("Was unable to successfully add packed magazine back into storage", LogLevel.ERROR);
				}
			}

			if(remainder > 0)
			{
				fromMag.SetAmmoCount(remainder);
				
				if(managerComp.TryRemoveItemFromInventory(fromEntity, storageFrom))
				{
					if(!managerComp.TryInsertItemInStorage(fromEntity, storageFrom))
					{
						Print("Was unable to successfully add packed magazine back into storage", LogLevel.ERROR);
					}
				}
		
			}				
			else
			{
				SCR_EntityHelper.DeleteEntityAndChildren(fromMag.GetOwner());
			}
		}	
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void ACE_MagRepack_RpcAsk_Repack(RplId fromMag, RplId toMag, RplId managerComp, RplId storageTo, RplId storageFrom)
	{
  		// Make sure we have everything
		if(!fromMag.IsValid())
		{
			Print("Invalid from magazine", LogLevel.ERROR);
			return;
		}
		
		if(!toMag.IsValid())
		{
			Print("Invalid to magazine", LogLevel.ERROR);
			return;
		}
		
		if(!managerComp.IsValid())
		{
			Print("Invalid manager", LogLevel.ERROR);
			return;
		}
		
		if(!storageTo.IsValid())
		{
			Print("Invalid storageTo", LogLevel.ERROR);
		}
		
		if(!storageFrom.IsValid())
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
