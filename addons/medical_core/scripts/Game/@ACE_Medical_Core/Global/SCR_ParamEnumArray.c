//------------------------------------------------------------------------------------------------
modded class SCR_ParamEnumArray : ParamEnumArray
{
	//------------------------------------------------------------------------------------------------
	static ParamEnumArray ACE_Medical_FromCharacterBaseHitZones()
	{
		IEntityComponentSource componentSource = SCR_BaseContainerTools.FindComponentSource(Resource.Load("{37578B1666981FCE}Prefabs/Characters/Core/Character_Base.et"), SCR_DamageManagerComponent);
		if (!componentSource)
			return null;
		
		BaseContainerList list = componentSource.GetObjectArray("Additional hit zones");
		
		ParamEnumArray params = new ParamEnumArray();
		int cnt = list.Count();
		
		for (int i = 0; i < cnt; ++i)
		{
			string hitZoneName = list.Get(i).GetName();
			params.Insert(new ParamEnum(hitZoneName, hitZoneName));
		}
		
		return params;
	}
}
