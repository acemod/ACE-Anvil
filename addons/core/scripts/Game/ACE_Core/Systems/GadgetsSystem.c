//------------------------------------------------------------------------------------------------
modded class GadgetsSystem : GameSystem
{
	[EventAttribute()]
	void ACE_OnRegister(SCR_GadgetComponent component);
	
	[EventAttribute()]
	void ACE_OnUnregister(SCR_GadgetComponent component);
	
	//------------------------------------------------------------------------------------------------
	override void Register(SCR_GadgetComponent component)
	{
		super.Register(component);
		ThrowEvent(ACE_OnRegister, component);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Unregister(SCR_GadgetComponent component)
	{
		super.Unregister(component);
		
		if (!m_bUpdating)
			ThrowEvent(ACE_OnUnregister, component);
	}
}
