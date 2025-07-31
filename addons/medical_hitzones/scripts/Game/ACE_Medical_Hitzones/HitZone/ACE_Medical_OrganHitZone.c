//-----------------------------------------------------------------------------------------------------------
//! Spherical organ hit zones for characters
class ACE_Medical_OrganHitZone: SCR_HitZone
{
	[Attribute()]
	protected ref PointInfo m_pPointInfo;
	
	[Attribute()]
	protected float m_rRadius;
	
	[Attribute(defvalue: "0", desc: "Probability for second chance for this hit zone", uiwidget: UIWidgets.Slider, params: "0 1")]
	protected float m_fACE_Medical_SecondChance;
	
	[Attribute(defvalue: "true", desc: "Whether the hit zone is vital and will kill the character when destroyed")]
	protected bool m_bACE_Medical_IsVital;
	
	// TO DO: Make whitelist based on wound types. Exclude melee, except for bayonets
	protected static const ref array<EDamageType> DAMAGE_TYPE_WHITELIST = {EDamageType.MELEE, EDamageType.KINETIC};
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		if (m_pPointInfo)
			m_pPointInfo.Init(pOwnerEntity);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Count as hit if trajectory intersects with sphere
	bool ShouldCountAsHit(notnull BaseDamageContext damageContext)
	{
		if (!DAMAGE_TYPE_WHITELIST.Contains(damageContext.damageType))
			return false;
		
		return Math3D.IntersectionRaySphere(damageContext.hitPosition, damageContext.hitDirection, m_pPointInfo.GetWorldTransformAxis(3), m_rRadius) >= 0;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns true when second chance is granted by this hit zone
	bool ACE_Medical_ShouldGrantSecondChance()
	{
		return m_fACE_Medical_SecondChance > Math.RandomFloat(0, 1);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Vital hit zones trigger registration at ACE_Medical_SecondChanceSystem when destroyed
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer() || newState != EDamageState.DESTROYED)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		if (!m_bACE_Medical_IsVital)
			return;
		
		ACE_Medical_SecondChanceSystem system = ACE_Medical_SecondChanceSystem.GetInstance();
		if (system)
			system.Register(damageManager, this);
		else
			// Put kill on callqueue to avoid potential crash (e.g. when called from HitZone::OnDamageStateChanged)
			GetGame().GetCallqueue().Call(damageManager.Kill, damageManager.GetInstigator());
	}
	
	//------------------------------------------------------------------------------------------------
	PointInfo GetPointInfo()
	{
		return m_pPointInfo;
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	void OnDiag(float timeSlice)
	{
		if (m_pPointInfo)
			Shape.CreateSphere(Color.RED, ShapeFlags.WIREFRAME | ShapeFlags.NOZBUFFER | ShapeFlags.ONCE, m_pPointInfo.GetWorldTransformAxis(3), m_rRadius);
	}
#endif
}
