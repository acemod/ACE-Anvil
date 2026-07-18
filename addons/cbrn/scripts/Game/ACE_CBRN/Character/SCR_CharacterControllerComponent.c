//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	void ACE_PerformHitReaction(EHitReactionType reactionType, float direction)
	{
		Rpc(RpcDo_ACE_PerformHitReactionOwner, reactionType, direction);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_PerformHitReactionOwner(EHitReactionType reactionType, float direction)
	{
		GetInputContext().SetHit(reactionType, direction);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);
		
		if (!Replication.IsServer())
			return;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return;

		SCR_CharacterDamageManagerComponent ownerCharDamageManager = SCR_CharacterDamageManagerComponent.Cast(ownerChar.GetDamageManager());
		if (!ownerCharDamageManager)
			return;
		
		ownerCharDamageManager.ACE_CBRN_UpdateSpasms();
	}
}
