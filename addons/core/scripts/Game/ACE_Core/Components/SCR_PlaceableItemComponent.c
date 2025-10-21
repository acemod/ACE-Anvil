//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/Components", description: "")]
modded class SCR_PlaceableItemComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "true", desc: "Can this entity be stacked.")]
	protected bool m_bACE_CanBeStacked;
	
	[Attribute(desc: "On what kind of surface it be placed (NONE means any)", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ACE_ESurfaceLabel))]
	protected ACE_ESurfaceLabel m_bACE_RequiredSurfaceLabel;
	
	//------------------------------------------------------------------------------------------------
	bool ACE_CanBeStacked()
	{
		return m_bACE_CanBeStacked;
	}

	//------------------------------------------------------------------------------------------------
	ACE_ESurfaceLabel ACE_RequiredSurfaceLabel()
	{
		return m_bACE_RequiredSurfaceLabel;
	}
}

//------------------------------------------------------------------------------------------------
modded class SCR_PlaceableItemComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	bool ACE_CanBeStacked()
	{
		SCR_PlaceableItemComponentClass data = SCR_PlaceableItemComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return true;

		return data.ACE_CanBeStacked();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_ESurfaceLabel ACE_RequiredSurfaceLabel()
	{
		SCR_PlaceableItemComponentClass data = SCR_PlaceableItemComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return ACE_ESurfaceLabel.NONE;

		return data.ACE_RequiredSurfaceLabel();
	}
}
