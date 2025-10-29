//------------------------------------------------------------------------------------------------
//! Shared data across all instances of a prefab
class ACE_Overheating_BarrelComponentClass : ScriptComponentClass
{
	// Default: Specific heat capacity of steel
	[Attribute(defvalue: "0.466", desc: "Specific heat capacity of the barrel [J / (g * K)]", category: "Barrel")]
	protected float m_fBarrelSpecificHeatCapacity;
	
	// Default: Emissivity of oxidized steel
	[Attribute(defvalue: "0.85", desc: "Emissivity of the barrel surface", category: "Barrel")]
	protected float m_fBarrelEmissivity;
	
	// Default: 0.550 (Value from ACE3)
	// M60 (MG_M60_base.et): 3.74 / 10.4 = 0.360 (Spare barrel mass (literature value) divided by total mass (ingame value))
	// M249 (MG_M249_base.et): 3.178 / 6.9 = 0.461
	// PKM (MachineGun_Base.et): 2.4 / 7.5 = 0.32
	[Attribute(defvalue: "0.55", desc: "Ratio between barrel mass and total mass of the weapon", category: "Barrel")]
	protected float m_fBarrelMassFraction;
	
	// Default (Measured value from AK74_body.xob): 0.01476
	// M60: 0.01828
	[Attribute(defvalue: "0.01476", desc: "Outer diameter of the barrel [m]. ", precision: 5, category: "Barrel")]
	protected float m_fBarrelDiameter;
	
	[Attribute(defvalue: "barrel_chamber", desc: "Name of the bone for the chamber")]
	protected string m_sChamberBoneName;
	
	[Attribute(defvalue: "chamber", desc: "If m_sChamberBoneName does not exist, it searches for the first bone that contains this pattern")]
	protected string m_sFallbackChamberBoneNamePattern;
	
	[Attribute(defvalue: "barrel_muzzle", desc: "Name of the bone for the muzzle")]
	protected string m_sMuzzleBoneName;
	
	[Attribute(defvalue: "muzzle", desc: "If m_sMuzzleBoneName does not exist, it searches for the first bone that contains this pattern")]
	protected string m_sFallbackMuzzleBoneNamePattern;
	
	// M60 (MachineGun_Base.et): 0.731 (Fitted together with m_fBaseHeatTransferCoefficient, such that cook-off progress will exactly reach 1.0 and then stop progressing for the next bullet and T_barrel_final=35°C when firing 100 RPM for 2 minutes and then cease fire for 15 minutes)
	// M249 (MG_M249_base.et): 1.000
	// PKM (MachineGun_Base.et): 0.508 (Calculated scale to get similar heat transfer per shot as M60)
	[Attribute(defvalue: "1", desc: "Heat transferred to the barrel is given by this scale times the kinetic energy of the bullet.")]
	protected float m_fHeatingScale;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Probability to jam vs temperature [K]", params: "1300 0.1 0 0")]
	protected ref Curve m_cJamChanceTemperatureCurve;
	
	// Additional temperature-dependent muzzle dispersion factor f
	// -----------------------------------------------------------
	// Base dispersion cone diameter gets multiplied by (1 + f)
	//
	// Formula: f = tan(a + da(T)) / tan(a) - 1
	// First-order Taylor approximation for small angles: f ≈ da(T) / a
	// where a is base dispersion angle and da(T) is the additional temperature-dependent dispersion
	//
	// Using Reforger's M16A4 base dispersion: Cone height = 400 m; Cone diameter = 0.4 m => a ≈ 0.5 mrad
	// and ACE3 additional dispersion: 0 mils at 0 °C, 0.5 mils at 333°C, 2.2 mils at 666 °C, 5 mils at 1000 °C
	// => Additional dispersion factors: 0.0 at 0 °C, 1.0 at 333°C, 4.4 at 666 °C, 10.0 at 1000 °C
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Additional muzzle dispersion factor vs temperature [K]", params: "1300 30 0 0")]
	protected ref Curve m_cMuzzleDispersionFactorTemperatureCurve;
	
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
		
		MuzzleComponent muzzle = MuzzleComponent.Cast(instanceOwner.FindComponent(MuzzleComponent));
		if (!muzzle)
			return;
		
