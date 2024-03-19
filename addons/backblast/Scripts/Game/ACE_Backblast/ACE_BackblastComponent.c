//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "ACE Anvil", description: "ACE Backblast")]
class ACE_BackblastComponentClass : ScriptComponentClass
{
	static override array<typename> Requires(IEntityComponentSource src)
	{
		return {EventHandlerManagerComponent};
	}
}

//------------------------------------------------------------------------------------------------
class ACE_BackblastComponent : ScriptComponent
{
	[Attribute(defvalue: "0", desc: "Enable debug mode")]
	private bool m_bDebugModeEnabled;

	private const float INNER_RANGE = 15;
	private const float OUTER_RANGE = 10;
	private const float MAX_DAMAGE = 200;
	private const float CONE_DEG = 90;

	//------------------------------------------------------------------------------------------------
	protected override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		if (!Replication.IsRunning() && !Replication.IsServer())
			return;
		
		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
		eventHandlerManager.RegisterScriptHandler("OnProjectileShot", this, OnProjectileShot);
	}

	//------------------------------------------------------------------------------------------------
	private void OnProjectileShot(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{
		if (weapon.GetWeaponType() != EWeaponType.WT_ROCKETLAUNCHER)
			return;

		vector minBounds;
		vector maxBounds;
		IEntity weaponEntity = weapon.GetOwner();
		weaponEntity.GetBounds(minBounds, maxBounds);

		if (m_bDebugModeEnabled)
		{
			ACE_Drawing.DrawBounds(weaponEntity);
		}

		vector origin = {
			(maxBounds[0] - minBounds[0]) / 2,
			(maxBounds[1] - minBounds[1]) / 2,
			0
		};
		origin += minBounds;

		vector matrix[4];
		weaponEntity.GetWorldTransform(matrix);

		origin = origin.Multiply4(matrix);

		AimingComponent aimingComponent = AimingComponent.Cast(GetOwner().FindComponent(AimingComponent));
		vector weaponDir = aimingComponent.GetAimingDirectionWorld();
		Backblast(origin, weaponDir);
	}

	//------------------------------------------------------------------------------------------------
	private void Backblast(vector origin, vector weaponDir)
	{
		ACE_BackblastQueryCollector query = new ACE_BackblastQueryCollector(GetOwner());
		GetGame().GetWorld().QueryEntitiesBySphere(origin, INNER_RANGE, query.QueryCallback, null, EQueryEntitiesFlags.ALL);
		array<SCR_ChimeraCharacter> affectedEntities = query.GetAffectedEntities();

		if (affectedEntities.Count() == 0)
			return;

		foreach (SCR_ChimeraCharacter character : affectedEntities)
		{
			BaseDamageContext damageContext = new BaseDamageContext();
			damageContext.hitPosition = character.AimingPosition();
			damageContext.damageValue = CalcDamage(origin, weaponDir * -1, damageContext.hitPosition);

			if (damageContext.damageValue <= 0)
				continue;

			damageContext.hitDirection = origin - damageContext.hitPosition;
			damageContext.hitNormal = damageContext.hitDirection;

			SCR_DamageManagerComponent damageManager = character.GetDamageManager();
			if (!damageManager)
				continue;

			damageContext.damageType = EDamageType.EXPLOSIVE;
			damageContext.hitEntity = character;
			damageContext.struckHitZone = damageManager.GetDefaultHitZone();
			damageContext.instigator = Instigator.CreateInstigator(GetOwner());
			damageManager.HandleDamage(damageContext);
		}

		if (m_bDebugModeEnabled)
		{
			ACE_BackblastDamageFunction damageFunction = new ACE_BackblastDamageFunction(weaponDir * -1);
			ACE_Drawing.DrawDamageSphere(origin, 1000, damageFunction);
		}
	}

	//------------------------------------------------------------------------------------------------
	static float CalcDamage(vector origin, vector blastDir, vector impact)
	{
		vector dirVector = impact - origin;
		float distance = dirVector.Length();
		float dotProduct = vector.Dot(dirVector, blastDir);
		float angle = Math.Acos(dotProduct / distance) * Math.RAD2DEG;

		float angleFactor = Math.Max(0, 1 - angle / (CONE_DEG / 2));
		float angleRange = OUTER_RANGE + (INNER_RANGE - OUTER_RANGE) * angleFactor;
		float rangeFactor = Math.Max(0, 1 - distance / angleRange);

		return MAX_DAMAGE * rangeFactor * angleFactor;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_BackblastQueryCollector
{
	protected IEntity m_pOwner;
	protected ref array<SCR_ChimeraCharacter> m_aAffectedEntities = {};

	//------------------------------------------------------------------------------------------------
	void ACE_BackblastQueryCollector(IEntity owner)
	{
		m_pOwner = owner;
	}

	//------------------------------------------------------------------------------------------------
	bool QueryCallback(IEntity entity)
	{
		if (entity == m_pOwner)
			return true;

		auto character = SCR_ChimeraCharacter.Cast(entity);
		if (!character)
			return true;

		m_aAffectedEntities.Insert(character);
		return true;
	}

	array<SCR_ChimeraCharacter> GetAffectedEntities()
	{
		return m_aAffectedEntities;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_BackblastDamageFunction : ACE_DrawingDamageFunction
{
	private vector m_vBlastDirection;
	
	//------------------------------------------------------------------------------------------------
	void ACE_BackblastDamageFunction(vector blastDirection)
	{
		m_vBlastDirection = blastDirection;
	}
	
	//------------------------------------------------------------------------------------------------
	override float CalculateDamage(vector origin, vector target)
	{
		return ACE_BackblastComponent.CalcDamage(origin, m_vBlastDirection, target);
	}
}
