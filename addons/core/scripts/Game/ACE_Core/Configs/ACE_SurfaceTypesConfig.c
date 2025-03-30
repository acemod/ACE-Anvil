//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_SurfaceLabelsConfig
{
	[Attribute(desc: "Registered surface labels")]
	protected ref array<ref ACE_SurfaceLabelConfigEntry> m_aEntries;
	
	//----------------------------------------------------------------------------------------
	bool HasLabel(BaseContainer gameMatContainer, ACE_ESurfaceLabel label)
	{
		foreach (ACE_SurfaceLabelConfigEntry entry : m_aEntries)
		{
			if (label != entry.GetLabel())
				continue;
			
			return entry.HasLabel(gameMatContainer);
		}
		
		return false;
	}
}

//----------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_ESurfaceLabel, "m_eLabel")]
class ACE_SurfaceLabelConfigEntry
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_ESurfaceLabel), desc: "A property label for surfaces")]
	protected ACE_ESurfaceLabel m_eLabel;
	
	[Attribute(desc: "Physical materials that have this label", uiwidget: UIWidgets.ResourceNamePicker, params: "physmat")]
	protected ref array<ResourceName> m_aPhysMatResNames;
	
	//----------------------------------------------------------------------------------------
	ACE_ESurfaceLabel GetLabel()
	{
		return m_eLabel;
	}
	
	//----------------------------------------------------------------------------------------
	bool HasLabel(BaseContainer gameMatContainer)
	{
		ResourceName resName;
		gameMatContainer.Get("Physics material", resName);
		
		foreach (ResourceName whitelistedResName : m_aPhysMatResNames)
		{
			if (whitelistedResName == resName)
				return true;
		}
		
		return false;
	}
}
