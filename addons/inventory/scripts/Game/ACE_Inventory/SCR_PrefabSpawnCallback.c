modded class SCR_PrefabSpawnCallback
{
	protected SCR_ResourcePlayerControllerInventoryComponent m_pACE_Inventory_Requester;
	protected int m_iACE_Inventory_Token;
	protected int m_iACE_Inventory_Operation;

	//------------------------------------------------------------------------------------------------
	void SCR_PrefabSpawnCallback(BaseInventoryStorageComponent targetStorage)
	{
		// Capture only callbacks constructed inside this addon's native arsenal request.
		m_pACE_Inventory_Requester = SCR_ResourcePlayerControllerInventoryComponent.ACE_Inventory_GetDispatch();
		if (m_pACE_Inventory_Requester)
			m_pACE_Inventory_Requester.ACE_Inventory_CaptureSpawn(targetStorage, m_iACE_Inventory_Token, m_iACE_Inventory_Operation);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnComplete()
	{
		super.OnComplete();
		if (m_pACE_Inventory_Requester && m_iACE_Inventory_Token > 0)
			m_pACE_Inventory_Requester.ACE_Inventory_OnSpawnResult(m_iACE_Inventory_Token, m_iACE_Inventory_Operation, true);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnFailed()
	{
		super.OnFailed();
		if (m_pACE_Inventory_Requester && m_iACE_Inventory_Token > 0)
			m_pACE_Inventory_Requester.ACE_Inventory_OnSpawnResult(m_iACE_Inventory_Token, m_iACE_Inventory_Operation, false);
	}
}
