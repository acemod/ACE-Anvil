[EntityEditorProps(category: "GameScripted/Components", description: "")]
modded class SCR_PlaceableItemComponent : ScriptComponent
{
	[Attribute(defvalue: "true", desc: "Can this entity be stacked.")]
	protected bool m_bACE_CanBeStacked;

	//------------------------------------------------------------------------------------------------
	bool ACE_CanBeStacked()
	{
		return m_bACE_CanBeStacked;
	}
}
