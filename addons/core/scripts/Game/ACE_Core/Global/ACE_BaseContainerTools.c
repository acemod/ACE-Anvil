//------------------------------------------------------------------------------------------------
class ACE_BaseContainerTools
{
	//------------------------------------------------------------------------------------------------
	//! Same as ACE_BaseContainerTools::FindComponentSource, but also works for searching
	//! child components of a IEntityComponentSource
	static IEntityComponentSource FindComponentSource(BaseContainer container, typename componentClass)
	{
		if (!container)
			return null;
		
		BaseContainerList list = container.GetObjectArray("components");
		
		for (int i = 0; i < list.Count(); ++i)
		{
			BaseContainer component = list.Get(i);
			if (component.GetClassName().ToType().IsInherited(componentClass))
				return component;
		}
		
		return null;
	}

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
