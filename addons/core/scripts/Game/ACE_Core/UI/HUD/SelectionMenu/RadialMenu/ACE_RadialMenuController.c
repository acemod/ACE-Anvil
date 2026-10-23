//------------------------------------------------------------------------------------------------
//! Same as SCR_RadialMenuController, but automatically adds entries in m_Data to radial menu
[BaseContainerProps(configRoot: true)]
class ACE_RadialMenuController : SCR_RadialMenuController
{
	//------------------------------------------------------------------------------------------------
	override void Control(IEntity owner, SCR_RadialMenu radialMenu = null)
	{
		super.Control(owner, radialMenu);
		UpdateMenuData();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnInputOpen()
	{
		super.OnInputOpen();
		DisableUnperformableEntries(m_RadialMenu.GetEntries());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DisableUnperformableEntries(array<ref SCR_SelectionMenuEntry> entries)
	{
		foreach (SCR_SelectionMenuEntry entry : entries)
		{
			ACE_SelectionMenuEntry aceEntry = ACE_SelectionMenuEntry.Cast(entry);
			if (aceEntry)
				aceEntry.Enable(aceEntry.CanBePerformed());
			
			SCR_SelectionMenuCategoryEntry categoryEntry = SCR_SelectionMenuCategoryEntry.Cast(entry);
			if (categoryEntry)
				DisableUnperformableEntries(categoryEntry.GetEntries());
			
			ACE_SelectionMenuCategoryEntry aceCategoryEntry = ACE_SelectionMenuCategoryEntry.Cast(entry);
			if (aceCategoryEntry)
				aceCategoryEntry.Enable(aceCategoryEntry.CanBePerformed());
		}
	}
}
