//------------------------------------------------------------------------------------------------
//! Creates radial menu entry for the item passed to constructor
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_SelectItemRadialMenuEntry : ACE_SelectionMenuEntry
{
	protected IEntity m_pItem;
	protected const ResourceName LAYOUT_ITEM = "{93472DECDA62C46F}UI/layouts/Common/RadialMenu/SelectionMenuEntryPreview.layout";
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		IEntity char = SCR_PlayerController.GetLocalControlledEntity();
		if (!char)
			return;
		
		SCR_CharacterInventoryStorageComponent storage = SCR_CharacterInventoryStorageComponent.Cast(char.FindComponent(SCR_CharacterInventoryStorageComponent));		
		if (m_pItem && storage && storage.CanUseItem(m_pItem))
			storage.UseItem(m_pItem);
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetEntryComponent(SCR_SelectionMenuEntryComponent entryComponent)
	{
		super.SetEntryComponent(entryComponent);
		
		SCR_SelectionMenuEntryPreviewComponent entryPreview = SCR_SelectionMenuEntryPreviewComponent.Cast(entryComponent);
		if (entryPreview)
			entryPreview.SetPreviewItem(m_pItem);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SelectItemRadialMenuEntry(IEntity item)
	{
		SetCustomLayout(LAYOUT_ITEM);
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;

		UIInfo uiInfo = itemComponent.GetUIInfo();
		if (uiInfo)
			SetName(uiInfo.GetName());
		
		m_pItem = item;
	}
}
