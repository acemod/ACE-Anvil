//------------------------------------------------------------------------------------------------
class ACE_BaseContainerTools
{
	//------------------------------------------------------------------------------------------------
	static SCR_EditableEntityUIInfo GetEditableEntityUIInfo(BaseContainer container)
	{
		IEntitySource entitySource = container.ToEntitySource();
		if (!entitySource)
			return null;
		
		IEntityComponentSource editableEntitySource = SCR_EditableEntityComponentClass.GetEditableEntitySource(entitySource);
		if (!editableEntitySource)
			return null;
		
		return SCR_EditableEntityComponentClass.GetInfo(editableEntitySource);
	}
}
