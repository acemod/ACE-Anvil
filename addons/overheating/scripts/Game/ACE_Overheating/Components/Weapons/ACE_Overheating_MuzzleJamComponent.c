//------------------------------------------------------------------------------------------------
//! TO DO: Rename to ACE_Overheating_BarrelComponentClass
//! Shared data across all instances of a prefab
class ACE_Overheating_MuzzleJamComponentClass : ScriptComponentClass
{
	// Default: Specific heat capacity of steel
	[Attribute(defvalue: "0.466", desc: "Specific heat capacity of the barrel [J / (g * K)]", category: "Barrel")]
	protected float m_fBarrelSpecificHeatCapacity;
	
	// Default: Emissivity of oxidized steel
	[Attribute(defvalue: "0.85", desc: "Emissivity of the barrel surface", category: "Barrel")]
	protected float m_fBarrelEmissivity;
	
	// Default: Value from ACE3
	[Attribute(defvalue: "0.55", desc: "Ratio between barrel mass and total mass of the weapon", category: "Barrel")]
	protected float m_fBarrelMassFraction;
	
	// Default: Measured value from AK74_body.xob
	[Attribute(defvalue: "0.01476", desc: "Outer diameter of the barrel [m]. ", precision: 5, category: "Barrel")]
	protected float m_fBarrelDiameter;
	
	// MachineGun_Base.et has 0.8 to matches the 200 Rnd <2 min reported in M60 manual: https://upload.wikimedia.org/wikipedia/commons/c/c7/TM-9-1005-224-10.pdf
	[Attribute(defvalue: "1", desc: "Heat transferred to the barrel is given by this scale times the kinetic energy of the bullet.")]
	protected float m_fHeatingScale;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Probability to jam vs temperature [K]", params: "1300 0.1 0 0")]
	protected ref Curve m_cJamChanceTemperatureCurve;
	
	protected bool m_bInitDone = false;
	protected float m_fHeatPerShot;
	protected float m_fBarrelHeatCapacity;
	protected float m_fBarrelSurfaceArea;
	protected float m_fBulletMass;
	
