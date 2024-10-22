//------------------------------------------------------------------------------------------------
modded class SCR_CampaignBuildingManagerComponent : SCR_BaseGameModeComponent
{
	[Attribute(defvalue: "{9D44ED260F5BBBE0}Configs/ACE/Trenches.conf", desc: "Placement mode configurations", params: "conf")]
	protected ResourceName m_sACE_Trenches_ConfigName;
	protected ref ACE_Trenches_Config m_ACE_Trenches_Config;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_ACE_Trenches_Config = SCR_ConfigHelperT<ACE_Trenches_Config>.GetConfigObject(m_sACE_Trenches_ConfigName);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Trenches_Config ACE_Trenches_GetConfig()
	{
		return m_ACE_Trenches_Config;
	}
}
