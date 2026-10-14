//------------------------------------------------------------------------------------------------
class ACE_InventoryItemMetricsComponent : ScriptedWidgetComponent
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
				Print("[ACE Core] Inventory metrics: Tooltip layout is missing its weight icon or text.", LogLevel.ERROR);
				m_bLayoutFailed = true;
				return;
			}

			m_wWeightRow = m_wWeightText.GetParent();
			m_wPanel = GetGame().GetWorkspace().CreateWidgets("{6A9B210991665251}UI/layouts/Inventory/ACE_InventoryItemMetrics.layout", m_wWeightRow);
			if (!m_wPanel)
			{
				Print("[ACE Core] Inventory metrics: Could not create item measurements layout.", LogLevel.ERROR);
				m_bLayoutFailed = true;
				return;
			}

			m_wVolume = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_Volume"));
			m_wStorageRow = m_wPanel.FindAnyWidget("ACE_StorageSummaryRow");
			m_wStorageVolume = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_StorageVolume"));
			m_wCompartmentCount = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_CompartmentCount"));
			Widget countIcon = m_wPanel.FindAnyWidget("ACE_CompartmentIconHolder");
			Widget weight = m_wPanel.FindAnyWidget("ACE_WeightRow");
			if (!m_wVolume || !m_wStorageRow || !m_wStorageVolume || !m_wCompartmentCount || !countIcon || !weight)
			{
				Print("[ACE Core] Inventory metrics: Item measurements layout is missing required widgets.", LogLevel.ERROR);
				m_wPanel.RemoveFromHierarchy();
				m_wPanel = null;
				m_bLayoutFailed = true;
				return;
			}

			if (!GetGame().GetWorkspace().CreateWidgets("{6A9B210991665253}UI/layouts/Inventory/ACE_InventoryCompartmentIcon.layout", countIcon))
			{
				Print("[ACE Core] Inventory metrics: Could not create tooltip compartment icon.", LogLevel.ERROR);
				m_wPanel.RemoveFromHierarchy();
				m_wPanel = null;
				m_bLayoutFailed = true;
				return;
			}

			// Keep the original weight widgets and their live vanilla bindings.
			weight.AddChild(m_wWeightIcon);
			weight.AddChild(m_wWeightText);
		}

		m_wVolume.GetParent().SetVisible(item != null);
		m_wStorageRow.SetVisible(false);
		m_wCompartmentCount.GetParent().SetVisible(false);
		if (!item)
			return;

		string volume = ACE_InventoryMetrics.Volume(item.GetTotalVolume());
		m_wVolume.SetText(volume);
		m_wVolume.GetParent().SetVisible(!volume.IsEmpty());
		BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(item.GetOwner().FindComponent(BaseInventoryStorageComponent));
		volume = ACE_InventoryMetrics.Capacity(storage);
		m_wStorageVolume.SetText(volume);
		m_wStorageVolume.GetParent().SetVisible(!volume.IsEmpty());
		int count = ACE_InventoryMetrics.CompartmentCount(storage);
		m_wCompartmentCount.SetText(count.ToString());
		m_wCompartmentCount.GetParent().SetVisible(count > 1);
		m_wStorageRow.SetVisible(!volume.IsEmpty() || count > 1);
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
