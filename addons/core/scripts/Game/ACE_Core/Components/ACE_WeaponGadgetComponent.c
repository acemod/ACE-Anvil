//------------------------------------------------------------------------------------------------
class ACE_WeaponGadgetComponentClass : SCR_WeaponComponentClass
{
	[Attribute(defvalue: "false", desc: "Whether the component should only get updated on the server")]
	protected bool m_bServerUpdateOnly
	
	//------------------------------------------------------------------------------------------------
	bool IsServerUpdateOnly()
	{
		return m_bServerUpdateOnly;
	}
}

//------------------------------------------------------------------------------------------------
//! Component for implementing per frame handling for equipped weapons
class ACE_WeaponGadgetComponent : SCR_WeaponComponent
{
	//------------------------------------------------------------------------------------------------
	override void OnWeaponActive()
	{
		super.OnWeaponActive();
		
		ACE_WeaponGadgetComponentClass data = ACE_WeaponGadgetComponentClass.Cast(GetComponentData(GetOwner()));
		if (data && data.IsServerUpdateOnly() && !Replication.IsServer())
			return;
		
		ACE_WeaponGadgetsSystem system = ACE_WeaponGadgetsSystem.GetInstance(GetGame().GetWorld());
		if (system)
			system.Register(this);
	}

	//------------------------------------------------------------------------------------------------
	override void OnWeaponInactive()
	{
		super.OnWeaponInactive();
		
		ACE_WeaponGadgetComponentClass data = ACE_WeaponGadgetComponentClass.Cast(GetComponentData(GetOwner()));
		if (data && data.IsServerUpdateOnly() && !Replication.IsServer())
			return;

		ACE_WeaponGadgetsSystem system = ACE_WeaponGadgetsSystem.GetInstance(GetGame().GetWorld());
		if (system)
			system.Unregister(this);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		ACE_WeaponGadgetComponentClass data = ACE_WeaponGadgetComponentClass.Cast(GetComponentData(owner));
		if (data && data.IsServerUpdateOnly() && !Replication.IsServer())
			return;
		
		ACE_WeaponGadgetsSystem system = ACE_WeaponGadgetsSystem.GetInstance(owner.GetWorld());
		if (system)
			system.Unregister(this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override this to add per frame handler
	void OnWeaponUpdate(float timeSlice);
}
