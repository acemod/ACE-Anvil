//------------------------------------------------------------------------------------------------
class ACE_Medical_AIPerformSmartHealing : SCR_AIPerformSmartHealing
{
	protected ECharacterHitZoneGroup m_eTargetHitZoneGroup;
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(AIAgent owner)
	{
		super.OnEnter(owner);
		GetVariableIn(PORT_ITEM, m_item);
		
		if (!m_item)
			return;
		
		SCR_EConsumableType itemType;
		
		SCR_ConsumableItemComponent consumableComponent = SCR_ConsumableItemComponent.Cast(m_item.FindComponent(SCR_ConsumableItemComponent));
		if (consumableComponent)
			itemType = consumableComponent.GetConsumableType();
		else if (m_item.FindComponent(SCR_HealSupportStationComponent))
			itemType = SCR_EConsumableType.MED_KIT;
		
		m_userActionString =  GetUserActionString(itemType);
		
		IEntity controlledEntity = owner.GetControlledEntity();
		if (!controlledEntity)
			return;
		
		if (!GetVariableIn(PORT_TARGET_ENTITY, m_targetEntity))
			m_targetEntity = controlledEntity;
		
		m_eTargetHitZoneGroup = GetTargetGroupForItem(m_targetEntity, itemType);
	}
	
	//------------------------------------------------------------------------------------------------
	protected string GetUserActionString(SCR_EConsumableType type)
	{
		typename actionType;
		
		switch (type)
		{
			case SCR_EConsumableType.BANDAGE: { actionType = SCR_BandageUserAction; break; }
			case SCR_EConsumableType.SALINE: { actionType = SCR_SalineBagUserAction; break; }
			case SCR_EConsumableType.MED_KIT: { actionType = SCR_HealSupportStationAction; break; }
			case SCR_EConsumableType.ACE_MEDICAL_EPINEPHRINE: { actionType = ACE_Medical_EpinephrineUserAction; break; }
		}
		
		return actionType.ToString();
	}
	
	//------------------------------------------------------------------------------------------------
	protected ECharacterHitZoneGroup GetTargetGroupForItem(IEntity targetEntity, SCR_EConsumableType type)
	{
		ChimeraCharacter targetChar = ChimeraCharacter.Cast(targetEntity);
		if (!targetChar)
			return ECharacterHitZoneGroup.VIRTUAL;
		
		if (type == SCR_EConsumableType.BANDAGE)
		{
			return super.GetTargetGroup(targetEntity);
		}
		
		if (type == SCR_EConsumableType.MED_KIT)
		{
			array<HitZone> hitZones = {};
			targetChar.GetDamageManager().GetPhysicalHitZones(hitZones);
			
			foreach (HitZone hitZone : hitZones)
			{
				SCR_HitZone scriptedHitZone = SCR_HitZone.Cast(hitZone);
				if (scriptedHitZone && scriptedHitZone.GetHealthScaled() < 1)
					return scriptedHitZone.GetHitZoneGroup();
			}
		}
		
		return ECharacterHitZoneGroup.VIRTUAL;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected ECharacterHitZoneGroup GetTargetGroup(IEntity targetEntity)
	{
		return m_eTargetHitZoneGroup;
	}
}
