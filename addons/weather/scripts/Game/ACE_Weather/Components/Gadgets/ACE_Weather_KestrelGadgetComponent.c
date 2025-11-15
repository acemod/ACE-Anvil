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
	
	protected TimeAndWeatherManagerEntity m_pWeatherManager;
	protected TNodeId m_iImpellerBone;
	
	protected static const float IMPELLER_SPIN_PER_WIND_SPEED = -2750; // [°/m]: ratio between spin speed [°/s] and wind speed [m/s]
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		ChimeraWorld world = GetGame().GetWorld();
		m_pWeatherManager = world.GetTimeAndWeatherManager();
		m_iImpellerBone = owner.GetAnimation().GetBoneIndex("i_impeller");
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
		float nextYaw = Math.Mod(yaw + IMPELLER_SPIN_PER_WIND_SPEED * timeSlice * GetWindSpeed(), 360);
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
		m_pWeatherManager.GetHoursMinutesSeconds(hours, minutes, seconds);
		return SCR_FormatHelper.FormatTime(hours, minutes, seconds);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetWeekDay()
	{
		return m_pWeatherManager.GetWeekDayString();
	}
	
	//------------------------------------------------------------------------------------------------
	string GetDate()
	{
		int year, month, day;
		m_pWeatherManager.GetDate(year, month, day);
		return string.Format("%1 %2, %3", SCR_DateTimeHelper.GetAbbreviatedMonthString(month), day, year);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetDirection()
	{
		vector dirDown = GetOwner().GetTransformAxis(2) - GetOwner().GetTransformAxis(1);
		return Math.Round(dirDown.ToYaw());
	}
	
	//------------------------------------------------------------------------------------------------
	float GetWindSpeed()
	{
		float gadgetDir = GetDirection();
		float windDir = m_pWeatherManager.GetWindDirection();
		float windAngleRad = Math.DEG2RAD * (windDir - gadgetDir);
		return -m_pWeatherManager.GetWindSpeed() * Math.Cos(windAngleRad);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns wind speed projected at m_iHeading + angle (0 for headwind and 90 for crosswind)
	float GetProjectedWindSpeed(float angle)
	{
		return GetWindSpeed() * Math.Cos(Math.DEG2RAD * (GetDirection() - m_iRefHeading - angle));
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAltitude()
	{
		return Math.Round(GetOwner().GetTransformAxis(3)[1]);
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
}

//------------------------------------------------------------------------------------------------
modded enum ACE_EGadgetScreenID
{
	KESTREL_HOME,
	KESTREL_DIRECTION,
	KESTREL_WIND,
	KESTREL_CROSSWIND,
	KESTREL_HEADWIND,
	KESTREL_ALTITUDE,
	KESTREL_REF_HEADING_MENU,
	KESTREL_REF_HEADING_AUTO_SET,
	KESTREL_REF_HEADING_MANUAL_SET
}
