//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/Weapon/Sights", description: "", color: "0 0 255 255")]
modded class SCR_2DOpticsComponentClass : ScriptedSightsComponentClass
{
	[Attribute(defvalue: "30 30 0", desc: "Maximum values of zeroing in mrads. Horizontal is x, vertical is y.", category: "BaseSights")]
	protected vector m_fACE_MaxZeroingsMrad;
	
	[Attribute(defvalue: "-30 -30 0", desc: "Minimal values of zeroing in mrads. Horizontal is x, vertical is y.", category: "BaseSights")]
	protected vector m_fACE_MinZeroingsMrad;
	
	[Attribute(defvalue: "1.0", desc: "Factor for scaling effective elevation change. Can be used for fine-tuning accuracy.", category: "BaseSights")]
	protected float m_fACE_EffectiveVerticalZeroingScale;
	
	bool m_bACE_Initialized = false;
	bool m_bACE_HasZeroings = false;
			
	//------------------------------------------------------------------------------------------------
	//! Enforce limit for zeroing angles in mrads
	void ACE_LimitZeroings(inout float horizontal, inout float vertical)
	{
		horizontal = Math.Clamp(horizontal, m_fACE_MinZeroingsMrad[0], m_fACE_MaxZeroingsMrad[0]);
		vertical = Math.Clamp(vertical, m_fACE_MinZeroingsMrad[1], m_fACE_MaxZeroingsMrad[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_GetEffectiveVerticalZeroingScale()
	{
		return m_fACE_EffectiveVerticalZeroingScale;
	}
}

//------------------------------------------------------------------------------------------------
modded class SCR_2DOpticsComponent : ScriptedSightsComponent
{
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data || data.m_bACE_Initialized)
			return;
		
		array<vector> sightsRanges = ACE_GetSightsRanges();
		data.m_bACE_HasZeroings = (sightsRanges && sightsRanges.Count() > 1);
		data.m_bACE_Initialized = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Changes horizontal and/or vertical zeroings by the given values in mrad
	void ACE_AdjustZeroings(float horizontalChange = 0.0, float verticalChange = 0.0)
	{		
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return;
		
		// Not available if scope has no zeroing in vanilla
		if (!data.m_bACE_HasZeroings)
			return;
		
		float horizontal;
		float vertical;
		ACE_GetZeroings(horizontal, vertical);
		horizontal += horizontalChange;
		vertical += verticalChange;
		ACE_SetZeroings(horizontal, vertical);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set horizontal and vertical zeroings in mrad
	void ACE_SetZeroings(float horizontal, float vertical)
	{
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (data)
		{
			data.ACE_LimitZeroings(horizontal, vertical);
			vertical *= data.ACE_GetEffectiveVerticalZeroingScale();
		}
		
		vector angles;
		angles[0] = Math.RAD2DEG * SCR_Math.ConvertToRadians(-horizontal, SCR_EOpticsAngleUnits.MILLIRADIANS);
		angles[1] = Math.RAD2DEG * SCR_Math.ConvertToRadians(-vertical, SCR_EOpticsAngleUnits.MILLIRADIANS);
		vector transform[4];
		Math3D.AnglesToMatrix(angles, transform);
		transform[3] = GetOwner().GetLocalTransformAxis(3);
		GetOwner().SetLocalTransform(transform);
		
		SCR_WeaponInfo display = ACE_GetWeaponInfoDisplay();
		if (display)
			display.ACE_UpdateZeroingIndicator();
		
		SCR_UISoundEntity.SoundEvent("ACE_SCOPES_SOUND_CLICK");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get horizontal and vertical zeroings in mrad
	void ACE_GetZeroings(out float horizontal, out float vertical)
	{
		vector transform[3];
		GetOwner().GetLocalTransform(transform);
		vector angles = Math3D.MatrixToAngles(transform);
		vertical = SCR_Math.ConvertFromRadians(Math.DEG2RAD * -angles[1], SCR_EOpticsAngleUnits.MILLIRADIANS);
		horizontal = SCR_Math.ConvertFromRadians(Math.DEG2RAD * -angles[0], SCR_EOpticsAngleUnits.MILLIRADIANS);
		
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (data)
			vertical /= data.ACE_GetEffectiveVerticalZeroingScale();
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_WeaponInfo ACE_GetWeaponInfoDisplay()
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!player)
			return null;
		
		SCR_BaseHUDComponent hudComponent = SCR_BaseHUDComponent.Cast(player.FindComponent(SCR_BaseHUDComponent));
		if (!hudComponent)
			return null;
		
		array<BaseInfoDisplay> displayInfos = {};
		int count = hudComponent.GetInfoDisplays(displayInfos);
		for (int i = 0; i < count; i++)
		{
			SCR_WeaponInfo current = SCR_WeaponInfo.Cast(displayInfos[i]);
			if (current)
				return current;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	array<vector> ACE_GetSightsRanges()
	{
		IEntityComponentSource source = GetComponentSource(GetOwner());
		if (!source)
			return null;
		
		BaseContainerList sightsRangeSources = source.GetObjectArray("SightsRanges");
		if (!sightsRangeSources)
			return null;
		
		array<vector> ranges = {};
		ranges.Reserve(sightsRangeSources.Count());
		
		for (int i = 0; i < sightsRangeSources.Count(); ++i)
		{
			BaseContainer sightsRangeSource = sightsRangeSources.Get(i);
			if (!sightsRangeSource)
				continue;
			
			vector range;
			sightsRangeSource.Get("Range", range);
			ranges.Insert(range);
		}
		
		return ranges;
	}
}
