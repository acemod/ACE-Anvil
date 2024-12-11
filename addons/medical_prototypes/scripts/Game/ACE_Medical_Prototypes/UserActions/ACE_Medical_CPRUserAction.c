//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRUserAction : ScriptedUserAction
{
	protected static const ResourceName HELPER_PREFAB_NAME = "{604E93B236B19A29}Prefabs/Helpers/ACE_Medical_CPRHelperCompartment.et";
	protected static const ref array<float> CPR_PERFORMER_ANGLES = {90, -90};
	protected static const ref array<vector> CPR_PERFORMER_OFFSETS = {{-0.3, 0, -0.8}, {0.3, 0, -0.8}};
	protected static const vector CPR_PERFORMER_BB_OFFSET = {0, 0.45, 0};
	protected static const vector CPR_PERFORMER_BB_HALF_EXTENDS = {0.15, 0.15, 0.15};
	
	//------------------------------------------------------------------------------------------------
	//! Can this action be shown in the UI to the provided user entity?
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;

		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;

		if (userChar.IsInVehicle())
			return false;

		CharacterControllerComponent userController = userChar.GetCharacterController();
		if (!userController)
			return false;

		if (userController.IsSwimming() || userController.IsFalling())
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar.IsInVehicle())
			return false;
		
		SCR_CharacterControllerComponent ownerController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerController)
			return false;
		
		if (!ownerController.IsUnconscious() || ownerController.ACE_Medical_GetUnconsciousPose() != ACE_Medical_EUnconsciousPose.BACK)
			return false;
		
		CharacterAnimationComponent ownerAnimation = ownerChar.GetAnimationComponent();
		if (!ownerAnimation)
			return false;
		
		if (ownerAnimation.IsRagdollActive())
			return false;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(ownerChar.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			false;
		
		if (cardiovascularComponent.IsCPRPerformed())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Block action if position is obstructed
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		IEntity owner = GetOwner();
		if (!owner)
			return false;
		
		TraceOBB trace = new TraceOBB();
		trace.Exclude = user;
		vector transform[4];
		GetEntryTransform(transform, owner, user);
		
		for (int i = 0; i < 3; i++)
		{
			trace.Mat[i] = transform[i];
		}
		
		trace.Maxs = CPR_PERFORMER_BB_HALF_EXTENDS;
		trace.Mins = -CPR_PERFORMER_BB_HALF_EXTENDS;
		trace.Start = transform[3] + CPR_PERFORMER_BB_OFFSET;
		
		if (GetGame().GetWorld().TracePosition(trace, TraceObstructionCallback) < 0)
		{
			SetCannotPerformReason("#AR-UserAction_SeatObstructed");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return the transform for where the user will do CPR from
	protected void GetEntryTransform(out vector transform[4], IEntity owner, IEntity user)
	{
		vector userPos = user.GetOrigin();
		vector bestTransform[4];
		
		// Get best orientation to perform CPR
		for (int i = 0; i < CPR_PERFORMER_ANGLES.Count(); i++)
		{
			owner.GetWorldTransform(transform);
			vector angles = Math3D.MatrixToAngles(transform);
			angles[0] = angles[0] + CPR_PERFORMER_ANGLES[i];
			Math3D.AnglesToMatrix(angles, transform);
			transform[3] = transform[3] + CPR_PERFORMER_OFFSETS[i].Multiply3(transform);
			
			if (vector.DistanceSqXZ(userPos, transform[3]) < vector.DistanceSqXZ(userPos, bestTransform[3]))
				bestTransform = transform;
		}
		
		transform = bestTransform;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Ignore inventory items
	protected bool TraceObstructionCallback(IEntity entity)
	{
		return !InventoryItemComponent.Cast(entity.FindComponent(InventoryItemComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//! TO DO: Move helper compartment creation to a tool class like ACE_Carrying_Tools
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Check on server that in fact no one is performing CPR
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent || cardiovascularComponent.IsCPRPerformed())
			return;
				
		vector userPos = pUserEntity.GetOrigin();
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetEntryTransform(params.Transform, pOwnerEntity, pUserEntity);
		
		Resource res = Resource.Load(HELPER_PREFAB_NAME);
		if (!res.IsValid())
			return;
		
		ACE_Medical_CPRHelperCompartment helper = ACE_Medical_CPRHelperCompartment.Cast(GetGame().SpawnEntityPrefab(res, null, params));
		if (!helper)
			return;
		
		helper.Init(pUserEntity, pOwnerEntity);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(pUserEntity.FindComponent(SCR_CompartmentAccessComponent));
		if (compartmentAccess)
			compartmentAccess.MoveInVehicle(helper, ECompartmentType.CARGO);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clients should not run the action
	override bool CanBroadcastScript()
	{
		return false;
	}
}
