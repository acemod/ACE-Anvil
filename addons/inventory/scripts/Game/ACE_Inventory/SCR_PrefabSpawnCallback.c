modded class SCR_PrefabSpawnCallback
{
	protected SCR_ResourcePlayerControllerInventoryComponent m_ACE_Inventory_Requester;
	protected int m_ACE_Inventory_Token;
	protected int m_ACE_Inventory_Operation;

	//------------------------------------------------------------------------------------------------
	void SCR_PrefabSpawnCallback(BaseInventoryStorageComponent targetStorage)
	{
		// Capture only callbacks constructed inside this addon's native arsenal request.
		m_ACE_Inventory_Requester = SCR_ResourcePlayerControllerInventoryComponent.ACE_Inventory_GetDispatch();
		if (m_ACE_Inventory_Requester)
			m_ACE_Inventory_Requester.ACE_Inventory_CaptureSpawn(targetStorage, m_ACE_Inventory_Token, m_ACE_Inventory_Operation);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnComplete()
	{
		super.OnComplete();
		if (m_ACE_Inventory_Requester && m_ACE_Inventory_Token > 0)
			m_ACE_Inventory_Requester.ACE_Inventory_OnSpawnResult(m_ACE_Inventory_Token, m_ACE_Inventory_Operation, true);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnFailed()
	{
		super.OnFailed();
		if (m_ACE_Inventory_Requester && m_ACE_Inventory_Token > 0)
			m_ACE_Inventory_Requester.ACE_Inventory_OnSpawnResult(m_ACE_Inventory_Token, m_ACE_Inventory_Operation, false);
	}
}
