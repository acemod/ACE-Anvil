[ComponentEditorProps(category: "ACE Anvil", description: "ACE Backblast")]
class ACE_BackblastComponentClass : ScriptComponentClass
{
	static override array<typename> Requires(IEntityComponentSource src)
	{
		return {EventHandlerManagerComponent};
	}
}

class ACE_BackblastComponent : ScriptComponent
{
	[Attribute(defvalue: "0", desc: "Enable debug mode")]
	private bool m_bDebugModeEnabled;

	private IEntity m_Owner;
	private const float INNER_RANGE = 15;
	private const float OUTER_RANGE = 10;
	private const float MAX_DAMAGE = 200;
	private const float CONE_DEG = 90;

	protected override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_Owner = owner;

		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
		eventHandlerManager.RegisterScriptHandler("OnProjectileShot", this, OnProjectileShot);
	}

	private void OnProjectileShot(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{
		if (weapon.GetWeaponType() != EWeaponType.WT_ROCKETLAUNCHER)
		{
			return;
		}

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

		AimingComponent aimingComponent = AimingComponent.Cast(m_Owner.FindComponent(AimingComponent));
		vector weaponDir = aimingComponent.GetAimingDirectionWorld();
		Backblast(origin, weaponDir);
	}

	private void Backblast(vector origin, vector weaponDir)
	{
		ACE_BackblastQueryCollector query = new ACE_BackblastQueryCollector(m_Owner);
		GetGame().GetWorld().QueryEntitiesBySphere(origin, INNER_RANGE, query.QueryCallback, null, EQueryEntitiesFlags.ALL);
		array<SCR_ChimeraCharacter> affectedEntities = query.GetAffectedEntities();

		if (affectedEntities.Count() == 0)
		{
			return;
		}

		foreach (SCR_ChimeraCharacter character : affectedEntities)
		{
			vector impact = character.AimingPosition();
			vector dirVector = origin - impact;
			float damage = CalcDamage(origin, weaponDir * -1, impact);

			if (damage <= 0)
			{
				continue;
			}

			vector hitPosDirNorm[3] = {
				impact,
				dirVector,
				dirVector
			};

			SCR_DamageManagerComponent damageManager = character.GetDamageManager();
			HitZone hitZone = damageManager.GetDefaultHitZone();
			Instigator instigator = Instigator.CreateInstigator(m_Owner);
			damageManager.HandleDamage(EDamageType.EXPLOSIVE, damage, hitPosDirNorm, character, hitZone, instigator, null, 0, 0);
		}

		if (m_bDebugModeEnabled)
		{
			ACE_BackblastDamageFunction damageFunction = new ACE_BackblastDamageFunction(weaponDir * -1);
			ACE_Drawing.DrawDamageSphere(origin, 1000, damageFunction);
		}
	}

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

class ACE_BackblastQueryCollector
{
	IEntity Owner;
	ref array<SCR_ChimeraCharacter> affectedEntities = {};

	void ACE_BackblastQueryCollector(IEntity owner)
	{
		Owner = owner;
	}

	bool QueryCallback(IEntity entity)
	{
		if (entity == Owner)
		{
			return false;
		}

		auto character = SCR_ChimeraCharacter.Cast(entity);
		if (!character)
		{
			return false;
		}

		affectedEntities.Insert(character);
		return false;
	}

	array<SCR_ChimeraCharacter> GetAffectedEntities()
	{
		return affectedEntities;
	}
}

class ACE_BackblastDamageFunction : ACE_DrawingDamageFunction
{
	private vector m_vBlastDirection;

	void ACE_BackblastDamageFunction(vector blastDirection)
	{
		m_vBlastDirection = blastDirection;
	}

	override float CalculateDamage(vector origin, vector target)
	{
		return ACE_BackblastComponent.CalcDamage(origin, m_vBlastDirection, target);
	}
}
