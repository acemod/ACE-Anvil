modded class SCR_NavigationBarUI
{
	//------------------------------------------------------------------------------------------------
	override void FillFromConfig()
	{
		if (m_aEntries)
		{
			foreach (NavigationButtonEntry entry : m_aEntries)
			{
				if (entry.m_sButtonID.StartsWith("ACE_Inventory_") && entry.m_Component)
					entry.m_Component.GetRootWidget().RemoveFromHierarchy();
			}
		}
		super.FillFromConfig();
		if (!m_sConfig.GetPath().EndsWith("Configs/Inventory/InventoryNavigationBar.conf"))
			return;

		ACE_Inventory_AddHint("ACE_Inventory_Small", "ACE_Inventory_ModifierSmall",
			string.Format("Move %1", ACE_Inventory_Settings.SMALL_BATCH_SIZE), 1);
		ACE_Inventory_AddHint("ACE_Inventory_Large", "ACE_Inventory_ModifierLarge",
			string.Format("Move %1", ACE_Inventory_Settings.LARGE_BATCH_SIZE), 2);
		ACE_Inventory_AddHint("ACE_Inventory_All", "ACE_Inventory_ModifierAll", "Move All", 1);
	}

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_UpdateHints(bool enabled)
	{
		bool visible;
		if (enabled)
		{
			array<string> buttons = {"ButtonBuy", "ButtonSell", "ButtonPickup", "ButtonDrop", "ButtonMove"};
			foreach (string id : buttons)
			{
				SCR_InputButtonComponent button = GetButton(id);
				if (button && button.GetRootWidget().IsVisible())
				{
					visible = true;
					break;
				}
			}
		}
		SetButtonEnabled("ACE_Inventory_Small", visible);
		SetButtonEnabled("ACE_Inventory_Large", visible);
		SetButtonEnabled("ACE_Inventory_All", visible);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_AddHint(string id, string action, string label, int keyCount)
	{
		Widget widget = GetGame().GetWorkspace().CreateWidgets(m_Layout, GetRootWidget());
		if (!widget)
		{
			PrintFormat("[ACE Inventory] Unable to create quantity hint %1.", id, level: LogLevel.ERROR);
			return;
		}

		SCR_InputButtonComponent component = SCR_InputButtonComponent.Cast(widget.FindHandler(SCR_InputButtonComponent));
		if (!component)
		{
			widget.RemoveFromHierarchy();
			PrintFormat("[ACE Inventory] Quantity hint %1 has no input button component.", id, level: LogLevel.ERROR);
			return;
		}

		component.ACE_Inventory_SetHint(action, label, keyCount);
		NavigationButtonEntry entry = new NavigationButtonEntry();
		entry.m_sButtonID = id;
		entry.m_sAction = action;
		entry.m_sDisplayName = label;
		entry.m_Component = component;
		m_aEntries.Insert(entry);
	}
}
