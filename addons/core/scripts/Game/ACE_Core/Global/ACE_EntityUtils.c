//------------------------------------------------------------------------------------------------
class ACE_EntityUtils
{
	//------------------------------------------------------------------------------------------------
	//! Returns true if the character is a player. Also returns false for GM-possessed AI.
	static bool IsPlayer(IEntity entity)
	{
		SCR_ECharacterControlType controlType = SCR_CharacterHelper.GetCharacterControlType(entity);
		if (controlType == SCR_ECharacterControlType.AI || controlType == SCR_ECharacterControlType.POSSESSED_AI)
			return false;
		
		return true;
	}
}
