modded class SCR_InventoryMenuUI
{
	protected static int s_iACE_Inventory_NextToken;
	protected int m_iACE_Inventory_Quantity;
	protected bool m_bACE_Inventory_Open;
	protected bool m_bACE_Inventory_InputReady;
	protected bool m_bACE_Inventory_ClickQueued;
	protected int m_iACE_Inventory_ClickPriority;
	protected int m_iACE_Inventory_ClickQuantity;
	protected SCR_InventorySlotUI m_pACE_Inventory_ClickSlot;
	protected bool m_bACE_Inventory_NativeMoveQueued;
	protected string m_sACE_Inventory_NativeMoveAction;
	protected SCR_InputButtonComponent m_pACE_Inventory_NativeMoveButton;
	protected SCR_InventoryStorageBaseUI m_pACE_Inventory_NativeMoveParent;
	protected int m_iACE_Inventory_NativeMoveTraverse;
	protected bool m_bACE_Inventory_Busy;
	protected bool m_bACE_Inventory_Waiting;
	protected bool m_bACE_Inventory_Stop;
	protected bool m_bACE_Inventory_AutomaticDestination;
	protected bool m_bACE_Inventory_Dropping;
	protected float m_fACE_Inventory_WaitTime;
	protected int m_iACE_Inventory_Token;
	protected int m_iACE_Inventory_Completed;
	protected int m_iACE_Inventory_Index;
	protected ref array<IEntity> m_aACE_Inventory_Items = {};
	protected ref array<BaseInventoryStorageComponent> m_aACE_Inventory_Sources = {};
	protected ref ACE_Inventory_MoveCallback m_pACE_Inventory_Callback;
	protected BaseInventoryStorageComponent m_pACE_Inventory_Destination;
	protected BaseInventoryStorageComponent m_pACE_Inventory_DestinationRoot;
	protected SCR_InventoryStorageBaseUI m_pACE_Inventory_SourceUI;
	protected SCR_InventoryStorageBaseUI m_pACE_Inventory_DestinationUI;
	protected SCR_ResourcePlayerControllerInventoryComponent m_pACE_Inventory_ArsenalRequester;

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		m_bACE_Inventory_Open = true;
		m_bACE_Inventory_InputReady = GetGame().GetInputManager().ActivateContext("InventoryMenuContext");
		bool quickRegistered;
		bool allRegistered;
		bool largeRegistered;
		for (int i = 0; i < GetGame().GetInputManager().GetActionCount(); i++)
		{
			if (GetGame().GetInputManager().GetActionName(i) == "ACE_Inventory_ModifierSmall")
				quickRegistered = true;
			if (GetGame().GetInputManager().GetActionName(i) == "ACE_Inventory_ModifierAll")
				allRegistered = true;
			if (GetGame().GetInputManager().GetActionName(i) == "ACE_Inventory_ModifierLarge")
				largeRegistered = true;
		}
		m_bACE_Inventory_InputReady = m_bACE_Inventory_InputReady && quickRegistered && allRegistered && largeRegistered;
		if (!m_bACE_Inventory_InputReady)
		{
			ACE_Inventory_Error("Quantity input binding was not loaded. Reload the project after input configuration changes.");
			return;
		}
		if (!ACE_Inventory_Settings.IsValidConfiguration())
		{
			m_bACE_Inventory_InputReady = false;
			ACE_Inventory_Error("Configured batch sizes and arsenal All ceiling must be positive integers.");
			return;
		}
		if (ACE_Inventory_Settings.DEBUG_LOGGING)
			PrintFormat("[ACE Inventory] Enabled: Ctrl+Move for %1, Ctrl+Shift+Move for %2, Shift+Move for All (arsenal ceiling %3).",
				ACE_Inventory_Settings.SMALL_BATCH_SIZE, ACE_Inventory_Settings.LARGE_BATCH_SIZE, ACE_Inventory_Settings.ALL_ARSENAL_LIMIT);
	}

	//------------------------------------------------------------------------------------------------
	override void NavigationBarUpdate()
	{
		super.NavigationBarUpdate();
		if (!m_pNavigationBar)
			return;

		m_pNavigationBar.ACE_Inventory_UpdateHints(m_bACE_Inventory_InputReady && !m_bIsUsingGamepad);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
		if (!m_bACE_Inventory_Busy || m_pACE_Inventory_ArsenalRequester)
			return;

		if (m_bACE_Inventory_Waiting)
		{
			m_fACE_Inventory_WaitTime += tDelta;
			if (!m_bACE_Inventory_Stop && m_fACE_Inventory_WaitTime >= 10)
			{
				m_bACE_Inventory_Stop = true;
				PrintFormat("[ACE Inventory] Move outcome pending after timeout: %1/%2 confirmed. No retry.", m_iACE_Inventory_Completed, m_iACE_Inventory_Quantity, level: LogLevel.WARNING);
			}
			return;
		}

		ACE_Inventory_NextMove();
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Inventory_TryStart(SCR_InventoryStorageBaseUI destinationUI, SCR_InventorySlotUI sourceSlot, BaseInventoryStorageComponent destinationOverride, bool automaticDestination, int quantity)
	{
		if (m_bACE_Inventory_Busy)
		{
			ACE_Inventory_Error("A batch is still pending.");
			return true;
		}

		m_iACE_Inventory_Quantity = quantity;
		if (!ACE_Inventory_Settings.IsSupportedQuantity(m_iACE_Inventory_Quantity))
		{
			ACE_Inventory_Error("Configured batch sizes must be positive integers.");
			return true;
		}

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

		m_aACE_Inventory_Items.Clear();
		m_aACE_Inventory_Sources.Clear();
		if (ACE_Inventory_Settings.DEBUG_LOGGING)
			PrintFormat("[ACE Inventory] Source snapshot: %1 entities; requested quantity=%2 (All=-1).", items.Count(), m_iACE_Inventory_Quantity);
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

			m_aACE_Inventory_Items.Insert(item);
			m_aACE_Inventory_Sources.Insert(source);
			if (m_aACE_Inventory_Items.Count() == m_iACE_Inventory_Quantity)
				break;
		}

		if (m_aACE_Inventory_Items.IsEmpty())
		{
			ACE_Inventory_Error("The source stack is empty.");
			return true;
		}

		if (m_iACE_Inventory_Quantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_iACE_Inventory_Quantity = m_aACE_Inventory_Items.Count();
		ACE_Inventory_Begin(sourceUI, destinationUI, destination, automaticDestination, dropping);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_QuickTransfer(int quantity)
	{
		if (ACE_Inventory_Settings.DEBUG_LOGGING)
			Print("[ACE Inventory] Modifier+right-click detected.");
		if (!m_bACE_Inventory_Open || !m_pFocusedSlotUI || m_DraggedSlot)
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
			if (quantity == ACE_Inventory_Settings.ALL_ITEMS)
			{
				if (destinationUI)
				{
					destination = destinationUI.GetCurrentNavigationStorage();
					if (!destination)
						destination = destinationUI.GetStorage();
				}
			}
			else
			{
				automaticDestination = true;
				if (SCR_ArsenalInventorySlotUI.Cast(m_pFocusedSlotUI))
					destination = m_InventoryManager.FindStorageForResource(m_pFocusedSlotUI.GetItemResource(), EStoragePurpose.PURPOSE_DEPOSIT);
				else
					destination = m_InventoryManager.FindStorageForItem(itemComponent.GetOwner(), EStoragePurpose.PURPOSE_DEPOSIT);
			}
		}

		if (!destinationUI || (!destination && !fromPlayer))
		{
			ACE_Inventory_Error("No compatible opposite inventory container is available.");
			return;
		}

		ACE_Inventory_TryStart(destinationUI, m_pFocusedSlotUI, destination, automaticDestination, quantity);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_CaptureQuantity()
	{
		if (!GetGame().GetInputManager().IsUsingMouseAndKeyboard())
			return;

		int priority;
		int quantity;
		if (GetGame().GetInputManager().GetActionValue("ACE_Inventory_ModifierLarge") > 0)
		{
			priority = 3;
			quantity = ACE_Inventory_Settings.LARGE_BATCH_SIZE;
		}
		else if (GetGame().GetInputManager().GetActionValue("ACE_Inventory_ModifierAll") > 0)
		{
			priority = 2;
			quantity = ACE_Inventory_Settings.ALL_ITEMS;
		}
		else if (GetGame().GetInputManager().GetActionValue("ACE_Inventory_ModifierSmall") > 0)
		{
			priority = 1;
			quantity = ACE_Inventory_Settings.SMALL_BATCH_SIZE;
		}

		if (priority > m_iACE_Inventory_ClickPriority)
		{
			m_iACE_Inventory_ClickPriority = priority;
			m_iACE_Inventory_ClickQuantity = quantity;
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_QueueClickResolution()
	{
		if (m_bACE_Inventory_ClickQueued)
			return;
		m_bACE_Inventory_ClickQueued = true;
		m_pACE_Inventory_ClickSlot = m_pFocusedSlotUI;
		// Capture modifiers now; execute only once after native input dispatch.
		GetGame().GetCallqueue().CallLater(ACE_Inventory_ResolveClick, 0, false);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_ClearClick()
	{
		m_bACE_Inventory_ClickQueued = false;
		m_iACE_Inventory_ClickPriority = 0;
		m_iACE_Inventory_ClickQuantity = 0;
		m_pACE_Inventory_ClickSlot = null;
		m_bACE_Inventory_NativeMoveQueued = false;
		m_sACE_Inventory_NativeMoveAction = string.Empty;
		m_pACE_Inventory_NativeMoveButton = null;
		m_pACE_Inventory_NativeMoveParent = null;
		m_iACE_Inventory_NativeMoveTraverse = -1;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_ResolveClick()
	{
		int priority = m_iACE_Inventory_ClickPriority;
		int quantity = m_iACE_Inventory_ClickQuantity;
		SCR_InventorySlotUI slot = m_pACE_Inventory_ClickSlot;
		bool nativeMove = m_bACE_Inventory_NativeMoveQueued;
		string action = m_sACE_Inventory_NativeMoveAction;
		SCR_InputButtonComponent button = m_pACE_Inventory_NativeMoveButton;
		SCR_InventoryStorageBaseUI parent = m_pACE_Inventory_NativeMoveParent;
		int traverse = m_iACE_Inventory_NativeMoveTraverse;
		ACE_Inventory_ClearClick();

		if (!m_bACE_Inventory_Open)
			return;
		if (priority > 0)
		{
			if (m_bACE_Inventory_Busy)
			{
				ACE_Inventory_Error("A batch is still pending.");
				return;
			}
			if (!slot || slot != m_pFocusedSlotUI)
			{
				ACE_Inventory_Error("Clicked source changed before transfer.");
				return;
			}
			if (ACE_Inventory_Settings.DEBUG_LOGGING)
				PrintFormat("[ACE Inventory] Click resolved: quantity=%1 (All=-1), priority=%2.", quantity, priority);
			ACE_Inventory_QuickTransfer(quantity);
		}
		else if (nativeMove)
		{
			if (slot != m_pFocusedSlotUI)
			{
				ACE_Inventory_Error("Clicked source changed before the native move.");
				return;
			}
			super.OnAction(button, action, parent, traverse);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnAction(SCR_InputButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		if (m_bACE_Inventory_InputReady && (action == "Inventory_Move" || action == "Inventory_Drop") && !m_DraggedSlot)
		{
			m_bACE_Inventory_NativeMoveQueued = true;
			m_sACE_Inventory_NativeMoveAction = action;
			m_pACE_Inventory_NativeMoveButton = comp;
			m_pACE_Inventory_NativeMoveParent = pParentStorage;
			m_iACE_Inventory_NativeMoveTraverse = traverseStorageIndex;
			ACE_Inventory_CaptureQuantity();
			ACE_Inventory_QueueClickResolution();
			return;
		}

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Begin(SCR_InventoryStorageBaseUI sourceUI, SCR_InventoryStorageBaseUI destinationUI, BaseInventoryStorageComponent destination, bool automaticDestination = false, bool dropping = false)
	{
		m_iACE_Inventory_Token = ++s_iACE_Inventory_NextToken;
		m_bACE_Inventory_Busy = true;
		m_bACE_Inventory_Waiting = false;
		m_bACE_Inventory_Stop = false;
		m_iACE_Inventory_Completed = 0;
		m_iACE_Inventory_Index = 0;
		m_pACE_Inventory_SourceUI = sourceUI;
		m_pACE_Inventory_DestinationUI = destinationUI;
		m_pACE_Inventory_Destination = destination;
		m_pACE_Inventory_DestinationRoot = destination;
		m_bACE_Inventory_AutomaticDestination = automaticDestination;
		m_bACE_Inventory_Dropping = dropping;
		if (ACE_Inventory_Settings.DEBUG_LOGGING)
			PrintFormat("[ACE Inventory] Batch %1 started; requested up to %2. Automatic storage=%3, ground drop=%4.",
				m_iACE_Inventory_Token, m_iACE_Inventory_Quantity, automaticDestination, dropping);
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

		if (!resource || !requester || !destination || !Replication.FindItemId(resource).IsValid() || !Replication.FindItemId(destination).IsValid())
		{
			ACE_Inventory_Error("Could not resolve the native arsenal request components.");
			return true;
		}

		int requestedQuantity = m_iACE_Inventory_Quantity;
		if (requestedQuantity == ACE_Inventory_Settings.ALL_ITEMS)
		{
			m_iACE_Inventory_Quantity = ACE_Inventory_Settings.ALL_ARSENAL_LIMIT;
			if (m_iACE_Inventory_Quantity <= 0)
			{
				ACE_Inventory_Error("Configured arsenal All ceiling must be a positive integer.");
				return true;
			}
		}

		ACE_Inventory_Begin(sourceUI, destinationUI, destination, automaticDestination);
		m_pACE_Inventory_ArsenalRequester = requester;
		requester.ACE_Inventory_GetResult().Insert(ACE_Inventory_OnArsenalResult);
		requester.ACE_Inventory_RequestBatch(m_iACE_Inventory_Token, Replication.FindItemId(resource), Replication.FindItemId(destination), slot.GetItemResource(), automaticDestination, requestedQuantity);
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
		BaseInventoryStorageComponent source;
		if (sourceUI)
			source = sourceUI.GetStorage();
		array<IEntity> items = {};
		if (!resource || !requester || !Replication.FindItemId(resource).IsValid()
			|| !sourceUI || !source || !Replication.FindItemId(source).IsValid() || !sourceUI.ACE_Inventory_GetStack(slot, items))
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
			if (ids.Count() == m_iACE_Inventory_Quantity)
				break;
		}

		if (ids.IsEmpty())
		{
			ACE_Inventory_Error("No source items are available to refund.");
			return true;
		}

		int requestedQuantity = m_iACE_Inventory_Quantity;
		if (requestedQuantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_iACE_Inventory_Quantity = ids.Count();
		ACE_Inventory_Begin(sourceUI, destinationUI, destination);
		m_pACE_Inventory_ArsenalRequester = requester;
		requester.ACE_Inventory_GetResult().Insert(ACE_Inventory_OnArsenalResult);
		requester.ACE_Inventory_RequestRefund(m_iACE_Inventory_Token, Replication.FindItemId(resource), Replication.FindItemId(source), ids, requestedQuantity);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_NextMove()
	{
		if (m_bACE_Inventory_Stop || m_iACE_Inventory_Index == m_aACE_Inventory_Items.Count())
		{
			ACE_Inventory_Finish("Finished.");
			return;
		}

		IEntity item = m_aACE_Inventory_Items[m_iACE_Inventory_Index];
		InventoryItemComponent component;
		if (item)
			component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));

		BaseInventoryStorageComponent currentSource;
		if (component && component.GetParentSlot())
			currentSource = component.GetParentSlot().GetStorage();
		BaseInventoryStorageComponent source = m_aACE_Inventory_Sources[m_iACE_Inventory_Index];
		if (!component || currentSource != source)
		{
			ACE_Inventory_Finish("Source changed before the next move.");
			return;
		}

		if (m_InventoryManager && m_bACE_Inventory_AutomaticDestination && !m_bACE_Inventory_Dropping)
			m_pACE_Inventory_Destination = m_InventoryManager.FindStorageForItem(item, EStoragePurpose.PURPOSE_DEPOSIT);
		else if (m_InventoryManager && m_pACE_Inventory_DestinationRoot)
			m_pACE_Inventory_Destination = m_InventoryManager.FindStorageForInsert(item, m_pACE_Inventory_DestinationRoot, EStoragePurpose.PURPOSE_DEPOSIT);

		bool permitted;
		if (m_InventoryManager && m_InventoryManager.CanMoveItem(item))
		{
			if (m_bACE_Inventory_Dropping)
				permitted = source && m_InventoryManager.CanRemoveItemFromStorage(item, source);
			else if (m_pACE_Inventory_Destination && !IsStorageArsenal(m_pACE_Inventory_Destination)
				&& !m_pACE_Inventory_Destination.IsInherited(CharacterInventoryStorageComponent)
				&& !m_pACE_Inventory_Destination.IsInherited(EquipedWeaponStorageComponent)
				&& !m_pACE_Inventory_Destination.IsInherited(SCR_WeaponAttachmentsStorageComponent)
				&& !m_pACE_Inventory_Destination.IsInherited(SCR_HandSlotStorageComponent))
			{
				if (source)
					permitted = m_InventoryManager.CanMoveItemToStorage(item, m_pACE_Inventory_Destination);
				else
					permitted = m_InventoryManager.CanInsertItemInStorage(item, m_pACE_Inventory_Destination);
			}
		}
		if (!GetCanInteract() || !permitted)
		{
			ACE_Inventory_Finish("Destination unavailable, incompatible or full.");
			return;
		}

		m_fACE_Inventory_WaitTime = 0;
		m_bACE_Inventory_Waiting = true;
		m_pACE_Inventory_Callback = new ACE_Inventory_MoveCallback(this, m_iACE_Inventory_Token, m_iACE_Inventory_Index);
		bool accepted;
		if (m_bACE_Inventory_Dropping)
			accepted = m_InventoryManager.TryRemoveItemFromInventory(item, source, m_pACE_Inventory_Callback);
		else if (source)
			accepted = m_InventoryManager.TryMoveItemToStorage(item, m_pACE_Inventory_Destination, -1, m_pACE_Inventory_Callback);
		else
			accepted = m_InventoryManager.TryInsertItemInStorage(item, m_pACE_Inventory_Destination, -1, m_pACE_Inventory_Callback);

		if (!accepted)
		{
			m_bACE_Inventory_Waiting = false;
			ACE_Inventory_Finish("Native move request rejected.");
		}
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_OnMoveResult(int token, int operationIndex, bool success)
	{
		if (token != m_iACE_Inventory_Token || operationIndex != m_iACE_Inventory_Index || !m_bACE_Inventory_Busy || !m_bACE_Inventory_Waiting)
			return;

		m_bACE_Inventory_Waiting = false;
		IEntity item = m_aACE_Inventory_Items[m_iACE_Inventory_Index];
		InventoryItemComponent component;
		if (item)
			component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));

		bool membershipConfirmed;
		if (component)
		{
			if (m_bACE_Inventory_Dropping)
				membershipConfirmed = component.GetParentSlot() == null;
			else if (component.GetParentSlot())
				membershipConfirmed = component.GetParentSlot().GetStorage() == m_pACE_Inventory_Destination;
		}
		if (!success || !membershipConfirmed)
		{
			ACE_Inventory_Finish("Native move failed or destination membership was not confirmed.");
			return;
		}

		m_iACE_Inventory_Completed++;
		m_iACE_Inventory_Index++;
		if (ACE_Inventory_Settings.DEBUG_LOGGING)
			PrintFormat("[ACE Inventory] Batch %1: %2/%3 moves confirmed.", token, m_iACE_Inventory_Completed, m_iACE_Inventory_Quantity);
		if (!m_bACE_Inventory_Open || m_bACE_Inventory_Stop)
			ACE_Inventory_Finish("Stopped; the outstanding move completed.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_OnArsenalResult(int token, int completed, string reason, bool finished)
	{
		if (token != m_iACE_Inventory_Token || !m_bACE_Inventory_Busy)
			return;

		m_iACE_Inventory_Completed = completed;
		if (ACE_Inventory_Settings.DEBUG_LOGGING || (!finished && reason.Contains("pending")))
			PrintFormat("[ACE Inventory] Batch %1: %2/%3 arsenal items confirmed. %4", token, completed, m_iACE_Inventory_Quantity, reason);
		if (finished)
			ACE_Inventory_Finish(reason);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Finish(string reason)
	{
		PrintFormat("[ACE Inventory] Batch %1 ended: %2/%3 confirmed. %4", m_iACE_Inventory_Token, m_iACE_Inventory_Completed, m_iACE_Inventory_Quantity, reason);
		if (m_pACE_Inventory_ArsenalRequester)
			m_pACE_Inventory_ArsenalRequester.ACE_Inventory_GetResult().Remove(ACE_Inventory_OnArsenalResult);

		m_pACE_Inventory_ArsenalRequester = null;
		m_bACE_Inventory_Busy = false;
		m_bACE_Inventory_Waiting = false;
		m_pACE_Inventory_Callback = null;
		m_aACE_Inventory_Items.Clear();
		m_aACE_Inventory_Sources.Clear();
		m_pACE_Inventory_Destination = null;
		m_pACE_Inventory_DestinationRoot = null;
		if (m_bACE_Inventory_Open)
		{
			RefreshLootUIListener();
			if (m_pACE_Inventory_SourceUI)
				m_pACE_Inventory_SourceUI.Refresh();
			if (m_pACE_Inventory_DestinationUI)
				m_pACE_Inventory_DestinationUI.Refresh();
			if (m_iACE_Inventory_Completed == 0)
				SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_INV_DROP_ERROR);
		}
		m_pACE_Inventory_SourceUI = null;
		m_pACE_Inventory_DestinationUI = null;
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
		m_bACE_Inventory_Open = false;
		m_bACE_Inventory_Stop = true;
		if (m_pACE_Inventory_ArsenalRequester)
		{
			m_pACE_Inventory_ArsenalRequester.ACE_Inventory_Cancel(m_iACE_Inventory_Token);
			if (m_bACE_Inventory_Busy)
				ACE_Inventory_Finish("Inventory closed; server cancellation requested, pending operation may still settle.");
		}
		else if (m_bACE_Inventory_Busy && !m_bACE_Inventory_Waiting)
			ACE_Inventory_Finish("Inventory closed; cancelled before the next move.");

		if (m_bACE_Inventory_Busy)
			PrintFormat("[ACE Inventory] Menu closed: %1/%2 confirmed; any outstanding operation may still finish.", m_iACE_Inventory_Completed, m_iACE_Inventory_Quantity);

		GetGame().GetCallqueue().Remove(ACE_Inventory_ResolveClick);
		ACE_Inventory_ClearClick();
		m_bACE_Inventory_InputReady = false;
		super.OnMenuClose();
		m_pACE_Inventory_SourceUI = null;
		m_pACE_Inventory_DestinationUI = null;
	}
}
