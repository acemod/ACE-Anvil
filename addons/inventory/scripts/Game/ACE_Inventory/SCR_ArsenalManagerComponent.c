modded class SCR_ArsenalManagerComponent
{
	//------------------------------------------------------------------------------------------------
	override static void OnItemRefunded_S(notnull IEntity item, notnull PlayerController playerController, SCR_ArsenalComponent arsenal)
	{
		// Native refund validation and resource generation precede this hook.
		SCR_ResourcePlayerControllerInventoryComponent requester = SCR_ResourcePlayerControllerInventoryComponent.ACE_Inventory_GetRefundDispatch();
		if (requester)
			requester.ACE_Inventory_CaptureRefundAcceptance(item, playerController, arsenal);
		super.OnItemRefunded_S(item, playerController, arsenal);
	}
}
