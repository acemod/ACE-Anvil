//------------------------------------------------------------------------------------------------
class ACE_SurfaceHelper
{
	protected static ref ACE_SurfaceLabelsConfig s_SurfaceLabelsConfig;
	protected static const ResourceName SURFACE_LABELS_CONFIG_NAME = "{4E2AF3E9D54A29B4}Configs/ACE/ACE_SurfaceLabels.conf";

	//------------------------------------------------------------------------------------------------
	protected static void Init()
	{
		s_SurfaceLabelsConfig = SCR_ConfigHelperT<ACE_SurfaceLabelsConfig>.GetConfigObject(SURFACE_LABELS_CONFIG_NAME);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true when the surface has the given label
	static bool HasLabel(notnull SurfaceProperties props, ACE_ESurfaceLabel label)
	{
		if (!s_SurfaceLabelsConfig)
			Init();
		
		return s_SurfaceLabelsConfig.HasLabel(props, label);
	}
}
