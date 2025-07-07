//------------------------------------------------------------------------------------------------
class ACE_Overheating_RackBoltState : ACE_FSM_IState<ACE_Overheating_WeaponAnimContext>
{
	// A reference to the context is needed for OnRackBoltAnimEvent
	protected ref ACE_Overheating_WeaponAnimContext m_pContext;
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Overheating_WeaponAnimContext context)
	{
		context.m_pCharAnim.CallCommand(context.m_iCharReloadCommandID, 1, 0);
		context.m_pWeaponAnim.CallCommand(context.m_iWeaponReloadCommandID, 1, 0);
		context.m_pCharController.GetOnAnimationEvent().Insert(OnRackBoltAnimEvent);
		context.m_pCharController.OverrideMaxSpeed(0.25);
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Overheating_WeaponAnimContext context, float timeSlice)
	{
		context.m_pCharController.SetWeaponADS(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Overheating_WeaponAnimContext context)
	{
		context.m_pCharController.GetOnAnimationEvent().Remove(OnRackBoltAnimEvent);
		context.m_pCharController.SetWeaponADS(context.m_bWasWeaponADS);
		context.m_pCharController.OverrideMaxSpeed(1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnRackBoltAnimEvent(AnimationEventID animEventType, AnimationEventID animUserString, int intParam, float timeFromStart, float timeToEnd)
	{
		if (animEventType != m_pContext.m_iRackBoltEvent)
			return;
		
		ClearJam(m_pContext);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clears jam and creates case ejection particle effect
	protected void ClearJam(ACE_Overheating_WeaponAnimContext context)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (playerController)
			playerController.ACE_Overheating_RequestClearJam(context.m_pJamComponent);
		
		BaseContainer weaponSource = context.m_pWeapon.GetOwner().GetPrefabData().GetPrefab();
	
		IEntityComponentSource weaponComponentSource = ACE_BaseContainerTools.FindComponentSource(weaponSource, WeaponComponent);
		if (!weaponComponentSource)
			return;
		
		IEntityComponentSource muzzleComponentSource =  ACE_BaseContainerTools.FindComponentSource(weaponComponentSource, MuzzleComponent);
		if (!muzzleComponentSource)
			return;
		
		IEntityComponentSource caseEjectionSource = ACE_BaseContainerTools.FindComponentSource(muzzleComponentSource, CaseEjectingEffectComponent);
		if (!caseEjectionSource)
			return;
		
		ResourceName particlePrefabName;
		caseEjectionSource.Get("ParticleEffect", particlePrefabName);
		EntitySlotInfo slotInfo;
		caseEjectionSource.Get("EffectPosition", slotInfo);
		
		if (!slotInfo)
			return;
					
		ParticleEffectEntitySpawnParams spawnParams = new ParticleEffectEntitySpawnParams();
		spawnParams.Parent =  context.m_pWeapon.GetOwner();
		spawnParams.PivotID = slotInfo.GetNodeId();
		slotInfo.GetLocalTransform(spawnParams.Transform);
		
		ParticleEffectEntity particleEmitter = ParticleEffectEntity.SpawnParticleEffect(particlePrefabName, spawnParams);
		Particles particles = particleEmitter.GetParticles();
		particles.SetParam(0, EmitterParam.EMITTING_TIME, 1);
		particles.MultParam(0, EmitterParam.VELOCITY, 0.5);
		particles.MultParam(0, EmitterParam.VELOCITY_RND, 0.5);
		particles.SetParam(1, EmitterParam.EMITTING_TIME, 0);
		particles.SetParam(2, EmitterParam.EMITTING_TIME, 0);
	}
}
