//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntityComponentClass : ScriptComponentClass
{
	[Attribute(desc: "Helper entities that should be spawned when the gadget is active.")]
	protected ref array<ref ACE_ActionsHelperEntityConfig> m_Configs;
	
	protected bool m_bInitDone = false;
	
	//------------------------------------------------------------------------------------------------
	array<ref ACE_ActionsHelperEntityConfig> GetConfigs()
	{
		if (!m_bInitDone)
			OnInit();
		
		return m_Configs;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnInit()
	{
		foreach (ACE_ActionsHelperEntityConfig config : m_Configs)
		{
			config.Init();
		}
		
		m_bInitDone = true;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntityComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		SCR_GadgetComponent component = SCR_GadgetComponent.Cast(owner.FindComponent(SCR_GadgetComponent));
		if (!component)
			return;

		World world = owner.GetWorld();
		GadgetsSystem system = GadgetsSystem.Cast(world.FindSystem(GadgetsSystem));
		if (!system)
			return;
		
		system.ConnectEventFiltered(system.ACE_OnRegister, OnStart, component);
		system.ConnectEventFiltered(system.ACE_OnUnregister, OnStop, component);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnDelete(IEntity owner)
	{
		SCR_GadgetComponent component = SCR_GadgetComponent.Cast(owner.FindComponent(SCR_GadgetComponent));
		if (!component)
			return;

		World world = owner.GetWorld();
		GadgetsSystem system = GadgetsSystem.Cast(world.FindSystem(GadgetsSystem));
		if (!system)
			return;
		
		system.DisconnectEventFiltered(system.ACE_OnRegister, OnStart, component);
		system.DisconnectEventFiltered(system.ACE_OnUnregister, OnStop, component);
	}
	
	//------------------------------------------------------------------------------------------------
	[ReceiverAttribute()]
	protected void OnStart(SCR_GadgetComponent component)
	{
		if (component.GetCharacterOwner() != SCR_PlayerController.GetLocalControlledEntity())
			return;
		
		ACE_ActionsHelperEntitySystem system = ACE_ActionsHelperEntitySystem.GetInstance(GetOwner().GetWorld());
		if (!system)
			return;
		
		ACE_ActionsHelperEntityComponentClass data = ACE_ActionsHelperEntityComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return;
		
		foreach (ACE_ActionsHelperEntityConfig config : data.GetConfigs())
		{
			system.Register(config);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[ReceiverAttribute()]
	protected void OnStop(SCR_GadgetComponent component)
	{
		if (component.GetCharacterOwner() != SCR_PlayerController.GetLocalControlledEntity())
			return;
		
		ACE_ActionsHelperEntitySystem system = ACE_ActionsHelperEntitySystem.GetInstance(GetOwner().GetWorld());
		if (!system)
			return;
		
		ACE_ActionsHelperEntityComponentClass data = ACE_ActionsHelperEntityComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return;
		
		foreach (ACE_ActionsHelperEntityConfig config : data.GetConfigs())
		{
			system.Unregister(config);
		}
	}
}
