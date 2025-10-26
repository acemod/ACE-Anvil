//------------------------------------------------------------------------------------------------
//! Introduce a second chance, which gets triggered when the character would have died without
//! falling unconscious.
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	protected const float ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;
	
	[RplProp(onRplName: "ACE_Medical_OnUnconsciousPoseChanged")]
	protected ACE_Medical_EUnconsciousPose m_eACE_Medical_UnconsciousPose;
	
	//------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add/remove second chance when life state changes
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);
		
		// Only run if ACE Medical has been initialized for this character
		if (!m_pACE_Medical_DamageManager.ACE_Medical_IsInitialized())
			return;
		
		// OnLifeStateChanged sometimes gets triggered without a change in state
		if (previousLifeState == newLifeState)
			return;
		
		switch (newLifeState)
		{
			// Add second chance when revived
			case ECharacterLifeState.ALIVE:
			{
				GetGame().GetCallqueue().Remove(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance);
				m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance(true);
				m_pACE_Medical_DamageManager.ACE_Medical_SetSecondChanceTrigged(false);
				break;
			}
			
			// Schedule removal of second chance when falling unconscious
			case ECharacterLifeState.INCAPACITATED:
			{
				GetGame().GetCallqueue().CallLater(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance, ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS, false, false);
				break;
			}
			
			// Remove second chance when dead
			case ECharacterLifeState.DEAD:
			{
				GetGame().GetCallqueue().Remove(m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance);
				m_pACE_Medical_DamageManager.ACE_Medical_EnableSecondChance(false);
				break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnUnconsciousPoseChanged()
	{		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!char)
			return;
		
		// Anim var has to be updated on owner and server only
		RplComponent rpl = char.GetRplComponent();
		if (!rpl || (!rpl.IsOwner() && rpl.Role() != RplRole.Authority))
			return;
		
		CharacterAnimationComponent animComponent = char.GetAnimationComponent();
		if (!animComponent)
			return;
		
		animComponent.SetVariableInt(animComponent.BindVariableInt("UnconsciousPose"), m_eACE_Medical_UnconsciousPose);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register unconscious pose changes in the animation graph
	protected override void OnAnimationEvent(AnimationEventID animEventType, AnimationEventID animUserString, int intParam, float timeFromStart, float timeToEnd)
	{
		super.OnAnimationEvent(animEventType, animUserString, intParam, timeFromStart, timeToEnd);
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!char)
			return;
		
		RplComponent rpl = char.GetRplComponent();
		if (!rpl || rpl.Role() != RplRole.Authority)
			return;
		
		CharacterAnimationComponent animComponent = char.GetAnimationComponent();
		if (!animComponent)
			return;
		
		if (animEventType != animComponent.BindEvent("ACE_Medical_Event_UnconsciousPoseChange"))
			return;
		
		if (intParam == m_eACE_Medical_UnconsciousPose)
			return;
		
		ACE_Medical_Reposition(intParam);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Reposition(ACE_Medical_EUnconsciousPose pose)
	{
		m_eACE_Medical_UnconsciousPose = pose;
		Replication.BumpMe();
		ACE_Medical_OnUnconsciousPoseChanged();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_EUnconsciousPose ACE_Medical_GetUnconsciousPose()
	{
		return m_eACE_Medical_UnconsciousPose;
	}
}
