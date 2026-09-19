//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class SCR_MapCursorModule: SCR_MapModuleBase
{
	//------------------------------------------------------------------------------------------------
	//! Handle pointing state
	bool ACE_Finger_HandlePointing(bool active)
	{
		// begin pointing state
		if (active)
		{
			if (~m_CursorState & EMapCursorState.ACE_FINGER_CS_POINTER)
			{
				SetCursorState(EMapCursorState.ACE_FINGER_CS_POINTER);
				return true;
			}
		}
		// end pointing state
		else if (m_CursorState & EMapCursorState.ACE_FINGER_CS_POINTER)
		{
			UnsetCursorState(EMapCursorState.ACE_FINGER_CS_POINTER);
		}
		
		return false;
	}
}
