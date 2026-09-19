//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp(onRplName: "ACE_Medical_OnUnconsciousPoseChanged")]
	protected ACE_Medical_EUnconsciousPose m_eACE_Medical_UnconsciousPose;
	
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
	//! Listen to unconscious pose changes in the animation graph
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
