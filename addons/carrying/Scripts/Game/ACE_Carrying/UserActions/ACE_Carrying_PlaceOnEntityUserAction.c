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
		vector placementTransform[4];
		m_ACE_Carrying_PointInfo.GetWorldTransform(placementTransform);
		
		array<vector> placementPos = {placementTransform};
			
		ACE_Carrying_Tools.ReleaseFromCarrier(pUserEntity, placementPos);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Methods are executed on the local player
	override bool CanBroadcastScript() { return false; };
}
