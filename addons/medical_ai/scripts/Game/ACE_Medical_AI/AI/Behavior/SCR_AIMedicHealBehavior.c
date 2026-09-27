//---------------------------------------------------------------------------------------------------------------------------------
modded class SCR_AIMedicHealBehavior : SCR_AIBehaviorBase
{
	//---------------------------------------------------------------------------------------------------------------------------------
	void SCR_AIMedicHealBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity entityToHeal, bool allowHealMove, float priority = PRIORITY_BEHAVIOR_MEDIC_HEAL, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		// Modified copy of MedicHeal.bt
		m_sBehaviorTree = "{A5C6D1F5CA300DBC}AI/BehaviorTrees/Chimera/Soldier/ACE_Medical_MedicHeal.bt";
	}
}
