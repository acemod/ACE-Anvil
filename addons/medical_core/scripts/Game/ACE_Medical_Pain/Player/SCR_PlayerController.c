//------------------------------------------------------------------------------------------------
//! Apply setting of pain HUD effect
//! Note that SCR_HUDManagerComponent does not support RplProp, so we do it here!
modded class SCR_PlayerController : PlayerController
{
	[RplProp(condition: RplCondition.OwnerOnly, onRplName: "ACE_Medical_OnPainEffectTypeChanged")]
	protected ACE_Medical_EPainEffectType m_eACE_Medical_PainEffectType;

	//------------------------------------------------------------------------------------------------
	protected override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;

		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (settings)
		{
			m_eACE_Medical_PainEffectType = settings.m_ePainScreenEffectType;
			ACE_Medical_OnPainEffectTypeChanged();
			Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnPainEffectTypeChanged()
	{
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.Cast(FindComponent(SCR_HUDManagerComponent));
		if (!hudManager)
			return;
		
		array<BaseInfoDisplay> displays = {};
		hudManager.GetInfoDisplays(displays);
		
		SCR_ScreenEffectsManager effectsManager;
		foreach (BaseInfoDisplay display : displays)
		{
			effectsManager = SCR_ScreenEffectsManager.Cast(display);
			if (effectsManager)
				break;
		}
		
		if (!effectsManager)
			return;
		
		array<BaseInfoDisplay> effects = {};
		effectsManager.GetInfoDisplays(effects);
		foreach (BaseInfoDisplay effect : effects)
		{
			ACE_Medical_PainScreenEffect painEffect = ACE_Medical_PainScreenEffect.Cast(effect);
			if (!painEffect)
				continue;
			
			painEffect.SetPainEffectType(m_eACE_Medical_PainEffectType);
			break;
		}
	}
}
