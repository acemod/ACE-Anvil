//------------------------------------------------------------------------------------------------
modded class SCR_DisarmMineAction : SCR_BaseMineAction
{
	[Attribute(defvalue: "#ACE-Explosives_FailReason_Buried", desc: "String for when mine is buried")]
	protected LocalizedString m_sACE_Explosives_Buried;
	
	protected SCR_MineWeaponComponent m_pACE_Explosives_MineWeaponComponent;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		m_pACE_Explosives_MineWeaponComponent = SCR_MineWeaponComponent.Cast(pOwnerEntity.FindComponent(SCR_MineWeaponComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		if (m_pACE_Explosives_MineWeaponComponent && m_pACE_Explosives_MineWeaponComponent.ACE_Exposives_IsBuried())
		{
			SetCannotPerformReason(m_sACE_Explosives_Buried);
			return false;
		}
		
		return true;
	}
}
