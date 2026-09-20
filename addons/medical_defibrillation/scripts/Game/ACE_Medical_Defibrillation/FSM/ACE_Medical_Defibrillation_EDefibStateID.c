//------------------------------------------------------------------------------------------------
enum ACE_Medical_Defibrillation_EDefibStateID : ACE_FSM_EStateID
{
	DISCONNECTED 	= 1 << 0,
	CONNECTED 		= 1 << 1,
	ANALYSING 		= 1 << 2,
	ANALYSED 		= 1 << 3,
	CHARGING 		= 1 << 4,
	CHARGED 		= 1 << 5,
	COOLDOWN 		= 1 << 6
}
