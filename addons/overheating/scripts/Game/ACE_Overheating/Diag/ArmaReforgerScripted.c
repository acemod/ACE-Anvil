#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ArmaReforgerScripted : ChimeraGame
{
	protected bool m_bACE_Overheating_PreviousJamState = false;
	protected bool m_bACE_Overheating_PreviousJamSetting = false;
	protected float m_fACE_Overheating_PreviousBarrelTemperatureSetting;
	protected float m_fACE_Overheating_PreviousAmmoTemperatureSetting;
	
	//------------------------------------------------------------------------------------------------
	override bool OnGameStart()
	{
		DiagMenu.RegisterMenu(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU, "ACE Overheating", "ACE");
		DiagMenu.RegisterBool(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_TEMPERATURE_DIAG, "", "Show temperature diag", "ACE Overheating");
		DiagMenu.RegisterBool(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_JAM_SETTER, "", "Weapon jammed", "ACE Overheating");
		DiagMenu.RegisterRange(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_BARREL_TEMPERATURE_SETTER, "", "Barrel temperature [K]", "ACE Overheating", "298.15 1298.15 298.15 10");
		m_fACE_Overheating_PreviousBarrelTemperatureSetting = DiagMenu.GetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_BARREL_TEMPERATURE_SETTER);
		DiagMenu.RegisterRange(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_AMMO_TEMPERATURE_SETTER, "", "Ammo temperature [K]", "ACE Overheating", "298.15 1298.15 298.15 10");
		m_fACE_Overheating_PreviousAmmoTemperatureSetting = DiagMenu.GetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_AMMO_TEMPERATURE_SETTER);
		
		return super.OnGameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(BaseWorld world, float timeslice)
	{
		super.OnUpdate(world, timeslice);
		
		ACE_Overheating_JammingSystem system = ACE_Overheating_JammingSystem.GetInstance();
		if (!system)
			return;
		
		ACE_Overheating_MuzzleJamComponent jamComponent = system.GetLocalActiveJamComponent();
		if (jamComponent)
		{
			ACE_Overheating_HandleJamSetterDiag(jamComponent);
			ACE_Overheating_HandleTemperatureSetterDiag(jamComponent);
		}
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_TEMPERATURE_DIAG))
			ACE_Overheating_ShowTemperatureDiag()
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Overheating_HandleJamSetterDiag(ACE_Overheating_MuzzleJamComponent jamComponent)
	{
		bool currentJamState = jamComponent.IsJammed();
		bool currentJamSetting = DiagMenu.GetBool(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_JAM_SETTER);
		if (currentJamState == currentJamSetting)
			return;
		
		if (m_bACE_Overheating_PreviousJamState != currentJamState)
		{
			DiagMenu.SetValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_JAM_SETTER, currentJamState);
			m_bACE_Overheating_PreviousJamState = currentJamState;
			m_bACE_Overheating_PreviousJamSetting = currentJamState;
		}
		else if (m_bACE_Overheating_PreviousJamSetting != currentJamSetting)
		{
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			if (playerController)
				playerController.ACE_Overheating_RequestSetJamState(jamComponent, currentJamSetting);
			
			m_bACE_Overheating_PreviousJamState = currentJamSetting;
			m_bACE_Overheating_PreviousJamSetting = currentJamSetting;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Overheating_HandleTemperatureSetterDiag(ACE_Overheating_MuzzleJamComponent jamComponent)
	{
		float newBarrelSetting = DiagMenu.GetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_BARREL_TEMPERATURE_SETTER);
		
		if (newBarrelSetting != m_fACE_Overheating_PreviousBarrelTemperatureSetting)
			jamComponent.SetBarrelTemperature(newBarrelSetting);
		else
			DiagMenu.SetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_BARREL_TEMPERATURE_SETTER, jamComponent.GetBarrelTemperature());
					
		m_fACE_Overheating_PreviousBarrelTemperatureSetting = DiagMenu.GetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_BARREL_TEMPERATURE_SETTER);
		
		float newAmmoSetting = DiagMenu.GetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_AMMO_TEMPERATURE_SETTER);
		
		if (newAmmoSetting != m_fACE_Overheating_PreviousAmmoTemperatureSetting)
			jamComponent.SetAmmoTemperature(newAmmoSetting);
		else
			DiagMenu.SetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_AMMO_TEMPERATURE_SETTER, jamComponent.GetAmmoTemperature());
		
		m_fACE_Overheating_PreviousAmmoTemperatureSetting = DiagMenu.GetRangeValue(SCR_DebugMenuID.ACE_OVERHEATING_DEBUGUI_MENU_AMMO_TEMPERATURE_SETTER);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Overheating_ShowTemperatureDiag()
	{
		IEntity target;
		string targetType;
		ACE_DiagTools.GetDiagTargetCharacter(target, targetType);
		
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(target);
		if (!targetChar)
			return;
		
		BaseWeaponManagerComponent weaponManager = targetChar.GetWeaponManager();
		if (!weaponManager)
			return;
		
		ACE_Overheating_MuzzleJamComponent barrel = ACE_Overheating_MuzzleJamComponent.FromWeapon(weaponManager.GetCurrentWeapon());
		if (!barrel)
			return;
		
		ACE_Overheating_Settings settings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
		if (!settings)
			return;
		
		DbgUI.Begin(string.Format("ACE Overheating Diag (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Heating rate:                 %1 K/Round", settings.m_fHeatingScale * barrel.GetData().GetHeatPerShot() /  barrel.GetData().GetBarrelHeatCapacity()));
		DbgUI.Text(string.Format("Heat Transfer Coefficient:    %1 W/(m^2*K)", barrel.GetHeatTransferCoefficient()));
		DbgUI.Text(string.Format("Jam Chance:                   %1", barrel.GetJamChance()));
		DbgUI.Text(string.Format("Is Jammed:                    %1", barrel.IsJammed().ToString()));
		DbgUI.Text(string.Format("Barrel Temperature:           %1 K", barrel.GetBarrelTemperature()));
		DbgUI.PlotLiveClamped("BarrelTemperature", 500, 300, barrel.GetBarrelTemperature(), ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE, ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE + 1000, 100, 1000);
		DbgUI.Text(string.Format("Ammo Temperature:             %1 K", barrel.GetAmmoTemperature()));
		DbgUI.PlotLiveClamped("AmmoTemperature", 500, 300, barrel.GetAmmoTemperature(), ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE, ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE + 1000, 100, 1000);
		DbgUI.Text(string.Format("Cookoff Progress:             %1 %%", 100 * barrel.GetCookOffProgress() / barrel.GetCookOffProgressScale()));
		DbgUI.PlotLiveClamped("CookoffProgress", 500, 300, barrel.GetCookOffProgress() / barrel.GetCookOffProgressScale(), 0, 1, 100, 1000);
		DbgUI.End();
	}
}
#endif
