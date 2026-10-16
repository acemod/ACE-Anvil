//------------------------------------------------------------------------------------------------
class ACE_Medical_InspectionDisplay : SCR_InfoDisplayExtended
{
	protected static const string INSPECTION_ACTION = "ACE_Medical_Inspection";

	[Attribute(defvalue: "0.5", desc: "Timeout between damage info updates in seconds.")]
	protected float m_fUpdateTimeout;

	[Attribute(defvalue: "2.5", desc: "Maximum inspection distance from the player in metres.")]
	protected float m_fInspectionRange;

	protected bool m_bIsActive;
	protected float m_fUpdateTimer;
	protected ChimeraCharacter m_Target;
	protected ACE_Medical_InspectionOverlaySystem m_InspectionOverlaySystem;
	protected ECharacterHitZoneGroup m_eHitZoneGroup;
	protected Widget m_wInspectionPanel;
	protected ACE_Medical_BodyPartNameUIComponent m_SelectedBodyPart;
	protected ACE_Medical_DamageInfoUIComponent m_DamageInfo;

	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		m_InspectionOverlaySystem = ACE_Medical_InspectionOverlaySystem.GetInstance(GetGame().GetWorld());

		if (m_wRoot)
		{
			m_wInspectionPanel = m_wRoot.FindAnyWidget("InspectionPanel");
			Widget selectedBodyPart = m_wRoot.FindAnyWidget("SelectedBodyPart");
			if (selectedBodyPart)
				m_SelectedBodyPart = ACE_Medical_BodyPartNameUIComponent.Cast(selectedBodyPart.FindHandler(ACE_Medical_BodyPartNameUIComponent));
			Widget damage = m_wRoot.FindAnyWidget("Damage");
			if (damage)
				m_DamageInfo = ACE_Medical_DamageInfoUIComponent.Cast(damage.FindHandler(ACE_Medical_DamageInfoUIComponent));

			// Anvil has no triage or treatment log yet.
			array<string> unsupportedWidgetNames = {"TriageStatus", "Activity"};
			foreach (string widgetName : unsupportedWidgetNames)
			{
				Widget widget = m_wRoot.FindAnyWidget(widgetName);
				if (widget)
					widget.SetVisible(false);
			}
		}

