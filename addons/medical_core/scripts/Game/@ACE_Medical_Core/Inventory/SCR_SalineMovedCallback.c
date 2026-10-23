//------------------------------------------------------------------------------------------------
modded class SCR_SalineMovedCallback : ScriptedInventoryOperationCallback
{
	//------------------------------------------------------------------------------------------------
	override protected void DestroySalineBag(IEntity item)
	{
		if (!item)
			return super.DestroySalineBag(item);
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (settings && !settings.m_bLitterEnabled)
			return super.DestroySalineBag(item);
		
		SCR_ConsumableItemComponent consumableComponent = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if (consumableComponent)
			consumableComponent.ACE_Medical_SpawnLitter(item.GetRootParent().GetOrigin());
		
		super.DestroySalineBag(item);
	}
}
