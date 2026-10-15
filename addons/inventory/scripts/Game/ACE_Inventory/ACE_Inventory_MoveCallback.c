class ACE_Inventory_MoveCallback : ScriptedInventoryOperationCallback
{
	protected SCR_InventoryMenuUI m_pMenu;
	protected int m_iToken;
	protected int m_iOperationIndex;

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_MoveCallback(SCR_InventoryMenuUI menu, int token, int operationIndex)
	{
		m_pMenu = menu;
		m_iToken = token;
		m_iOperationIndex = operationIndex;
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnComplete()
	{
		if (m_pMenu)
			m_pMenu.ACE_Inventory_OnMoveResult(m_iToken, m_iOperationIndex, true);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnFailed()
	{
		if (m_pMenu)
			m_pMenu.ACE_Inventory_OnMoveResult(m_iToken, m_iOperationIndex, false);
	}
}
