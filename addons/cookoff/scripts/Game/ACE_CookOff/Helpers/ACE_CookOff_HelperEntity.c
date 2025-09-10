//------------------------------------------------------------------------------------------------
class ACE_CookOff_HelperEntityClass : GenericEntityClass
{
	[Attribute()]
	protected ref SCR_AudioSourceConfiguration m_AudioSourceConfigBullet;

	[Attribute()]
	protected ref SCR_AudioSourceConfiguration m_AudioSourceConfigShell;

	[Attribute()]
	protected ref SCR_AudioSourceConfiguration m_AudioSourceConfigRocket;
	
	//------------------------------------------------------------------------------------------------
	SCR_AudioSourceConfiguration GetAudioSourceConfig(ACE_CookOff_EAmmoType ammoType)
	{
		switch (ammoType)
		{
			case ACE_CookOff_EAmmoType.BULLET : { return m_AudioSourceConfigBullet; };
			case ACE_CookOff_EAmmoType.SHELL : { return m_AudioSourceConfigShell; };
			case ACE_CookOff_EAmmoType.ROCKET : { return m_AudioSourceConfigRocket; };
		}
		
		return null;
	}
}


//------------------------------------------------------------------------------------------------
class ACE_CookOff_HelperEntity : GenericEntity
{
	protected static ACE_CookOff_HelperEntityClass s_pData;
	
	//------------------------------------------------------------------------------------------------
	void ACE_CookOff_HelperEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!s_pData)
			s_pData = ACE_CookOff_HelperEntityClass.Cast(GetPrefabData());
	}
	
	//------------------------------------------------------------------------------------------------
	void DetonateAmmoInMagazine(notnull BaseMagazineComponent magazine, notnull IEntity parent)
	{
		ACE_CookOff_EAmmoType ammoType = ACE_CookOff_EAmmoType.BULLET;
		
		IEntity magazineEntity = magazine.GetOwner();
		if (!magazineEntity)
			return;
		
		if (magazineEntity.FindComponent(ShellMoveComponent))
			ammoType = ACE_CookOff_EAmmoType.SHELL;
		else if (magazineEntity.FindComponent(MissileMoveComponent))
			ammoType = ACE_CookOff_EAmmoType.ROCKET;
		
		if (!System.IsConsoleApp())
			RpcDo_PlayCookoffSoundBroadcast(ammoType);
		
		Rpc(RpcDo_PlayCookoffSoundBroadcast, ammoType);
		
		if (Math.RandomFloat(0, 1) > 0.8)
			return;
		
		IEntityComponentSource magazineSrc = magazine.GetComponentSource(magazine.GetOwner());
		if (!magazineSrc)
			return;
		
		ResourceName ammoConfigName;
		magazineSrc.Get("AmmoConfig", ammoConfigName);
		
		Resource ammoConfigRes = Resource.Load(ammoConfigName);
		if (!ammoConfigRes.IsValid())
			return;
		
		BaseContainer ammonConfigSrc = ammoConfigRes.GetResource().ToBaseContainer();
		if (!ammonConfigSrc)
			return;
		
		array<ResourceName> ammoNames = {};
		ammonConfigSrc.Get("AmmoResourceArray", ammoNames);
		SpawnProjectile(ACE_BaseContainerTools.ToInternalResourcename(ammoNames.GetRandomElement()), parent);
	}
	
	//------------------------------------------------------------------------------------------------
	void SpawnExplosion(ResourceName explosiveName, notnull IEntity parent)
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.Parent = parent;
		params.Transform[3] = Vector(Math.RandomFloat(-0.2, 0.2), Math.RandomFloat(0, 3), Math.RandomFloat(-0.2, 0.2));
		IEntity explosive = GetGame().SpawnEntityPrefab(Resource.Load(explosiveName), null, params);
		GetGame().GetCallqueue().CallLater(DetonateExplosive, 100, false, explosive, parent);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetonateExplosive(IEntity explosive, IEntity parent)
	{
		RplComponent rpl = RplComponent.Cast(explosive.FindComponent(RplComponent));
		Rpc(RpcDo_DetonateExplosiveBroadcast, rpl.Id());
		RpcDo_DetonateExplosiveBroadcast(rpl.Id());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SpawnProjectile(ResourceName ammoName, IEntity parent)
	{
		RplComponent parentRpl = RplComponent.Cast(parent.FindComponent(RplComponent));
		if (!parentRpl)
			return;
		
		vector localPos = Vector(Math.RandomFloat(-0.2, 0.2), Math.RandomFloat(0, 3), Math.RandomFloat(-0.2, 0.2));
		vector dir = Vector(Math.RandomFloat(-1, 1), Math.RandomFloat(-0.2, 0.8), Math.RandomFloat(-1, 1));
		float initSpeedCoef = Math.RandomFloat(0.001, 0.025);
		
		Rpc(RpcDo_SpawnProjectileBroadcast, ammoName, localPos, dir, initSpeedCoef, parentRpl.Id());
		RpcDo_SpawnProjectileBroadcast(ammoName, localPos, dir, initSpeedCoef, parentRpl.Id());
	}
			
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_PlayCookoffSoundBroadcast(ACE_CookOff_EAmmoType ammoType)
	{
		SCR_SoundManagerModule.CreateAndPlayAudioSource(this, s_pData.GetAudioSourceConfig(ammoType));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_SpawnProjectileBroadcast(ResourceName ammoName, vector localPos, vector dir, float initSpeedCoef, RplId parentID)
	{
		RplComponent parentRpl = RplComponent.Cast(Replication.FindItem(parentID));
		if (!parentRpl)
			return;
		
		IEntity parent = parentRpl.GetEntity();
		if (!parent)
			return;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Parent = parent;
		params.Transform[3] = localPos;
		
		Resource ammoRes = Resource.Load(ammoName);
		if (!ammoRes.IsValid())
			return;
		
		IEntity projectile = GetGame().SpawnEntityPrefab(ammoRes, null, params);
		if (!projectile)
			return;
		
		ProjectileMoveComponent moveComp = ProjectileMoveComponent.Cast(projectile.FindComponent(ProjectileMoveComponent));
		if (!moveComp)
			return;
		
		moveComp.Launch(dir, parent.GetPhysics().GetVelocity(), initSpeedCoef, projectile, null, null, null, null);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_DetonateExplosiveBroadcast(RplId explosiveID)
	{
		RplComponent rpl = RplComponent.Cast(Replication.FindItem(explosiveID));
		if (!rpl)
			return;
		
		IEntity explosive = rpl.GetEntity();
		if (!explosive)
			return;
		
		BaseTriggerComponent trigger = BaseTriggerComponent.Cast(explosive.FindComponent(BaseTriggerComponent));
		if (trigger)
			trigger.SetLive();
		
		SCR_DamageManagerComponent damagemanager = SCR_DamageManagerComponent.Cast(explosive.FindComponent(SCR_DamageManagerComponent));
		if (damagemanager)
			damagemanager.SetHealthScaled(0);
	}
}

//------------------------------------------------------------------------------------------------
enum ACE_CookOff_EAmmoType
{
	BULLET,
	SHELL,
	ROCKET
}
