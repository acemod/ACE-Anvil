//------------------------------------------------------------------------------------------------
//! Add handling for protective equipment
modded class SCR_DamageArea : DamageArea
{
	[Attribute(defvalue: "AIRWAYS", desc: "Required protection to suvive in this area.", uiwidget: UIWidgets.Flags, enums: ParamEnumArray.FromEnum(ACE_CBRN_ECharacterProtectedArea))]
	protected ACE_CBRN_ECharacterProtectedArea m_eACE_CBRN_RequiredProtection;
		
	//------------------------------------------------------------------------------------------------
	//! Attach event handlers
	override void OnAreaEntered(notnull IEntity entity)
	{
		super.OnAreaEntered(entity);
		
		ACE_CBRN_CharacterProtectionComponent charProtection = ACE_CBRN_CharacterProtectionComponent.Cast(entity.FindComponent(ACE_CBRN_CharacterProtectionComponent));
		if (charProtection)
		{
			charProtection.GetOnStateChanged().Insert(ACE_CBRN_OnCharacterProtectionChanged);
			
			if (charProtection.HasRequiredProtection(m_eACE_CBRN_RequiredProtection))
				ACE_CBRN_RemoveEffectInstantly(entity);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Detach event handlers
	override void OnAreaExit(IEntity entity)
	{
		super.OnAreaExit(entity);
		
		if (!entity)
			return;
		
		ACE_CBRN_CharacterProtectionComponent charProtection = ACE_CBRN_CharacterProtectionComponent.Cast(entity.FindComponent(ACE_CBRN_CharacterProtectionComponent));
		if (charProtection)
			charProtection.GetOnStateChanged().Remove(ACE_CBRN_OnCharacterProtectionChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Reevaluate when protection has changed
	protected void ACE_CBRN_OnCharacterProtectionChanged(ChimeraCharacter character, ACE_CBRN_ECharacterProtectedArea protectedAreas)
	{
		ReevaluateDamageEffect(character, null);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove effect if character has required protection
	override protected void ReevaluateDamageEffect(ChimeraCharacter playerCharacter, IEntity compartmentEntity)
	{
		if (!playerCharacter)
			return;
		
		ACE_CBRN_CharacterProtectionComponent charProtection = ACE_CBRN_CharacterProtectionComponent.Cast(playerCharacter.FindComponent(ACE_CBRN_CharacterProtectionComponent));
		if (charProtection && charProtection.HasRequiredProtection(m_eACE_CBRN_RequiredProtection))
		{
			RemoveEffect(playerCharacter);
			return;
		}
		
		super.ReevaluateDamageEffect(playerCharacter, compartmentEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Same as RemoveEffect, but without exit timeout
	protected void ACE_CBRN_RemoveEffectInstantly(IEntity entity)
	{
		const SCR_ExtendedDamageManagerComponent dmgMgr = SCR_ExtendedDamageManagerComponent.Cast(SCR_DamageManagerComponent.GetDamageManager(entity));
		if (!dmgMgr)
			return;

		array<ref SCR_PersistentDamageEffect> damageEffects = {};
		if (dmgMgr.FindAllDamageEffectsOfType(GetDamageEffect().Type(), damageEffects) < 1)
			return;

		const IEntity owner = GetParent();
		foreach (SCR_PersistentDamageEffect effect : damageEffects)
		{
			if (effect.GetInstigator().GetInstigatorEntity() == owner)
				effect.Terminate();
		}
	}
}
