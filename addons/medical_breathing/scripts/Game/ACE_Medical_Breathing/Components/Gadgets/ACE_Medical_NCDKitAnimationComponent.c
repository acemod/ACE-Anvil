//------------------------------------------------------------------------------------------------
class ACE_Medical_NCDKitAnimationComponentClass : BaseItemAnimationComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_NCDKitAnimationComponent : BaseItemAnimationComponent
{
	protected IEntity m_pHose;
	protected IEntity m_pNeedle;
	protected int m_iAnimEventSpawnNeedle;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_NCDKitAnimationComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		if (!GetGame().InPlayMode())
			return;
		
		array<string> eventNames = {};
		SCR_Enum.GetEnumNames(ACE_Medical_ENCDKitAnimEvent, eventNames);
		
		foreach (string eventName : eventNames)
		{
			m_iAnimEventSpawnNeedle = GameAnimationUtils.RegisterAnimationEvent(eventName);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void OnAnimationEvent(AnimationEventID animEventType, AnimationEventID animUserString, int intParam, float timeFromStart, float timeToEnd)
	{
		super.OnAnimationEvent(animEventType, animUserString, intParam, timeFromStart, timeToEnd);
		ACE_Medical_ENCDKitAnimEvent globalEventID = typename.StringToEnum(ACE_Medical_ENCDKitAnimEvent, GameAnimationUtils.GetEventString(animEventType));
		
		IEntity owner = GetOwner();
		if (!owner)
			return;
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (rpl && rpl.Role() == RplRole.Authority)
			Rpc(RpcDo_HandleAnimationEventBroadcast, globalEventID);
		
		HandleAnimationEventLocal(globalEventID);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HandleAnimationEventLocal(AnimationEventID globalEventID)
	{
		IEntity user = GetOwner().GetRootParent();
		if (!user)
			return;
		
		Animation userAnim = user.GetAnimation();
		if (!userAnim)
			return;
		
		switch (globalEventID)
		{
			case ACE_Medical_ENCDKitAnimEvent.ACE_MEDICAL_EVENT_SPAWN_NEEDLE:
			{
				if (m_pNeedle)
					return;
				
				m_pNeedle = GetGame().SpawnEntityPrefabLocal(Resource.Load("{C687D937CA92EF2A}Prefabs/Items/Medicine/NCDKit/ACE_medical_NCDKit_Needle.et"));
				user.AddChild(m_pNeedle, userAnim.GetBoneIndex("RightHandProp"));
				m_pHose = GetGame().SpawnEntityPrefabLocal(Resource.Load("{1323E0ADD9389CFD}Prefabs/Items/Medicine/NCDKit/ACE_medical_NCDKit_Hose.et"));
				user.AddChild(m_pHose, userAnim.GetBoneIndex("RightHandProp"));
				break;
			}
			
			case ACE_Medical_ENCDKitAnimEvent.ACE_MEDICAL_EVENT_DETACH_HOSE:
			{
				if (!m_pHose)
					return;
				
				user.RemoveChild(m_pHose, true);
				//user.AddChild(m_pHose, userAnim.GetBoneIndex("LeftHandProp"));
				break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_HandleAnimationEventBroadcast(AnimationEventID globalEventID)
	{
		HandleAnimationEventLocal(globalEventID);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_Medical_NCDKitAnimationComponent()
	{
		delete m_pNeedle;
		delete m_pHose;
	}
}

//------------------------------------------------------------------------------------------------
enum ACE_Medical_ENCDKitAnimEvent
{
	ACE_MEDICAL_EVENT_SPAWN_NEEDLE,
	ACE_MEDICAL_EVENT_DETACH_HOSE
}