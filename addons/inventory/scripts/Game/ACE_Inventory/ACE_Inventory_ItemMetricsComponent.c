//------------------------------------------------------------------------------------------------
class ACE_Inventory_ItemMetricsComponent : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	protected Widget m_wPanel;
	protected TextWidget m_wVolume;
	protected Widget m_wStorageRow;
	protected TextWidget m_wStorageVolume;
	protected TextWidget m_wCompartmentCount;
	protected Widget m_wWeightRow;
	protected Widget m_wWeightIcon;
	protected Widget m_wWeightText;
	protected bool m_bLayoutFailed;
	protected ref ACE_Inventory_SizeReadout m_SizeReadout;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		m_bLayoutFailed = false;
	}

	//------------------------------------------------------------------------------------------------
	void UpdateMetrics(InventoryItemComponent item)
	{
		if (!m_wRoot || m_bLayoutFailed)
			return;

		if (!m_wPanel)
		{
			m_wWeightIcon = m_wRoot.FindAnyWidget("ItemInfo_weightIcon");
			m_wWeightText = m_wRoot.FindAnyWidget("ItemInfo_weight");
			if (!m_wWeightIcon || !m_wWeightText)
			{
				Print("[ACE Inventory] Metrics: Tooltip layout is missing its weight icon or text.", LogLevel.ERROR);
				m_bLayoutFailed = true;
				return;
			}

			m_wWeightRow = m_wWeightText.GetParent();
			m_wPanel = GetGame().GetWorkspace().CreateWidgets("{6A9B210991665251}UI/layouts/Inventory/ACE_Inventory_ItemMetrics.layout", m_wWeightRow);
			if (!m_wPanel)
			{
				Print("[ACE Inventory] Metrics: Could not create item measurements layout.", LogLevel.ERROR);
				m_bLayoutFailed = true;
				return;
			}

			m_wVolume = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_Volume"));
			m_wStorageRow = m_wPanel.FindAnyWidget("ACE_StorageSummaryRow");
			m_wStorageVolume = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_StorageVolume"));
			m_wCompartmentCount = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_CompartmentCount"));
			Widget countIcon = m_wPanel.FindAnyWidget("ACE_CompartmentIconHolder");
			Widget weight = m_wPanel.FindAnyWidget("ACE_WeightRow");
			Widget itemSizeHolder = m_wPanel.FindAnyWidget("ACE_ItemSizeHolder");
			Widget pocketSizeHolder = m_wPanel.FindAnyWidget("ACE_PocketSizeHolder");
			if (!m_wVolume || !m_wStorageRow || !m_wStorageVolume || !m_wCompartmentCount || !countIcon || !weight || !itemSizeHolder || !pocketSizeHolder)
			{
				Print("[ACE Inventory] Metrics: Item measurements layout is missing required widgets.", LogLevel.ERROR);
				m_wPanel.RemoveFromHierarchy();
				m_wPanel = null;
				m_bLayoutFailed = true;
				return;
			}

			if (!GetGame().GetWorkspace().CreateWidgets("{6A9B210991665253}UI/layouts/Inventory/ACE_Inventory_CompartmentIcon.layout", countIcon))
			{
				Print("[ACE Inventory] Metrics: Could not create tooltip compartment icon.", LogLevel.ERROR);
				m_wPanel.RemoveFromHierarchy();
				m_wPanel = null;
				m_bLayoutFailed = true;
				return;
			}

			m_SizeReadout = new ACE_Inventory_SizeReadout();
			if (!m_SizeReadout.Init(m_wPanel, false, itemSizeHolder, pocketSizeHolder))
			{
				m_wPanel.RemoveFromHierarchy();
				m_wPanel = null;
				m_SizeReadout = null;
				m_bLayoutFailed = true;
				return;
			}

			// Keep the original weight widgets and their live vanilla bindings.
			weight.AddChild(m_wWeightIcon);
			weight.AddChild(m_wWeightText);
		}

		if (!item)
		{
			m_wVolume.GetParent().SetVisible(false);
			m_wStorageRow.SetVisible(false);
			m_wCompartmentCount.GetParent().SetVisible(false);
			m_SizeReadout.Update(null, null);
			return;
		}

		string volume = ACE_Inventory_Metrics.Volume(item.GetTotalVolume());
		m_wVolume.SetText(volume);
		m_wVolume.GetParent().SetVisible(!volume.IsEmpty());
		BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(item.GetOwner().FindComponent(BaseInventoryStorageComponent));
		volume = ACE_Inventory_Metrics.Capacity(storage);
		m_wStorageVolume.SetText(volume);
		m_wStorageVolume.GetParent().SetVisible(!volume.IsEmpty());
		int count = ACE_Inventory_Metrics.CompartmentCount(storage);
		m_wCompartmentCount.SetText(count.ToString());
		bool showCount = count > 1 && ACE_Inventory_DisplaySettings.ShowCompartmentCount();
		m_wCompartmentCount.GetParent().SetVisible(showCount);
		m_SizeReadout.Update(item, storage);
		m_wStorageRow.SetVisible(!volume.IsEmpty() || showCount || m_SizeReadout.HasAvailableSize());
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		if (m_wPanel)
		{
			if (m_wWeightRow && m_wWeightIcon)
				m_wWeightRow.AddChild(m_wWeightIcon);
			if (m_wWeightRow && m_wWeightText)
				m_wWeightRow.AddChild(m_wWeightText);
			m_wPanel.RemoveFromHierarchy();
		}
		m_wPanel = null;
		m_SizeReadout = null;
		m_wVolume = null;
		m_wStorageRow = null;
		m_wStorageVolume = null;
		m_wCompartmentCount = null;
		m_wWeightRow = null;
		m_wWeightIcon = null;
		m_wWeightText = null;
		m_wRoot = null;
	}
}
