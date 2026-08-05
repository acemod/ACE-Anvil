//------------------------------------------------------------------------------------------------
//! Same as vanilla, but keeps searching if a suitable medic is just temporarily busy elsewhere
modded class SCR_AIFindMedic : AITaskScripted
{
	[Attribute(defvalue: "5", desc: "Timeout for next search in seconds if required consumables are available, but AI was busy.")]
	protected float m_fACE_Medical_Timeout;
	protected float m_fACE_Medical_Timer;
	
	protected SCR_EConsumableType m_eACE_Medical_RequiredConsumableType;
	protected bool m_bACE_Medical_IsRequiredConsumableAvailable;
	
	//------------------------------------------------------------------------------------------------
	//! Schedule new search if required comsumables are available, but AI was busy
	override ENodeResult EOnTaskSimulate(AIAgent owner, float dt)
	{
		m_fACE_Medical_Timer -= dt;
		if (m_fACE_Medical_Timer > 0)
			return ENodeResult.RUNNING;
		
		m_fACE_Medical_Timer = m_fACE_Medical_Timeout;
		
		ChimeraCharacter patient;
		GetVariableIn(PORT_ENTITY_IN, patient);
		ACE_Medical_AIDecideMedicAction task = new ACE_Medical_AIDecideMedicAction();
		m_eACE_Medical_RequiredConsumableType = task.SelectConsumableTypeToApply(patient);
		
		if (m_eACE_Medical_RequiredConsumableType == SCR_EConsumableType.NONE)
			ENodeResult.FAIL;
		
		m_bACE_Medical_IsRequiredConsumableAvailable = false;
		ENodeResult result = super.EOnTaskSimulate(owner, dt);
		
		if (result != ENodeResult.FAIL || !m_bACE_Medical_IsRequiredConsumableAvailable)
			return result;
		
		return ENodeResult.RUNNING;
	}
	
	//----------------------------------------------------------------------------------------------------------------------------------------------
	//! Same as vanilla, but check for required consumable item
	override protected AIAgent FindAgentMedic(IEntity entToHeal, notnull array<AIAgent> agents, notnull array<AIAgent> agentsToExclude, bool excludeDrivers = false)
	{
		ChimeraCharacter charToHeal = ChimeraCharacter.Cast(entToHeal);
		if (!charToHeal)
			return null;
		
		AIAgent closestMedic = null;
		float closestMedicDistance = float.MAX;
		
		foreach (AIAgent agent : agents)
		{
			SCR_ChimeraAIAgent chimera = SCR_ChimeraAIAgent.Cast(agent);
			SCR_AIInfoComponent info = chimera.m_InfoComponent;
			if (!info)
				continue;
			
			if(!agent.GetControlledEntity())
				continue;
			
			if(agent.GetControlledEntity() == entToHeal)
				continue;
			
			if(agentsToExclude.Contains(agent))
				continue;
			
			if (info.HasUnitState(EUnitState.UNCONSCIOUS))
				continue;
			
			if (excludeDrivers)
			{
				if (info.HasUnitState(EUnitState.PILOT))
					continue;
			}
			
			if (!info.ACE_Medical_HasConsumable(m_eACE_Medical_RequiredConsumableType))
				continue;
			
			m_bACE_Medical_IsRequiredConsumableAvailable = true;
			
			if (info.GetAIState() != EUnitAIState.AVAILABLE)
				continue;
			
			float dist = vector.DistanceSq(agent.GetControlledEntity().GetOrigin(), entToHeal.GetOrigin());
			if (dist < closestMedicDistance)
			{
				closestMedicDistance = dist;
				closestMedic = agent;
			}
		}
		
		return closestMedic;
	}
}
