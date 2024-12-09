//------------------------------------------------------------------------------------------------
//! Type of consumable gadget
modded enum SCR_EConsumableType
{
	ACE_MEDICAL_EPINEPHRINE
};

//------------------------------------------------------------------------------------------------
//! Reason why consumable cannot be applied
modded enum SCR_EConsumableFailReason
{
	ACE_MEDICAL_NOT_INCAPACITATED,
	ACE_MEDICAL_TOO_DAMAGED,
	ACE_MEDICAL_TOO_DAMAGED_MEDICAL_FACILITY_REQUIRED
}
