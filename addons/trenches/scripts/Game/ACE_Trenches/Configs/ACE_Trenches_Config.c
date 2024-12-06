//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Trenches_Config
{
	[Attribute(defvalue: "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat", desc: "Material for the outline if it can be placed", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	ResourceName m_sCanPlacePreviewMaterial;
	
	[Attribute(defvalue: "{14A9DCEA57D1C381}Assets/Conflict/CannotBuild.emat", desc: "Material for the outline if it cannot be placed", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	ResourceName m_sCannotPlacePreviewMaterial;
	
	[Attribute(desc: "Structures that can be placed with the E-tool")]
	protected ref array<ref ACE_Trenches_PlaceablePrefabConfig> m_aPlaceablePrefabConfigs;
	
	[Attribute(desc: "Surface game materials on which placement of terrain objects is not possible", uiwidget: UIWidgets.ResourceNamePicker, params: "gamemat")]
	protected ref array<ResourceName> m_aBlacklistedTerrainSurfaces;
	
	//----------------------------------------------------------------------------------------
	void ACE_Trenches_Config()
	{
		foreach (int id, ACE_Trenches_PlaceablePrefabConfig entry : m_aPlaceablePrefabConfigs)
		{
			entry.SetID(id);
		}
	}
	
	//----------------------------------------------------------------------------------------
	ACE_Trenches_PlaceablePrefabConfig GetPlaceablePrefabConfig(int prefabID)
	{
		if (!m_aPlaceablePrefabConfigs.IsIndexValid(prefabID))
			return null;
		
		return m_aPlaceablePrefabConfigs[prefabID];
	}
	
	//----------------------------------------------------------------------------------------
	array<ref ACE_Trenches_PlaceablePrefabConfig> GetPlaceablePrefabConfigs()
	{
		return m_aPlaceablePrefabConfigs;
	}
	
	//----------------------------------------------------------------------------------------
	bool IsTerrainSurfaceValid(SurfaceProperties props)
	{
		ResourceName currentSurface = props.GetResourceName();
		
		foreach (ResourceName surface : m_aBlacklistedTerrainSurfaces)
		{
			if (currentSurface == surface)
				return false;
		}
		
		return true;
	}
}

//----------------------------------------------------------------------------------------
[BaseContainerProps(), BaseContainerCustomDoubleCheckIntResourceNameTitleField("m_bEnabled", "m_sPrefabName", 1, "%1", "DISABLED - %1")]
class ACE_Trenches_PlaceablePrefabConfig
{
	[Attribute(desc: "Prefab names of structures that can be placed with the E-tool", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	protected ResourceName m_sPrefabName;
	
	[Attribute(defvalue: "true", desc: "Terrain objects can only be placed on terrain surfaces that are not blacklisted.")]
	protected bool m_bIsTerrainObject;
	
	[Attribute(defvalue: "true")]
	protected bool m_bEnabled;
	
	protected int m_iSupplyCost;
	protected int m_iID;
	
	//----------------------------------------------------------------------------------------
	void ACE_Trenches_PlaceablePrefabConfig()
	{
		Resource res = Resource.Load(m_sPrefabName);
		if (!res.IsValid())
			return;
		
		SCR_EditableEntityUIInfo info = ACE_BaseContainerTools.GetEditableEntityUIInfo(res.GetResource());
		if (!info)
			return;
		
		array<ref SCR_EntityBudgetValue> budgets = {};
		info.GetEntityBudgetCost(budgets);
		
		foreach (SCR_EntityBudgetValue budget : budgets)
		{
			if (budget.GetBudgetType() == EEditableEntityBudget.CAMPAIGN)
			{
				m_iSupplyCost = budget.GetBudgetValue();
				return;
			}
		}
	}
	
	//----------------------------------------------------------------------------------------
	ResourceName GetPrefabName()
	{
		return m_sPrefabName;
	}
	
	//----------------------------------------------------------------------------------------
	bool IsTerrainObject()
	{
		return m_bIsTerrainObject;
	}
	
	//----------------------------------------------------------------------------------------
	bool IsEnabled()
	{
		return m_bEnabled;
	}
	
	//----------------------------------------------------------------------------------------
	int GetSupplyCost()
	{
		return m_iSupplyCost;
	}
	
	//----------------------------------------------------------------------------------------
	void SetID(int id)
	{
		m_iID = id;
	}
	
	//----------------------------------------------------------------------------------------
	int GetID()
	{
		return m_iID;
	}
}
