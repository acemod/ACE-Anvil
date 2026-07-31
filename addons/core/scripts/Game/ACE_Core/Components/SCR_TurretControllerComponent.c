//------------------------------------------------------------------------------------------------
modded class SCR_TurretControllerComponent : TurretControllerComponent
{
	protected bool m_bACE_InitDone = false;
	protected IEntity m_pACE_NextMagazine = null;
	
	protected static const int ATTEMPT_TIMEOUT = 100;
	protected static const int MAX_ATTEMPTS = 10;
	
	//------------------------------------------------------------------------------------------------
	void ACE_ReloadWeaponWith(notnull IEntity magazine)
	{
		RplComponent magazineRpl = RplComponent.Cast(magazine.FindComponent(RplComponent));
		if (!magazineRpl)
			return;
		
		Rpc(RpcDo_ACE_ReloadWeaponWithOwner, magazineRpl.Id());
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ACE_ReloadWeaponWithOwner(RplId magazineID)
	{
		if (!m_bACE_InitDone)
			ACE_Init();
		
		RplComponent magazineRpl =  RplComponent.Cast(Replication.FindItem(magazineID));
		if (!magazineRpl)
			return;
		
		m_pACE_NextMagazine = magazineRpl.GetEntity();
		// Has to be put on callqueue, as instant execution would fail
		GetGame().GetCallqueue().Call(GetGame().GetInputManager().SetActionValue, "TurretReload", 1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Init()
	{
		EventHandlerManagerComponent eventHandler = EventHandlerManagerComponent.Cast(GetOwner().FindComponent(EventHandlerManagerComponent));
		if (!eventHandler)
			return;
		
		eventHandler.RegisterScriptHandler("OnTurretReload", this, ACE_OnWeaponReload);
		m_bACE_InitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_OnWeaponReload(BaseWeaponComponent weapon, bool finished, TurretControllerComponent turretController)
	{
		if (!finished || !m_pACE_NextMagazine)
			return;
		
		RplComponent magazineRpl = RplComponent.Cast(m_pACE_NextMagazine.FindComponent(RplComponent));
		if (!magazineRpl)
			return;
		
		Rpc(RpcDo_ACE_ForceReloadWeaponWithServer, magazineRpl.Id(), 0);
		m_pACE_NextMagazine = null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Instantly reloads weapon with the given magazine
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_ACE_ForceReloadWeaponWithServer(RplId magazineID, int iAttempt)
	{
		// Check if occupied, as DoReloadWeaponWith crashes otherwise
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment || !compartment.IsOccupied())
			return;
		
		RplComponent magazineRpl = RplComponent.Cast(Replication.FindItem(magazineID));
		if (!magazineRpl)
			return;
		
		IEntity magazine = magazineRpl.GetEntity();
		if (!magazine)
			return;
		
		if (DoReloadWeaponWith(magazine))
			return;
		
		if (++iAttempt >= MAX_ATTEMPTS)
		{
			Debug.Error("Maximum number of attempts exceeded!");
			return;
		}
		
		// Reschedule getting out if it failed, for instance, when called while still reloading
		GetGame().GetCallqueue().CallLater(RpcDo_ACE_ForceReloadWeaponWithServer, ATTEMPT_TIMEOUT, false, magazineID, iAttempt);
	}
}
