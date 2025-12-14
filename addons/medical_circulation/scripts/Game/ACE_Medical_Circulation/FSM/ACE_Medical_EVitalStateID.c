//------------------------------------------------------------------------------------------------
enum ACE_Medical_EVitalStateID : ACE_FSM_EStateID
{
	STABLE         = 1 << 0,
	UNSTABLE       = 1 << 1,
	CRITICAL       = 1 << 2,
	RESUSCITATION  = 1 << 3,
	CARDIAC_ARREST = 1 << 4,
	CARDIAC_ARREST_SHOCKABLE = 1 << 5
}
