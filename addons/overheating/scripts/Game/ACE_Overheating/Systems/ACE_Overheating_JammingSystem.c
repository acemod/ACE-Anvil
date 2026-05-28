//------------------------------------------------------------------------------------------------
class ACE_Overheating_JammingSystem : GameSystem
{
	protected ACE_Overheating_BarrelComponent m_pLocalActiveBarrel = null;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_JammingSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Overheating_JammingSystem.Cast(world.FindSystem(ACE_Overheating_JammingSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull ChimeraCharacter char, bool isOwner)
	{
		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(char.FindComponent(EventHandlerManagerComponent));
		if (!eventHandlerManager)
			return;
		
		if (Replication.IsServer())
			eventHandlerManager.RegisterScriptHandler("OnProjectileShot", this, OnProjectileShot);
		
		if (isOwner)
		{
			BaseWeaponManagerComponent weaponManager = char.GetWeaponManager();
			if (weaponManager)
				OnWeaponChangedLocal(weaponManager.GetCurrentWeapon(), null);
			
			eventHandlerManager.RegisterScriptHandler("OnWeaponChanged", this, OnWeaponChangedLocal);
			eventHandlerManager.RegisterScriptHandler("OnMuzzleChanged", this, OnMuzzleChangedLocal);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(notnull ChimeraCharacter char, bool isOwner)
	{
		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(char.FindComponent(EventHandlerManagerComponent));
		if (!eventHandlerManager)
			return;
		
		if (Replication.IsServer())
			eventHandlerManager.RemoveScriptHandler("OnProjectileShot", this, OnProjectileShot);
		
		if (isOwner)
		{
			eventHandlerManager.RemoveScriptHandler("OnWeaponChanged", this, OnWeaponChangedLocal);
			eventHandlerManager.RemoveScriptHandler("OnMuzzleChanged", this, OnMuzzleChangedLocal);
			m_pLocalActiveBarrel = null;
			UpdateFireHandlerLocal(char, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnProjectileShot(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{
		ACE_Overheating_BarrelComponent barrel = ACE_Overheating_BarrelComponent.FromWeapon(weapon);
		if (!barrel)
			return;
		
		// TO DO: Move to temperature systems
		barrel.IncremenHeatCounter();
		barrel.SetAmmoTemperature(ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE);
		barrel.SetCookOffProgress(0);
		
		if (Math.RandomFloat(0, 1) < barrel.GetJamChance())
			barrel.SetState(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnJamStateChanged(ACE_Overheating_BarrelComponent barrel, bool isJammed)
	{
		//! Only handle on local client
		if (m_pLocalActiveBarrel != barrel)
			return;
		
		ChimeraCharacter localPlayer = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		
		if (isJammed)
			GetGame().GetCallqueue().CallLater(PlayWeaponSound, 500, false, barrel.GetOwner(), "SOUND_RELOAD_BOLT_PULL");
		
		UpdateFireHandlerLocal(localPlayer, !isJammed);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler when weapon changed
	protected void OnWeaponChangedLocal(BaseWeaponComponent newWeapon, BaseWeaponComponent prevWeapon)
	{
		ChimeraCharacter localPlayer = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		
		m_pLocalActiveBarrel = ACE_Overheating_BarrelComponent.FromWeapon(newWeapon);
		UpdateFireHandlerLocal(localPlayer, !m_pLocalActiveBarrel || !m_pLocalActiveBarrel.IsJammed());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler when muzzle changed
	protected void OnMuzzleChangedLocal(BaseWeaponComponent weapon, BaseMuzzleComponent newMuzzle, BaseMuzzleComponent prevMuzzle)
	{
		ChimeraCharacter localPlayer = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		 
		m_pLocalActiveBarrel = ACE_Overheating_BarrelComponent.FromMuzzle(newMuzzle);
		UpdateFireHandlerLocal(localPlayer, !m_pLocalActiveBarrel || !m_pLocalActiveBarrel.IsJammed());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler
	protected void UpdateFireHandlerLocal(ChimeraCharacter char, bool canHandleWeaponFire)
	{
		CharacterAnimationComponent animComponent = char.GetAnimationComponent();
		if (!animComponent)
			return;
		
		SCR_CharacterCommandHandlerComponent cmdHandler = SCR_CharacterCommandHandlerComponent.Cast(animComponent.GetCommandHandler());
		if (!cmdHandler)
			return;
		
		cmdHandler.ACE_SetCanHandleWeaponFire(canHandleWeaponFire);
		
		GetGame().GetInputManager().RemoveActionListener("CharacterFire", EActionTrigger.DOWN, OnFireFailedLocal);
		
		if (!canHandleWeaponFire)
			GetGame().GetInputManager().AddActionListener("CharacterFire", EActionTrigger.DOWN, OnFireFailedLocal);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Play dry sound
	protected void OnFireFailedLocal(float value = 0.0, EActionTrigger trigger = 0)
	{
		if (m_pLocalActiveBarrel)
			PlayWeaponSound(m_pLocalActiveBarrel.GetOwner(), "SOUND_DRY");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PlayWeaponSound(IEntity weapon, string soundEvent)
	{
		WeaponSoundComponent soundComponent = WeaponSoundComponent.Cast(weapon.FindComponent(WeaponSoundComponent));
		if (soundComponent)
			soundComponent.SoundEvent(soundEvent);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Overheating_BarrelComponent GetLocalActiveBarrel()
	{
		return m_pLocalActiveBarrel;
	}
}
