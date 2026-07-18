//------------------------------------------------------------------------------------------------
class ACE_DamageAreaShapeCylinder : SCR_BaseDamageAreaShape
{
	[Attribute(desc: "Radius of the cylinder in meters")]
	protected float m_fRadius;
	
	[Attribute(desc: "Height of cylinder in meters. The cylinder's origin is at the centroid.")]
	protected float m_fHeight;
	
	//------------------------------------------------------------------------------------------------
	void SetRadius(float radius)
	{
		m_fRadius = radius;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRadius()
	{
		return m_fRadius;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHeight(float height)
	{
		m_fHeight = height;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetHeight()
	{
		return m_fHeight;
	}
	
	//------------------------------------------------------------------------------------------------
	override float GetDiameter()
	{
		return 2 * m_fRadius;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldAffectEntity(IEntity entityToAffect)
	{
		vector worldPos;
		ChimeraCharacter character = ChimeraCharacter.Cast(entityToAffect);
		if (character)
		{
			worldPos = character.AimingPosition();
		}
		else
		{
			Physics phys = entityToAffect.GetPhysics();
			if (phys)
				worldPos = phys.GetCenterOfMass();
			else
				worldPos = entityToAffect.GetOrigin();
		}

		return AffectsPosition(worldPos);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check altitutde
	override bool AffectsPosition(vector position)
	{
		float yRelAbs = Math.AbsFloat(position[1] - GetWorldPosition()[1]);
		return (yRelAbs <= 0.5 * m_fHeight);
	}
}
