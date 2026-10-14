modded class SCR_InventoryMenuUI
{
	protected static int s_ACE_Inventory_NextToken;
	protected int m_ACE_Inventory_DragQuantity;
	protected int m_ACE_Inventory_Quantity;
	protected bool m_ACE_Inventory_Open;
	protected bool m_ACE_Inventory_InputReady;
	protected bool m_ACE_Inventory_ControlHeld;
	protected bool m_ACE_Inventory_ShiftHeld;
	protected bool m_ACE_Inventory_Busy;
	protected bool m_ACE_Inventory_Waiting;
	protected bool m_ACE_Inventory_Stop;
	protected bool m_ACE_Inventory_AutomaticDestination;
	protected bool m_ACE_Inventory_Dropping;
	protected float m_ACE_Inventory_WaitTime;
	protected int m_ACE_Inventory_Token;
	protected int m_ACE_Inventory_Completed;
	protected int m_ACE_Inventory_Index;
	protected ref array<IEntity> m_ACE_Inventory_Items = {};
	protected ref array<BaseInventoryStorageComponent> m_ACE_Inventory_Sources = {};
	protected ref ACE_Inventory_MoveCallback m_ACE_Inventory_Callback;
	protected BaseInventoryStorageComponent m_ACE_Inventory_Destination;
	protected BaseInventoryStorageComponent m_ACE_Inventory_DestinationRoot;
	protected SCR_InventoryStorageBaseUI m_ACE_Inventory_SourceUI;
	protected SCR_InventoryStorageBaseUI m_ACE_Inventory_DestinationUI;
	protected SCR_ResourcePlayerControllerInventoryComponent m_ACE_Inventory_ArsenalRequester;

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		m_ACE_Inventory_Open = true;
		m_ACE_Inventory_InputReady = GetGame().GetInputManager().ActivateContext("InventoryMenuContext");
		bool quickRegistered;
		bool shiftRegistered;
		for (int i = 0; i < GetGame().GetInputManager().GetActionCount(); i++)
		{
			if (GetGame().GetInputManager().GetActionName(i) == "ACE_Inventory_QuickFive")
				quickRegistered = true;
			if (GetGame().GetInputManager().GetActionName(i) == "ACE_Inventory_Shift")
				shiftRegistered = true;
		}
		m_ACE_Inventory_InputReady = m_ACE_Inventory_InputReady && quickRegistered && shiftRegistered;
		if (!m_ACE_Inventory_InputReady)
			ACE_Inventory_Error("Quantity input binding was not loaded. Reload the project after input configuration changes.");
		GetGame().GetInputManager().AddActionListener("ACE_Inventory_QuickFive", EActionTrigger.DOWN, ACE_Inventory_QuickTransfer);
		GetGame().GetInputManager().AddActionListener("ACE_Inventory_Five", EActionTrigger.DOWN, ACE_Inventory_ControlDown);
		GetGame().GetInputManager().AddActionListener("ACE_Inventory_Five", EActionTrigger.UP, ACE_Inventory_ControlUp);
		GetGame().GetInputManager().AddActionListener("ACE_Inventory_Shift", EActionTrigger.DOWN, ACE_Inventory_ShiftDown);
		GetGame().GetInputManager().AddActionListener("ACE_Inventory_Shift", EActionTrigger.UP, ACE_Inventory_ShiftUp);
		PrintFormat("[ACE Inventory] Enabled: Ctrl+right-click for %1, Ctrl+Shift+right-click for %2, Shift+right-click for All (arsenal ceiling %3); drag also captures quantity.",
			ACE_Inventory_Settings.SMALL_BATCH_SIZE, ACE_Inventory_Settings.LARGE_BATCH_SIZE, ACE_Inventory_Settings.ALL_ARSENAL_LIMIT);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
		if (!m_ACE_Inventory_Busy || m_ACE_Inventory_ArsenalRequester)
			return;

		if (m_ACE_Inventory_Waiting)
		{
			m_ACE_Inventory_WaitTime += tDelta;
			if (!m_ACE_Inventory_Stop && m_ACE_Inventory_WaitTime >= 10)
			{
				m_ACE_Inventory_Stop = true;
				PrintFormat("[ACE Inventory] Move outcome pending after timeout: %1/%2 confirmed. No retry.", m_ACE_Inventory_Completed, m_ACE_Inventory_Quantity, level: LogLevel.WARNING);
			}
			return;
		}

		ACE_Inventory_NextMove();
	}

	//------------------------------------------------------------------------------------------------
	override void Action_DragDown()
	{
		if (m_ACE_Inventory_Busy)
		{
			ACE_Inventory_Error("A batch is still pending.");
			return;
		}

		if (!m_ACE_Inventory_InputReady)
		{
			ACE_Inventory_Error("Dragging blocked: quantity input context is unavailable.");
			return;
		}

		m_ACE_Inventory_DragQuantity = 0;
		if (m_ACE_Inventory_ControlHeld || m_ACE_Inventory_ShiftHeld)
		{
			m_ACE_Inventory_DragQuantity = ACE_Inventory_GetHeldQuantity();
			if (!ACE_Inventory_Settings.IsSupportedQuantity(m_ACE_Inventory_DragQuantity))
			{
				ACE_Inventory_Error("Configured batch sizes must be positive integers.");
				return;
			}
		}
		PrintFormat("[ACE Inventory] Drag start: quantity=%1, context=%2.", m_ACE_Inventory_DragQuantity,
			GetGame().GetInputManager().IsContextActive("InventoryMenuContext"));
		super.Action_DragDown();
	}

	//------------------------------------------------------------------------------------------------
	override void Action_DragUp()
	{
		super.Action_DragUp();
		m_ACE_Inventory_DragQuantity = 0;
	}

	//------------------------------------------------------------------------------------------------
	override void MoveItem(SCR_InventoryStorageBaseUI pStorageBaseUI = null)
	{
		SCR_InventoryStorageBaseUI storageUI = pStorageBaseUI;
		if (!storageUI)
			storageUI = m_pActiveHoveredStorageUI;

		if (!ACE_Inventory_TryStart(storageUI))
			super.MoveItem(pStorageBaseUI);
	}

	//------------------------------------------------------------------------------------------------
	override void MoveItemToStorageSlot()
	{
		SCR_InventoryStorageBaseUI storageUI;
		if (m_pFocusedSlotUI)
			storageUI = m_pFocusedSlotUI.GetStorageUI();

		if (!ACE_Inventory_TryStart(storageUI))
			super.MoveItemToStorageSlot();
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Inventory_TryStart(SCR_InventoryStorageBaseUI destinationUI, SCR_InventorySlotUI sourceSlot = null, bool quick = false, BaseInventoryStorageComponent destinationOverride = null, bool automaticDestination = false)
	{
		if (m_ACE_Inventory_Busy)
		{
			ACE_Inventory_Error("A batch is still pending.");
			return true;
		}

		if (!quick && (m_ACE_Inventory_DragQuantity == 0 || !m_DraggedSlot))
		{
			PrintFormat("[ACE Inventory] Single-item path: quantity=%1, draggedSlot=%2.", m_ACE_Inventory_DragQuantity, m_DraggedSlot != null);
			return false;
		}

		m_ACE_Inventory_Quantity = m_ACE_Inventory_DragQuantity;
		if (quick)
			m_ACE_Inventory_Quantity = ACE_Inventory_GetHeldQuantity();
		if (!ACE_Inventory_Settings.IsSupportedQuantity(m_ACE_Inventory_Quantity))
		{
			ACE_Inventory_Error("Configured batch sizes must be positive integers.");
			return true;
		}

		if (!sourceSlot)
			sourceSlot = m_pSelectedSlotUI;

		if (!GetCanInteract() || !sourceSlot || !destinationUI)
		{
			ACE_Inventory_Error("Missing source/destination or interaction is unavailable.");
			return true;
		}

		if (SCR_SupplyInventorySlotUI.Cast(sourceSlot))
		{
			ACE_Inventory_Error("Direct supplies-stack transfers are outside this addon.");
			return true;
		}

		BaseInventoryStorageComponent destination = destinationOverride;
		if (!destination)
			destination = destinationUI.GetCurrentNavigationStorage();
		if (destinationUI.IsArsenal() || (destination && IsStorageArsenal(destination)))
		{
			if (!destination)
				destination = destinationUI.GetStorage();
			return ACE_Inventory_StartRefund(sourceSlot, destinationUI, destination);
		}

		bool dropping = !destination && destinationUI == m_pStorageLootUI;
		if ((!destination && !dropping) || (destination && (IsStorageArsenal(destination) || destination.IsInherited(CharacterInventoryStorageComponent)
			|| destination.IsInherited(EquipedWeaponStorageComponent) || destination.IsInherited(SCR_WeaponAttachmentsStorageComponent)
			|| destination.IsInherited(SCR_HandSlotStorageComponent))))
		{
			ACE_Inventory_Error("An ordinary destination container is required; equipping is not supported.");
			return true;
		}

		SCR_InventoryStorageBaseUI sourceUI = sourceSlot.GetStorageUI();
		if (!sourceUI || sourceUI == destinationUI)
		{
			ACE_Inventory_Error("Choose a different destination container.");
			return true;
		}

		SCR_ArsenalInventorySlotUI arsenalSlot = SCR_ArsenalInventorySlotUI.Cast(sourceSlot);
		if (arsenalSlot)
		{
			if (dropping)
			{
				ACE_Inventory_Error("Acquire arsenal items into inventory before dropping them.");
				return true;
			}
			return ACE_Inventory_StartArsenal(arsenalSlot, sourceUI, destinationUI, destination, automaticDestination);
		}

		array<IEntity> items = {};
		if (!sourceUI.ACE_Inventory_GetStack(sourceSlot, items))
		{
			ACE_Inventory_Error("Could not snapshot the selected stack.");
			return true;
		}

		m_ACE_Inventory_Items.Clear();
		m_ACE_Inventory_Sources.Clear();
		foreach (IEntity item : items)
		{
			InventoryItemComponent component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!component)
			{
				ACE_Inventory_Error("A source item no longer has an inventory component.");
				return true;
			}

			BaseInventoryStorageComponent source;
			if (component.GetParentSlot())
				source = component.GetParentSlot().GetStorage();

			if (!source && (sourceUI != m_pStorageLootUI || sourceUI.GetCurrentNavigationStorage() || sourceUI.IsArsenal()))
			{
				ACE_Inventory_Error("Source is neither a stored item nor an ordinary ground item.");
				return true;
			}

			if ((source && (source == destination || IsStorageArsenal(source)
				|| source.IsInherited(SCR_WeaponAttachmentsStorageComponent) || source.IsInherited(SCR_HandSlotStorageComponent)
				|| source.IsInherited(CharacterInventoryStorageComponent) || source.IsInherited(EquipedWeaponStorageComponent)))
				|| (!source && dropping))
			{
				ACE_Inventory_Error("Source storage is unsupported or identical to the destination.");
				return true;
			}

			m_ACE_Inventory_Items.Insert(item);
			m_ACE_Inventory_Sources.Insert(source);
			if (m_ACE_Inventory_Items.Count() == m_ACE_Inventory_Quantity)
				break;
		}

		if (m_ACE_Inventory_Items.IsEmpty())
		{
			ACE_Inventory_Error("The source stack is empty.");
			return true;
		}

		if (m_ACE_Inventory_Quantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_ACE_Inventory_Quantity = m_ACE_Inventory_Items.Count();
		ACE_Inventory_Begin(sourceUI, destinationUI, destination, automaticDestination, dropping);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_QuickTransfer()
	{
		Print("[ACE Inventory] Modifier+right-click detected.");
		if (!m_ACE_Inventory_Open || !m_pFocusedSlotUI || m_DraggedSlot)
			return;

		if (WidgetManager.GetWidgetUnderCursor() != m_pFocusedSlotUI.GetButtonWidget())
			return;

		PlayerController controller = GetGame().GetPlayerController();
		if (!GetCanInteract() || !controller || !m_InventoryManager)
		{
			ACE_Inventory_Error("Quick transfer unavailable: player inventory cannot be accessed.");
			return;
		}

		InventoryItemComponent itemComponent = m_pFocusedSlotUI.GetInventoryItemComponent();
		if (!itemComponent)
		{
			ACE_Inventory_Error("Clicked slot no longer has an inventory item.");
			return;
		}

		bool fromPlayer;
		InventoryStorageSlot parentSlot = itemComponent.GetParentSlot();
		if (parentSlot)
		{
			IEntity owner = parentSlot.GetOwner();
			IEntity character = controller.GetControlledEntity();
			while (owner)
			{
				if (owner == character)
				{
					fromPlayer = true;
					break;
				}
				owner = owner.GetParent();
			}
		}

		SCR_InventoryStorageBaseUI destinationUI;
		BaseInventoryStorageComponent destination;
		bool automaticDestination;
		if (fromPlayer)
		{
			destinationUI = GetLootStorage();
			if (destinationUI)
				destination = destinationUI.GetCurrentNavigationStorage();
		}
		else
		{
			destinationUI = GetActualStorageInCharacterStorageUI();
			automaticDestination = true;
			if (SCR_ArsenalInventorySlotUI.Cast(m_pFocusedSlotUI))
				destination = m_InventoryManager.FindStorageForResource(m_pFocusedSlotUI.GetItemResource(), EStoragePurpose.PURPOSE_DEPOSIT);
			else
				destination = m_InventoryManager.FindStorageForItem(itemComponent.GetOwner(), EStoragePurpose.PURPOSE_DEPOSIT);
		}

		if (!destinationUI || (!destination && !fromPlayer))
		{
			ACE_Inventory_Error("No compatible opposite inventory container is available.");
			return;
		}

		ACE_Inventory_TryStart(destinationUI, m_pFocusedSlotUI, true, destination, automaticDestination);
	}

	//------------------------------------------------------------------------------------------------
	override void OnAction(SCR_InputButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		if (m_ACE_Inventory_Busy && (action == "Inventory_Select" || action == "Inventory_Move"))
			return;

		if ((m_ACE_Inventory_ControlHeld || m_ACE_Inventory_ShiftHeld) && action == "Inventory_Move")
		{
			ACE_Inventory_QuickTransfer();
			return;
		}

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	}

	//------------------------------------------------------------------------------------------------
	protected int ACE_Inventory_GetHeldQuantity()
	{
		if (m_ACE_Inventory_ControlHeld && m_ACE_Inventory_ShiftHeld)
			return ACE_Inventory_Settings.LARGE_BATCH_SIZE;
		if (m_ACE_Inventory_ShiftHeld)
			return ACE_Inventory_Settings.ALL_ITEMS;
		return ACE_Inventory_Settings.SMALL_BATCH_SIZE;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_ControlDown()
	{
		m_ACE_Inventory_ControlHeld = true;
		Print("[ACE Inventory] Ctrl held.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_ControlUp()
	{
		m_ACE_Inventory_ControlHeld = false;
		Print("[ACE Inventory] Ctrl released.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_ShiftDown()
	{
		m_ACE_Inventory_ShiftHeld = true;
		Print("[ACE Inventory] Shift held.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_ShiftUp()
	{
		m_ACE_Inventory_ShiftHeld = false;
		Print("[ACE Inventory] Shift released.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Begin(SCR_InventoryStorageBaseUI sourceUI, SCR_InventoryStorageBaseUI destinationUI, BaseInventoryStorageComponent destination, bool automaticDestination = false, bool dropping = false)
	{
		m_ACE_Inventory_Token = ++s_ACE_Inventory_NextToken;
		m_ACE_Inventory_Busy = true;
		m_ACE_Inventory_Waiting = false;
		m_ACE_Inventory_Stop = false;
		m_ACE_Inventory_Completed = 0;
		m_ACE_Inventory_Index = 0;
		m_ACE_Inventory_SourceUI = sourceUI;
		m_ACE_Inventory_DestinationUI = destinationUI;
		m_ACE_Inventory_Destination = destination;
		m_ACE_Inventory_DestinationRoot = destination;
		m_ACE_Inventory_AutomaticDestination = automaticDestination;
		m_ACE_Inventory_Dropping = dropping;
		PrintFormat("[ACE Inventory] Batch %1 started; requested up to %2. Automatic storage=%3, ground drop=%4.",
			m_ACE_Inventory_Token, m_ACE_Inventory_Quantity, automaticDestination, dropping);
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Inventory_StartArsenal(SCR_ArsenalInventorySlotUI slot, SCR_InventoryStorageBaseUI sourceUI, SCR_InventoryStorageBaseUI destinationUI, BaseInventoryStorageComponent destination, bool automaticDestination = false)
	{
		IEntity arsenal = SCR_InventoryStorageBaseUI.ARSENAL_SLOT_STORAGES.Get(slot);
		SCR_ResourceComponent resource;
		if (arsenal)
			resource = SCR_ResourceComponent.FindResourceComponent(arsenal);

		PlayerController controller = GetGame().GetPlayerController();
		SCR_ResourcePlayerControllerInventoryComponent requester;
		if (controller)
			requester = SCR_ResourcePlayerControllerInventoryComponent.Cast(controller.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));

		if (destination.IsInherited(ClothNodeStorageComponent))
			destination = m_InventoryManager.FindActualStorageForItemResource(slot.GetItemResource(), destination);

		if (!resource || !requester || !destination || !Replication.FindItemId(resource).IsValid() || !Replication.FindItemId(destination).IsValid())
		{
			ACE_Inventory_Error("Could not resolve the native arsenal request components.");
			return true;
		}

		int requestedQuantity = m_ACE_Inventory_Quantity;
		if (requestedQuantity == ACE_Inventory_Settings.ALL_ITEMS)
		{
			m_ACE_Inventory_Quantity = ACE_Inventory_Settings.ALL_ARSENAL_LIMIT;
			if (m_ACE_Inventory_Quantity <= 0)
			{
				ACE_Inventory_Error("Configured arsenal All ceiling must be a positive integer.");
				return true;
			}
		}

		ACE_Inventory_Begin(sourceUI, destinationUI, destination, automaticDestination);
		m_ACE_Inventory_ArsenalRequester = requester;
		requester.ACE_Inventory_GetResult().Insert(ACE_Inventory_OnArsenalResult);
		requester.ACE_Inventory_RequestBatch(m_ACE_Inventory_Token, Replication.FindItemId(resource), Replication.FindItemId(destination), slot.GetItemResource(), automaticDestination, requestedQuantity);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Inventory_StartRefund(SCR_InventorySlotUI slot, SCR_InventoryStorageBaseUI destinationUI, BaseInventoryStorageComponent destination)
	{
		if (!destination || SCR_ArsenalInventorySlotUI.Cast(slot) || DoesSlotContainNonRefundableItems(slot))
		{
			ACE_Inventory_Error("Arsenal returns require existing refundable inventory items.");
			return true;
		}

		SCR_ResourceComponent resource = SCR_ResourceComponent.FindResourceComponent(destination.GetOwner());
		PlayerController controller = GetGame().GetPlayerController();
		SCR_ResourcePlayerControllerInventoryComponent requester;
		if (controller)
			requester = SCR_ResourcePlayerControllerInventoryComponent.Cast(controller.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));

		SCR_InventoryStorageBaseUI sourceUI = slot.GetStorageUI();
		array<IEntity> items = {};
		if (!resource || !requester || !Replication.FindItemId(resource).IsValid()
			|| !sourceUI || !sourceUI.ACE_Inventory_GetStack(slot, items))
		{
			ACE_Inventory_Error("Could not resolve the arsenal return or source stack.");
			return true;
		}

		array<RplId> ids = {};
		foreach (IEntity item : items)
		{
			InventoryItemComponent component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!component || !component.GetParentSlot() || !Replication.FindItemId(component).IsValid())
			{
				ACE_Inventory_Error("A refund source item is no longer in inventory.");
				return true;
			}
			ids.Insert(Replication.FindItemId(component));
			if (ids.Count() == m_ACE_Inventory_Quantity)
				break;
		}

		if (ids.IsEmpty())
		{
			ACE_Inventory_Error("No source items are available to refund.");
			return true;
		}

		int requestedQuantity = m_ACE_Inventory_Quantity;
		if (requestedQuantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_ACE_Inventory_Quantity = ids.Count();
		ACE_Inventory_Begin(sourceUI, destinationUI, destination);
		m_ACE_Inventory_ArsenalRequester = requester;
		requester.ACE_Inventory_GetResult().Insert(ACE_Inventory_OnArsenalResult);
		requester.ACE_Inventory_RequestRefund(m_ACE_Inventory_Token, Replication.FindItemId(resource), ids, requestedQuantity);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_NextMove()
	{
		if (m_ACE_Inventory_Stop || m_ACE_Inventory_Index == m_ACE_Inventory_Items.Count())
		{
			ACE_Inventory_Finish("Finished.");
			return;
		}

		IEntity item = m_ACE_Inventory_Items[m_ACE_Inventory_Index];
		InventoryItemComponent component;
		if (item)
			component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));

		BaseInventoryStorageComponent currentSource;
		if (component && component.GetParentSlot())
			currentSource = component.GetParentSlot().GetStorage();
		BaseInventoryStorageComponent source = m_ACE_Inventory_Sources[m_ACE_Inventory_Index];
		if (!component || currentSource != source)
		{
			ACE_Inventory_Finish("Source changed before the next move.");
			return;
		}

		if (m_InventoryManager && m_ACE_Inventory_AutomaticDestination && !m_ACE_Inventory_Dropping)
			m_ACE_Inventory_Destination = m_InventoryManager.FindStorageForItem(item, EStoragePurpose.PURPOSE_DEPOSIT);
		else if (m_InventoryManager && m_ACE_Inventory_DestinationRoot && m_ACE_Inventory_DestinationRoot.IsInherited(ClothNodeStorageComponent))
			m_ACE_Inventory_Destination = m_InventoryManager.FindStorageForInsert(item, m_ACE_Inventory_DestinationRoot, EStoragePurpose.PURPOSE_ANY);

		bool permitted;
		if (m_InventoryManager && m_InventoryManager.CanMoveItem(item))
		{
			if (m_ACE_Inventory_Dropping)
				permitted = source && m_InventoryManager.CanRemoveItemFromStorage(item, source);
			else if (m_ACE_Inventory_Destination && !IsStorageArsenal(m_ACE_Inventory_Destination)
				&& !m_ACE_Inventory_Destination.IsInherited(CharacterInventoryStorageComponent)
				&& !m_ACE_Inventory_Destination.IsInherited(EquipedWeaponStorageComponent)
				&& !m_ACE_Inventory_Destination.IsInherited(SCR_WeaponAttachmentsStorageComponent)
				&& !m_ACE_Inventory_Destination.IsInherited(SCR_HandSlotStorageComponent))
			{
				if (source)
					permitted = m_InventoryManager.CanMoveItemToStorage(item, m_ACE_Inventory_Destination);
				else
					permitted = m_InventoryManager.CanInsertItemInStorage(item, m_ACE_Inventory_Destination);
			}
		}
		if (!GetCanInteract() || !permitted)
		{
			ACE_Inventory_Finish("Destination unavailable, incompatible or full.");
			return;
		}

		m_ACE_Inventory_WaitTime = 0;
		m_ACE_Inventory_Waiting = true;
		m_ACE_Inventory_Callback = new ACE_Inventory_MoveCallback(this, m_ACE_Inventory_Token);
		bool accepted;
		if (m_ACE_Inventory_Dropping)
			accepted = m_InventoryManager.TryRemoveItemFromInventory(item, source, m_ACE_Inventory_Callback);
		else if (source)
			accepted = m_InventoryManager.TryMoveItemToStorage(item, m_ACE_Inventory_Destination, -1, m_ACE_Inventory_Callback);
		else
			accepted = m_InventoryManager.TryInsertItemInStorage(item, m_ACE_Inventory_Destination, -1, m_ACE_Inventory_Callback);

		if (!accepted)
		{
			m_ACE_Inventory_Waiting = false;
			ACE_Inventory_Finish("Native move request rejected.");
		}
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_OnMoveResult(int token, bool success)
	{
		if (token != m_ACE_Inventory_Token || !m_ACE_Inventory_Busy || !m_ACE_Inventory_Waiting)
			return;

		m_ACE_Inventory_Waiting = false;
		IEntity item = m_ACE_Inventory_Items[m_ACE_Inventory_Index];
		InventoryItemComponent component;
		if (item)
			component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));

		bool membershipConfirmed;
		if (component)
		{
			if (m_ACE_Inventory_Dropping)
				membershipConfirmed = component.GetParentSlot() == null;
			else if (component.GetParentSlot())
				membershipConfirmed = component.GetParentSlot().GetStorage() == m_ACE_Inventory_Destination;
		}
		if (!success || !membershipConfirmed)
		{
			ACE_Inventory_Finish("Native move failed or destination membership was not confirmed.");
			return;
		}

		m_ACE_Inventory_Completed++;
		m_ACE_Inventory_Index++;
		PrintFormat("[ACE Inventory] Batch %1: %2/%3 moves confirmed.", token, m_ACE_Inventory_Completed, m_ACE_Inventory_Quantity);
		if (!m_ACE_Inventory_Open || m_ACE_Inventory_Stop)
			ACE_Inventory_Finish("Stopped; the outstanding move completed.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_OnArsenalResult(int token, int completed, string reason, bool finished)
	{
		if (token != m_ACE_Inventory_Token || !m_ACE_Inventory_Busy)
			return;

		m_ACE_Inventory_Completed = completed;
		PrintFormat("[ACE Inventory] Batch %1: %2/%3 arsenal items confirmed. %4", token, completed, m_ACE_Inventory_Quantity, reason);
		if (finished)
			ACE_Inventory_Finish(reason);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Finish(string reason)
	{
		PrintFormat("[ACE Inventory] Batch %1 ended: %2/%3 confirmed. %4", m_ACE_Inventory_Token, m_ACE_Inventory_Completed, m_ACE_Inventory_Quantity, reason);
		if (m_ACE_Inventory_ArsenalRequester)
			m_ACE_Inventory_ArsenalRequester.ACE_Inventory_GetResult().Remove(ACE_Inventory_OnArsenalResult);

		m_ACE_Inventory_ArsenalRequester = null;
		m_ACE_Inventory_Busy = false;
		m_ACE_Inventory_Waiting = false;
		m_ACE_Inventory_Callback = null;
		m_ACE_Inventory_Items.Clear();
		m_ACE_Inventory_Sources.Clear();
		m_ACE_Inventory_Destination = null;
		m_ACE_Inventory_DestinationRoot = null;
		if (m_ACE_Inventory_Open)
		{
			RefreshLootUIListener();
			if (m_ACE_Inventory_SourceUI)
				m_ACE_Inventory_SourceUI.Refresh();
			if (m_ACE_Inventory_DestinationUI)
				m_ACE_Inventory_DestinationUI.Refresh();
			if (m_ACE_Inventory_Completed == 0)
				SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_DROP_ERROR);
		}
		m_ACE_Inventory_SourceUI = null;
		m_ACE_Inventory_DestinationUI = null;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Error(string reason)
	{
		Print("[ACE Inventory] " + reason, LogLevel.WARNING);
		SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_DROP_ERROR);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		m_ACE_Inventory_Open = false;
		m_ACE_Inventory_Stop = true;
		m_ACE_Inventory_DragQuantity = 0;
		m_ACE_Inventory_ControlHeld = false;
		m_ACE_Inventory_ShiftHeld = false;
		if (m_ACE_Inventory_ArsenalRequester)
		{
			m_ACE_Inventory_ArsenalRequester.ACE_Inventory_GetResult().Remove(ACE_Inventory_OnArsenalResult);
			m_ACE_Inventory_ArsenalRequester.ACE_Inventory_Cancel(m_ACE_Inventory_Token);
			m_ACE_Inventory_ArsenalRequester = null;
		}

		if (m_ACE_Inventory_Busy)
			PrintFormat("[ACE Inventory] Menu closed: %1/%2 confirmed; any outstanding operation may still finish.", m_ACE_Inventory_Completed, m_ACE_Inventory_Quantity);

		GetGame().GetInputManager().RemoveActionListener("ACE_Inventory_QuickFive", EActionTrigger.DOWN, ACE_Inventory_QuickTransfer);
		GetGame().GetInputManager().RemoveActionListener("ACE_Inventory_Five", EActionTrigger.DOWN, ACE_Inventory_ControlDown);
		GetGame().GetInputManager().RemoveActionListener("ACE_Inventory_Five", EActionTrigger.UP, ACE_Inventory_ControlUp);
		GetGame().GetInputManager().RemoveActionListener("ACE_Inventory_Shift", EActionTrigger.DOWN, ACE_Inventory_ShiftDown);
		GetGame().GetInputManager().RemoveActionListener("ACE_Inventory_Shift", EActionTrigger.UP, ACE_Inventory_ShiftUp);
		super.OnMenuClose();
		m_ACE_Inventory_SourceUI = null;
		m_ACE_Inventory_DestinationUI = null;
	}
}
