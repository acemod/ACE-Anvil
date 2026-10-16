//------------------------------------------------------------------------------------------------
class ACE_Inventory_SizeReadout
{
	protected Widget m_wPanel;
	protected TextWidget m_wItem;
	protected TextWidget m_wAvailable;
	protected bool m_bInline;
	protected bool m_bHeader;
	protected bool m_bHasAvailableSize;

	//------------------------------------------------------------------------------------------------
	bool HasAvailableSize()
	{
		return m_bHasAvailableSize;
	}

	//------------------------------------------------------------------------------------------------
	bool Init(Widget parent, bool header = false, Widget itemHolder = null, Widget pocketHolder = null)
	{
		m_bHeader = header;
		m_wPanel = GetGame().GetWorkspace().CreateWidgets("{73A84B209C112001}UI/layouts/Inventory/ACE_Inventory_SizeMetrics.layout", parent);
		ImageWidget itemIcon;
		ImageWidget pocketIcon;
		if (m_wPanel)
		{
			m_wItem = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_ItemSize"));
			m_wAvailable = TextWidget.Cast(m_wPanel.FindAnyWidget("ACE_AvailableSize"));
			itemIcon = ImageWidget.Cast(m_wPanel.FindAnyWidget("ACE_ItemSizeIcon"));
			pocketIcon = ImageWidget.Cast(m_wPanel.FindAnyWidget("ACE_AvailableSizeIcon"));
		}
		if (!m_wItem || !m_wAvailable || !itemIcon || !pocketIcon)
		{
			Print("[ACE Inventory] Metrics: Could not create size readout or its required widgets.", LogLevel.ERROR);
			if (m_wPanel)
				m_wPanel.RemoveFromHierarchy();
			m_wPanel = null;
			return false;
		}

		if (header)
		{
			AlignableSlot.SetVerticalAlign(m_wPanel, LayoutVerticalAlign.Bottom);
			AlignableSlot.SetPadding(m_wPanel, 0, 0, 8, 10);
			m_wItem.SetExactFontSize(12);
			m_wAvailable.SetExactFontSize(12);
			itemIcon.SetSize(12, 12);
			pocketIcon.SetSize(12, 12);
			m_wItem.GetParent().AddChild(itemIcon);
			m_wAvailable.GetParent().AddChild(pocketIcon);
			AlignableSlot.SetPadding(m_wItem, 0, 0, 0, 0);
			AlignableSlot.SetPadding(m_wAvailable, 0, 0, 0, 0);
			AlignableSlot.SetPadding(itemIcon, 4, 0, 0, 0);
			AlignableSlot.SetPadding(pocketIcon, 4, 0, 0, 0);
			AlignableSlot.SetPadding(m_wAvailable.GetParent(), 0, 0, 0, 0);
		}
		if (itemHolder && pocketHolder)
		{
			m_bInline = true;
			itemHolder.AddChild(m_wItem.GetParent());
			pocketHolder.AddChild(m_wAvailable.GetParent());
			AlignableSlot.SetPadding(m_wItem.GetParent(), 0, 0, 0, 0);
			AlignableSlot.SetPadding(m_wAvailable.GetParent(), 0, 0, 0, 0);
			m_wPanel.RemoveFromHierarchy();
			m_wPanel = null;
		}
		if (m_bInline)
			return true;
		m_wPanel.SetVisible(false);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	void Update(InventoryItemComponent item, BaseInventoryStorageComponent storage)
	{
		if (!m_wItem || !m_wAvailable)
			return;

		string size;
		string available;
		if (ACE_Inventory_DisplaySettings.ShowDimensions())
		{
			size = ACE_Inventory_Metrics.ItemSize(item);
			available = ACE_Inventory_Metrics.AvailableSize(storage, m_bHeader && ACE_Inventory_DisplaySettings.ShowParenthesizedValues());
		}
		m_bHasAvailableSize = !available.IsEmpty();
		m_wItem.SetText(size);
		m_wAvailable.SetText(available);
		m_wItem.GetParent().SetVisible(!size.IsEmpty());
		m_wAvailable.GetParent().SetVisible(!available.IsEmpty());
		if (m_bInline)
		{
			m_wItem.GetParent().GetParent().SetVisible(!size.IsEmpty());
			m_wAvailable.GetParent().GetParent().SetVisible(!available.IsEmpty());
			return;
		}
		m_wPanel.SetVisible(!size.IsEmpty() || !available.IsEmpty());
	}
}
