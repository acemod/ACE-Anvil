//------------------------------------------------------------------------------------------------
modded class SCR_WeaponInfo : SCR_InfoDisplayExtended
{
	//------------------------------------------------------------------------------------------------
	override protected void OnMuzzleChanged(BaseWeaponComponent weapon, BaseMuzzleComponent muzzle, BaseMuzzleComponent prevMuzzle)
	{
		super.OnMuzzleChanged(weapon, muzzle, prevMuzzle);
		ACE_UpdateZeroingIndicator();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnAttachmentChanged(WeaponComponent weapon, bool isAttached, IEntity attachmentEntity)
	{
		super.OnAttachmentChanged(weapon, isAttached, attachmentEntity);
		ACE_UpdateZeroingIndicator();
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_UpdateZeroingIndicator()
	{
		if (!m_WeaponState || !m_Widgets)
			return;
		
		if (!m_WeaponState.m_Sights)
		{
			m_Widgets.m_wACE_Zeroing.SetVisible(false);
			return;
		}
		
		m_eWeaponStateEvent |= EWeaponFeature.ZEROING;
		
		float horizontalZeroing;
		float verticalZeroing;
		m_WeaponState.m_Sights.ACE_GetZeroings(horizontalZeroing, verticalZeroing);
		
		m_Widgets.m_wACE_Zeroing.SetVisible(true);
		string verticalZeroingText = verticalZeroing.ToString(lenDec: 1);
		string horizontalZeroingText = Math.AbsFloat(horizontalZeroing).ToString(lenDec: 1);
		
		string horizontalZeroingSuffix;
		if (float.AlmostEqual(horizontalZeroing, 0.0))
			horizontalZeroingSuffix = "";
		else if (horizontalZeroing > 0.0)
			horizontalZeroingSuffix = "R";
		else
			horizontalZeroingSuffix = "L";
		
		m_Widgets.m_wACE_ZeroingText.SetText(string.Format("%1\n%2", verticalZeroingText, horizontalZeroingText));
		m_Widgets.m_wACE_ZeroingSuffix.SetText(string.Format("\n%1", horizontalZeroingSuffix));
		AnimateWidget_ColorFlash(m_Widgets.m_wZeroing, EWeaponFeature.ZEROING);
	}
}
