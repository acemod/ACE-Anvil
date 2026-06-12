//------------------------------------------------------------------------------------------------
class ACE_Weather_KestrelGadgetComponentClass : ACE_ScreenGadgetComponentClass
{
}


//------------------------------------------------------------------------------------------------
class ACE_Weather_KestrelGadgetComponent : ACE_ScreenGadgetComponent
{
	[RplProp()]
	protected int m_iRefHeading;
	
	[RplProp(onRplName: "OnRefreshScreen")]
	protected int m_iTmpRefHeading;
	
	[RplProp(onRplName: "OnRefreshScreen")]
	protected ACE_EGadgetScreenID m_eRefHeadingSetMode;
	
	[RplProp()]
	protected bool m_bUseTrueNorth = false;
	
	[RplProp(onRplName: "OnRefreshScreen")]
	protected bool m_bTmpUseTrueNorth = false;
	
	protected ChimeraCharacter m_User;
	protected TimeAndWeatherManagerEntity m_WeatherManager;
	protected TNodeId m_iImpellerBone;
	protected float m_fImpellerSpinSpeed;
	
	protected static const float IMPELLER_SPIN_PER_WIND_SPEED = -2750; // [°/m]: ratio between spin speed [°/s] and wind speed [m/s]
	protected static const float IMPELLER_SPIN_RATE_CONSTANT = 1; // [1/s]: How fast current spin converges towards target spin (exponential rate constant)
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		ChimeraWorld world = GetGame().GetWorld();
		m_WeatherManager = world.GetTimeAndWeatherManager();
		m_iImpellerBone = owner.GetAnimation().GetBoneIndex("i_impeller");
	}
	
	//------------------------------------------------------------------------------------------------
	override void ActivateGadgetUpdate()
	{
		super.ActivateGadgetUpdate();
		m_fImpellerSpinSpeed = 0;
		m_User = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
	}
	
	//------------------------------------------------------------------------------------------------
	override void DeactivateGadgetUpdate()
	{
		super.DeactivateGadgetUpdate();
		m_fImpellerSpinSpeed = 0;
		m_User = null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update impeller rotation
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
		
		vector transform[4];
		GetOwner().GetAnimation().GetBoneMatrix(m_iImpellerBone, transform);
		float yaw = Math3D.MatrixToAngles(transform)[0];
		transform[3] = vector.Zero;
		float progress = Math.Min(IMPELLER_SPIN_RATE_CONSTANT * timeSlice, 1);
		m_fImpellerSpinSpeed = Math.Lerp(m_fImpellerSpinSpeed, IMPELLER_SPIN_PER_WIND_SPEED * GetEffectiveWindSpeed(), progress);
		float nextYaw = Math.Mod(yaw + m_fImpellerSpinSpeed * timeSlice, 360);
		Math3D.AnglesToMatrix(Vector(nextYaw, 0, 0), transform);
		GetOwner().GetAnimation().SetBoneMatrix(GetOwner(), m_iImpellerBone, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeRaised()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsUsingADSControls()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetTime()
	{
		int hours, minutes, seconds;
		m_WeatherManager.GetHoursMinutesSeconds(hours, minutes, seconds);
		return SCR_FormatHelper.FormatTime(hours, minutes, seconds);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetWeekDay()
	{
		return m_WeatherManager.GetWeekDayString();
	}
	
	//------------------------------------------------------------------------------------------------
	string GetDate()
	{
		int year, month, day;
		m_WeatherManager.GetDate(year, month, day);
		return string.Format("%1 %2, %3", SCR_DateTimeHelper.GetAbbreviatedMonthString(month), day, year);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTrueDirection()
	{
		if (m_User.IsInVehicle())
			return m_User.GetAngles()[1];
		else
			return SCR_Math.fmod(Math.RAD2DEG * m_User.GetCharacterController().GetInputContext().GetHeadingAngle(), 360);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDirection()
	{
		if (GetUseTrueNorth())
			return GetTrueDirection();
		else
			return GetTrueDirection() - GetDeclination();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDeclination()
	{
		#ifdef ACE_COMPASS
			return m_WeatherManager.ACE_GetMagneticDeclination();
		#else
			return 0.0;
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns effective relative wind speed [m/s]
	float GetEffectiveWindSpeed()
	{
		float gadgetDir = GetTrueDirection();
		float windDir = m_WeatherManager.GetWindDirection();
		float windAngleRad = Math.DEG2RAD * (windDir - gadgetDir);
		return -m_WeatherManager.GetWindSpeed() * Math.Cos(windAngleRad);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the measured wind speed based on impeller speed [m/s]
	float GetMeasuredWindSpeed()
	{
		return m_fImpellerSpinSpeed / IMPELLER_SPIN_PER_WIND_SPEED;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns measured wind speed projected at m_iHeading + angle (0 for headwind and 90 for crosswind)
	float GetMeasuredProjectedWindSpeed(float angle)
	{
		return GetMeasuredWindSpeed() * Math.Cos(Math.DEG2RAD * (GetTrueDirection() - m_iRefHeading - angle));
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAltitude()
	{
		return Math.Round(GetOwner().GetTransformAxis(3)[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTemperature()
	{
		return m_WeatherManager.ACE_GetAirTemperature(GetAltitude()) - ACE_PhysicalConstants.ZERO_CELSIUS;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRefHeading(int heading)
	{
		m_iRefHeading = heading;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetRefHeading()
	{
		return m_iRefHeading;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTmpRefHeading(int heading)
	{
		m_iTmpRefHeading = heading;
		OnRefreshScreen();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetTmpRefHeading()
	{
		return m_iTmpRefHeading;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetRefHeadingSetMode(ACE_EGadgetScreenID mode)
	{
		m_eRefHeadingSetMode = mode;
		OnRefreshScreen();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void ToggleRefHeadingSetMode()
	{
		if (m_eRefHeadingSetMode == ACE_EGadgetScreenID.KESTREL_REF_HEADING_AUTO_SET)
			SetRefHeadingSetMode(ACE_EGadgetScreenID.KESTREL_REF_HEADING_MANUAL_SET);
		else
			SetRefHeadingSetMode(ACE_EGadgetScreenID.KESTREL_REF_HEADING_AUTO_SET);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_EGadgetScreenID GetRefHeadingSetMode()
	{
		return m_eRefHeadingSetMode;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUseTrueNorth(bool doUse)
	{
		m_bUseTrueNorth = doUse;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetUseTrueNorth()
	{
		return m_bUseTrueNorth;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTmpUseTrueNorth(bool doUse)
	{
		m_bTmpUseTrueNorth = doUse;
		OnRefreshScreen();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetTmpUseTrueNorth()
	{
		return m_bTmpUseTrueNorth;
	}
}

//------------------------------------------------------------------------------------------------
modded enum ACE_EGadgetScreenID
{
	KESTREL_HOME,
	KESTREL_DIRECTION,
	KESTREL_WIND,
	KESTREL_CROSSWIND,
	KESTREL_HEADWIND,
	KESTREL_TEMPERATURE,
	KESTREL_ALTITUDE,
	KESTREL_REF_HEADING_MENU,
	KESTREL_REF_HEADING_AUTO_SET,
	KESTREL_REF_HEADING_MANUAL_SET,
	KESTREL_DIR_MODE_SET,
}
