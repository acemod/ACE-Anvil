//------------------------------------------------------------------------------------------------
class ACE_Inventory_ContainerMetricsComponent : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	protected Widget m_wVolumeRow;
	protected TextWidget m_wVolume;
	protected Widget m_wHeaderPanel;
	protected TextWidget m_wCompartmentCount;
	protected Widget m_wWeight;
	protected Widget m_wWeightParent;
	protected Widget m_wCapacityIndicator;
	protected Widget m_wCapacityParent;
	protected Widget m_wTitlePanel;
	protected LayoutVerticalAlign m_eTitleAlignment;
	protected SizeLayoutWidget m_wHeader;
	protected float m_fOriginalHeaderHeight;
	protected bool m_bLayoutFailed;
	protected ref ACE_Inventory_SizeReadout m_SizeReadout;
	protected BaseInventoryStorageComponent m_Storage;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		m_bLayoutFailed = false;
		ACE_Inventory_DisplaySettings.s_OnChanged.Insert(ACE_Inventory_RefreshSettings);
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_RefreshSettings()
	{
		UpdateMetrics(m_Storage);
	}

	//------------------------------------------------------------------------------------------------
	void UpdateMetrics(BaseInventoryStorageComponent storage)
	{
		m_Storage = storage;
		if (!m_wRoot || m_bLayoutFailed)
			return;

		if (!m_wVolumeRow)
		{
			Widget progress = m_wRoot.FindAnyWidget("ProgressBar");
			if (!progress)
			{
				Print("[ACE Inventory] Metrics: Container layout is missing ProgressBar.", LogLevel.ERROR);
				m_bLayoutFailed = true;
				return;
			}

			m_wVolumeRow = GetGame().GetWorkspace().CreateWidgets("{6A9B210991665241}UI/layouts/Inventory/ACE_Inventory_Volume.layout", progress.GetParent());
			if (m_wVolumeRow)
				m_wVolume = TextWidget.Cast(m_wVolumeRow.FindAnyWidget("ACE_Volume"));
			if (!m_wVolume)
			{
				Print("[ACE Inventory] Metrics: Could not create container volume readout.", LogLevel.ERROR);
				if (m_wVolumeRow)
					m_wVolumeRow.RemoveFromHierarchy();
				m_wVolumeRow = null;
				m_bLayoutFailed = true;
				return;
			}
		}

		string volume = ACE_Inventory_Metrics.Container(storage, ACE_Inventory_DisplaySettings.ShowParenthesizedValues());
		m_wVolume.SetText(volume);
		m_wVolumeRow.SetVisible(!volume.IsEmpty());
		if (!m_wHeaderPanel)
		{
			m_wWeight = m_wRoot.FindAnyWidget("Weight");
			m_wCapacityIndicator = m_wRoot.FindAnyWidget("StorageSpaceIndicator");
			m_wTitlePanel = m_wRoot.FindAnyWidget("TitlePanel");
			Widget headerOverlay = m_wRoot.FindAnyWidget("HeaderOverlay");
			m_wHeader = SizeLayoutWidget.Cast(m_wRoot.FindAnyWidget("Header"));
			if (!m_wWeight || !m_wCapacityIndicator || !m_wTitlePanel || !headerOverlay || !m_wHeader)
			{
				Print("[ACE Inventory] Metrics: Container layout is missing required header widgets.", LogLevel.ERROR);
				m_bLayoutFailed = true;
				return;
			}

			m_wWeightParent = m_wWeight.GetParent();
			m_wCapacityParent = m_wCapacityIndicator.GetParent();
			m_wHeaderPanel = GetGame().GetWorkspace().CreateWidgets("{6A9B210991665252}UI/layouts/Inventory/ACE_Inventory_Header.layout", headerOverlay);
			Widget volumeHolder;
			Widget sizeWeightRow;
			Widget countIcon;
			if (m_wHeaderPanel)
			{
				m_wCompartmentCount = TextWidget.Cast(m_wHeaderPanel.FindAnyWidget("ACE_CompartmentCount"));
				countIcon = m_wHeaderPanel.FindAnyWidget("ACE_CompartmentIconHolder");
				volumeHolder = m_wHeaderPanel.FindAnyWidget("ACE_VolumeHolder");
				sizeWeightRow = m_wHeaderPanel.FindAnyWidget("ACE_CompartmentWeightRow");
			}
			if (!m_wCompartmentCount || !countIcon || !volumeHolder || !sizeWeightRow)
			{
				Print("[ACE Inventory] Metrics: Could not create container compartment readout.", LogLevel.ERROR);
				if (m_wHeaderPanel)
					m_wHeaderPanel.RemoveFromHierarchy();
				m_wHeaderPanel = null;
				m_bLayoutFailed = true;
				return;
			}

			if (!GetGame().GetWorkspace().CreateWidgets("{6A9B210991665253}UI/layouts/Inventory/ACE_Inventory_CompartmentIcon.layout", countIcon))
			{
				Print("[ACE Inventory] Metrics: Could not create header compartment icon.", LogLevel.ERROR);
				m_wHeaderPanel.RemoveFromHierarchy();
				m_wHeaderPanel = null;
				m_bLayoutFailed = true;
				return;
			}

			m_SizeReadout = new ACE_Inventory_SizeReadout();
			if (!m_SizeReadout.Init(sizeWeightRow, true))
			{
				m_wHeaderPanel.RemoveFromHierarchy();
				m_wHeaderPanel = null;
				m_SizeReadout = null;
				m_bLayoutFailed = true;
				return;
			}

			volumeHolder.AddChild(m_wCapacityIndicator);
			sizeWeightRow.AddChild(m_wWeight);
			m_eTitleAlignment = AlignableSlot.GetVerticalAlign(m_wTitlePanel);
			AlignableSlot.SetVerticalAlign(m_wTitlePanel, LayoutVerticalAlign.Top);
			m_fOriginalHeaderHeight = m_wHeader.GetHeightOverride();
			m_wHeader.SetHeightOverride(m_fOriginalHeaderHeight + 16);
		}
		int count = ACE_Inventory_Metrics.CompartmentCount(storage);
		int availableCount = ACE_Inventory_Metrics.CompartmentCount(storage, true);
		string countText;
		if (availableCount >= 0)
			countText = availableCount.ToString();
		m_wCompartmentCount.SetText(countText);
		m_wCompartmentCount.GetParent().SetVisible(count > 1 && availableCount >= 0 && ACE_Inventory_DisplaySettings.ShowCompartmentCount());
		m_SizeReadout.Update(null, storage);
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		ACE_Inventory_DisplaySettings.s_OnChanged.Remove(ACE_Inventory_RefreshSettings);
		m_Storage = null;
		if (m_wVolumeRow)
			m_wVolumeRow.RemoveFromHierarchy();
		if (m_wHeaderPanel)
		{
			if (m_wCapacityIndicator && m_wCapacityParent)
				m_wCapacityParent.AddChild(m_wCapacityIndicator);
			if (m_wWeight && m_wWeightParent)
				m_wWeightParent.AddChild(m_wWeight);
			if (m_wTitlePanel)
				AlignableSlot.SetVerticalAlign(m_wTitlePanel, m_eTitleAlignment);
			if (m_wHeader)
				m_wHeader.SetHeightOverride(m_fOriginalHeaderHeight);
			m_wHeaderPanel.RemoveFromHierarchy();
		}
		m_wVolumeRow = null;
		m_wVolume = null;
		m_wHeaderPanel = null;
		m_SizeReadout = null;
		m_wCompartmentCount = null;
		m_wWeight = null;
		m_wWeightParent = null;
		m_wCapacityIndicator = null;
		m_wCapacityParent = null;
		m_wTitlePanel = null;
		m_wHeader = null;
		m_wRoot = null;
	}
}
