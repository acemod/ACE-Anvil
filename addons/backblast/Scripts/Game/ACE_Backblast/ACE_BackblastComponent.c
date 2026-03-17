//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "ACE Anvil", description: "ACE Backblast")]
class ACE_BackblastComponentClass : ScriptComponentClass {
    static override array<typename> Requires(IEntityComponentSource src) {
        return { EventHandlerManagerComponent };
    }
}

//------------------------------------------------------------------------------------------------
class ACE_BackblastComponent : ScriptComponent {
    [Attribute(defvalue: "0", desc: "Enable debug mode")]
    protected bool m_bDebugModeEnabled;

    protected const float INNER_RANGE_M = 15;
    protected const float OUTER_RANGE_M = 10;
    protected const float MAX_DAMAGE = 25;
    protected const float CONE_DEG = 90;
    protected const float BLEEDING_FACTOR = 2.0;  //<- Bleeding probability is BLEEDING_FACTOR * damage / MAX_DAMAGE

    //------------------------------------------------------------------------------------------------
    protected override void OnPostInit(IEntity owner) {
        super.OnPostInit(owner);

        if (!GetGame().InPlayMode())
            return;

        if (!Replication.IsRunning() && !Replication.IsServer())
            return;

        EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
        eventHandlerManager.RegisterScriptHandler("OnProjectileShot", this, OnProjectileShot);
    }

    //------------------------------------------------------------------------------------------------
    protected void OnProjectileShot(int playerID, BaseWeaponComponent weapon, IEntity entity) {
        if (weapon.GetWeaponType() != EWeaponType.WT_ROCKETLAUNCHER)
            return;

        vector minBounds;
        vector maxBounds;

        IEntity weaponEntity = weapon.GetOwner();
        if (!weaponEntity)
            return;

        weaponEntity.GetBounds(minBounds, maxBounds);

        if (m_bDebugModeEnabled) {
            ACE_Drawing.DrawBounds(weaponEntity);
        }

        vector origin = { (maxBounds[0] - minBounds[0]) / 2, (maxBounds[1] - minBounds[1]) / 2, 0 };
        origin += minBounds;

        vector transform[4];
        weaponEntity.GetWorldTransform(transform);
        origin = origin.Multiply4(transform);
        vector weaponDir = transform[2];
        Backblast(origin, weaponDir);
    }

    //------------------------------------------------------------------------------------------------
    protected void Backblast(vector origin, vector weaponDir) {
        ACE_BackblastQueryCollector query = new ACE_BackblastQueryCollector(GetOwner());
        GetGame().GetWorld().QueryEntitiesBySphere(origin, INNER_RANGE_M, query.QueryCallback);
        array<SCR_ChimeraCharacter> affectedEntities = query.GetAffectedEntities();

        if (affectedEntities.IsEmpty())
            return;

        foreach (SCR_ChimeraCharacter character: affectedEntities) {
            SCR_DamageManagerComponent damageManager = character.GetDamageManager();
            if (!damageManager)
                continue;

            vector charOrigin = character.GetOrigin();

            Animation anim = character.GetAnimation();
            if (!anim)
                continue;

            array<HitZone> hitZones = {};
            damageManager.GetPhysicalHitZones(hitZones);

            foreach (HitZone hitZone: hitZones) {
                array<int> nodes = {};
                hitZone.GetColliderIDs(nodes);
                if (nodes.IsEmpty())
                    continue;

                vector nodeTransform[4];
                anim.GetBoneMatrix(nodes[0], nodeTransform);
                vector hitPosition = charOrigin + nodeTransform[3];
                float damage = CalcDamage(origin, -weaponDir, hitPosition, { GetOwner().GetRootParent(), character });

                if (damage <= 0)
                    continue;

                hitZone.HandleDamage(damage, EDamageType.EXPLOSIVE, GetOwner());

                // Probability of bleeding scales with percent damage
                if (Math.RandomFloatInclusive(0, 1) > BLEEDING_FACTOR * damage / MAX_DAMAGE)
                    continue;

                SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitZone);
                if (!charHitZone)
                    continue;

                charHitZone.AddBleeding(nodes[0]);
            };
        }

        if (m_bDebugModeEnabled) {
            ACE_BackblastDrawDamageFunction damageFunction = new ACE_BackblastDrawDamageFunction(weaponDir * -1);
            ACE_Drawing.DrawDamageSphere(origin, 1000, damageFunction);
        }
    }

    //------------------------------------------------------------------------------------------------
    static float CalcDamage(vector origin, vector blastDir, vector impact, array<IEntity> excludedEntities = null) {
        // Check if there's no obstacle in the path
        TraceParam trace = new TraceParam();
        trace.Flags = TraceFlags.ENTS | TraceFlags.WORLD | TraceFlags.OCEAN;
        trace.Start = origin;
        trace.End = impact;
        trace.ExcludeArray = excludedEntities;

        if (GetGame().GetWorld().TraceMove(trace, null) < 0.999)
            return 0;

        vector dirVector = impact - origin;
        float distance = dirVector.Length();
        float dotProduct = vector.Dot(dirVector, blastDir);
        float angle = Math.Acos(dotProduct / distance) * Math.RAD2DEG;

        float angleFactor = Math.Max(0, 1 - angle / (CONE_DEG / 2));
        float angleRange = OUTER_RANGE_M + (INNER_RANGE_M - OUTER_RANGE_M) * angleFactor;
        float rangeFactor = Math.Max(0, 1 - distance / angleRange);

        return MAX_DAMAGE * rangeFactor * angleFactor;
    }
}

//------------------------------------------------------------------------------------------------
class ACE_BackblastQueryCollector {
    protected IEntity m_pOwner;
    protected ref array<SCR_ChimeraCharacter> m_aAffectedEntities = {};

    //------------------------------------------------------------------------------------------------
    void ACE_BackblastQueryCollector(IEntity owner) {
        m_pOwner = owner;
    }

    //------------------------------------------------------------------------------------------------
    bool QueryCallback(IEntity entity) {
        if (entity == m_pOwner)
            return true;

        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
        if (!character)
            return true;

        m_aAffectedEntities.Insert(character);
        return true;
    }

    array<SCR_ChimeraCharacter> GetAffectedEntities() {
        return m_aAffectedEntities;
    }
}

//------------------------------------------------------------------------------------------------
class ACE_BackblastDrawDamageFunction : ACE_DrawingDamageFunction {
    protected vector m_vBlastDirection;

    //------------------------------------------------------------------------------------------------
    void ACE_BackblastDrawDamageFunction(vector blastDirection) {
        m_vBlastDirection = blastDirection;
    }

    //------------------------------------------------------------------------------------------------
    override float CalculateDamage(vector origin, vector target) {
        return ACE_BackblastComponent.CalcDamage(origin, m_vBlastDirection, target);
    }
}
