//------------------------------------------------------------------------------------------------
class ACE_Overheating_MuzzleJamComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_MuzzleJamComponent : ScriptComponent
{
	[RplProp(onRplName: "OnStateChanged")]
	protected bool m_bIsJammed = false;
	
	protected MuzzleComponent m_pMuzzle;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_MuzzleJamComponent FromWeapon(IEntity weapon)
	{
		if (!weapon)
			return null;
		
		return ACE_Overheating_MuzzleJamComponent.FromWeapon(BaseWeaponComponent.Cast(weapon.FindComponent(BaseWeaponComponent)));
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_MuzzleJamComponent FromWeapon(BaseWeaponComponent weapon)
	{
		if (!weapon)
			return null;
		
		return ACE_Overheating_MuzzleJamComponent.FromMuzzle(weapon.GetCurrentMuzzle());
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_MuzzleJamComponent FromMuzzle(BaseMuzzleComponent muzzle)
	{
		if (!muzzle)
			return null;
		
		return ACE_Overheating_MuzzleJamComponent.Cast(muzzle.FindComponent(ACE_Overheating_MuzzleJamComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		WeaponComponent weapon = WeaponComponent.Cast(owner.FindComponent(WeaponComponent));
		m_pMuzzle = MuzzleComponent.Cast(weapon.FindComponent(MuzzleComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	void SetState(bool isJammed)
	{
		if (!isJammed)
			m_pMuzzle.ClearChamber(m_pMuzzle.GetCurrentBarrelIndex());
		
		m_bIsJammed = isJammed;
		OnStateChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsJammed()
	{
		return m_bIsJammed;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnStateChanged()
	{
		ACE_Overheating_JammingSystem system = ACE_Overheating_JammingSystem.GetInstance();
		if (system)
			system.OnJamStateChanged(this, m_bIsJammed);
	}
	
	//------------------------------------------------------------------------------------------------
	MuzzleComponent GetMuzzle()
	{
		return m_pMuzzle;
	}
}
