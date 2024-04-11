//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
    //------------------------------------------------------------------------------------------------
    //! Initialize ACE Medical on player controlled characters
    override void OnControlledEntityChanged(IEntity from, IEntity to)
    {
	super.OnControlledEntityChanged(from, to);

	//! Do not initialize for GM controlled AI
	if (!to || IsPossessing())
	    return;

	SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(to.FindComponent(SCR_CharacterDamageManagerComponent));
	if (damageManager)
	    damageManager.ACE_Medical_Initialize(to);
    }
}
