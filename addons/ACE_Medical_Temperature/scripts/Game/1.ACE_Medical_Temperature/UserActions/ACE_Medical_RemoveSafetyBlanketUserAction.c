class ACE_Medical_RemoveSafetyBlanketUserAction : SCR_ScriptedUserAction
{
	override bool CanBeShownScript(IEntity user)
	{	
		if (!super.CanBeShownScript(user))
		{
			return false;
		}
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(GetOwner().FindComponent(SCR_CharacterControllerComponent));
		if (charController.GetLifeState() != ECharacterLifeState.INCAPACITATED)
		{
			return false;
		}
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(GetOwner().FindComponent(ACE_EquipmentStorageComponent));
		if (!storageComponent)
			return false;
		
		if (!storageComponent.IsSlotOccupied(ACE_EEquipementSlot.SAFETY_BLANKET))
			
			return false;
		
		return true;	
	}
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;

		SCR_InventoryStorageManagerComponent pInventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!pInventoryManager)
			return false;

		if (!pInventoryManager.IsAnimationReady())
			return false;
		//TODO:Make this shithead caninsertitem actually freaking work
		pInventoryManager.SetReturnCodeDefault();
		ACE_EquipmentStorageComponent ACEEquipmentStorageComponent = ACE_EquipmentStorageComponent.Cast(GetOwner().FindComponent(ACE_EquipmentStorageComponent));
		InventoryItemComponent itemInventoryItemComponent = ACEEquipmentStorageComponent.GetItem(ACE_EEquipementSlot.SAFETY_BLANKET);
		//Workaround for VME after removing blanket
		if (!itemInventoryItemComponent)
			return false;
		IEntity item = itemInventoryItemComponent.GetOwner();
		
		BaseInventoryStorageComponent storage = pInventoryManager.FindStorageForItem(item,EStoragePurpose.PURPOSE_DEPOSIT);
		if (!storage)
		{
			EInventoryRetCode ERetCode = pInventoryManager.GetReturnCode();
			string sReason = "";
			
			if ( ( ERetCode & EInventoryRetCode.RETCODE_ITEM_TOO_BIG ) == EInventoryRetCode.RETCODE_ITEM_TOO_BIG )
				sReason += " " + "#AR-Inventory_InfoTooBig";
					
			if ( ( ERetCode & EInventoryRetCode.RETCODE_ITEM_TOO_HEAVY ) == EInventoryRetCode.RETCODE_ITEM_TOO_HEAVY )
				sReason += " " + "#AR-Inventory_InfoTooHeavy";
			SetCannotPerformReason(sReason);
			return false;
		}
		
		return true;
	}
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_InventoryStorageManagerComponent storageComponentOwner = SCR_InventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		ACE_EquipmentStorageComponent ACEStorageComponentOwner = ACE_EquipmentStorageComponent.Cast(pOwnerEntity.FindComponent(ACE_EquipmentStorageComponent));
		IEntity item = ACEStorageComponentOwner.GetItem(ACE_EEquipementSlot.SAFETY_BLANKET).GetOwner();
		
		SCR_InventoryStorageManagerComponent storageComponentUser = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		BaseInventoryStorageComponent storage = storageComponentUser.FindStorageForItem(item, EStoragePurpose.PURPOSE_DEPOSIT);
		if (!storage)
			return;
		
		storageComponentUser.InsertItem(item, storage,ACEStorageComponentOwner);
	}


}