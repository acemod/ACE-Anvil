modded class SCR_InputButtonComponent
{
	protected bool m_bACE_Inventory_Hint;

	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_SetHint(string action, string label, int keyCount)
	{
		if (!m_sOldActionName.IsEmpty())
		{
			m_InputManager.RemoveActionListener(m_sOldActionName, EActionTrigger.DOWN, OnButtonPressed);
			m_InputManager.RemoveActionListener(m_sOldActionName, EActionTrigger.VALUE, OnButtonHold);
			m_InputManager.RemoveActionListener(m_sOldActionName, EActionTrigger.UP, ActionReleased);
		}
		m_bACE_Inventory_Hint = true;
		m_iMaxShownKeys = keyCount;
		SetAction(action, forceUpdate: true);
		SetLabel(label);
	}

	//------------------------------------------------------------------------------------------------
	override protected void SetInputAction()
	{
		// Quantity hints must not register modifier presses as inventory actions.
		if (m_bACE_Inventory_Hint)
		{
			m_sOldActionName = m_sActionName;
			return;
		}

		super.SetInputAction();
	}
}
