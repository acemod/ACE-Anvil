//------------------------------------------------------------------------------------------------
class ACE_BallisticTableComponentClass : SCR_BallisticTableComponentClass
{
	[Attribute(SCR_Enum.GetDefault(SCR_EOpticsAngleUnits.MILLIRADIANS), uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(SCR_EOpticsAngleUnits))]
	protected SCR_EOpticsAngleUnits m_eUnitType;
	
	//------------------------------------------------------------------------------------------------
	void UpdateBallisticData()
	{
		m_aBallisticPages.Clear();
		
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!player)
			return;
		
		array<BaseWeaponComponent> weapons = {};
		player.GetWeaponManager().GetWeapons(weapons);
		array<ResourceName> allBulletPrefab = {};
		array<float> speedCoefs = {};
		array<float> defaultZeroingRanges = {};
		
		foreach (BaseWeaponComponent weapon : weapons)
		{
			if (weapon.GetWeaponType() == EWeaponType.WT_HANDGUN)
				continue;
			
			float defaultZeroingRange = weapon.GetCurrentSightsZeroing();
			array<BaseMuzzleComponent> muzzles = {};
			weapon.GetMuzzlesList(muzzles);
			
			foreach (BaseMuzzleComponent muzzle : muzzles)
			{
				float speedCoef = muzzle.GetBulletInitSpeedCoef();
				
				foreach (ResourceName bulletPrefab : ACE_BulletTools.GetDefaultResourceNamesFromMuzzle(muzzle))
				{
					int idx = allBulletPrefab.Find(bulletPrefab);
					if ((idx >= 0) && (speedCoefs[idx] == speedCoef) && (defaultZeroingRanges[idx] == defaultZeroingRange))
						continue;
					
					allBulletPrefab.Insert(bulletPrefab);
					speedCoefs.Insert(speedCoef);
					defaultZeroingRanges.Insert(defaultZeroingRange);
				}
			}
		}
		
		m_aBallisticPages.Reserve(allBulletPrefab.Count());
		
		foreach (int i, ResourceName bulletPrefab : allBulletPrefab)
		{
			ACE_VisualisedBallisticConfig page = new ACE_VisualisedBallisticConfig(bulletPrefab, speedCoefs[i], defaultZeroingRanges[i], m_eUnitType);
			page.GenerateBallisticData();
			m_aBallisticPages.Insert(page);
		}
	}
}

//------------------------------------------------------------------------------------------------
class ACE_BallisticTableComponent : SCR_BallisticTableComponent
{
	//------------------------------------------------------------------------------------------------
	protected override void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		if (mode == EGadgetMode.IN_HAND && charOwner == SCR_PlayerController.GetLocalControlledEntity())
		{
			ACE_BallisticTableComponentClass data = ACE_BallisticTableComponentClass.Cast(GetComponentData(GetOwner()));
			if (data)
			{
				data.UpdateBallisticData();
				m_iNumberOfPages = data.GetNumberOfPages();
			}
		}

		super.ModeSwitch(mode, charOwner);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Disable broadcasting of pages
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	override void RpcDo_SyncPageChange(int selectedPage)
	{
	}

	//------------------------------------------------------------------------------------------------
	//! Disable broadcasting of pages
	override bool RplSave(ScriptBitWriter writer)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Disable broadcasting of pages
	override bool RplLoad(ScriptBitReader reader)
	{
		return true;
	}
}
