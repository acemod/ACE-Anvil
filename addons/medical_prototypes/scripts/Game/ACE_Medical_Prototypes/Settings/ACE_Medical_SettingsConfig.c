//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
modded class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Enables second chance for shots to the chest")]
	bool m_bSecondChanceEnabled;
	
	[Attribute(defvalue: "0", desc: "Chance for exiting cardiac arrest while performing CPR at class IV hemorrhage")]
	float m_fCPRSuccessChanceMin;
	
	[Attribute(defvalue: "0.4", desc: "Chance for exiting cardiac arrest while performing CPR at above class II hemorrhage")]
	float m_fCPRSuccessChanceMax;
	
	[Attribute(defvalue: "15", desc: "Default delay between check for successful CPR in seconds")]
	float m_fCPRSuccessCheckTimeoutS;
}
