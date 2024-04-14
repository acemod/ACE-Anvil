//------------------------------------------------------------------------------------------------
modded class SCR_InventoryMenuUI
{
	//------------------------------------------------------------------------------------------------
	override void MoveItemToStorageSlot()
	{
		if (!SESOF_MagRepack())
		{
			super.MoveItemToStorageSlot();
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Checks to ensure we have two magazines that should be repacked, and passes the necessary bits to RplAsk_RepackMags()-function in SCR_PlayerController
	//! 
	//! Returns true if repacking occurred 
	//! Returns true if magwells were incompatible (feels nicer to have them not move.) 					This prevents swapping positions of mags - remove?
	//! Returns true if magwells are the same, but toMag is full and no repacking can happen. Feels nicer. 	This prevents swapping positions of mags - remove?
	//! Returns true if dropping on same non-stacked slot. Feels nicer.
	//! 	
	//! Returns false if dragging from or dropping on an arsenal.
	//! Returns false if not dragging a mag onto another mag
	bool SESOF_MagRepack()
	{
		// Make sure we are not repacking arsenals
		if (IsStorageArsenal(m_pFocusedSlotUI.GetStorageUI().GetCurrentNavigationStorage()))
			return false;
		
		if (IsStorageArsenal(m_pSelectedSlotUI.GetStorageUI().GetCurrentNavigationStorage()))
			return false;
		
		// Make sure we are dragging and dropping a mag onto another mag
		if (m_pSelectedSlotUI.GetSlotedItemFunction() != ESlotFunction.TYPE_MAGAZINE ||	m_pFocusedSlotUI.GetSlotedItemFunction() != ESlotFunction.TYPE_MAGAZINE)
			return false;							
			
		// Get item components in selected and focused slots, and check they are not the same item
		InventoryItemComponent fromInventoryItemComponent, toInventoryItemComponent
		IEntity fromEntity, toEntity	
		
		bool repackOnAStack = false;	
		
		bool repackFromAStack = m_pSelectedSlotUI.IsStacked();
		if (repackFromAStack)
			Print("Dragged from a stack");
		
		if (!repackFromAStack)
			Print("Did not drag from a stack.");
		
		bool isToSlotStacked = m_pFocusedSlotUI.IsStacked();
		if (isToSlotStacked)
			Print("Dropped on a stack.");
		
		if (!isToSlotStacked)
			Print("Did not drop on a stack.");
		
		fromInventoryItemComponent = m_pSelectedSlotUI.GetInventoryItemComponent();
		fromEntity = fromInventoryItemComponent.GetOwner();
		
		// If we are dropping on the same slot we dragged from, only repack if the slot holds a stack
		if (m_pSelectedSlotUI == m_pFocusedSlotUI)																				
		{
			if (m_pFocusedSlotUI.IsStacked())
			{	
				ref array<IEntity> items = {};
				
				// The array will fill with items in order
				GetInventoryStorageManager().GetItems(items);
				int topMagPosition = items.Find(fromEntity);
				
				// The item below in the stack is the next from the one focused and selected
				toEntity = items[topMagPosition + 1];
				toInventoryItemComponent = m_pFocusedSlotUI.GetInventoryItemComponent();
				
				repackOnAStack = true;																		
			}			
			
			// Not a stack, so there's no mag there to repack with
			else
			{
				Print("Dragged and dropped onto the same mag. No repacking.");
				m_InventoryManager.PlayItemSound(fromEntity, SCR_SoundEvent.SOUND_INV_DROP_ERROR);
				return true;																										
			}																									
		}
		// If we are not dragging and dropping onto the same slot, we can just get the item in the focused slot directly
		else
		{
			toInventoryItemComponent = m_pFocusedSlotUI.GetInventoryItemComponent();										
			toEntity = toInventoryItemComponent.GetOwner();
		}
		
		
		// If we somehow have the same item at this point, no repacking should occur
		if (fromEntity.GetID() == toEntity.GetID())
		{
			Print("Dragged and dropped onto the same mag. No repacking.");
			return false;
		}																		
		
		// If we somehow failed to get two items at this point, no repacking should occur
		if (!fromEntity)
			Print("Failed to acquire fromEntity!", LogLevel.ERROR);
		
		if (!toEntity)
			Print("Failed to acquire toEntity!", LogLevel.ERROR);
		
		if (!fromEntity || !toEntity)
			return false;
		
		
		// We now have two mags, and can see if they are compatible to repack
		MagazineComponent fromMag = MagazineComponent.Cast(fromEntity.FindComponent(MagazineComponent));	
		MagazineComponent toMag = MagazineComponent.Cast(toEntity.FindComponent(MagazineComponent));
		
		if (!fromMag)
			Print("Failed to acquire fromMag!", LogLevel.ERROR);
		
		if (!toMag)
			Print("Failed to acquire toMag!", LogLevel.ERROR);
		
		if (!fromMag || !toMag)
			return false;
		
		if (fromMag.GetMagazineWell().Type() != toMag.GetMagazineWell().Type())							
		{
			Print("Incompatible MagazineWells.");
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_DROP_ERROR);
			return false;
		}
		
			 
		// If both magazines are full 
		if (toMag.GetAmmoCount() == toMag.GetMaxAmmoCount() && fromMag.GetAmmoCount() == fromMag.GetMaxAmmoCount())
		{				
			Print("toMag & fromMag are both full, so this should be a regular attempted move.");
			return false;
		}
		
		
		// If we're dragging onto a full mag then there will not be any repacking
		if (toMag.GetAmmoCount() == toMag.GetMaxAmmoCount())
		{				
			Print("toMag is full. No repacking.");
			m_InventoryManager.PlayItemSound(fromEntity, SCR_SoundEvent.SOUND_INV_DROP_ERROR);
			return false;
		}
		
			
		/* 
		--------------------------------------------------------------------------------------------------------------------------------
			REPACK
			Repacking should indeed occur, but that can only be done on the master ... so rpl time. 
			We need to pass the inventorymanager along with the storages and mags involved to SESOF_SCR_PlayerController.c
			where we can do that.
		--------------------------------------------------------------------------------------------------------------------------------
		*/ 
		IEntity owner = m_InventoryManager.GetOwner();

		m_pCallBack.m_pStorageFrom = m_pSelectedSlotUI.GetStorageUI();
		m_pCallBack.m_pStorageTo = m_pFocusedSlotUI.GetStorageUI();

		BaseInventoryStorageComponent fromItemStorageComponent = m_pCallBack.m_pStorageFrom.GetCurrentNavigationStorage();
		BaseInventoryStorageComponent toItemStorageComponent = m_pFocusedSlotUI.GetAsStorage();	
		
		IEntity playerEntity = SCR_PlayerController.GetLocalControlledEntity();
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(playerEntity);
		protected SCR_PlayerController m_PlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		
		RplComponent rpl = RplComponent.Cast(fromMag.GetOwner().FindComponent(RplComponent));
		
		if (rpl.IsMaster())
		{
			m_PlayerController.RepackMags(fromMag, toMag, m_InventoryManager, toItemStorageComponent, fromItemStorageComponent);
		}
		else
		{
			m_PlayerController.Rpc(m_PlayerController.RpcAsk_RepackMags, 
				Replication.FindId(fromMag), 
				Replication.FindId(toMag),
				Replication.FindId(m_InventoryManager),
				Replication.FindId(toItemStorageComponent),
				Replication.FindId(fromItemStorageComponent),
			);
		}
		
		// Repacking complete. Refresh visuals and play sound to indicate success.
		
		m_pCallBack.m_pStorageTo.Refresh();
		m_pCallBack.m_pStorageFrom.Refresh();

		m_InventoryManager.PlayItemSound(toEntity, SCR_SoundEvent.SOUND_PICK_UP);
		
		return true;
	}

}
