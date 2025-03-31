//------------------------------------------------------------------------------------------------
//! This is basically a variant of EDamageState, but we don't inherit as it will break the attributes otherwise
enum ACE_Medical_EBloodState
{
	NORMAL = 0,
	CLASS_1_HEMORRHAGE = 1,
	CLASS_2_HEMORRHAGE = 3,
	CLASS_3_HEMORRHAGE = 4,
	CLASS_4_HEMORRHAGE = 5,
	FATAL = 2
}
