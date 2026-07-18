//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class ACE_CBRN_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "false", desc: "AI will die instead of becoming paralyzed if false")]
	float m_bFullParalysisForAIEnabled;
}
