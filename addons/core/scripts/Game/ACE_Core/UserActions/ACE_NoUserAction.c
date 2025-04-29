//------------------------------------------------------------------------------------------------
//! Used for removing user actions when overriding prefabs
class ACE_NoUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return false;
	}
}
