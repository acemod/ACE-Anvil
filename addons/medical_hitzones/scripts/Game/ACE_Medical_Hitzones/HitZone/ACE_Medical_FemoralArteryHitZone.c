//-----------------------------------------------------------------------------------------------------------
class ACE_Medical_FemoralArteryHitZone : ACE_Medical_OrganHitZone
{
	[Attribute("8", "Multiplier of hitzone health that will be applied as bleeding damage over time when damaged")]
	protected float m_fBleedingRateScale;
	
	protected ParticleEffectEntity m_pBleedingParticles;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Add bleeding when wounded and knock the character out
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (newState <= ECharacterDamageState.INTERMEDIARY)
			return;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!charDamageManager)
			return;
		
		if (Replication.IsServer())
		{
			charDamageManager.AddBleedingEffectOnHitZone(SCR_CharacterHitZone.Cast(charDamageManager.GetHitZoneByName("Hips")));
			charDamageManager.GetResilienceHitZone().SetHealthScaled(0);
		}
		
		if (!System.IsConsoleApp())
			charDamageManager.ACE_Medical_CreateFemoralBleedingParticleEffect(this);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	float CalculateBleedingRate()
	{
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!charDamageManager)
			return 1;
		
		return m_fBleedingRateScale * charDamageManager.GetBleedingScale() * (GetMaxHealth() - GetHealth());
	}
}
