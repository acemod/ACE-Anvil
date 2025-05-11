//------------------------------------------------------------------------------------------------
class ACE_Medical_BaseComponent2Class : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! This is a copy of ACE_Medical_BaseComponent2
//! TO DO: Delete once systems support inheritance
class ACE_Medical_BaseComponent2 : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Returns the type of the system that updates this component
	//! Derived classes should always override this!
	typename GetAssociatedSystemType()
	{
		return ACE_Medical_BaseSystem2;
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
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(owner);
		if (!ownerChar)
			return;
		
		ACE_Medical_BaseSystem2 system = ACE_Medical_BaseSystem2.GetInstance(GetAssociatedSystemType());
		if (ShouldRegisterAtSystemOnInit() && system)
			system.Register(ownerChar);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(ownerChar.GetDamageManager());
		if (damageManager)
			damageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from system when killed
	protected void OnDamageStateChanged(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		
		ACE_Medical_BaseSystem2 system = ACE_Medical_BaseSystem2.GetInstance(GetAssociatedSystemType());
		if (system)
			system.Unregister(SCR_ChimeraCharacter.Cast(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from system when deleted
	override protected void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		ACE_Medical_BaseSystem2 system = ACE_Medical_BaseSystem2.GetInstance(GetAssociatedSystemType());
		if (system)
			system.Unregister(SCR_ChimeraCharacter.Cast(owner));
	}
}
