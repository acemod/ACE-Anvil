//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	protected ref map<int, ACE_Finger_MapPointer> m_aACE_Finger_Pointers = new map<int, ACE_Finger_MapPointer>();
	
	//------------------------------------------------------------------------------------------------
	//! Create map pointer for the player
	override void OnPlayerConnected(int playerId)
	{
		super.OnPlayerConnected(playerId);
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		Resource res = Resource.Load("{177172E4559E8F95}Prefabs/Helpers/ACE_Finger_MapPointer.et");
		if (!res.IsValid())
			return;
		
		ACE_Finger_MapPointer ptr = ACE_Finger_MapPointer.Cast(GetGame().SpawnEntityPrefab(res));
		if (!ptr)
			return;
		
		ptr.InitServer(playerId);
		m_aACE_Finger_Pointers[playerId] = ptr;
		
		RplComponent ptrRpl = RplComponent.Cast(ptr.FindComponent(RplComponent));
		if (!ptrRpl)
			return;
		
		ptrRpl.Give(playerController.GetRplIdentity());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Delete map pointer of the player
	override protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);
		
		ACE_Finger_MapPointer ptr;
		if (m_aACE_Finger_Pointers.Find(playerId, ptr))
			SCR_EntityHelper.DeleteEntityAndChildren(ptr);
	}
}
