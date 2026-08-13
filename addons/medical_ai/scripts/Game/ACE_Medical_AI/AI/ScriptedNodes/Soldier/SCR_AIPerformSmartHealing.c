//------------------------------------------------------------------------------------------------
//! Extend to support heal station actions
modded class SCR_AIPerformSmartHealing : AITaskScripted
{
	protected SCR_HealSupportStationAction m_ACE_SupportStationAction;
	
	//------------------------------------------------------------------------------------------------
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		if (m_ACE_SupportStationAction)
			return ACE_Medical_OnPerformSupportStationAction(owner, dt);
		
		ENodeResult result = super.EOnTaskSimulate(owner, dt);
		if (result != ENodeResult.FAIL)
			return result;
		
		if (m_iItemUsageInProgress != USAGE_NONE)
			return ENodeResult.FAIL;
		
		if (!m_sUserAction.IsInherited(SCR_HealSupportStationAction))
			return ENodeResult.FAIL;
		
		m_ACE_SupportStationAction = ACE_Medical_FindUsableSupportStationAction(owner, dt);
		if (m_ACE_SupportStationAction)
		{
			m_iItemUsageInProgress = USAGE_WAITING_TO_START;
			return ENodeResult.RUNNING;
		}
		
		return ENodeResult.FAIL;
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_HealSupportStationAction ACE_Medical_FindUsableSupportStationAction(AIAgent owner, float dt)
	{
		IEntity controlledEntity = owner.GetControlledEntity();
		if (!controlledEntity)
			return null;
		
		array<BaseUserAction> outActions = {};
		SCR_HealSupportStationAction action;
		GetActions(m_targetEntity, outActions);
		foreach (BaseUserAction baseAction : outActions)
		{
			action = SCR_HealSupportStationAction.Cast(baseAction);
			if (!action)
				continue;
			
			// Find the action on the correct hitzone group
			if (action.GetHitZoneGroup() != GetTargetGroup(m_targetEntity))
				continue;
			
			if (m_sUserAction != action.Type())
				continue;
			
			if (!action.CanBeShownScript(controlledEntity))
				continue;
			
			if (!action.CanBePerformedScript(controlledEntity))
				continue;
			
			return action;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected ENodeResult ACE_Medical_OnPerformSupportStationAction(AIAgent owner, float dt)
	{
		ChimeraCharacter controlledEntity = ChimeraCharacter.Cast(owner.GetControlledEntity());
		if (!controlledEntity)
			return ENodeResult.FAIL;
		
		if (m_iItemUsageInProgress == USAGE_WAITING_TO_START)
		{
			m_iItemUsageInProgress = USAGE_IN_PROGRESS;
			m_ACE_SupportStationAction.OnActionStart(controlledEntity);
			return ENodeResult.RUNNING;
		}
		
		if (controlledEntity.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
		{
			if (USAGE_IN_PROGRESS)
				m_ACE_SupportStationAction.OnActionCanceled(m_targetEntity, controlledEntity);
			
			return ENodeResult.FAIL;
		}
		
		if (!m_ACE_SupportStationAction.CanBeShownScript(controlledEntity))
		{
			if (USAGE_IN_PROGRESS)
				m_ACE_SupportStationAction.OnActionCanceled(m_targetEntity, controlledEntity);
			
			return ENodeResult.SUCCESS;
		}
		
		if (!m_ACE_SupportStationAction.CanBePerformedScript(controlledEntity))
		{
			if (USAGE_IN_PROGRESS)
				m_ACE_SupportStationAction.OnActionCanceled(m_targetEntity, controlledEntity);
			
			return ENodeResult.SUCCESS;
		}
		
		m_ACE_SupportStationAction.PerformContinuousAction(m_targetEntity, controlledEntity, dt);
		return ENodeResult.RUNNING;
	}
}
