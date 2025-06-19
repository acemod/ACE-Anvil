//------------------------------------------------------------------------------------------------
class ACE_Medical_BaseSystem_EntityContext : ACE_BaseSystem_EntityContext
{
	SCR_CharacterDamageManagerComponent m_pDamageManagerComponent;
	SCR_CharacterBloodHitZone m_pBloodHitZone;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_CardiovascularSystem_EntityContext(IEntity entity)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(entity);
		if (!char)
			return;
		
		m_pDamageManagerComponent = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		m_pBloodHitZone = m_pDamageManagerComponent.GetBloodHitZone();
	}
}
