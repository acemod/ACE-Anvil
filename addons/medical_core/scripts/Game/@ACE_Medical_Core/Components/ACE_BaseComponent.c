//------------------------------------------------------------------------------------------------
class ACE_BaseComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_BaseComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Returns the type of the system that updates this component
	//! Derived classes should always override this!
	typename GetAssociatedSystemType()
	{
		return ACE_BaseSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should register at the associated system during EOnInit
	protected bool ShouldRegisterAtSystemOnInit()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register to system on init
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		ACE_BaseSystem system = ACE_BaseSystem.GetInstance(GetAssociatedSystemType());
		if (system && ShouldRegisterAtSystemOnInit())
			system.Register(owner);
		
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
		if (damageManager)
			damageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from system when entity is destroyed
	protected void OnDamageStateChanged(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		
		ACE_BaseSystem system = ACE_BaseSystem.GetInstance(GetAssociatedSystemType());
		if (system)
			system.Unregister(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from system when deleted
	override protected void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		ACE_BaseSystem system = ACE_BaseSystem.GetInstance(GetAssociatedSystemType());
		if (system)
			system.Unregister(owner);
	}
}
