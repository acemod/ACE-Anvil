//------------------------------------------------------------------------------------------------
//! Facepaint effect
[BaseContainerProps()]
class ACE_ConsumableFacepaint : SCR_ConsumableEffectBase
{
	protected static const string US_FACTION_CONFIG = "{5EB46557DF2AA24F}Configs/Factions/US.conf";
	
	//------------------------------------------------------------------------------------------------
	static string GetTargetHead(VisualIdentity currentIdentity, FactionIdentity factionIdentity)
	{
		ResourceName targetHead;
		array<ref VisualIdentity> visualIdentities = {};
		factionIdentity.GetVisualIdentities(visualIdentities);
		foreach (VisualIdentity visualIdentity : visualIdentities)
		{
			array<ref ResourceName> headCamos = {};
			// sane max length if we somehow get an infinite loop
			int i;
			// GetHeadCamos returns the first element in the array if index exceeds array length
			while (i < 100)
			{
				ResourceName headCamo = visualIdentity.GetHeadCamo(i);

				if (!headCamo)
					break;
				if (headCamos.Contains(headCamo))
					break;

				headCamos.Insert(headCamo);
				i++;
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
		targetHead = GetTargetHead(visualIdentity, factionIdentity);
		
		if (!targetHead || targetHead == visualIdentity.GetHead())
		{
			// fallback on vanilla US mapping if none found
			SCR_Faction usFaction = SCR_ConfigHelperT<SCR_Faction>.GetConfigObject(US_FACTION_CONFIG);
			if (!usFaction)
				return;

			FactionIdentity usIdentity = usFaction.GetFactionIdentity();
			if (!factionIdentity)
				return;
			
			targetHead = GetTargetHead(visualIdentity, usIdentity);
		}
		
		if (!targetHead)
			return;
		
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
		m_eConsumableType = SCR_EConsumableType.ACE_FACEPAINT;
	}		
}
