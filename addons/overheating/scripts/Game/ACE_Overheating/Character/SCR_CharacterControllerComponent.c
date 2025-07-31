//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	protected override void OnControlledByPlayer(IEntity owner, bool controlled)
	{
		super.OnControlledByPlayer(owner, controlled);
		
		if (controlled && owner ==  SCR_PlayerController.GetLocalControlledEntity())
			GetGame().GetInputManager().AddActionListener("ACE_Overheating_ClearJam", EActionTrigger.DOWN, ACE_Overheating_ClearJamAction);
		else
			GetGame().GetInputManager().RemoveActionListener("ACE_Overheating_ClearJam", EActionTrigger.DOWN, ACE_Overheating_ClearJamAction);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Overheating_ClearJamAction(float value = 0.0, EActionTrigger trigger = 0)
	{
		ChimeraCharacter ownerChar = ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return;
		
		BaseWeaponManagerComponent weaponManager = ownerChar.GetWeaponManager();
		if (!weaponManager)
			return;
		
		BaseWeaponComponent weapon = weaponManager.GetCurrentWeapon();
		if (!weapon)
			return;
		
		ACE_Overheating_TryClearJam(weapon);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_TryClearJam(notnull BaseWeaponComponent weapon, BaseMuzzleComponent muzzle = null)
	{
		ACE_Overheating_MuzzleJamComponent jamComponent;
		if (muzzle)
			jamComponent = ACE_Overheating_MuzzleJamComponent.FromMuzzle(muzzle);
		else
			jamComponent = ACE_Overheating_MuzzleJamComponent.FromWeapon(weapon);
		
		ACE_Overheating_WeaponAnimContext context = new ACE_Overheating_WeaponAnimContext(ChimeraCharacter.Cast(GetOwner()), weapon, jamComponent);
		if (context.IsReloading())
			return;
		
		ACE_FrameJobSystem system = ACE_FrameJobSystem.GetInstance();
		if (!system)
			return;
		
		ACE_Overheating_WeaponAnimMachine stateMachine = new ACE_Overheating_WeaponAnimMachine();
		stateMachine.SetContext(context);
		stateMachine.AddState(new ACE_Overheating_RemoveMagState(ACE_Overheating_EWeaponAnimStateID.REMOVE_MAGAZINE));
		stateMachine.AddState(new ACE_Overheating_RackBoltState(ACE_Overheating_EWeaponAnimStateID.RACK_BOLT));
		stateMachine.AddTransition(new ACE_Overheating_RemoveMagCompleted(ACE_Overheating_EWeaponAnimStateID.REMOVE_MAGAZINE, ACE_Overheating_EWeaponAnimStateID.RACK_BOLT));
		stateMachine.AddTransition(new ACE_Overheating_RemoveMagFailed(ACE_Overheating_EWeaponAnimStateID.REMOVE_MAGAZINE, ACE_Overheating_EWeaponAnimStateID.STOP));
		stateMachine.AddTransition(new ACE_Overheating_RackBoltCompleted(ACE_Overheating_EWeaponAnimStateID.RACK_BOLT, ACE_Overheating_EWeaponAnimStateID.STOP));
		system.Register(stateMachine, ESystemPoint.PostFixedFrame);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_WeaponAnimMachine : ACE_FSM_Machine<ACE_Overheating_WeaponAnimContext>
{
}
