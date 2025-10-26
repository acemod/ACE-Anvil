//------------------------------------------------------------------------------------------------
class ACE_CarriableEntityComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_CarriableEntityComponent : ScriptComponent
{
	[Attribute(desc: "Info for the attachment point on the carrier")]
	protected ref PointInfo m_AttachmentPoint;
	
	[Attribute(defvalue: "1", desc: "Input limit for the carrier's walking", category: "Carrier modifier")]
	protected float m_fCarrierSpeedLimit;
	
	[Attribute(defvalue: "true", desc: "Whether the carrier can walk forward", category: "Carrier modifier")]
	protected bool m_bCarrierAllowWalkingForward;
	
	[Attribute(defvalue: "true", desc: "Whether the carrier can walk sideways", category: "Carrier modifier")]
	protected bool m_bCarrierAllowWalkingSideways;
	
	[Attribute(defvalue: "true", desc: "Whether the carrier can jump and vault", category: "Carrier modifier")]
	protected bool m_bCarrierAllowJumping;
	
	[Attribute(defvalue: "true", desc: "Whether the carrier can be in stand stance", category: "Carrier modifier")]
	protected bool m_bCarrierAllowStandStance;
	
	[Attribute(defvalue: "true", desc: "Whether the carrier can be in crouch stance", category: "Carrier modifier")]
	protected bool m_bCarrierAllowCrouchStance;
	
	[Attribute(defvalue: "false", desc: "Whether the carrier can be in prone stance", category: "Carrier modifier")]
	protected bool m_bCarrierAllowProneStance;
	
	[Attribute(defvalue: "false", desc: "Whether the carrier can use their weapon", category: "Carrier modifier")]
	protected bool m_bCarrierAllowWeapon;
	
	[RplProp(onRplName: "OnCarrierChanged")]
	protected RplId m_iCarrierID;
	protected SCR_ChimeraCharacter m_pCarrier;
	
