modded class SCR_ResourcePlayerControllerInventoryComponent
{
	protected static SCR_ResourcePlayerControllerInventoryComponent s_pACE_Inventory_Dispatch;
	protected static SCR_ResourcePlayerControllerInventoryComponent s_pACE_Inventory_RefundDispatch;
	protected ref ScriptInvoker m_pACE_Inventory_Result = new ScriptInvoker();
	protected int m_iACE_Inventory_Token;
	protected int m_iACE_Inventory_Operation;
	protected int m_iACE_Inventory_Completed;
	protected int m_iACE_Inventory_Quantity;
	protected bool m_bACE_Inventory_Waiting;
	protected bool m_bACE_Inventory_Captured;
	protected bool m_bACE_Inventory_Cancelled;
	protected bool m_bACE_Inventory_AutomaticDestination;
	protected bool m_bACE_Inventory_Refunding;
	protected bool m_bACE_Inventory_RefundAccepted;
	protected IEntity m_pACE_Inventory_RefundItem;
	protected SCR_ArsenalComponent m_pACE_Inventory_RefundArsenal;
	protected ref array<RplId> m_aACE_Inventory_RefundItems = {};
	protected ref array<BaseInventoryStorageComponent> m_aACE_Inventory_RefundSources = {};
	protected RplId m_ACE_Inventory_Arsenal;
	protected BaseInventoryStorageComponent m_pACE_Inventory_Destination;
	protected BaseInventoryStorageComponent m_pACE_Inventory_DestinationRoot;
	protected ResourceName m_sACE_Inventory_Resource;

	//------------------------------------------------------------------------------------------------
	static SCR_ResourcePlayerControllerInventoryComponent ACE_Inventory_GetDispatch()
	{
		return s_pACE_Inventory_Dispatch;
	}

	//------------------------------------------------------------------------------------------------
	static SCR_ResourcePlayerControllerInventoryComponent ACE_Inventory_GetRefundDispatch()
	{
		return s_pACE_Inventory_RefundDispatch;
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvoker ACE_Inventory_GetResult()
	{
		return m_pACE_Inventory_Result;
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_RequestBatch(int token, RplId arsenal, RplId destination, ResourceName resource, bool automaticDestination, int quantity)
	{
		Rpc(ACE_Inventory_RpcAsk_Batch, token, arsenal, destination, resource, automaticDestination, quantity);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_Cancel(int token)
	{
		Rpc(ACE_Inventory_RpcAsk_Cancel, token);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_RequestRefund(int token, RplId arsenal, RplId source, array<RplId> items, int quantity)
	{
		Rpc(ACE_Inventory_RpcAsk_Refund, token, arsenal, source, items, quantity);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void ACE_Inventory_RpcAsk_Refund(int token, RplId arsenal, RplId source, array<RplId> items, int quantity)
	{
		if (m_iACE_Inventory_Token > 0)
		{
			Print("[ACE Inventory] Rejected overlapping refund batch.", LogLevel.WARNING);
			if (token != m_iACE_Inventory_Token)
				Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Another arsenal operation is still pending.", true);
			return;
		}

		if (token <= 0 || !arsenal.IsValid() || !source.IsValid() || !ACE_Inventory_Settings.IsSupportedQuantity(quantity) || !items || items.IsEmpty()
			|| (quantity != ACE_Inventory_Settings.ALL_ITEMS && items.Count() > quantity))
		{
			Print("[ACE Inventory] Invalid refund batch.", LogLevel.WARNING);
			Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Invalid refund batch.", true);
			return;
		}

		BaseInventoryStorageComponent sourceStorage = BaseInventoryStorageComponent.Cast(Replication.FindItem(source));
		InventoryItemComponent representative = InventoryItemComponent.Cast(Replication.FindItem(items[0]));
		array<IEntity> group = {};
		if (representative)
			ACE_Inventory_Stack.GetGroup(sourceStorage, representative.GetOwner(), group);
		if (!sourceStorage || group.IsEmpty() || items.Count() > group.Count())
		{
			Print("[ACE Inventory] Refund source group is unavailable or exceeded.", LogLevel.WARNING);
			Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Refund source group is unavailable or exceeded.", true);
			return;
		}

		m_aACE_Inventory_RefundItems.Clear();
		m_aACE_Inventory_RefundSources.Clear();
		foreach (RplId id : items)
		{
			InventoryItemComponent component = InventoryItemComponent.Cast(Replication.FindItem(id));
			if (!id.IsValid() || !component || !component.GetParentSlot() || m_aACE_Inventory_RefundItems.Find(id) >= 0
				|| group.Find(component.GetOwner()) < 0)
			{
				Print("[ACE Inventory] Refund snapshot changed or contains duplicate items.", LogLevel.WARNING);
				Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Refund snapshot changed or contains duplicate items.", true);
				m_aACE_Inventory_RefundItems.Clear();
				m_aACE_Inventory_RefundSources.Clear();
				return;
			}

			m_aACE_Inventory_RefundItems.Insert(id);
			m_aACE_Inventory_RefundSources.Insert(component.GetParentSlot().GetStorage());
		}

		m_iACE_Inventory_Token = token;
		m_iACE_Inventory_Operation = 0;
		m_iACE_Inventory_Completed = 0;
		m_bACE_Inventory_Cancelled = false;
		m_bACE_Inventory_Refunding = true;
		m_iACE_Inventory_Quantity = quantity;
		if (quantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_iACE_Inventory_Quantity = items.Count();
		m_ACE_Inventory_Arsenal = arsenal;
		ACE_Inventory_Next();
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void ACE_Inventory_RpcAsk_Batch(int token, RplId arsenal, RplId destination, ResourceName resource, bool automaticDestination, int quantity)
	{
		if (m_iACE_Inventory_Token > 0)
		{
			Print("[ACE Inventory] Rejected overlapping arsenal batch.", LogLevel.WARNING);
			if (token != m_iACE_Inventory_Token)
				Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Another arsenal operation is still pending.", true);
			return;
		}

		if (token <= 0 || !arsenal.IsValid() || !destination.IsValid() || resource.IsEmpty() || !ACE_Inventory_Settings.IsSupportedQuantity(quantity)
			|| (quantity == ACE_Inventory_Settings.ALL_ITEMS && ACE_Inventory_Settings.ALL_ARSENAL_LIMIT <= 0))
		{
			Print("[ACE Inventory] Invalid arsenal batch request.", LogLevel.WARNING);
			Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Invalid arsenal request.", true);
			return;
		}

		m_pACE_Inventory_Destination = BaseInventoryStorageComponent.Cast(Replication.FindItem(destination));
		m_pACE_Inventory_DestinationRoot = m_pACE_Inventory_Destination;
		m_iACE_Inventory_Token = token;
		m_iACE_Inventory_Operation = 0;
		m_iACE_Inventory_Completed = 0;
		m_bACE_Inventory_Cancelled = false;
		m_bACE_Inventory_Refunding = false;
		m_iACE_Inventory_Quantity = quantity;
		if (quantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_iACE_Inventory_Quantity = ACE_Inventory_Settings.ALL_ARSENAL_LIMIT;
		m_bACE_Inventory_AutomaticDestination = automaticDestination && quantity != ACE_Inventory_Settings.ALL_ITEMS;
		m_ACE_Inventory_Arsenal = arsenal;
		m_sACE_Inventory_Resource = resource;
		ACE_Inventory_Next();
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Next()
	{
		if (m_iACE_Inventory_Token <= 0)
			return;

		if (m_bACE_Inventory_Cancelled)
		{
			ACE_Inventory_Finish("Cancelled; no further requests issued.");
			return;
		}

		if (m_iACE_Inventory_Completed >= m_iACE_Inventory_Quantity || (m_bACE_Inventory_Refunding && m_iACE_Inventory_Completed == m_aACE_Inventory_RefundItems.Count()))
		{
			ACE_Inventory_Finish("Finished.");
			return;
		}

		PlayerController controller = PlayerController.Cast(GetOwner());
		IEntity character;
		if (controller)
			character = controller.GetControlledEntity();

		SCR_InventoryStorageManagerComponent manager;
		if (character)
			manager = SCR_InventoryStorageManagerComponent.Cast(character.FindComponent(SCR_InventoryStorageManagerComponent));

		if (m_bACE_Inventory_Refunding)
		{
			if (!manager)
				ACE_Inventory_Finish("Player inventory is unavailable.", true);
			else
				ACE_Inventory_NextRefund();
			return;
		}

		if (manager && m_bACE_Inventory_AutomaticDestination)
			m_pACE_Inventory_Destination = manager.FindStorageForResource(m_sACE_Inventory_Resource, EStoragePurpose.PURPOSE_DEPOSIT);
		else if (manager && m_pACE_Inventory_DestinationRoot)
			m_pACE_Inventory_Destination = manager.FindStorageForResourceInsert(m_sACE_Inventory_Resource, m_pACE_Inventory_DestinationRoot, EStoragePurpose.PURPOSE_DEPOSIT);
		else
			m_pACE_Inventory_Destination = null;

		if (!manager || !m_pACE_Inventory_Destination || !manager.CanInsertResourceInStorage(m_sACE_Inventory_Resource, m_pACE_Inventory_Destination))
		{
			ACE_Inventory_Finish("Destination unavailable, incompatible or full.");
			return;
		}

		m_iACE_Inventory_Operation++;
		m_bACE_Inventory_Waiting = true;
		m_bACE_Inventory_Captured = false;
		GetGame().GetCallqueue().CallLater(ACE_Inventory_Timeout, 10000, false, m_iACE_Inventory_Token, m_iACE_Inventory_Operation);

		SCR_ResourcePlayerControllerInventoryComponent previousDispatch = s_pACE_Inventory_Dispatch;
		s_pACE_Inventory_Dispatch = this;
		// Keep native ownership, access, catalog, supply-cost and allocation validation.
		super.RpcAsk_ArsenalRequestItem_(m_ACE_Inventory_Arsenal, Replication.FindItemId(m_pACE_Inventory_Destination), m_sACE_Inventory_Resource, EResourceType.SUPPLIES);
		s_pACE_Inventory_Dispatch = previousDispatch;

		if (m_iACE_Inventory_Token > 0 && !m_bACE_Inventory_Captured)
			ACE_Inventory_Finish("Native arsenal request rejected; check supplies, access and availability.", true);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_CaptureSpawn(BaseInventoryStorageComponent destination, out int token, out int operation)
	{
		token = 0;
		operation = 0;
		if (!m_bACE_Inventory_Waiting || m_bACE_Inventory_Captured || destination != m_pACE_Inventory_Destination)
			return;

		m_bACE_Inventory_Captured = true;
		token = m_iACE_Inventory_Token;
		operation = m_iACE_Inventory_Operation;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ACE_Inventory_IsRefundable(InventoryItemComponent component)
	{
		SCR_ItemAttributeCollection attributes = SCR_ItemAttributeCollection.Cast(component.GetAttributes());
		if (!attributes || !attributes.IsRefundable())
			return false;

		BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(component.GetOwner().FindComponent(BaseInventoryStorageComponent));
		if (!storage)
			return true;

		array<InventoryItemComponent> contents = {};
		storage.GetOwnedItems(contents, true);
		foreach (InventoryItemComponent content : contents)
		{
			attributes = SCR_ItemAttributeCollection.Cast(content.GetAttributes());
			if (!attributes || !attributes.IsRefundable())
				return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_NextRefund()
	{
		InventoryItemComponent component = InventoryItemComponent.Cast(Replication.FindItem(m_aACE_Inventory_RefundItems[m_iACE_Inventory_Completed]));
		SCR_ResourceComponent resource = SCR_ResourceComponent.Cast(Replication.FindItem(m_ACE_Inventory_Arsenal));
		SCR_ArsenalComponent arsenal;
		if (resource)
			arsenal = SCR_ArsenalComponent.FindArsenalComponent(resource.GetOwner());

		if (!arsenal || !arsenal.IsArsenalEnabled() || !component || !component.GetParentSlot()
			|| component.GetParentSlot().GetStorage() != m_aACE_Inventory_RefundSources[m_iACE_Inventory_Completed]
			|| !ACE_Inventory_IsRefundable(component))
		{
			ACE_Inventory_Finish("Arsenal/source changed or item contents are not refundable.", true);
			return;
		}

		m_iACE_Inventory_Operation++;
		m_bACE_Inventory_Waiting = true;
		m_bACE_Inventory_RefundAccepted = false;
		m_pACE_Inventory_RefundItem = component.GetOwner();
		m_pACE_Inventory_RefundArsenal = arsenal;
		GetGame().GetCallqueue().CallLater(ACE_Inventory_Timeout, 10000, false, m_iACE_Inventory_Token, m_iACE_Inventory_Operation);
		SCR_ResourcePlayerControllerInventoryComponent previousDispatch = s_pACE_Inventory_RefundDispatch;
		s_pACE_Inventory_RefundDispatch = this;
		// Native code owns eligibility, distance checks, refund effects and resource credit.
		super.RpcAsk_ArsenalRefundItem_(m_ACE_Inventory_Arsenal, m_aACE_Inventory_RefundItems[m_iACE_Inventory_Completed], EResourceType.SUPPLIES);
		s_pACE_Inventory_RefundDispatch = previousDispatch;
		if (m_iACE_Inventory_Token <= 0)
			return;
		if (!m_bACE_Inventory_RefundAccepted)
		{
			m_bACE_Inventory_Waiting = false;
			ACE_Inventory_Finish("Native refund request rejected; no further returns issued.", true);
			return;
		}
		GetGame().GetCallqueue().CallLater(ACE_Inventory_CheckRefund, 0, false);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_CaptureRefundAcceptance(IEntity item, PlayerController playerController, SCR_ArsenalComponent arsenal)
	{
		if (m_bACE_Inventory_Refunding && m_bACE_Inventory_Waiting
			&& item == m_pACE_Inventory_RefundItem && playerController == PlayerController.Cast(GetOwner())
			&& arsenal == m_pACE_Inventory_RefundArsenal)
			m_bACE_Inventory_RefundAccepted = true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_CheckRefund()
	{
		if (!m_bACE_Inventory_Refunding || !m_bACE_Inventory_RefundAccepted || !m_bACE_Inventory_Waiting || m_iACE_Inventory_Token <= 0)
			return;

		if (!m_pACE_Inventory_RefundItem || m_pACE_Inventory_RefundItem.IsDeleted())
		{
			GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
			m_bACE_Inventory_Waiting = false;
			m_iACE_Inventory_Completed++;
			Rpc(ACE_Inventory_RpcDo_Result, m_iACE_Inventory_Token, m_iACE_Inventory_Completed, "Native refund accepted and item removal confirmed.", false);
			GetGame().GetCallqueue().CallLater(ACE_Inventory_Next, 0, false);
			return;
		}

		int delay = 50;
		if (m_bACE_Inventory_Cancelled)
			delay = 500;
		GetGame().GetCallqueue().CallLater(ACE_Inventory_CheckRefund, delay, false);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_OnSpawnResult(int token, int operation, bool success)
	{
		if (token != m_iACE_Inventory_Token || operation != m_iACE_Inventory_Operation || !m_bACE_Inventory_Waiting)
			return;

		GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
		m_bACE_Inventory_Waiting = false;
		if (!success)
		{
			ACE_Inventory_Finish("Native item insertion failed.", true);
			return;
		}

		m_iACE_Inventory_Completed++;
		Rpc(ACE_Inventory_RpcDo_Result, token, m_iACE_Inventory_Completed, "Native spawn completed.", false);
		GetGame().GetCallqueue().CallLater(ACE_Inventory_Next, 0, false);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Timeout(int token, int operation)
	{
		if (token != m_iACE_Inventory_Token || operation != m_iACE_Inventory_Operation || !m_bACE_Inventory_Waiting)
			return;

		m_bACE_Inventory_Cancelled = true;
		Print("[ACE Inventory] Arsenal operation timed out; no further requests will be issued. The pending outcome is still monitored.", LogLevel.WARNING);
		Rpc(ACE_Inventory_RpcDo_Result, m_iACE_Inventory_Token, m_iACE_Inventory_Completed, "Outcome pending after timeout; batch stopped without retry.", false);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void ACE_Inventory_RpcAsk_Cancel(int token)
	{
		if (token != m_iACE_Inventory_Token)
			return;

		m_bACE_Inventory_Cancelled = true;
		if (!m_bACE_Inventory_Waiting)
			ACE_Inventory_Finish("Cancelled.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Finish(string reason, bool failed = false)
	{
		GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
		GetGame().GetCallqueue().Remove(ACE_Inventory_Next);
		GetGame().GetCallqueue().Remove(ACE_Inventory_CheckRefund);
		if (failed)
			Print("[ACE Inventory] " + reason, LogLevel.WARNING);
		Rpc(ACE_Inventory_RpcDo_Result, m_iACE_Inventory_Token, m_iACE_Inventory_Completed, reason, true);
		m_iACE_Inventory_Token = 0;
		m_bACE_Inventory_Waiting = false;
		m_pACE_Inventory_Destination = null;
		m_pACE_Inventory_DestinationRoot = null;
		m_bACE_Inventory_Refunding = false;
		m_bACE_Inventory_RefundAccepted = false;
		m_pACE_Inventory_RefundItem = null;
		m_pACE_Inventory_RefundArsenal = null;
		m_aACE_Inventory_RefundItems.Clear();
		m_aACE_Inventory_RefundSources.Clear();
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void ACE_Inventory_RpcDo_Result(int token, int completed, string reason, bool finished)
	{
		m_pACE_Inventory_Result.Invoke(token, completed, reason, finished);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
		GetGame().GetCallqueue().Remove(ACE_Inventory_Next);
		GetGame().GetCallqueue().Remove(ACE_Inventory_CheckRefund);
		m_iACE_Inventory_Token = 0;
		m_bACE_Inventory_Waiting = false;
		m_bACE_Inventory_Cancelled = true;
		m_bACE_Inventory_RefundAccepted = false;
		m_pACE_Inventory_RefundItem = null;
		m_pACE_Inventory_RefundArsenal = null;
		m_pACE_Inventory_Destination = null;
		m_pACE_Inventory_DestinationRoot = null;
		m_aACE_Inventory_RefundItems.Clear();
		m_aACE_Inventory_RefundSources.Clear();
		if (s_pACE_Inventory_Dispatch == this)
			s_pACE_Inventory_Dispatch = null;
		if (s_pACE_Inventory_RefundDispatch == this)
			s_pACE_Inventory_RefundDispatch = null;
		super.OnDelete(owner);
	}
}
