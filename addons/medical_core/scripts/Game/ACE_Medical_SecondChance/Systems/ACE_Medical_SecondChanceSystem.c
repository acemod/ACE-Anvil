//-----------------------------------------------------------------------------------------------------------
//! Second Chance System
//! --------------------
//! A character's damage manager gets registered when a killing blow is dealt to it.
//! If second chance is granted, the character survives, but gets knocked out.
//! The character will stay alive for ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS.
//! After that all destroyed physical hit zones get checked if they grant second chance.
//! If they grant it, they'll heal by SECOND_CHANCE_SCALED_RECOVERED_HEALTH.
//! If they don't grant it, the character gets killed.
//! Finally, the character's damage manager gets unregistered.
//! After ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS has passed, no more second chances are granted
//! until the character wakes up.
class ACE_Medical_SecondChanceSystem : GameSystem
{
	protected ref array<SCR_CharacterDamageManagerComponent> m_aDamageManagers = {};
	protected ref array<SCR_CharacterDamageManagerComponent> m_aDamageManagersToRegister = {};
	protected ref array<SCR_CharacterDamageManagerComponent> m_aDamageManagersToUnregister = {};
	protected bool m_bIsUpdating = false;
	protected bool m_bScheduledQueueUpdate = false;
	
	protected static const float SECOND_CHANCE_SCALED_RECOVERED_HEALTH = 0.01;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_SecondChanceSystem GetInstance(ChimeraWorld world)
	{
		return ACE_Medical_SecondChanceSystem.Cast(world.FindSystem(ACE_Medical_SecondChanceSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Should be paused when simulation is paused
	override bool ShouldBePaused()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		
		if (m_aDamageManagers.IsEmpty())
			Enable(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		// Set updating lock such that m_aQueue is left untouched by Register/Unregister
		m_bIsUpdating = true;
		
		// Deactivate second chance for managers for which it has expired
		foreach (SCR_CharacterDamageManagerComponent damageManager : m_aDamageManagers)
		{
			if (!damageManager || damageManager.IsDestroyed())
				Unregister(damageManager);
			else if (damageManager.ACE_Medical_ShouldDeactivateSecondChance())
				HandleSecondChanceDeactivation(damageManager);
		}
		
		// Unset updating lock
		m_bIsUpdating = false;
		
		// Process entities scheduled for registering/unregistering from the queue
		if (m_bScheduledQueueUpdate)
		{
			foreach (SCR_CharacterDamageManagerComponent damageManager : m_aDamageManagersToRegister)
			{
				Register(damageManager);
			}
			
			foreach (SCR_CharacterDamageManagerComponent damageManager : m_aDamageManagersToUnregister)
			{
				Unregister(damageManager);
			}
			
			m_aDamageManagersToRegister.Clear();
			m_aDamageManagersToUnregister.Clear();
			m_bScheduledQueueUpdate = false;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check each destroyed hit zone if it grants second chance
	//! If not, kill the character
	protected void HandleSecondChanceDeactivation(SCR_CharacterDamageManagerComponent damageManager)
	{
		Unregister(damageManager);
		
		if (!damageManager.ACE_Medical_IsSecondChanceEnabled())
			return damageManager.Kill(damageManager.GetInstigator());
		
		array<HitZone> hitZones = {};
		damageManager.GetPhysicalHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (!charHitZone)
				continue;
			
			if (charHitZone.GetDamageState() != EDamageState.DESTROYED)
				continue;
			
			if (!charHitZone.ACE_Medical_ShouldGrantSecondChance())
			{
				damageManager.Kill(damageManager.GetInstigator());
				break;
			}
			
			ApplySecondChanceRegenToHitZone(damageManager, charHitZone);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Heals the hit zone by SECOND_CHANCE_SCALED_RECOVERED_HEALTH
	protected void ApplySecondChanceRegenToHitZone(SCR_CharacterDamageManagerComponent damageManager, SCR_HitZone hitZone)
	{
		if (!hitZone)
			return;
		
		vector hitPosDirNorm[3];
		SCR_DamageContext regenContext = new SCR_DamageContext(
			EDamageType.REGENERATION,
			-hitZone.GetMaxHealth() * SECOND_CHANCE_SCALED_RECOVERED_HEALTH,
			hitPosDirNorm,
			damageManager.GetOwner(),
			hitZone,
			null, null, -1, -1
		);
		regenContext.damageEffect = new InstantDamageEffect();
		damageManager.HandleDamage(regenContext);
	}
	
	//------------------------------------------------------------------------------------------------
	//! struckHitZone is the hit zone that caused the registration and is evaluated for second chance
	void Register(notnull SCR_CharacterDamageManagerComponent damageManager, SCR_CharacterHitZone struckHitZone)
	{
		// Refuse registration and kill character if second chance is not granted
		if (!struckHitZone || !damageManager.ACE_Medical_IsSecondChanceEnabled() || damageManager.ACE_Medical_ShouldDeactivateSecondChance() || !struckHitZone.ACE_Medical_ShouldGrantSecondChance())
		{
			// Put kill on callqueue to avoid potential crash (e.g. when called from HitZone::OnDamageStateChanged)
			GetGame().GetCallqueue().Call(damageManager.Kill, damageManager.GetInstigator());
			return;
		}
		
		damageManager.ACE_Medical_OnSecondChanceGranted();
		Register(damageManager);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Register(SCR_CharacterDamageManagerComponent damageManager)
	{
		// Only directly touch queue while no updating is going on
		if (m_bIsUpdating)
		{
			m_aDamageManagersToRegister.Insert(damageManager);
			m_aDamageManagersToUnregister.RemoveItem(damageManager);
			m_bScheduledQueueUpdate = true;
		}
		else
		{
			if (!m_aDamageManagers.Contains(damageManager))
				m_aDamageManagers.Insert(damageManager);
		
			if (!IsEnabled())
				Enable(true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(SCR_CharacterDamageManagerComponent damageManager)
	{
		// Only directly touch queue while no updating is going on
		if (m_bIsUpdating)
		{
			m_aDamageManagersToUnregister.Insert(damageManager);
			m_aDamageManagersToRegister.RemoveItem(damageManager);
			m_bScheduledQueueUpdate = true;
		}
		else
		{
			m_aDamageManagers.RemoveItem(damageManager);
			
			if (m_aDamageManagers.IsEmpty())
				Enable(false);
		}
	}
}