		float initialBulletSpeed;
		GetBulletProperties(muzzle, m_fBulletMass, initialBulletSpeed);
		// Heat produced by a shot is the initial kinetic energy of the bullet times a heating scale
		m_fHeatPerShot = m_fHeatingScale * 0.5 * m_fBulletMass * Math.Pow(initialBulletSpeed, 2);
		m_fBarrelHeatCapacity = m_fBarrelSpecificHeatCapacity * ComputeBarrelMass(instanceOwner) * 1000; // 1000 for kg to g
		m_fBarrelSurfaceArea = Math.PI * m_fBarrelDiameter * ComputeBarrelLength(instanceOwner);
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Estimates mass of the barrel in kg
	protected float ComputeBarrelMass(IEntity weapon)
	{
		SCR_WeaponAttachmentsStorageComponent weaponStorage = SCR_WeaponAttachmentsStorageComponent.Cast(weapon.FindComponent(SCR_WeaponAttachmentsStorageComponent));
		if (!weaponStorage)
			return 0;
		
		SCR_ItemAttributeCollection attributes = SCR_ItemAttributeCollection.Cast(weaponStorage.GetAttributes());
		if (!attributes)
			return 0;
		
		return m_fBarrelMassFraction * attributes.GetWeight();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Calculated from barrel memory points in meters
	protected float ComputeBarrelLength(IEntity weapon)
	{
		Animation weaponAnim = weapon.GetAnimation();
		if (!weaponAnim)
			return 0;
		
		TNodeId chamberBoneID = FindBoneID(weaponAnim, m_sChamberBoneName, m_sFallbackChamberBoneNamePattern);
		TNodeId muzzleBoneID = FindBoneID(weaponAnim, m_sMuzzleBoneName, m_sFallbackMuzzleBoneNamePattern);
		vector chamberTransform[4], muzzleTransform[4];
		weaponAnim.GetBoneLocalMatrix(chamberBoneID, chamberTransform);
		weaponAnim.GetBoneLocalMatrix(muzzleBoneID, muzzleTransform);
		return vector.Distance(chamberTransform[3], muzzleTransform[3]);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets bone ID for a weapon
	//! Returns bone ID for defaultBoneName if it exists
	//! Otherwise searches for fallbackPattern and returns the first match
	protected TNodeId FindBoneID(Animation weaponAnim, string defaultBoneName, string fallbackPattern)
	{
		TNodeId boneID = weaponAnim.GetBoneIndex(defaultBoneName);
		if (boneID >= 0)
			return boneID;
		
		array<string> boneNames = {};
		weaponAnim.GetBoneNames(boneNames);
		
		foreach (string name : boneNames)
		{
			name.ToLower();
			if (name.Contains(fallbackPattern))
				return weaponAnim.GetBoneIndex(name);
		}
		
		return -1;
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
		return LegacyCurve.Curve(ECurveType.CurveProperty2D, temperature, m_cJamChanceTemperatureCurve)[1];
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeAdditionalMuzzleDispersionFactor(float temperature)
	{
		return LegacyCurve.Curve(ECurveType.CurveProperty2D, temperature, m_cMuzzleDispersionFactorTemperatureCurve)[1];
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_BarrelComponent : ScriptComponent
{
	[RplProp(onRplName: "OnStateChanged")]
	protected bool m_bIsJammed = false;
	
	[RplProp()]
	protected float m_fClearJamFailureChance = 0.1;
	
	protected float m_fBarrelTemperature;
	protected float m_fAmmoTemperature;
	protected float m_fCookOffProgress;
	protected float m_fCookOffProgressScale;
	protected float m_fLastTemperatureTimestamp;
	protected int m_iHeatCounter;
	protected float m_fJamChance;
	
	protected MuzzleComponent m_pMuzzle;
	protected ACE_Overheating_BarrelComponentClass m_pData;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_BarrelComponent FromWeapon(IEntity weapon)
	{
		if (!weapon)
			return null;
		
		return ACE_Overheating_BarrelComponent.FromWeapon(BaseWeaponComponent.Cast(weapon.FindComponent(BaseWeaponComponent)));
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_BarrelComponent FromWeapon(BaseWeaponComponent weapon)
	{
		if (!weapon)
			return null;
		
		return ACE_Overheating_BarrelComponent.FromMuzzle(weapon.GetCurrentMuzzle());
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_BarrelComponent FromMuzzle(BaseMuzzleComponent muzzle)
	{
		if (!muzzle)
			return null;
		
		return ACE_Overheating_BarrelComponent.Cast(muzzle.FindComponent(ACE_Overheating_BarrelComponent));
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
		
		// Server side init
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (rpl && rpl.IsProxy())
			return;
		
		m_pData = ACE_Overheating_BarrelComponentClass.Cast(GetComponentData(owner));
		m_pData.Init(owner);
		
		ACE_Overheating_Settings settings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
		if (settings)
			m_fClearJamFailureChance = settings.m_fClearJamFailureChance;
		
		// Temporary solution: Use standard ambient temperature until we got a proper weather system
		m_fBarrelTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		m_fAmmoTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		m_fJamChance = m_pData.ComputeJamChance(m_fBarrelTemperature);
		InitCookOffCookOffProgressScale();
		Replication.BumpMe();
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
		ACE_Overheating_JammingSystem system = ACE_Overheating_JammingSystem.GetInstance(GetOwner().GetWorld());
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
	ACE_Overheating_BarrelComponentClass GetData()
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
	
	//------------------------------------------------------------------------------------------------
	float GetClearJamFailureChance()
	{
		return m_fClearJamFailureChance;
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
