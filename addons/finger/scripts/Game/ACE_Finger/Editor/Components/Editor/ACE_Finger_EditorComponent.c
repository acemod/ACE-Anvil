//------------------------------------------------------------------------------------------------
class ACE_Finger_EditorComponentClass : SCR_BaseEditorComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Attached to SCR_EditorManagerEntity in EditorManager.et
//! Equivalent to SCR_PingEditorComponent for finger pings
class ACE_Finger_EditorComponent : SCR_BaseEditorComponent
{
    [Attribute(defvalue: "1000", desc: "Maximum pointing distance in meters")]
    protected float m_fMaxPointingDistanceM;

    [Attribute(defvalue: "10", desc: "Range of the ping in meters. Only players in range will see it.")]
    protected float m_fPingRangeM;

    [Attribute(defvalue: "6", desc: "Lifetime of ping in seconds")]
    protected float m_fPingLifetimeS;

    [Attribute(defvalue: "1.5", desc: "Cooldown in seconds until next ping can be sent")]
    protected float m_fPingCooldownS;

    [Attribute(desc: "Effects of the ping")]
    protected ref array<ref SCR_BaseEditorEffect> m_PointingEffects;

    protected ref ScriptInvoker Event_OnPingEntityRegister = new ScriptInvoker;
    protected ref ScriptInvoker Event_OnPingEntityUnregister = new ScriptInvoker;
    protected float m_fLastPingTime = 0;

    //------------------------------------------------------------------------------------------------
    //! Method for local player to request pings on a position or target
    void SendPing(vector targetPos, SCR_EditableEntityComponent target = null)
    {
        float currentCooldown = m_fPingCooldownS - (GetGame().GetWorld().GetWorldTime() - m_fLastPingTime) / 1000;
        if (m_fLastPingTime > 0 && currentCooldown > 0)
        {
            SCR_NotificationsComponent.SendLocal(ENotification.ACTION_ON_COOLDOWN, currentCooldown * 100);
            return;
        };
        m_fLastPingTime = GetGame().GetWorld().GetWorldTime();

        IEntity player = GetGame().GetPlayerController().GetControlledEntity();
        if (!player)
            return;

        Rpc(RpcAsk_SendPing, GetGame().GetPlayerController().GetPlayerId(), player.GetOrigin(), targetPos, Replication.FindId(target));
    }

    //------------------------------------------------------------------------------------------------
    //! Broadcasts ping to all clients
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_SendPing(int reporterID, vector reporterPos, vector targetPos, RplId targetID)
    {
        // Send ping to server as well if not dedicated
        if (RplSession.Mode() != RplMode.Dedicated)
            RpcDo_SendPing(reporterID, reporterPos, targetPos, targetID);

        Rpc(RpcDo_SendPing, reporterID, reporterPos, targetPos, targetID);
    }

    //------------------------------------------------------------------------------------------------
    //! Creates ping effect and notification for local player
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_SendPing(int reporterID, vector reporterPos, vector targetPos, RplId targetID)
    {
        IEntity player = GetGame().GetPlayerController().GetControlledEntity();
        if (!player || vector.Distance(player.GetOrigin(), reporterPos) > m_fPingRangeM)
            return;

        SCR_EditableEntityComponent target = SCR_EditableEntityComponent.Cast(Replication.FindItem(targetID));
        set<SCR_EditableEntityComponent> targets = new set<SCR_EditableEntityComponent>;
        if (target)
            targets.Insert(target);

        ACE_Finger_EditorComponent localInstance = ACE_Finger_EditorComponent.Cast(ACE_Finger_EditorComponent.GetInstance(ACE_Finger_EditorComponent, true));
        SCR_BaseEditorEffect.Activate(m_PointingEffects, localInstance, targetPos, targets);

        if (target)
        {
            SCR_NotificationsComponent.SendLocal(ENotification.ACE_FINGER_PING_TARGET_ENTITY, reporterID, targetID);
            localInstance.OnPingEntityRegister(target);
        }
        else
        {
            SCR_NotificationsComponent.SendLocal(ENotification.ACE_FINGER_PING, targetPos, reporterID);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Calls register event when effect is a ping entity
    override void EOnEffect(SCR_BaseEditorEffect effect)
    {
        SCR_EntityEditorEffect entityEffect = SCR_EntityEditorEffect.Cast(effect);
        if (!entityEffect)
            return;

        OnPingEntityRegister(SCR_EditableEntityComponent.GetEditableEntity(entityEffect.GetEntity()));
    }

    //------------------------------------------------------------------------------------------------
    //! Schedules deletion of ping entity when registered
    protected void OnPingEntityRegister(SCR_EditableEntityComponent pingEntity)
    {
        Event_OnPingEntityRegister.Invoke(-1, pingEntity);
        GetGame().GetCallqueue().CallLater(Expire, m_fPingLifetimeS * 1000, false, pingEntity);
    }

    //------------------------------------------------------------------------------------------------
    //! Deletes the ping entity
    protected void Expire(SCR_EditableEntityComponent pingEntity)
    {
        if (!pingEntity)
            return;

        Event_OnPingEntityUnregister.Invoke(-1, pingEntity);

        if (SCR_EditorPingEntity.Cast(pingEntity.GetOwner()))
            pingEntity.Delete();
    }

    //------------------------------------------------------------------------------------------------
    //! Return maximum distance
    float GetMaxPointingDistance()
    {
        return m_fMaxPointingDistanceM;
    }

    //------------------------------------------------------------------------------------------------
    //! Returns invoker for ping entity registration event
    ScriptInvoker GetOnPingEntityRegister()
    {
        return Event_OnPingEntityRegister;
    }

    //------------------------------------------------------------------------------------------------
    //! Returns invoker for ping entity unregistration event
    ScriptInvoker GetOnPingEntityUnregister()
    {
        return Event_OnPingEntityUnregister;
    }
}
