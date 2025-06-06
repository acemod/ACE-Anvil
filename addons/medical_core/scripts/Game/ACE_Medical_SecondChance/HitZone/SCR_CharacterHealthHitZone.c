//-----------------------------------------------------------------------------------------------------------
//! health hitzone - Receives damage from physical hitzones
modded class SCR_CharacterHealthHitZone : SCR_HitZone
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	protected ref SCR_DamageContext m_pACE_Medical_SecondChanceDamageContext;
	
	protected static const float ACE_MEDICAL_SECOND_CHANCE_SCALED_RECOVERED_HEALTH = 0.01;

	//-----------------------------------------------------------------------------------------------------------
	//! Initialize members
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		vector hitPosDirNorm[3];
		m_pACE_Medical_SecondChanceDamageContext = new SCR_DamageContext(EDamageType.REGENERATION, 0, hitPosDirNorm, GetOwner(), null, null, null, -1, -1);
		m_pACE_Medical_SecondChanceDamageContext.damageEffect = new InstantDamageEffect();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Handle second chance and death when health hit zone is destroyed
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer())
			return;
		
		if (newState != EDamageState.DESTROYED)
			return;
		
		SCR_CharacterHitZone struckHitZone = m_pACE_Medical_DamageManager.ACE_Medical_GetLastStruckPhysicalHitZone();
		if (ACE_Medical_ShouldGrantSecondChance(struckHitZone))
			ACE_Medical_GrantSecondChance(struckHitZone);
		else
			m_pACE_Medical_DamageManager.Kill(m_pACE_Medical_DamageManager.GetInstigator());
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Whether the character should be saved by second chance or die
	protected bool ACE_Medical_ShouldGrantSecondChance(SCR_CharacterHitZone struckHitZone)
	{
		if (!m_pACE_Medical_DamageManager.ACE_Medical_IsSecondChanceEnabled())
			return false;
		
		if (!struckHitZone)
			return true;
		
		return struckHitZone.ACE_Medical_ShouldGrantSecondChance();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected void ACE_Medical_GrantSecondChance(SCR_CharacterHitZone struckHitZone)
	{
		m_pACE_Medical_DamageManager.ACE_Medical_OnSecondChanceGranted();
		
		if (!struckHitZone)
			return;
		
		m_pACE_Medical_SecondChanceDamageContext.struckHitZone = struckHitZone;
		m_pACE_Medical_SecondChanceDamageContext.damageValue = -struckHitZone.GetMaxHealth() * ACE_MEDICAL_SECOND_CHANCE_SCALED_RECOVERED_HEALTH;
		// Health recovery has to be handled later, as otherwise replication to client fails
		GetGame().GetCallqueue().Call(m_pACE_Medical_DamageManager.HandleDamage, m_pACE_Medical_SecondChanceDamageContext);
		struckHitZone.ACE_Medical_SetWasSecondChanceGranted(true);
	}
}
