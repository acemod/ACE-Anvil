//------------------------------------------------------------------------------------------------
//! Ammo effect that spawns a damage area entity at the hit position
class ACE_DamageAreaAmmoEffect : BaseProjectileEffect
{
	[Attribute(desc: "Resource name of the damage area entity", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_DamageAreaPrefabName;
	
	[Attribute(defvalue: "-1", desc: "Lifetime of the damage area in seconds. Ulimited when negative.")]
	protected float m_fLifetime;
	
	//------------------------------------------------------------------------------------------------
	override void OnEffect(IEntity pHitEntity, inout vector outMat[3], IEntity damageSource, notnull Instigator instigator, string colliderName, float speed)
	{
		if (!Replication.IsServer())
			return;
		
		Resource res = Resource.Load(m_DamageAreaPrefabName);
		if (!res.IsValid())
			return;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = outMat[0];
		
		IEntity damageAreaEntity = GetGame().SpawnEntityPrefab(res, params: params);
		if (!damageAreaEntity)
			return;
		
		if (m_fLifetime >= 0)
			GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, m_fLifetime * 1000, param1: damageAreaEntity);
	}
}
