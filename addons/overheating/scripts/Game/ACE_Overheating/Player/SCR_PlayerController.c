//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	//------------------------------------------------------------------------------------------------
	override protected void OnControlledEntityChanged(IEntity from, IEntity to)
	{
		super.OnControlledEntityChanged(from, to);
		
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (!rpl)
			return;
		
		SCR_ChimeraCharacter fromChar = SCR_ChimeraCharacter.Cast(from);
		if (fromChar)
		{
			ACE_Overheating_JammingSystem jammingSystem = ACE_Overheating_JammingSystem.GetInstance(fromChar.GetWorld());
			if (jammingSystem)
				jammingSystem.Unregister(fromChar, rpl.IsOwner());
			
			// Server side deregistrations
			if (!rpl.IsProxy())
			{
				EquipedWeaponStorageComponent weaponStorage = EquipedWeaponStorageComponent.Cast(fromChar.FindComponent(EquipedWeaponStorageComponent));
				if (weaponStorage)
					weaponStorage.ACE_Overheating_ToggleRegisterWeapons(false);
				
				ACE_Overheating_AmmoTemperatureSystem ammoSystem = ACE_Overheating_AmmoTemperatureSystem.GetInstance(fromChar.GetWorld());
				if (ammoSystem)
					ammoSystem.Unregister(fromChar);
			}	
		}
		
		SCR_ChimeraCharacter toChar = SCR_ChimeraCharacter.Cast(to);
		if (toChar)
		{
			ACE_Overheating_JammingSystem jammingSystem = ACE_Overheating_JammingSystem.GetInstance(toChar.GetWorld());
			if (jammingSystem)
				jammingSystem.Register(toChar, rpl.IsOwner());
			
			// Server side registrations
			if (!rpl.IsProxy())
			{
				EquipedWeaponStorageComponent weaponStorage = EquipedWeaponStorageComponent.Cast(toChar.FindComponent(EquipedWeaponStorageComponent));
				if (weaponStorage)
					weaponStorage.ACE_Overheating_ToggleRegisterWeapons(true);
				
				ACE_Overheating_AmmoTemperatureSystem ammoSystem = ACE_Overheating_AmmoTemperatureSystem.GetInstance(toChar.GetWorld());
				if (ammoSystem)
					ammoSystem.Register(toChar);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_RequestSetJamState(ACE_Overheating_BarrelComponent barrel, bool state)
	{
		Rpc(RplAsk_ACE_Overheating_RequestSetJamStateServer, Replication.FindItemId(barrel), state);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RplAsk_ACE_Overheating_RequestSetJamStateServer(RplId barrelID, bool state)
	{
		ACE_Overheating_BarrelComponent barrel = ACE_Overheating_BarrelComponent.Cast(Replication.FindItem(barrelID));
		if (barrel)
			barrel.SetState(state);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_RequestWeaponStateNotification(ENotification type, BaseWeaponComponent weapon)
	{
		Rpc(RpcAsk_ACE_Overheating_SendWeaponStateNotification, type, Replication.FindItemId(weapon));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_Overheating_SendWeaponStateNotification(ENotification type, RplId weaponId)
	{
		BaseWeaponComponent weapon = BaseWeaponComponent.Cast(Replication.FindItem(weaponId));
		if (!weapon)
			return;
		
		ACE_Overheating_BarrelComponent barrel = ACE_Overheating_BarrelComponent.FromWeapon(weapon);
		if (!barrel)
			return;
		
		int param1;
		switch (type)
		{
			case ENotification.ACE_OVERHEATING_BARREL_TEMPERATURE_RESULT:
			{
				param1 = barrel.GetBarrelTemperature() - ACE_PhysicalConstants.ZERO_CELSIUS;
				break;
			}
		}
		
		SCR_NotificationsComponent.SendToPlayer(GetPlayerId(), type, param1);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_RequestSwapBarrel(BaseWeaponComponent weapon)
	{
		Rpc(RpcAsk_ACE_Overheating_SwapBarrel, Replication.FindItemId(weapon));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_Overheating_SwapBarrel(RplId weaponId)
	{
		BaseWeaponComponent weapon = BaseWeaponComponent.Cast(Replication.FindItem(weaponId));
		if (!weapon)
			return;
		
		ACE_Overheating_BarrelComponent barrel = ACE_Overheating_BarrelComponent.FromWeapon(weapon);
		if (barrel)
			barrel.SetBarrelTemperature(ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE);
		
		ACE_Overheating_BarrelGlowEffectComponent glowEffect = ACE_Overheating_BarrelGlowEffectComponent.FromMuzzle(weapon.GetCurrentMuzzle());
		if (glowEffect)
			glowEffect.ForceIntensity(0);
		
		ACE_Overheating_SmokeEffectComponent smokeEffect = ACE_Overheating_SmokeEffectComponent.FromMuzzle(weapon.GetCurrentMuzzle());
		if (smokeEffect)
			smokeEffect.TerminateEffects();
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_RequestCoolBarrel(BaseWeaponComponent weapon)
	{
		Rpc(RpcAsk_ACE_Overheating_CoolBarrel, Replication.FindItemId(weapon));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_Overheating_CoolBarrel(RplId weaponId)
	{
		BaseWeaponComponent weapon = BaseWeaponComponent.Cast(Replication.FindItem(weaponId));
		if (!weapon)
			return;
		
		ACE_Overheating_BarrelComponent barrel = ACE_Overheating_BarrelComponent.FromWeapon(weapon);
		if (!barrel)
			return;
		
		ACE_Overheating_Settings settings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
		if (!settings)
			return;
		
		ACE_Overheating_BarrelTemperatureJob job = new ACE_Overheating_BarrelTemperatureJob();
		job.SetContext(new ACE_Overheating_BarrelContext(barrel));
		job.OnUpdate(settings.m_fWaterCoolingScale);
		RpcAsk_ACE_Overheating_SendWeaponStateNotification(ENotification.ACE_OVERHEATING_BARREL_TEMPERATURE_RESULT, weaponId);
	}
}
