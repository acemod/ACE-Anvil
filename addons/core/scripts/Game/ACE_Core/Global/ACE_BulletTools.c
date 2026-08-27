//------------------------------------------------------------------------------------------------
class ACE_BulletTools
{
	//------------------------------------------------------------------------------------------------
	//! Return default bullet resource names of a muzzle
	static array<ResourceName> GetDefaultResourceNamesFromMuzzle(BaseMuzzleComponent muzzle)
	{
		ResourceName magazineOrProjectilePrefabName = muzzle.GetDefaultMagazineOrProjectileName();
		if (magazineOrProjectilePrefabName.IsEmpty())
		{
			BaseMagazineComponent magazine = muzzle.GetMagazine();
			if (!magazine)
				return {};
			
			magazineOrProjectilePrefabName = magazine.GetOwner().GetPrefabData().GetPrefabName();
		}
		
		array<ResourceName> bulletPrefabNames = GetResourceNamesFromMagazine(magazineOrProjectilePrefabName);
		if (bulletPrefabNames)
			return bulletPrefabNames;
		
		return {magazineOrProjectilePrefabName};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return all bullet resource names of a magazine
	static array<ResourceName> GetResourceNamesFromMagazine(ResourceName magazinePrefabName)
	{
		Resource magazineRes = Resource.Load(magazinePrefabName);
		if (!magazineRes.IsValid())
			return null;
		
		BaseContainer mazagineSource = magazineRes.GetResource().ToBaseContainer();
		if (!mazagineSource)
			return null;
		
		IEntityComponentSource magazineComponentSource = ACE_BaseContainerTools.FindComponentSource(mazagineSource, BaseMagazineComponent);
		if (!magazineComponentSource)
			return null;
		
		ResourceName ammoConfigName;
		magazineComponentSource.Get("AmmoConfig", ammoConfigName);
		
		Resource ammoConfigRes = Resource.Load(ammoConfigName);
		if (!ammoConfigRes.IsValid())
			return null;
		
		BaseContainer ammonConfigSrc = ammoConfigRes.GetResource().ToBaseContainer();
		if (!ammonConfigSrc)
			return null;
		
		array<ResourceName> ammoNames = {};
		ammonConfigSrc.Get("AmmoResourceArray", ammoNames);
		return ammoNames;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return mass of bullet
	static float GetMass(ResourceName bulletPrefabName)
	{
		return GetFloatProperty(bulletPrefabName, "Mass");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return initial speed of bullet
	static float GetInitialSpeed(ResourceName bulletPrefabName)
	{
		return GetFloatProperty(bulletPrefabName, "InitSpeed");
	}
	
	//------------------------------------------------------------------------------------------------
	protected static float GetFloatProperty(ResourceName bulletPrefabName, string propertyName)
	{
		Resource bulletRes = Resource.Load(bulletPrefabName);
		if (!bulletRes.IsValid())
			return 0;
		
		IEntitySource bulletSource = bulletRes.GetResource().ToEntitySource();
		if (!bulletSource)
			return 0;
		
		IEntityComponentSource shellMovementSource = ACE_BaseContainerTools.FindComponentSource(bulletSource, ShellMoveComponent);
		if (!shellMovementSource)
			return 0;
		
		float value;
		shellMovementSource.Get(propertyName, value);
		return value;
	}
}
