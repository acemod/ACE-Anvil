//! Facepaint effect
[BaseContainerProps()]
class ACE_ConsumableFacepaint : SCR_ConsumableEffectBase
{
	static string getTargetHead(VisualIdentity currentIdentity, FactionIdentity factionIdentity)
	{
		ResourceName targetHead;
		array <ref VisualIdentity> visualIdentities = {};
		factionIdentity.GetVisualIdentities(visualIdentities);
		foreach (VisualIdentity visualIdentity : visualIdentities)
		{
			array <ref ResourceName> headCamos = {};
			int i;
			// GetHeadCamos returns the first element in the array if index exceeds array length
			while (true)
			{
				ResourceName headCamo = visualIdentity.GetHeadCamo(i);

				if (!headCamo)
					break;
				if (headCamos.Contains(headCamo))
					break;

				headCamos.Insert(headCamo);
				i++;

				// sane max length if we somehow get an infinite loop
				if (i == 100)
					break;
			}
			
			if (headCamos.Contains(currentIdentity.GetHead()))
			{
				targetHead = visualIdentity.GetHead();
				break;
			}
			else if (visualIdentity.GetHead() == currentIdentity.GetHead())
			{
				targetHead = headCamos[Math.RandomInt(0, headCamos.Count())];
				break;
			}
		}
		return targetHead;
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
		targetHead = getTargetHead(visualIdentity, factionIdentity);
		
		if (!targetHead || targetHead == visualIdentity.GetHead())
		{
			SCR_FactionManager fm = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			if (!fm)
				return;

			Faction usFaction = Faction.Cast(fm.GetFactionByIndex(0));
			if (!usFaction)
				return;

			FactionIdentity usIdentity = usFaction.GetFactionIdentity();
			if (!factionIdentity)
				return;
			
			targetHead = getTargetHead(visualIdentity, usIdentity);
		}
		
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
