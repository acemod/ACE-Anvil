//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
modded class SCR_SelectionMenu
{
	protected ref ScriptInvoker<SCR_SelectionMenu> m_ACE_OnBackInputClose;
	
	//------------------------------------------------------------------------------------------------
	//! Same as vanilla, but with invoker
	override protected void OnBackInput()
	{
		// Prevent entry performing
		m_SelectedEntry = null;

		// Should close on top level
		if (CurrentCategory())
		{
			LeaveCategory();
		}
		else
		{
			// Close on top level
			Close();
			m_ACE_OnBackInputClose.Invoke(this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker ACE_GetOnBackInputClose()
	{
		if (!m_ACE_OnBackInputClose)
			m_ACE_OnBackInputClose = new ScriptInvoker();

		return m_ACE_OnBackInputClose;
	}
}
