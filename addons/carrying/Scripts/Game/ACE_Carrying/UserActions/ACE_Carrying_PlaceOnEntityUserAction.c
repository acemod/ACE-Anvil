//------------------------------------------------------------------------------------------------
class ACE_Carrying_PlaceOnEntityUserAction: ScriptedUserAction 
{
	[Attribute(desc: "Placement Info")]
	private ref PointInfo m_ACE_Carrying_PointInfo;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		if (m_ACE_Carrying_PointInfo)
			m_ACE_Carrying_PointInfo.Init(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{		
		return ACE_Carrying_Tools.IsCarrier(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		IEntity carried = ACE_Carrying_Tools.GetCarried(pUserEntity);
		if (!carried)
			return;
					
		ACE_Carrying_Tools.PlaceOnEntity(pOwnerEntity, carried, m_ACE_Carrying_PointInfo);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Methods are executed on the local player
	override bool CanBroadcastScript() { return false; };
}
