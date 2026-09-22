//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntityComponentClass : ScriptComponentClass
{
	[Attribute(desc: "Helper entities that should be spawned when the gadget is active.")]
	protected ref array<ref ACE_ActionsHelperEntityConfig> m_Configs;
	
	//------------------------------------------------------------------------------------------------
	void ACE_ActionsHelperEntityComponentClass(IEntityComponentSource componentSource, IEntitySource parentSource, IEntitySource prefabSource)
	{
		if (!GetGame().InPlayMode())
			return;
		
		GetGame().GetCallqueue().Call(OnInit);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnInit()
	{
		foreach (ACE_ActionsHelperEntityConfig config : m_Configs)
		{
			config.Init();
		}
		
		ACE_ActionsHelperEntitySystem system = ACE_ActionsHelperEntitySystem.GetInstance(GetGame().GetWorld());
		if (!system)
			return;
	
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		system.Register(playerController);
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ACE_ActionsHelperEntityConfig> GetConfigs()
	{
		return m_Configs;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntityComponent : ScriptComponent
{
}
