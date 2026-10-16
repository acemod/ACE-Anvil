//------------------------------------------------------------------------------------------------
class ACE_Medical_BodyPartNameUIComponent : ScriptedWidgetComponent
{
	protected TextWidget m_wText;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wText = TextWidget.Cast(w);
	}

	//------------------------------------------------------------------------------------------------
	bool SetHitZoneGroup(ECharacterHitZoneGroup hitZoneGroup)
	{
		string bodyPartName = GetHitZoneGroupName(hitZoneGroup);
		if (m_wText)
			m_wText.SetText(bodyPartName);

		return !bodyPartName.IsEmpty();
	}

	//------------------------------------------------------------------------------------------------
	protected string GetHitZoneGroupName(ECharacterHitZoneGroup hitZoneGroup)
	{
		switch (hitZoneGroup)
		{
			case ECharacterHitZoneGroup.HEAD:
			{
				return "#AR-Inventory_Head";
			}

			case ECharacterHitZoneGroup.UPPERTORSO:
			{
				return "#AR-Inventory_Chest";
			}

			case ECharacterHitZoneGroup.LOWERTORSO:
			{
				return "#AR-Inventory_Abdomen";
			}

			case ECharacterHitZoneGroup.LEFTARM:
			{
				return "#AR-Inventory_LeftArm";
			}

			case ECharacterHitZoneGroup.RIGHTARM:
			{
				return "#AR-Inventory_RightArm";
			}

			case ECharacterHitZoneGroup.LEFTLEG:
			{
				return "#AR-Inventory_LeftLeg";
			}

			case ECharacterHitZoneGroup.RIGHTLEG:
			{
				return "#AR-Inventory_RightLeg";
			}

		}

		return "";
	}
}
