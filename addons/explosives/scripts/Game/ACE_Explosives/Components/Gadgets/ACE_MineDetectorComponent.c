//------------------------------------------------------------------------------------------------
class ACE_MineDetectorComponentClass : ACE_WeaponGadgetComponentClass
{
	[Attribute(defvalue: "3.5", desc: "Maximum distance in meter the sensor can detect mines.", category: "Detector Range")]
	protected float m_fMaxDetectionDistance;
	
	[Attribute(defvalue: "1.0", desc: "Timeout between queries in seconds.", category: "Detector Range")]
	protected float m_fQueryTimeout;
	
	[Attribute(defvalue: "7.0", desc: "Radius in meters for querying mines.", category: "Detector Range")]
	protected float m_fQueryRadius;
	
	[Attribute(defvalue: "0.5", desc: "Minimum timeout between beeps in seconds.", category: "Detector Sound")]
	protected float m_fMinTimeout;
	
	[Attribute(defvalue: "1.0", desc: "Maximum timeout between beeps in seconds.", category: "Detector Sound")]
	protected float m_fMaxTimeout;
	
	[Attribute(defvalue: "0.06", desc: "Distance in meters for minimum timout.", category: "Detector Sound")]
	protected float m_fMinTimeoutDistance;
	
	[Attribute(defvalue: "2.0", desc: "Distance in meters for maximum timout.", category: "Detector Sound")]
	protected float m_fMaxTimeoutDistance;
	
	[Attribute(defvalue: "1.0", desc: "Maximum pitch of beep sound.", category: "Detector Sound")]
	protected float m_fMaxPitch;
	
	[Attribute(defvalue: "0.7", desc: "Minimum pitch of beep sound.", category: "Detector Sound")]
	protected float m_fMinPitch;
	
	[Attribute(defvalue: "0.06", desc: "Distance in meters for maximum pitch.", category: "Detector Sound")]
	protected float m_fMaxPitchDistance;
	
	[Attribute(defvalue: "2.0", desc: "Distance in meters for minimum pitch.", category: "Detector Sound")]
	protected float m_fMinPitchDistance;
	
	//------------------------------------------------------------------------------------------------
	float GetMaxDetectionDistance()
	{
		return m_fMaxDetectionDistance;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetQueryTimeout()
	{
		return m_fQueryTimeout;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetQueryRadius()
	{
		return m_fQueryRadius;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTimeoutForDistance(float distance)
	{
		float lambda = Math.InverseLerp(m_fMinTimeoutDistance, m_fMaxTimeoutDistance, distance);
		return Math.Lerp(m_fMinTimeout, m_fMaxTimeout, Math.Clamp(lambda, 0, 1));
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPitchForDistance(float distance)
	{
		float lambda = Math.InverseLerp(m_fMaxPitchDistance, m_fMinPitchDistance, distance);
		return Math.Lerp(m_fMaxPitch, m_fMinPitch, Math.Clamp(lambda, 0, 1));
	}
}

//------------------------------------------------------------------------------------------------
class ACE_MineDetectorComponent : ACE_WeaponGadgetComponent
{
	protected static ACE_Explosives_Settings s_Settings;
	protected ACE_MineDetectorComponentClass m_Data;
	protected WeaponSoundComponent m_SoundComponent;
	protected SignalsManagerComponent m_SignalsManager;
	protected ref array<vector> m_aSourcePositions = {};
	protected int m_iPitchSignal;
	protected float m_fTimer;
	protected float m_fTimeout = 1.0;
	protected float m_fQueryTimer;
	protected vector m_vHeadOffset;
	
	protected static const string PITCH_SIGNAL_NAME = "Pitch";
	protected static const string DETECTOR_HEAD_BONE_NAME = "i_head";
	protected static const string BEEP_SOUND_NAME = "SOUND_BEEP";
	
	//------------------------------------------------------------------------------------------------
	override void OnWeaponActive()
	{
		m_SoundComponent = WeaponSoundComponent.Cast(GetOwner().FindComponent(WeaponSoundComponent));
		m_SignalsManager = SignalsManagerComponent.Cast(GetOwner().FindComponent(SignalsManagerComponent));
		m_iPitchSignal = m_SignalsManager.AddOrFindSignal(PITCH_SIGNAL_NAME);
		
		if (Replication.IsServer())
		{
			if (!s_Settings)
				s_Settings = ACE_SettingsHelperT<ACE_Explosives_Settings>.GetModSettings();
			
			if (!m_Data)
				m_Data = ACE_MineDetectorComponentClass.Cast(GetComponentData(GetOwner()));
			
			vector headTransform[4];
			Animation anim = GetOwner().GetAnimation();
			anim.GetBoneMatrix(anim.GetBoneIndex(DETECTOR_HEAD_BONE_NAME), headTransform);
			m_vHeadOffset = headTransform[3];
			
			// Ensure that first query runs immediately
			m_fQueryTimer = m_Data.GetQueryTimeout();
			m_fTimer = m_fTimeout;
		}
		
		super.OnWeaponActive();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnWeaponUpdate(float timeSlice)
	{
		m_fQueryTimer += timeSlice;
		if (m_fQueryTimer >= m_Data.GetQueryTimeout())
		{
			m_fQueryTimer = 0;
			QueryTargets();
		}
		
		m_fTimer += timeSlice;
		if (m_fTimer < m_fTimeout)
			return;
		
		m_fTimer = 0;
		vector headPos = GetOwner().CoordToParent(m_vHeadOffset);
		float minDistance = float.INFINITY;
		
		foreach (vector pos : m_aSourcePositions)
		{
			minDistance = Math.Min(minDistance, vector.Distance(headPos, pos));
		}
		
		m_fTimeout = m_Data.GetTimeoutForDistance(minDistance);
		
		if (minDistance > m_Data.GetMaxDetectionDistance())
			return;
		
		float pitch = m_Data.GetPitchForDistance(minDistance);
		Rpc(PlayBeep, pitch);
		
		if (!System.IsConsoleApp())
			PlayBeep(pitch);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void QueryTargets()
	{
		vector headPos = GetOwner().CoordToParent(m_vHeadOffset);
		m_aSourcePositions.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(headPos, m_Data.GetQueryRadius(), MetalQueryFilter);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool MetalQueryFilter(IEntity entity)
	{
		// Exclude the character's loadout
		if (entity.GetRootParent() == GetOwner().GetRootParent())
			return true;
		
		if (entity.FindComponent(SCR_MineDamageManager))
		{
			m_aSourcePositions.Insert(entity.GetOrigin());
			return true;
		}
		
		if (!s_Settings.m_bAnyMetalDetectionEnabled || Building.Cast(entity))
			return true;
		
		Physics physics = entity.GetPhysics();
		if (!physics)
			return true;
		
		for (int i, count = physics.GetNumGeoms(); i < count; i++)
		{
			array<SurfaceProperties> surfaces = {};
			physics.GetGeomSurfaces(i, surfaces);
			
			foreach (SurfaceProperties surface : surfaces)
			{
				if (ACE_SurfaceHelper.HasLabel(surface, ACE_ESurfaceLabel.MAGNETIC))
					m_aSourcePositions.Insert(physics.GetGeomWorldPosition(i));
			}
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void PlayBeep(float pitch)
	{
		if (m_SignalsManager)
			m_SignalsManager.SetSignalValue(m_iPitchSignal, pitch);
		
		if (m_SoundComponent)
			m_SoundComponent.SoundEvent(BEEP_SOUND_NAME);
	}
}
