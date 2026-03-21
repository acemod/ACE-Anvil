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
	
	static void getTargetHead(VisualIdentity currentIdentity, FactionIdentity factionIdentity, string targetHead)
	{
		Print(currentIdentity.GetHead());
		
		Resource factionIdentityResource = BaseContainerTools.CreateContainerFromInstance(factionIdentity);
		if (!factionIdentityResource)
			return;
		BaseContainer factionIdentityContainer = factionIdentityResource.GetResource().ToBaseContainer();
		
		Print(factionIdentityContainer);

		BaseContainerList visualIdentities = factionIdentityContainer.GetObjectArray("VisualIdentityArray");
		
		Print(visualIdentities);
		Print(visualIdentities.Count());
		
		//foreach (VisualIdentity visualIdentity : visualIdentities)
		//{
		//	Resource visualIdentityResource = BaseContainerTools.CreateContainerFromInstance(visualIdentity);
		//	BaseContainer visualIdentityContainer = visualIdentityResource.GetResource().ToBaseContainer();
		//	array <ref ResourceName> headCamos;
		//	visualIdentityContainer.Get("Headcamo", headCamos);
		//	Print(headCamos);
		//	if (headCamos.Contains(currentIdentity.GetHead()))
		//	{
		//		targetHead = visualIdentity.GetHead();
		//		return;
		//	}
		//	else if (visualIdentity.GetHead() == currentIdentity.GetHead())
		//	{
		//		targetHead = headCamos[Math.RandomInt(0, headCamos.Count())];
		//		return;
		//	}
		//}
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
		if (!visualIdentity)
			return;

		SCR_CharacterFactionAffiliationComponent factionAffiliationComponent = SCR_CharacterFactionAffiliationComponent.Cast(char.FindComponent(SCR_CharacterFactionAffiliationComponent));
		if (!factionAffiliationComponent)
			return;
		
		SCR_Faction faction = SCR_Faction.Cast(factionAffiliationComponent.GetAffiliatedFaction());
		if (!faction)
			return;
		
		FactionIdentity factionIdentity = faction.GetFactionIdentity();
		if (!factionIdentity)
			return;
		
		ResourceName targetHead;
		getTargetHead(visualIdentity, factionIdentity, targetHead);
		
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