	//------------------------------------------------------------------------------------------------
	//! Calculate derived quantities
	void Init(IEntity instanceOwner)
	{
		if (m_bInitDone)
			return;
		
		SCR_WeaponAttachmentsStorageComponent weaponStorage = SCR_WeaponAttachmentsStorageComponent.Cast(instanceOwner.FindComponent(SCR_WeaponAttachmentsStorageComponent));
		if (!weaponStorage)
			return;
		
		SCR_ItemAttributeCollection attributes = SCR_ItemAttributeCollection.Cast(weaponStorage.GetAttributes());
		if (!attributes)
			return;	
		
		MuzzleComponent muzzle = MuzzleComponent.Cast(instanceOwner.FindComponent(MuzzleComponent));
		if (!muzzle)
			return;
		
		BaseMagazineComponent magazine = muzzle.GetMagazine();
		if (!magazine)
			return;
		
		float initialBulletSpeed;
		GetBulletProperties(muzzle, m_fBulletMass, initialBulletSpeed);
		// Heat produced by a shot is the initial kinetic energy of the bullet times a heating scale
		m_fHeatPerShot = m_fHeatingScale * 0.5 * m_fBulletMass * Math.Pow(initialBulletSpeed, 2);
		m_fBarrelHeatCapacity = m_fBarrelSpecificHeatCapacity * m_fBarrelMassFraction * attributes.GetWeight() * 1000; // 1000 for kg to g
		m_fBarrelSurfaceArea = Math.PI * m_fBarrelDiameter * ComputeBarrelLength(instanceOwner);
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Calculated from barrel memory points
	protected float ComputeBarrelLength(IEntity weapon)
	{
		Animation weaponAnim = weapon.GetAnimation();
		if (!weaponAnim)
			return 0;
		
		vector chamberTransform[4], muzzleTransform[4];
		weaponAnim.GetBoneLocalMatrix(weaponAnim.GetBoneIndex("barrel_chamber"), chamberTransform);
		weaponAnim.GetBoneLocalMatrix(weaponAnim.GetBoneIndex("barrel_muzzle"), muzzleTransform);
		return vector.Distance(chamberTransform[3], muzzleTransform[3]);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Retrieves initial speed and mass for bullet in first magazine
	protected void GetBulletProperties(MuzzleComponent muzzle, out float bulletMass, out float initialBulletSpeed)
	{
		BaseMagazineComponent magazine = muzzle.GetMagazine();
		if (!magazine)
			return;
		
		IEntityComponentSource magazineSrc = magazine.GetComponentSource(magazine.GetOwner());
		if (!magazineSrc)
			return;
		
		ResourceName ammoConfigName;
		magazineSrc.Get("AmmoConfig", ammoConfigName);
		
		Resource ammoConfigRes = Resource.Load(ammoConfigName);
		if (!ammoConfigRes.IsValid())
			return;
		
		BaseContainer ammonConfigSrc = ammoConfigRes.GetResource().ToBaseContainer();
		if (!ammonConfigSrc)
			return;
		
		array<ResourceName> ammoNames = {};
		ammonConfigSrc.Get("AmmoResourceArray", ammoNames);
		
		if (ammoNames.IsEmpty())
			return;
		
		Resource ammoRes = Resource.Load(ammoNames[0]);
		if (!ammoRes.IsValid())
			return;
		
		IEntitySource ammoSrc = ammoRes.GetResource().ToEntitySource();
		if (!ammoSrc)
			return;
		
		IEntityComponentSource shellMovementSource = ACE_BaseContainerTools.FindComponentSource(ammoSrc, ShellMoveComponent);
		if (!shellMovementSource)
			return;
		
		shellMovementSource.Get("Mass", bulletMass);
		shellMovementSource.Get("InitSpeed", initialBulletSpeed);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns heat capacity of the barrel in J / K
	float GetBarrelHeatCapacity()
	{
		return m_fBarrelHeatCapacity;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the emissivity of the barrel surface for radiative heat loss
	float GetBarrelEmissivity()
	{
		return m_fBarrelEmissivity;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns outer surface area of the barrel in m^2
	float GetBarrelSurfaceArea()
	{
		return m_fBarrelSurfaceArea;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns outer diameter of the barrel in m
	float GetBarrelDiameter()
	{
		return m_fBarrelDiameter;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns mass of bullet in kg
	float GetBulletMass()
	{
		return m_fBulletMass;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the heat transferred to the barrel per shot in J
	float GetHeatPerShot()
	{
		return m_fHeatPerShot;
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeJamChance(float temperature)
	{
		return Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cJamChanceTemperatureCurve)[1];
	}
}

//------------------------------------------------------------------------------------------------
//! TO DO: Rename to ACE_Overheating_BarrelComponent
class ACE_Overheating_MuzzleJamComponent : ScriptComponent
{
	[RplProp(onRplName: "OnStateChanged")]
	protected bool m_bIsJammed = false;
	
	protected float m_fBarrelTemperature;
	protected float m_fAmmoTemperature;
	protected float m_fCookOffProgress;
	protected float m_fCookOffProgressScale;
	protected float m_fLastTemperatureTimestamp;
	protected int m_iHeatCounter;
	protected float m_fJamChance;
	
	protected MuzzleComponent m_pMuzzle;
	protected ACE_Overheating_MuzzleJamComponentClass m_pData;
	
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
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		WeaponComponent weapon = WeaponComponent.Cast(owner.FindComponent(WeaponComponent));
		if (!weapon)
			return;
		
		m_pMuzzle = MuzzleComponent.Cast(weapon.FindComponent(MuzzleComponent));
		if (!m_pMuzzle)
			return;
		
		m_pData = ACE_Overheating_MuzzleJamComponentClass.Cast(GetComponentData(owner));
		m_pData.Init(owner);
		// Temporary solution: Use standard ambient temperature until we got a proper weather system
		m_fBarrelTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		m_fAmmoTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		m_fJamChance = m_pData.ComputeJamChance(m_fBarrelTemperature);
		InitCookOffCookOffProgressScale();
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
	
	//------------------------------------------------------------------------------------------------
	void SetBarrelTemperature(float temperature)
	{
		m_fBarrelTemperature = temperature;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetBarrelTemperature()
	{
		return m_fBarrelTemperature;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAmmoTemperature(float temperature)
	{
		m_fAmmoTemperature = temperature;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAmmoTemperature()
	{
		return m_fAmmoTemperature;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetCookOffProgress()
	{
		return m_fCookOffProgress;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCookOffProgress(float value)
	{
		m_fCookOffProgress = value;
	}
	
	//------------------------------------------------------------------------------------------------
	void InitCookOffCookOffProgressScale()
	{
		m_fCookOffProgressScale = Math.RandomGaussFloat(0.078541632, 1);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetCookOffProgressScale()
	{
		return m_fCookOffProgressScale;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetLastTemperatureTimestamp(float timestamp)
	{
		m_fLastTemperatureTimestamp = timestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetLastTemperatureTimestamp()
	{
		return m_fLastTemperatureTimestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Increments the heat counter by one fired bullet
	void IncremenHeatCounter()
	{
		++m_iHeatCounter;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the accumulated heat and resets the heat counter
	float PopAccumulatedHeat()
	{
		float heat = m_pData.GetHeatPerShot() * m_iHeatCounter;
		m_iHeatCounter = 0;
		return heat;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Overheating_MuzzleJamComponentClass GetData()
	{
		return m_pData;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetJamChance(float probability)
	{
		m_fJamChance = probability;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetJamChance()
	{
		return m_fJamChance;
	}
	
#ifdef ENABLE_DIAG
	protected float m_fHeatTransferCoefficient;
	
	//------------------------------------------------------------------------------------------------
	void SetHeatTransferCoefficient(float coefficient)
	{
		m_fHeatTransferCoefficient = coefficient;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetHeatTransferCoefficient()
	{
		return m_fHeatTransferCoefficient;
	}
#endif
}