	//------------------------------------------------------------------------------------------------
	static ACE_CarriableEntityComponent GetCarriableEntity(IEntity entity)
	{
		ACE_CarriableEntityComponent carriableComponent = ACE_CarriableEntityComponent.Cast(entity.FindComponent(ACE_CarriableEntityComponent));
		if (carriableComponent)
			return carriableComponent;
		
		ACE_AnimationHelperCompartment helper = ACE_AnimationHelperCompartment.Cast(entity.GetParent());
		if (!helper)
			return null;
		
		return ACE_CarriableEntityComponent.Cast(helper.FindComponent(ACE_CarriableEntityComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compartment gets attached to the carrier
	void Carry(notnull SCR_ChimeraCharacter carrier)
	{
		if (m_pCarrier)
			return;	
		
		m_iCarrierID = Replication.FindItemId(carrier);
		OnCarrierChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Release()
	{
		if (!m_pCarrier)
			return;
		
		m_iCarrierID = RplId.Invalid();
		OnCarrierChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnCarrierChanged()
	{
		SCR_ChimeraCharacter prevCarrier = SCR_ChimeraCharacter.Cast(GetOwner().GetParent());
		if (prevCarrier)
		{
			                                            
			if (!prevCarrier)
				return;
			
			RplComponent prevCarrierRpl = prevCarrier.GetRplComponent();
			if (!prevCarrierRpl)
				return;
			
			prevCarrier.RemoveChild(GetOwner(), true);
			
			if (!prevCarrierRpl.IsProxy())
				DetachHandlersAuthority();
			
			if (prevCarrierRpl.IsOwner())
				DetachHandlersOwner();
		}
		
		m_pCarrier = SCR_ChimeraCharacter.Cast(Replication.FindItem(m_iCarrierID));
		if (m_pCarrier)
		{
			RplComponent newCarrierRpl = m_pCarrier.GetRplComponent();			
			m_AttachmentPoint.Init(m_pCarrier);
			m_pCarrier.AddChild(GetOwner(), m_AttachmentPoint.GetNodeId());
			vector transform[4];
			m_AttachmentPoint.GetLocalTransform(transform);
			GetOwner().SetLocalTransform(transform);
			
			if (!newCarrierRpl.IsProxy())
				AttachHandlersAuthority();
			
			if (newCarrierRpl.IsOwner())
				AttachHandlersOwner();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AttachHandlersAuthority()
	{
		// Set carrier as owner
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController performerPlayerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerManager.GetPlayerIdFromControlledEntity(m_pCarrier)));
		
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (performerPlayerController && rpl)
			rpl.Give(performerPlayerController.GetRplIdentity());
				
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnCarrierDisconnected);
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCarrier.GetCharacterController());	
		if (!charController)
			return;
		
		charController.m_OnLifeStateChanged.Insert(OnCarrierLifeStateChanged);
		charController.ACE_SetCarried(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlersAuthority()
	{
		// Set server as owner
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (rpl)
			rpl.Give(RplIdentity.Local());
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Remove(OnCarrierDisconnected);
		
		if (!m_pCarrier)
			return;
			
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCarrier.GetCharacterController());	
		if (!charController)
			return;

		charController.m_OnLifeStateChanged.Remove(OnCarrierLifeStateChanged);
		charController.ACE_SetCarried(null);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AttachHandlersOwner()
	{
		GetGame().GetInputManager().AddActionListener("ACE_StopCarrying", EActionTrigger.DOWN, ActionReleaseCallback);
		SetEventMask(GetOwner(), EntityEvent.FRAME);
		
		BaseWeaponManagerComponent weaponManager = m_pCarrier.GetWeaponManager();
		if (!m_bCarrierAllowWeapon && weaponManager)
			weaponManager.m_OnWeaponChangeStartedInvoker.Insert(OnCarrierWeaponSelected);
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pCarrier.GetCharacterController());	
		if (!charController)
			return;
		
		// Put away weapon
		if (!m_bCarrierAllowWeapon)
			charController.SelectWeapon(null);
		
		// Make sure we are in an allowed stance
		switch (charController.GetStance())
		{
			case ECharacterStance.STAND:
			{
				if (m_bCarrierAllowStandStance)
					break;
				
				if (m_bCarrierAllowCrouchStance)
					charController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
				else
					charController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOPRONE);
			}
			
			case ECharacterStance.CROUCH:
			{
				if (m_bCarrierAllowCrouchStance)
					break;
				
				if (m_bCarrierAllowStandStance)
					charController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);
				else
					charController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOPRONE);
				
				break;
			}
			
			default:
			{
				if (m_bCarrierAllowProneStance)
					break;
				
				if (m_bCarrierAllowCrouchStance)
					charController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
				else
					charController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);
				
				break;
			}
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlersOwner()
	{
		GetGame().GetInputManager().RemoveActionListener("ACE_StopCarrying", EActionTrigger.DOWN, ActionReleaseCallback);
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
		
		if (!m_pCarrier)
			return;
		
		BaseWeaponManagerComponent weaponManager = m_pCarrier.GetWeaponManager();
		if (!m_bCarrierAllowWeapon && weaponManager)
			weaponManager.m_OnWeaponChangeStartedInvoker.Remove(OnCarrierWeaponSelected);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Limit walking speed and prevent prone stance
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		float forwardInput = GetGame().GetInputManager().GetActionValue("CharacterForward");
		float rightInput = GetGame().GetInputManager().GetActionValue("CharacterRight");
		
		if (!m_bCarrierAllowWalkingForward && forwardInput > 0)
		{
			forwardInput = 0;
			GetGame().GetInputManager().SetActionValue("CharacterForward", forwardInput);
		}
		
		if (!m_bCarrierAllowWalkingSideways)
		{
			rightInput = 0;
			GetGame().GetInputManager().SetActionValue("CharacterRight", rightInput);
		}
		
		// Enforce speed limit
		float inputMagnitude = Vector(forwardInput, 0, rightInput).Length();
		if (inputMagnitude > m_fCarrierSpeedLimit)
		{
			GetGame().GetInputManager().SetActionValue("CharacterForward", forwardInput * m_fCarrierSpeedLimit / inputMagnitude);
			GetGame().GetInputManager().SetActionValue("CharacterRight", rightInput * m_fCarrierSpeedLimit / inputMagnitude);
		}

		// Prevent jumping and valuting
		if (!m_bCarrierAllowJumping)
			GetGame().GetInputManager().SetActionValue("CharacterJump", 0);
		
		// Make sure disallowed stances cannot be selected
		if (!m_bCarrierAllowStandStance || !m_bCarrierAllowCrouchStance || !m_bCarrierAllowProneStance)
			GetGame().GetInputManager().SetActionValue("CharacterStanceModifier", 0);
		
		if (!m_bCarrierAllowStandStance)
		{
			GetGame().GetInputManager().SetActionValue("CharacterStand", 0);
			GetGame().GetInputManager().SetActionValue("CharacterStandCrouchToggle", 0);
			GetGame().GetInputManager().SetActionValue("CharacterStandProneToggle", 0);
		}
		
		if (!m_bCarrierAllowCrouchStance)
		{
			GetGame().GetInputManager().SetActionValue("CharacterCrouch", 0);
			GetGame().GetInputManager().SetActionValue("CharacterStandCrouchToggle", 0);
		}
		
		if (!m_bCarrierAllowProneStance)
		{
			GetGame().GetInputManager().SetActionValue("CharacterProne", 0);
			GetGame().GetInputManager().SetActionValue("CharacterStandProneToggle", 0);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when carrier gets incapacitated or dies
	protected void OnCarrierLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		Release();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when carrier selects a weapon
	protected void OnCarrierWeaponSelected(BaseWeaponComponent newWeaponSlot)
	{
		if (newWeaponSlot)
			Rpc(Release);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer or patient disconnects
	protected void OnCarrierDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (player == m_pCarrier)
			Release();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for the release keybind
	protected void ActionReleaseCallback()
	{
		Rpc(Release);
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetCarrier()
	{
		return m_pCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsCarried()
	{
		return m_pCarrier;
	}
}
