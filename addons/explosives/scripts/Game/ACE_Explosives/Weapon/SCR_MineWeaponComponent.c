//------------------------------------------------------------------------------------------------
modded class SCR_MineWeaponComponent : WeaponComponent
{
	[Attribute(defvalue: "0 -0.09 0", desc: "Local offset vector applied when buried")]
	protected vector m_vACE_Explosives_BurialOffset;
	
	[RplProp(onRplName: "ACE_Exposives_OnToggleBurial")]
	protected bool m_bACE_Explosives_IsBuried;
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Exposives_IsBuried()
	{
		return m_bACE_Explosives_IsBuried;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Exposives_ToggleBurial(bool bury)
	{
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rpl || rpl.IsProxy())
			return;
		
		m_bACE_Explosives_IsBuried = bury;
		ACE_Exposives_OnToggleBurial();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Exposives_OnToggleBurial()
	{
		IEntity mine = GetOwner();
		float sign = 2 * m_bACE_Explosives_IsBuried - 1;
		mine.SetOrigin(mine.GetOrigin() + sign * mine.VectorToParent(m_vACE_Explosives_BurialOffset));
		mine.Update();
	}
}
