//------------------------------------------------------------------------------------------------
modded class SCR_ChimeraAIAgent : ChimeraAIAgent
{
	//------------------------------------------------------------------------------------------------
	//! Same as vanilla, but called when character is not bleeding
	override protected void SendWoundedMsg()
	{
		super.SendWoundedMsg();
		IEntity controlled = GetControlledEntity();
		
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(controlled.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageMgr)
			return;
		
		// Bleeding case is already handled by base game
		if (damageMgr.IsBleeding())
			return;
		
		SCR_AIGroup msgReceiverGroup = null;
		
		// if wounded is AI
		SCR_AIGroup myGroup = SCR_AIGroup.Cast(GetParentGroup());
		if (myGroup)
			msgReceiverGroup = myGroup;			// Send to AI group
		
		// if wounded is player
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(controlled);
		if (playerId != 0)
		{
			SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
			if (!groupsManager)
				return;
			
			SCR_AIGroup playerGroup = groupsManager.GetPlayerGroup(playerId);
			if (!playerGroup)
				return;
			
			SCR_AIGroup slaveGroup = playerGroup.GetSlave();
			
			if (slaveGroup)
				msgReceiverGroup = slaveGroup;	// Send to our slave group - this is the one which has AIs and will heal us
			else
				msgReceiverGroup = playerGroup;	// Send to our group
		}
		
		if (!msgReceiverGroup)
			return;
		
		// Inject message to the group mailbox directly.
		// This bypasses problems of our own mailbox being disabled (because character is possessed by GM, or because we are unconscious)
		AICommunicationComponent comms = msgReceiverGroup.GetCommunicationComponent();
		if (!comms)
			return;
		
		SCR_AIMessage_Wounded msg = SCR_AIMessage_Wounded.Create(GetControlledEntity());
		msg.SetReceiver(msgReceiverGroup);
		comms.RequestBroadcast(msg, msgReceiverGroup);
	} 
}
