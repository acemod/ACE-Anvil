//------------------------------------------------------------------------------------------------
//! Handles dynamic spawning/despawning of user actions helper entities
class ACE_ActionsHelperEntitySystem : GameSystem
{
	[Attribute(defvalue: "0.5", desc: "Timeout between updates [s]")]
	protected float m_fUpdateTimeout;
	
	[Attribute(defvalue: "5.0", desc: "Radius in meters for searching entities to spawn helpers on.")]
	protected float m_fEntityDetectionRadius;
	
	protected ref array<ref ACE_ActionsHelperEntityGroupContext> m_aContexts = {};
	protected ref array<ACE_ActionsHelperEntityComponent> m_aDeletedComponents = {};
	protected bool m_bUpdating = false;
	protected float m_fUpdateTimer;
	protected SCR_PlayerController m_LocalPlayerController;
	
	//------------------------------------------------------------------------------------------------
	static ACE_ActionsHelperEntitySystem GetInstance(ChimeraWorld world)
	{
		return ACE_ActionsHelperEntitySystem.Cast(world.FindSystem(ACE_ActionsHelperEntitySystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Client)
			.AddPoint(WorldSystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		Enable(!m_aContexts.IsEmpty());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register local player controller for detecting change in controlled entity
	void Register(notnull SCR_PlayerController playerController)
	{
		if (m_LocalPlayerController)
			return;
		
		m_LocalPlayerController = playerController;
		
		IEntity controlledEntity = m_LocalPlayerController.GetControlledEntity();
		if (controlledEntity)
			OnControlledEntityChanged(null, controlledEntity);
		
		m_LocalPlayerController.m_OnControlledEntityChanged.Insert(OnControlledEntityChanged);
		
		SCR_EditorManagerEntity editorManagerEntity = SCR_EditorManagerEntity.GetInstance();
		if (!editorManagerEntity)
			return;

		editorManagerEntity.GetOnOpened().Insert(OnEditorOpened);
		editorManagerEntity.GetOnClosed().Insert(OnEditorClosed);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		ChimeraCharacter fromChar = ChimeraCharacter.Cast(from);
		if (fromChar)
			Unregister(fromChar);
		
		ChimeraCharacter toChar = ChimeraCharacter.Cast(to);
		if (toChar)
			Register(toChar);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnEditorOpened()
	{
		m_LocalPlayerController.m_OnControlledEntityChanged.Remove(OnControlledEntityChanged);
		
		ChimeraCharacter char = ChimeraCharacter.Cast(m_LocalPlayerController.GetControlledEntity());
		if (char)
			Unregister(char);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnEditorClosed()
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(m_LocalPlayerController.GetControlledEntity());
		if (char)
			Register(char);
		
		m_LocalPlayerController.m_OnControlledEntityChanged.Insert(OnControlledEntityChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Start monitoring character for changes in held gadget
	protected void Register(ChimeraCharacter char)
	{
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (charController)
			charController.m_OnGadgetStateChangedInvoker.Insert(OnGadgetStateChanged);
		
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(char);
		if (gadgetManager && gadgetManager.GetHeldGadget())
			OnGadgetStateChanged(gadgetManager.GetHeldGadget(), true, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Unregister(ChimeraCharacter char)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(char);
		if (gadgetManager && gadgetManager.GetHeldGadget())
			OnGadgetStateChanged(gadgetManager.GetHeldGadget(), false, false);
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (charController)
			charController.m_OnGadgetStateChangedInvoker.Remove(OnGadgetStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnGadgetStateChanged(IEntity gadget, bool isInHand, bool isOnGround)
	{
		ACE_ActionsHelperEntityComponent component = ACE_ActionsHelperEntityComponent.Cast(gadget.FindComponent(ACE_ActionsHelperEntityComponent));
		if (!component)
			return;

		if (isInHand)
			Register(component);
		else
			Unregister(component);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnStarted()
	{
		m_fUpdateTimer = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		m_fUpdateTimer -= args.GetTimeSliceSeconds();
		if (m_fUpdateTimer > 0)
			return;
		
		m_fUpdateTimer = m_fUpdateTimeout;
		
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player)
			return;
		
		m_bUpdating = true;
		
		GetWorld().QueryEntitiesBySphere(player.GetOrigin(), m_fEntityDetectionRadius, OnEntityDetected);
		
		foreach (ACE_ActionsHelperEntityGroupContext context : m_aContexts)
		{
			// Delete helpers no longer in range
			for (int i = context.m_aEntitiesInRange.Count() - 1; i >= 0; i--)
			{
				IEntity entity = context.m_aEntitiesInRange[i];
				if (!entity || !context.m_aNewEntitiesInRange.Contains(entity))
				{
					SCR_EntityHelper.DeleteEntityAndChildren(context.m_aHelperEntities[i]);
					context.m_aEntitiesInRange.Remove(i);
					context.m_aHelperEntities.Remove(i);
				}
			}
			
			// Create new helpers in range
			foreach (IEntity newEntity : context.m_aNewEntitiesInRange)
			{
				if (newEntity && !context.m_aEntitiesInRange.Contains(newEntity))
				{
					context.m_aHelperEntities.Insert(context.m_Config.SpawnHelper(newEntity));
					context.m_aEntitiesInRange.Insert(newEntity);
				}
			}
			
			context.m_aNewEntitiesInRange.Clear();
		}
		
		m_bUpdating = false;
		
		foreach (ACE_ActionsHelperEntityComponent component : m_aDeletedComponents)
		{
			Unregister(component);
		}
		
		m_aDeletedComponents.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool OnEntityDetected(IEntity ent)
	{
		foreach (ACE_ActionsHelperEntityGroupContext context : m_aContexts)
		{
			if (!context.m_Config.ValidateEntity(ent))
				continue;
			
			context.m_aNewEntitiesInRange.Insert(ent);
			break;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(ACE_ActionsHelperEntityComponent component)
	{
		ACE_ActionsHelperEntityComponentClass data = ACE_ActionsHelperEntityComponentClass.Cast(component.GetComponentData(component.GetOwner()));
		if (!data)
			return;
		
		foreach (ACE_ActionsHelperEntityConfig config : data.GetConfigs())
		{
			m_aContexts.Insert(new ACE_ActionsHelperEntityGroupContext(config));
		}
		
		Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_ActionsHelperEntityComponent component)
	{
		if (m_bUpdating)
		{
			m_aDeletedComponents.Insert(component);
			return;
		}

		ACE_ActionsHelperEntityComponentClass data = ACE_ActionsHelperEntityComponentClass.Cast(component.GetComponentData(component.GetOwner()));
		if (!data)
			return;
			
		foreach (ACE_ActionsHelperEntityConfig config : data.GetConfigs())
		{
			for (int i = m_aContexts.Count() - 1; i >= 0; i--)
			{
				if (m_aContexts[i].m_Config != config)
					continue;
				
				foreach (IEntity helper : m_aContexts[i].m_aHelperEntities)
				{
					SCR_EntityHelper.DeleteEntityAndChildren(helper);
				}
				
				m_aContexts.Remove(i);
				break;
			}
		}
		
		if (m_aContexts.IsEmpty())
			Enable(false);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntityGroupContext : Managed
{
	ACE_ActionsHelperEntityConfig m_Config;
	ref array<IEntity> m_aEntitiesInRange = {};
	ref array<IEntity> m_aNewEntitiesInRange = {};
	ref array<IEntity> m_aHelperEntities = {};
	
	//------------------------------------------------------------------------------------------------
	void ACE_ActionsHelperEntityGroupContext(ACE_ActionsHelperEntityConfig config)
	{
		m_Config = config;
	}
}
