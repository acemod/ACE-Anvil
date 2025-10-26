//------------------------------------------------------------------------------------------------
//! System responsible for handling captive characters
class ACE_Captives_CaptiveSystem : GameSystem
{
	protected ref map<SCR_ChimeraCharacter, ref ACE_Captives_CaptiveSystemCharacterWrapper> m_aCaptives = new map<SCR_ChimeraCharacter, ref ACE_Captives_CaptiveSystemCharacterWrapper>();
	
	//------------------------------------------------------------------------------------------------
	static ACE_Captives_CaptiveSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_Captives_CaptiveSystem.Cast(world.FindSystem(ACE_Captives_CaptiveSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetLocation(ESystemLocation.Server);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnLifeStateChanged(SCR_ChimeraCharacter char, ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		if (previousLifeState == newLifeState)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(char.GetCompartmentAccessComponent());
		if (!compartmentAccess)
			return;
		
		// Move revived characters outside of vehicles to tied state
		if (newLifeState == ECharacterLifeState.ALIVE && (!compartmentAccess.GetVehicle() || ACE_AnimationTools.GetHelperCompartment(char)))
			ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.TIED, char);
		// Unregister dead characters
		else if (newLifeState == ECharacterLifeState.DEAD)
			Unregister(char);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Put conscious captives that left a vehicle in tied state
	void OnCompartmentLeft(SCR_ChimeraCharacter char, IEntity vehicle, BaseCompartmentManagerComponent manager, int mgrID, int slotID, bool move)
	{
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!charController)
			return;
		
		// Stop captivity if we are transitioning to surrender
		if (charController.ACE_Captives_HasSurrendered())
			Unregister(char);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register a character to become a captive
	void Register(notnull SCR_ChimeraCharacter char)
	{
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!charController)
			return;
		
		if (charController.GetLifeState() == ECharacterLifeState.DEAD)
			return;
		
		m_aCaptives[char] = new ACE_Captives_CaptiveSystemCharacterWrapper(char);	
		
		SCR_EditableEntityComponent editableChar = SCR_EditableEntityComponent.GetEditableEntity(char);
		if (editableChar)
			editableChar.GetOnDeleted().Insert(Unregister);
		
		// Make character CIV so they don't get shot by AI
		SCR_CharacterFactionAffiliationComponent factionAffiliation = SCR_CharacterFactionAffiliationComponent.Cast(char.FindComponent(SCR_CharacterFactionAffiliationComponent));
		if (factionAffiliation)
			factionAffiliation.SetAffiliatedFactionByKey("CIV");
		
		// Remove last instigator, as they would otherwise get punished when the captive respawns
		SCR_DamageManagerComponent damageManager = char.GetDamageManager();
		if (damageManager)
			damageManager.SetInstigatorEntity(null);
		
		charController.ACE_Captives_SetIsCaptive(true);
		
		// Put in tied animation when not unconscious and not in a vehicle
		IEntity vehicle = SCR_CompartmentAccessComponent.GetVehicleIn(char);
		if ((vehicle && !ACE_AnimationHelperCompartment.Cast(vehicle)) || charController.GetLifeState() == ECharacterLifeState.INCAPACITATED)
			return;
		
		ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.TIED, char);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister a character to become released from captivity
	void Unregister(SCR_ChimeraCharacter char)
	{
		m_aCaptives.Remove(char);
		
		if (!char)
			return;
		
		SCR_EditableEntityComponent editableChar = SCR_EditableEntityComponent.GetEditableEntity(char);
		if (editableChar)
			editableChar.GetOnDeleted().Remove(Unregister);
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!charController)
			return;
		
		// Do not reset faction when we transition to surrendered state
		SCR_CharacterFactionAffiliationComponent factionAffiliation = SCR_CharacterFactionAffiliationComponent.Cast(char.FindComponent(SCR_CharacterFactionAffiliationComponent));
		if (factionAffiliation && !charController.ACE_Captives_HasSurrendered())
			factionAffiliation.SetAffiliatedFaction(factionAffiliation.GetDefaultAffiliatedFaction());
		
		charController.ACE_Captives_SetIsCaptive(false);
		
		// If character is about to surrender, we don't need to terminate the helper
		if (!charController.ACE_Captives_HasSurrendered())
			ACE_AnimationTools.TerminateHelperCompartment(char);
	}
}

//------------------------------------------------------------------------------------------------
//! Forwards character events to ACE_Captives_CaptiveSystem's event handlers
class ACE_Captives_CaptiveSystemCharacterWrapper : Managed
{
	protected static ACE_Captives_CaptiveSystem s_pSystem;
	protected SCR_ChimeraCharacter m_pCharacter;	
	
	//------------------------------------------------------------------------------------------------
	//! Register event handlers
	void ACE_Captives_CaptiveSystemCharacterWrapper(notnull SCR_ChimeraCharacter char)
	{
		if (!s_pSystem)
			s_pSystem = ACE_Captives_CaptiveSystem.GetInstance();
		
		m_pCharacter = char;
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCharacter.GetCharacterController());
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnLifeStateChanged);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pCharacter.GetCompartmentAccessComponent());
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		s_pSystem.OnLifeStateChanged(m_pCharacter, previousLifeState, newLifeState);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnCompartmentLeft(IEntity vehicle, BaseCompartmentManagerComponent manager, int mgrID, int slotID, bool move)
	{
		s_pSystem.OnCompartmentLeft(m_pCharacter, vehicle, manager, mgrID, slotID, move);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister event handlers
	void ~ACE_Captives_CaptiveSystemCharacterWrapper()
	{
		if (!m_pCharacter)
			return;
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCharacter.GetCharacterController());
		if (charController)
			charController.m_OnLifeStateChanged.Remove(OnLifeStateChanged);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pCharacter.GetCompartmentAccessComponent());
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
	}
}
