[BaseContainerProps()]
class ACE_CamoFaceMap
{
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Uncamo face")]
	string m_uncamoFace;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Camo faces")]
	ref array<string> m_camoFaces;
}

//! Facepaint effect
[BaseContainerProps()]
class ACE_ConsumableFacepaint : SCR_ConsumableEffectBase
{
	[Attribute("", uiwidget: UIWidgets.Object, desc: "Map of non-camo faces to camo faces", category: "General")]
	ref array<ref ACE_CamoFaceMap> m_fCamoFaceMap;
	
	ACE_CamoFaceMap GetFaceMap(string head)
    {   
		foreach ( ACE_CamoFaceMap faceMap : m_fCamoFaceMap )
		{
			if (faceMap.m_uncamoFace == head)
				return faceMap;
			if (faceMap.m_camoFaces.Contains(head))
				return faceMap;
		}
		return new ACE_CamoFaceMap;
    }
	
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return;
		
		SCR_CharacterIdentityComponent identityComponent = SCR_CharacterIdentityComponent.Cast(char.FindComponent(SCR_CharacterIdentityComponent));
		if (!identityComponent)
			return;
		
		VisualIdentity visualIdentity = identityComponent.GetIdentity().GetVisualIdentity();
		
		string targetHead;
		ACE_CamoFaceMap headMap = GetFaceMap(visualIdentity.GetHead());
		
		if (visualIdentity.GetHead().Contains("_camo"))
			targetHead = headMap.m_uncamoFace;
		else
			targetHead = headMap.m_camoFaces.GetRandomElement();
		
		if (!targetHead)
		{
			return;
		}
		
		visualIdentity.SetHead(targetHead);
		identityComponent.CommitChanges();
		identityComponent.SetIdentity(identityComponent.GetIdentity());
		
		super.ApplyEffect(target, user, item, animParams);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	// constructor
	void ACE_ConsumableFacepaint()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_FACE_PAINT;
	}		
}
