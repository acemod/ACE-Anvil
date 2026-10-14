modded class SCR_ResourcePlayerControllerInventoryComponent
{
	protected static SCR_ResourcePlayerControllerInventoryComponent s_ACE_Inventory_Dispatch;
	protected ref ScriptInvoker m_ACE_Inventory_Result = new ScriptInvoker();
	protected int m_ACE_Inventory_Token;
	protected int m_ACE_Inventory_Operation;
	protected int m_ACE_Inventory_Completed;
	protected int m_ACE_Inventory_Quantity;
	protected bool m_ACE_Inventory_Waiting;
	protected bool m_ACE_Inventory_Captured;
	protected bool m_ACE_Inventory_Cancelled;
	protected bool m_ACE_Inventory_AutomaticDestination;
	protected bool m_ACE_Inventory_Refunding;
	protected ref array<RplId> m_ACE_Inventory_RefundItems = {};
	protected ref array<BaseInventoryStorageComponent> m_ACE_Inventory_RefundSources = {};
	protected RplId m_ACE_Inventory_Arsenal;
	protected BaseInventoryStorageComponent m_ACE_Inventory_Destination;
	protected ResourceName m_ACE_Inventory_Resource;

	//------------------------------------------------------------------------------------------------
	static SCR_ResourcePlayerControllerInventoryComponent ACE_Inventory_GetDispatch()
	{
		return s_ACE_Inventory_Dispatch;
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvoker ACE_Inventory_GetResult()
	{
		return m_ACE_Inventory_Result;
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
	void ACE_Inventory_RequestRefund(int token, RplId arsenal, array<RplId> items, int quantity)
	{
		Rpc(ACE_Inventory_RpcAsk_Refund, token, arsenal, items, quantity);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void ACE_Inventory_RpcAsk_Refund(int token, RplId arsenal, array<RplId> items, int quantity)
	{
		if (m_ACE_Inventory_Token > 0)
		{
			Print("[ACE Inventory] Rejected overlapping refund batch.", LogLevel.WARNING);
			if (token != m_ACE_Inventory_Token)
				Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Another arsenal operation is still pending.", true);
			return;
		}

		if (token <= 0 || !arsenal.IsValid() || !ACE_Inventory_Settings.IsSupportedQuantity(quantity) || !items || items.IsEmpty()
			|| (quantity != ACE_Inventory_Settings.ALL_ITEMS && items.Count() > quantity))
		{
			Print("[ACE Inventory] Invalid refund batch.", LogLevel.WARNING);
			Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Invalid refund batch.", true);
			return;
		}

		m_ACE_Inventory_RefundItems.Clear();
		m_ACE_Inventory_RefundSources.Clear();
		foreach (RplId id : items)
		{
			InventoryItemComponent component = InventoryItemComponent.Cast(Replication.FindItem(id));
			if (!id.IsValid() || !component || !component.GetParentSlot() || m_ACE_Inventory_RefundItems.Find(id) >= 0)
			{
				Print("[ACE Inventory] Refund snapshot changed or contains duplicate items.", LogLevel.WARNING);
				Rpc(ACE_Inventory_RpcDo_Result, token, 0, "Refund snapshot changed or contains duplicate items.", true);
				m_ACE_Inventory_RefundItems.Clear();
				m_ACE_Inventory_RefundSources.Clear();
				return;
			}

			m_ACE_Inventory_RefundItems.Insert(id);
			m_ACE_Inventory_RefundSources.Insert(component.GetParentSlot().GetStorage());
		}

		m_ACE_Inventory_Token = token;
		m_ACE_Inventory_Operation = 0;
		m_ACE_Inventory_Completed = 0;
		m_ACE_Inventory_Cancelled = false;
		m_ACE_Inventory_Refunding = true;
		m_ACE_Inventory_Quantity = quantity;
		if (quantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_ACE_Inventory_Quantity = items.Count();
		m_ACE_Inventory_Arsenal = arsenal;
		ACE_Inventory_Next();
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void ACE_Inventory_RpcAsk_Batch(int token, RplId arsenal, RplId destination, ResourceName resource, bool automaticDestination, int quantity)
	{
		if (m_ACE_Inventory_Token > 0)
		{
			Print("[ACE Inventory] Rejected overlapping arsenal batch.", LogLevel.WARNING);
			if (token != m_ACE_Inventory_Token)
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

		m_ACE_Inventory_Destination = BaseInventoryStorageComponent.Cast(Replication.FindItem(destination));
		m_ACE_Inventory_Token = token;
		m_ACE_Inventory_Operation = 0;
		m_ACE_Inventory_Completed = 0;
		m_ACE_Inventory_Cancelled = false;
		m_ACE_Inventory_Refunding = false;
		m_ACE_Inventory_Quantity = quantity;
		if (quantity == ACE_Inventory_Settings.ALL_ITEMS)
			m_ACE_Inventory_Quantity = ACE_Inventory_Settings.ALL_ARSENAL_LIMIT;
		m_ACE_Inventory_AutomaticDestination = automaticDestination;
		m_ACE_Inventory_Arsenal = arsenal;
		m_ACE_Inventory_Resource = resource;
		ACE_Inventory_Next();
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Next()
	{
		if (m_ACE_Inventory_Token <= 0)
			return;

		if (m_ACE_Inventory_Cancelled)
		{
			ACE_Inventory_Finish("Cancelled; no further requests issued.");
			return;
		}

		if (m_ACE_Inventory_Completed >= m_ACE_Inventory_Quantity || (m_ACE_Inventory_Refunding && m_ACE_Inventory_Completed == m_ACE_Inventory_RefundItems.Count()))
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

		if (m_ACE_Inventory_Refunding)
		{
			if (!manager)
				ACE_Inventory_Finish("Player inventory is unavailable.");
			else
				ACE_Inventory_NextRefund();
			return;
		}

		if (manager && m_ACE_Inventory_AutomaticDestination)
			m_ACE_Inventory_Destination = manager.FindStorageForResource(m_ACE_Inventory_Resource, EStoragePurpose.PURPOSE_DEPOSIT);

		if (!manager || !m_ACE_Inventory_Destination || !manager.CanInsertResourceInStorage(m_ACE_Inventory_Resource, m_ACE_Inventory_Destination))
		{
			ACE_Inventory_Finish("Destination unavailable, incompatible or full.");
			return;
		}

		m_ACE_Inventory_Operation++;
		m_ACE_Inventory_Waiting = true;
		m_ACE_Inventory_Captured = false;
		GetGame().GetCallqueue().CallLater(ACE_Inventory_Timeout, 10000, false);

		s_ACE_Inventory_Dispatch = this;
		// Keep native ownership, access, catalog, supply-cost and allocation validation.
		super.RpcAsk_ArsenalRequestItem_(m_ACE_Inventory_Arsenal, Replication.FindItemId(m_ACE_Inventory_Destination), m_ACE_Inventory_Resource, EResourceType.SUPPLIES);
		s_ACE_Inventory_Dispatch = null;

		if (m_ACE_Inventory_Token > 0 && !m_ACE_Inventory_Captured)
			ACE_Inventory_Finish("Native arsenal request rejected; check supplies, access and availability.");
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_CaptureSpawn(BaseInventoryStorageComponent destination, out int token, out int operation)
	{
		token = 0;
		operation = 0;
		if (!m_ACE_Inventory_Waiting || destination != m_ACE_Inventory_Destination)
			return;

		m_ACE_Inventory_Captured = true;
		token = m_ACE_Inventory_Token;
		operation = m_ACE_Inventory_Operation;
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
		InventoryItemComponent component = InventoryItemComponent.Cast(Replication.FindItem(m_ACE_Inventory_RefundItems[m_ACE_Inventory_Completed]));
		SCR_ResourceComponent resource = SCR_ResourceComponent.Cast(Replication.FindItem(m_ACE_Inventory_Arsenal));
		SCR_ArsenalComponent arsenal;
		if (resource)
			arsenal = SCR_ArsenalComponent.FindArsenalComponent(resource.GetOwner());

		if (!arsenal || !arsenal.IsArsenalEnabled() || !component || !component.GetParentSlot()
			|| component.GetParentSlot().GetStorage() != m_ACE_Inventory_RefundSources[m_ACE_Inventory_Completed]
			|| !ACE_Inventory_IsRefundable(component))
		{
			ACE_Inventory_Finish("Arsenal/source changed or item contents are not refundable.");
			return;
		}

		m_ACE_Inventory_Operation++;
		m_ACE_Inventory_Waiting = true;
		GetGame().GetCallqueue().CallLater(ACE_Inventory_Timeout, 10000, false);
		// Native code owns eligibility, distance checks, refund effects and resource credit.
		super.RpcAsk_ArsenalRefundItem_(m_ACE_Inventory_Arsenal, m_ACE_Inventory_RefundItems[m_ACE_Inventory_Completed], EResourceType.SUPPLIES);
		GetGame().GetCallqueue().CallLater(ACE_Inventory_CheckRefund, 0, false);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_CheckRefund()
	{
		if (!m_ACE_Inventory_Refunding || !m_ACE_Inventory_Waiting || m_ACE_Inventory_Token <= 0)
			return;

		InventoryItemComponent component = InventoryItemComponent.Cast(Replication.FindItem(m_ACE_Inventory_RefundItems[m_ACE_Inventory_Completed]));
		if (!component || !component.GetOwner() || component.GetOwner().IsDeleted())
		{
			GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
			m_ACE_Inventory_Waiting = false;
			m_ACE_Inventory_Completed++;
			Rpc(ACE_Inventory_RpcDo_Result, m_ACE_Inventory_Token, m_ACE_Inventory_Completed, "Native refund item removal confirmed.", false);
			GetGame().GetCallqueue().CallLater(ACE_Inventory_Next, 0, false);
			return;
		}

		GetGame().GetCallqueue().CallLater(ACE_Inventory_CheckRefund, 50, false);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_OnSpawnResult(int token, int operation, bool success)
	{
		if (token != m_ACE_Inventory_Token || operation != m_ACE_Inventory_Operation || !m_ACE_Inventory_Waiting)
			return;

		GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
		m_ACE_Inventory_Waiting = false;
		if (!success)
		{
			ACE_Inventory_Finish("Native item insertion failed.");
			return;
		}

		m_ACE_Inventory_Completed++;
		Rpc(ACE_Inventory_RpcDo_Result, token, m_ACE_Inventory_Completed, "Native spawn completed.", false);
		GetGame().GetCallqueue().CallLater(ACE_Inventory_Next, 0, false);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Timeout()
	{
		if (!m_ACE_Inventory_Waiting)
			return;

		m_ACE_Inventory_Cancelled = true;
		GetGame().GetCallqueue().Remove(ACE_Inventory_CheckRefund);
		Print("[ACE Inventory] Arsenal callback timed out; no further requests will be issued.", LogLevel.WARNING);
		Rpc(ACE_Inventory_RpcDo_Result, m_ACE_Inventory_Token, m_ACE_Inventory_Completed, "Outcome pending after timeout; batch stopped without retry.", false);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void ACE_Inventory_RpcAsk_Cancel(int token)
	{
		if (token != m_ACE_Inventory_Token)
			return;

		m_ACE_Inventory_Cancelled = true;
		if (!m_ACE_Inventory_Waiting)
			ACE_Inventory_Finish("Cancelled.");
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_Finish(string reason)
	{
		GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
		GetGame().GetCallqueue().Remove(ACE_Inventory_Next);
		GetGame().GetCallqueue().Remove(ACE_Inventory_CheckRefund);
		PrintFormat("[ACE Inventory] Arsenal: %1/%2 confirmed. %3", m_ACE_Inventory_Completed, m_ACE_Inventory_Quantity, reason);
		Rpc(ACE_Inventory_RpcDo_Result, m_ACE_Inventory_Token, m_ACE_Inventory_Completed, reason, true);
		m_ACE_Inventory_Token = 0;
		m_ACE_Inventory_Waiting = false;
		m_ACE_Inventory_Destination = null;
		m_ACE_Inventory_Refunding = false;
		m_ACE_Inventory_RefundItems.Clear();
		m_ACE_Inventory_RefundSources.Clear();
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void ACE_Inventory_RpcDo_Result(int token, int completed, string reason, bool finished)
	{
		m_ACE_Inventory_Result.Invoke(token, completed, reason, finished);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		GetGame().GetCallqueue().Remove(ACE_Inventory_Timeout);
		GetGame().GetCallqueue().Remove(ACE_Inventory_Next);
		GetGame().GetCallqueue().Remove(ACE_Inventory_CheckRefund);
		if (s_ACE_Inventory_Dispatch == this)
			s_ACE_Inventory_Dispatch = null;
		super.OnDelete(owner);
	}
}
