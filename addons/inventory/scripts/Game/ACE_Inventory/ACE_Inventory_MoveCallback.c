class ACE_Inventory_MoveCallback : ScriptedInventoryOperationCallback
{
	protected SCR_InventoryMenuUI m_Menu;
	protected int m_Token;

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_MoveCallback(SCR_InventoryMenuUI menu, int token)
	{
		m_Menu = menu;
		m_Token = token;
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnComplete()
	{
		if (m_Menu)
			m_Menu.ACE_Inventory_OnMoveResult(m_Token, true);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnFailed()
	{
		if (m_Menu)
			m_Menu.ACE_Inventory_OnMoveResult(m_Token, false);
	}
}
