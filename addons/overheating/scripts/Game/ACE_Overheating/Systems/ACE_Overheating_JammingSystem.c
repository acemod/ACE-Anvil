//------------------------------------------------------------------------------------------------
class ACE_Overheating_JammingSystem : GameSystem
{
	protected ACE_Overheating_MuzzleJamComponent m_pLocalActiveJamComponent = null;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_JammingSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Overheating_JammingSystem.Cast(world.FindSystem(ACE_Overheating_JammingSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.BeforeEntitiesCreated);
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
			m_pLocalActiveJamComponent = null;
			UpdateFireHandlerLocal(char, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnProjectileShot(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{
		ACE_Overheating_MuzzleJamComponent jamComponent = ACE_Overheating_MuzzleJamComponent.FromWeapon(weapon);
		if (!jamComponent)
			return;
		
		// TO DO: Move to temperature systems
		jamComponent.IncremenHeatCounter();
		jamComponent.SetAmmoTemperature(ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE);
		
		if (Math.RandomFloat(0, 1) < jamComponent.GetJamChance())
			jamComponent.SetState(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnJamStateChanged(ACE_Overheating_MuzzleJamComponent jamComponent, bool isJammed)
	{
		//! Only handle on local client
		if (m_pLocalActiveJamComponent != jamComponent)
			return;
		
		ChimeraCharacter localPlayer = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		
		if (isJammed)
			GetGame().GetCallqueue().CallLater(PlayWeaponSound, 500, false, jamComponent.GetOwner(), "SOUND_RELOAD_BOLT_PULL");
		
		UpdateFireHandlerLocal(localPlayer, !isJammed);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler when weapon changed
	protected void OnWeaponChangedLocal(BaseWeaponComponent newWeapon, BaseWeaponComponent prevWeapon)
	{
		ChimeraCharacter localPlayer = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		
		m_pLocalActiveJamComponent = ACE_Overheating_MuzzleJamComponent.FromWeapon(newWeapon);
		UpdateFireHandlerLocal(localPlayer, !m_pLocalActiveJamComponent || !m_pLocalActiveJamComponent.IsJammed());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler when muzzle changed
	protected void OnMuzzleChangedLocal(BaseWeaponComponent weapon, BaseMuzzleComponent newMuzzle, BaseMuzzleComponent prevMuzzle)
	{
		ChimeraCharacter localPlayer = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return;
		 
		m_pLocalActiveJamComponent = ACE_Overheating_MuzzleJamComponent.FromMuzzle(newMuzzle);
		UpdateFireHandlerLocal(localPlayer, !m_pLocalActiveJamComponent || !m_pLocalActiveJamComponent.IsJammed());
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
		if (m_pLocalActiveJamComponent)
			PlayWeaponSound(m_pLocalActiveJamComponent.GetOwner(), "SOUND_DRY");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PlayWeaponSound(IEntity weapon, string soundEvent)
	{
		WeaponSoundComponent soundComponent = WeaponSoundComponent.Cast(weapon.FindComponent(WeaponSoundComponent));
		if (soundComponent)
			soundComponent.SoundEvent(soundEvent);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Overheating_MuzzleJamComponent GetLocalActiveJamComponent()
	{
		return m_pLocalActiveJamComponent;
	}
}