		Disable();
		GetGame().GetInputManager().AddActionListener(INSPECTION_ACTION, EActionTrigger.DOWN, OnInspectionStart);
		GetGame().GetInputManager().AddActionListener(INSPECTION_ACTION, EActionTrigger.UP, OnInspectionEnd);
	}

	//------------------------------------------------------------------------------------------------
	void SetTarget(ChimeraCharacter target)
	{
		if (m_Target == target)
			return;

		m_Target = target;
		m_eHitZoneGroup = ECharacterHitZoneGroup.VIRTUAL;
		m_fUpdateTimer = 0;
		ResetWidgets();

		if (m_InspectionOverlaySystem)
			m_InspectionOverlaySystem.GenerateOverlay(target);
	}

	//------------------------------------------------------------------------------------------------
	void Enable()
	{
		if (m_bIsActive || !CanInspect())
			return;

		m_bIsActive = true;
		LowerWeapon();
		Show(true);

		if (m_bCanShow)
			UpdateFocus();
	}

	//------------------------------------------------------------------------------------------------
	void Disable()
	{
		m_bIsActive = false;
		DeleteOverlays();
		m_Target = null;
		m_eHitZoneGroup = ECharacterHitZoneGroup.VIRTUAL;
		m_fUpdateTimer = 0;
		ResetWidgets();
		Show(false);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnInspectionStart(float value, EActionTrigger reason)
	{
		Enable();
	}

	//------------------------------------------------------------------------------------------------
	protected void OnInspectionEnd(float value, EActionTrigger reason)
	{
		Disable();
	}

	//------------------------------------------------------------------------------------------------
	protected bool CanInspect()
	{
		ChimeraCharacter player = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!player)
			return false;

		CharacterControllerComponent charController = player.GetCharacterController();
		return charController && !charController.IsDead() && !charController.IsUnconscious();
	}

	//------------------------------------------------------------------------------------------------
	protected void LowerWeapon()
	{
		ChimeraCharacter player = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!player)
			return;

		CharacterControllerComponent charController = player.GetCharacterController();
		if (!charController)
			return;

		charController.SetWeaponADSInput(false);
		charController.SetWeaponADS(false);
		charController.SetWeaponRaised(false);
	}

	//------------------------------------------------------------------------------------------------
	protected ChimeraCharacter TracePatient(out ECharacterHitZoneGroup hitZoneGroup)
	{
		hitZoneGroup = ECharacterHitZoneGroup.VIRTUAL;
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player)
			return null;

		BaseWorld world = player.GetWorld();
		vector cameraTransform[4];
		world.GetCurrentCamera(cameraTransform);

		TraceParam trace = new TraceParam();
		trace.Start = cameraTransform[3];
		// Include the camera offset, so inspection also works in third person.
		float traceLength = vector.Distance(cameraTransform[3], player.GetOrigin()) + m_fInspectionRange;
		trace.End = cameraTransform[3] + cameraTransform[2] * traceLength;
		trace.Exclude = player;
		trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		trace.TargetLayers = EPhysicsLayerDefs.FireGeometry;
		
		float traceFrac = world.TraceMove(trace, null);
		if (traceFrac >= 1.0 || !trace.TraceEnt)
			return null;
		
		vector traceVector = traceFrac * (trace.End - trace.Start);
		if (vector.DistanceSq(player.GetOrigin(), trace.Start + traceVector) > Math.Pow(m_fInspectionRange, 2))
			return null;

		ChimeraCharacter target = ChimeraCharacter.Cast(trace.TraceEnt);
		if (!target)
		{
			// If traced entity is a worn item, trace the owner character
			InventoryItemComponent item = InventoryItemComponent.Cast(trace.TraceEnt.FindComponent(InventoryItemComponent));
			while (item)
			{
				InventoryStorageSlot slot = item.GetParentSlot();
				if (!slot)
					break;
				
				IEntity slotOwner = slot.GetOwner();
				if (!slotOwner)
					break;
				
				target = ChimeraCharacter.Cast(slotOwner);
				if (target)
					break;
				
				item = InventoryItemComponent.Cast(slotOwner.FindComponent(InventoryItemComponent));
			}
			
			if (!target)
				return null;
			
			trace.Start += traceVector;
			trace.Include = target;
			trace.Flags = TraceFlags.ENTS;
			world.TraceMove(trace, null);
		}
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.GetDamageManager());
		if (!damageManager)
			return null;

		SCR_CharacterHitZone hitZone = SCR_CharacterHitZone.Cast(damageManager.GetHitZoneByColliderID(trace.ColliderIndex));
		if (hitZone)
			hitZoneGroup = hitZone.GetHitZoneGroup();

		return target;
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateFocus()
	{
		ECharacterHitZoneGroup hitZoneGroup;
		ChimeraCharacter target = TracePatient(hitZoneGroup);
		bool targetChanged = target != m_Target;
		SetTarget(target);
		if (!m_Target)
			return;

		bool hitZoneGroupChanged = hitZoneGroup != m_eHitZoneGroup;
		m_eHitZoneGroup = hitZoneGroup;
		if (m_InspectionOverlaySystem && (targetChanged || hitZoneGroupChanged))
			m_InspectionOverlaySystem.SetHighlightedHitZoneGroup(hitZoneGroup);

		if (targetChanged || hitZoneGroupChanged || m_fUpdateTimer <= 0)
			UpdateInspection();
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateInspection()
	{
		if (!m_Target)
			return;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Target.GetDamageManager());
		if (!damageManager)
		{
			SetTarget(null);
			return;
		}

		m_fUpdateTimer = m_fUpdateTimeout;
		UpdateDamage(damageManager);
	}

	//------------------------------------------------------------------------------------------------
	protected void ResetWidgets()
	{
		if (m_wInspectionPanel)
			m_wInspectionPanel.SetVisible(false);
		if (m_SelectedBodyPart)
			m_SelectedBodyPart.SetHitZoneGroup(ECharacterHitZoneGroup.VIRTUAL);

		if (m_DamageInfo)
			m_DamageInfo.Reset();
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateDamage(SCR_CharacterDamageManagerComponent damageManager)
	{
		bool hasGroup = m_SelectedBodyPart && m_SelectedBodyPart.SetHitZoneGroup(m_eHitZoneGroup) && m_DamageInfo && m_DamageInfo.UpdateDamage(damageManager, m_eHitZoneGroup);
		if (!hasGroup)
		{
			ResetWidgets();
			return;
		}

		if (m_wInspectionPanel)
			m_wInspectionPanel.SetVisible(true);
	}

	//------------------------------------------------------------------------------------------------
	protected void DeleteOverlays()
	{
		if (m_InspectionOverlaySystem)
			m_InspectionOverlaySystem.DeleteOverlay();
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		Disable();
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayStopDraw(IEntity owner)
	{
		GetGame().GetInputManager().RemoveActionListener(INSPECTION_ACTION, EActionTrigger.DOWN, OnInspectionStart);
		GetGame().GetInputManager().RemoveActionListener(INSPECTION_ACTION, EActionTrigger.UP, OnInspectionEnd);
		Disable();
		m_wInspectionPanel = null;
		m_SelectedBodyPart = null;
		m_DamageInfo = null;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		if (!m_bIsActive)
			return;

		// Also check the action value in case releasing the key was consumed by a menu.
		InputManager inputManager = GetGame().GetInputManager();
		if (!CanInspect() || !inputManager.IsContextActive("CharacterGeneralContext") || inputManager.GetActionValue(INSPECTION_ACTION) <= 0)
		{
			Disable();
			return;
		}

		if (!m_bCanShow)
			return;

		LowerWeapon();
		m_fUpdateTimer -= timeSlice;
		UpdateFocus();
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayOnSuspended()
	{
		SetTarget(null);
	}

	//------------------------------------------------------------------------------------------------
	override void DisplayOnResumed()
	{
		if (!m_bIsActive)
			return;

		if (CanInspect() && GetGame().GetInputManager().GetActionValue(INSPECTION_ACTION) > 0)
		{
			LowerWeapon();
			UpdateFocus();
		}
		else
			Disable();
	}

	//------------------------------------------------------------------------------------------------
	void ~ACE_Medical_InspectionDisplay()
	{
		DeleteOverlays();
	}
}
