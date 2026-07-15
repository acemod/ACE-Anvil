//------------------------------------------------------------------------------------------------
class ACE_RetextureMeshComponentClass : ScriptComponentClass
{
	[Attribute(desc: "Material assignment applied to xob", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	protected ref array<ResourceName> m_aMaterialsOverride;
	
	//------------------------------------------------------------------------------------------------
	array<ResourceName> GetMaterialsOverride()
	{
		return m_aMaterialsOverride;
	}
}

//------------------------------------------------------------------------------------------------
//! Used for entities that can't be retextured otherwise like Projectiles
class ACE_RetextureMeshComponent : ScriptComponent
{
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
		
		ACE_RetextureMeshComponentClass data = ACE_RetextureMeshComponentClass.Cast(GetComponentData(owner));
		if (!data)
			return;
		
		VObject mesh = owner.GetVObject();
		if (!mesh)
			return;
		
		string remap;
		string materials[256];
		int numMats = mesh.GetMaterials(materials);
		
		foreach (int i, ResourceName newMaterial : data.GetMaterialsOverride())
		{
			remap += string.Format("$remap '%1' '%2';", materials[i], newMaterial);
		}
		
		owner.SetObject(mesh, remap);
	}
}
