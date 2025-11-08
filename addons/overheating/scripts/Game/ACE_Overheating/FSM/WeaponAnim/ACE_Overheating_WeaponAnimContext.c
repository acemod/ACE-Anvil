//------------------------------------------------------------------------------------------------
class ACE_Overheating_WeaponAnimContext : Managed
{
	SCR_InventoryStorageManagerComponent m_pInventoryManager;
	SCR_CharacterControllerComponent m_pCharController;
	bool m_bWasWeaponADS;
	CharacterAnimationComponent m_pCharAnim;
	BaseWeaponComponent m_pWeapon;
	ACE_Overheating_BarrelComponent m_pBarrel;
	WeaponAnimationComponent m_pWeaponAnim;
	TAnimGraphCommand m_iCharReloadCommandID;
	TAnimGraphCommand m_iWeaponReloadCommandID;
	TAnimGraphEvent m_iRackBoltEvent;
	TAnimGraphTag m_iReloadTag;
	bool m_bIsStartRemovingMag = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_WeaponAnimContext(ChimeraCharacter char, BaseWeaponComponent weapon, ACE_Overheating_BarrelComponent barrel)
	{
		m_pInventoryManager = SCR_InventoryStorageManagerComponent.Cast(char.FindComponent(SCR_InventoryStorageManagerComponent));
		m_pCharController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		m_bWasWeaponADS = m_pCharController.GetInputContext().IsWeaponADS();
		m_pCharAnim = char.GetAnimationComponent();
		m_iCharReloadCommandID = m_pCharAnim.BindCommand("CMD_Weapon_Reload");
		m_iRackBoltEvent = m_pCharAnim.BindEvent("Weapon_Rack_Bolt");		
		m_iReloadTag = m_pCharAnim.BindTag("TagWeaponReload");
		m_pWeapon = weapon;
		m_pBarrel = barrel;
		m_pWeaponAnim = WeaponAnimationComponent.Cast(weapon.GetOwner().FindComponent(WeaponAnimationComponent));
		m_iWeaponReloadCommandID = m_pWeaponAnim.BindCommand("CMD_Weapon_Reload");
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReloading()
	{
		return (m_pCharAnim.IsPrimaryTag(m_iReloadTag) || m_pCharAnim.IsSecondaryTag(m_iReloadTag));
	}
}
