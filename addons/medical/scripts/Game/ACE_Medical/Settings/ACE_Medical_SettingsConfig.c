//------------------------------------------------------------------------------------------------
//! Settings for a mod
[BaseContainerProps()]
class ACE_Medical_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0", desc: "Medical kits can only be used with supplies if enabled")]
	bool m_bEnableSuppliesForMedicalKit;
}
