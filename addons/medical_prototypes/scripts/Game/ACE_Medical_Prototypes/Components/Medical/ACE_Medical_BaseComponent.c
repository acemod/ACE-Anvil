//------------------------------------------------------------------------------------------------
class ACE_Medical_BaseComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_BaseComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Returns the type of the system that updates this component
	//! Derived classes should always override this!
	typename GetAssociatedSystemType()
	{
		return ACE_Medical_BaseSystem;
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
		
		ACE_Medical_BaseSystem system = ACE_Medical_BaseSystem.GetInstance(GetAssociatedSystemType());
		if (ShouldRegisterAtSystemOnInit() && system)
			system.Register(owner);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
			damageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from system when killed
	protected void OnDamageStateChanged(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		
		ACE_Medical_BaseSystem system = ACE_Medical_BaseSystem.GetInstance(GetAssociatedSystemType());
		if (system)
			system.Unregister(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from system in destructor is too late, hence we do it in SCR_EditableCharacterComponent.OnDelete instead
	void ~ACE_Medical_BaseComponent();
}
