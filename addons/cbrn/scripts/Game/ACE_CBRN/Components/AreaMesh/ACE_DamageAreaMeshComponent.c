[ComponentEditorProps(category: "GameScripted/Area Mesh", description: "")]
class ACE_DamageAreaMeshComponentClass : SCR_BaseAreaMeshComponentClass
{
}

class ACE_DamageAreaMeshComponent : SCR_BaseAreaMeshComponent
{
	//------------------------------------------------------------------------------------------------
	override float GetRadius()
	{
		SCR_DamageAreaComponent areaComponent = SCR_DamageAreaComponent.Cast(GetOwner().FindComponent(SCR_DamageAreaComponent));
		if (!areaComponent)
			return 0;
		
		array<ref DamageArea> areas = {};
		areaComponent.GetDamageAreas(areas);
		float maxRadius;
		
		foreach (DamageArea area : areas)
		{
			BaseDamageAreaShape shape = area.GetShape();
			if (shape)
				maxRadius = Math.Max(maxRadius, 0.5 * shape.GetDiameter());
		}
		
		return maxRadius;
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		GenerateAreaMesh();
	}
}
