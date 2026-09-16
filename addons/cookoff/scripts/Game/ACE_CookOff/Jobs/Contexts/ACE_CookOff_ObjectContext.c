//------------------------------------------------------------------------------------------------
class ACE_CookOff_ObjectContext : ACE_FrameJobScheduler_IObjectContext<IEntity>
{
	InventoryStorageManagerComponent m_pStorageManager;
	ref array<BaseMagazineComponent> m_aMagazines = {};
	ref array<BaseTriggerComponent> m_aExplosives = {};
	ACE_CookOff_HelperEntity m_pHelperEntity;
	int m_iTotalAmmoCount = 0;
	float m_fDetonationCountdown;
	
	protected static const ResourceName HELPER_ENTITY_PREFAB_NAME = "{503A79B1A99FEF74}Prefabs/Helpers/ACE_CookOff_HelperEntity.et";
	
	//------------------------------------------------------------------------------------------------
	void ACE_CookOff_ObjectContext(IEntity object)
	{
		ACE_CookOff_Settings settings = ACE_SettingsHelperT<ACE_CookOff_Settings>.GetModSettings();
		if (settings)
			m_fDetonationCountdown = Math.RandomFloat(settings.m_fMinCookOffDelay, settings.m_fMaxCookOffDelay);
		
		CompileMagazinesInStorage(object);
		CompileMagazinesInTurrets(object);
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Parent = object;
		m_pHelperEntity = ACE_CookOff_HelperEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load(HELPER_ENTITY_PREFAB_NAME), null, params));
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CompileMagazinesInStorage(IEntity vehicle)
	{
		m_pStorageManager = InventoryStorageManagerComponent.Cast(vehicle.FindComponent(InventoryStorageManagerComponent));
		if (!m_pStorageManager)
			return;
		
		array<IEntity> items = {};
		m_pStorageManager.FindItems(items, new ACE_CookOff_AmmoPredicate());
		
		foreach (IEntity item : items)
		{
			BaseMagazineComponent magazine = BaseMagazineComponent.Cast(item.FindComponent(BaseMagazineComponent));
			if (magazine)
			{
				int ammoCount = magazine.GetAmmoCount();
				if (ammoCount == 0)
					continue;
				
				m_aMagazines.Insert(magazine);
				m_iTotalAmmoCount += ammoCount;
				continue;
			}
			
			BaseTriggerComponent explosive = BaseTriggerComponent.Cast(item.FindComponent(BaseTriggerComponent));
			if (explosive)
			{
				WeaponComponent weapon = WeaponComponent.Cast(item.FindComponent(WeaponComponent));
				if (weapon && weapon.GetWeaponType() == EWeaponType.WT_SMOKEGRENADE)
					continue;
				
				m_aExplosives.Insert(explosive);
				++m_iTotalAmmoCount;
				continue;
			}	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CompileMagazinesInTurrets(IEntity vehicle)
	{
		SCR_BaseCompartmentManagerComponent compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		if (!compartmentManager)
			return;
		
		array<BaseCompartmentSlot> compartments = {};
		compartmentManager.GetCompartments(compartments);
		
		foreach (BaseCompartmentSlot compartment : compartments)
		{
			BaseWeaponManagerComponent weaponManager = BaseWeaponManagerComponent.Cast(compartment.GetOwner().FindComponent(BaseWeaponManagerComponent));
			if (!weaponManager)
				continue;
			
			array<BaseWeaponComponent> weapons = {};
			weaponManager.GetWeapons(weapons);
			
			foreach (BaseWeaponComponent weapon : weapons)
			{
				BaseMagazineComponent magazine = weapon.GetCurrentMagazine();
				if (!magazine)
					continue;
				
				int ammoCount = magazine.GetAmmoCount();
				if (ammoCount == 0)
					continue;
				
				m_aMagazines.Insert(magazine);
				m_iTotalAmmoCount += ammoCount;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;
		
		return !m_aMagazines.IsEmpty() || !m_aExplosives.IsEmpty();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_CookOff_ObjectContext()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pHelperEntity);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_CookOff_AmmoPredicate : ACE_InventorySearchPredicate
{
	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return (item.FindComponent(BaseMagazineComponent) || item.FindComponent(ProjectileMoveComponent) || item.FindComponent(SCR_MineDamageManager));
	}
}
