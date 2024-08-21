//------------------------------------------------------------------------------------------------
class ACE_Medical_CPRUserAction : ScriptedUserAction
{
	protected static const ResourceName HELPER_PREFAB_NAME = "{604E93B236B19A29}Prefabs/Helpers/ACE_Medical_CPRHelperCompartment.et";
	protected static const ref array<float> CPR_PERFORMER_ANGLES = {90, -90};
	protected static const ref array<vector> CPR_PERFORMER_OFFSETS = {{-0.2, 0.6, -0.85}, {0.35, 0.6, -0.85}};
	
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
		
		CharacterControllerComponent ownerController = ownerChar.GetCharacterController();
		if (!ownerController)
			return false;
		
		if (!ownerController.IsUnconscious())
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
	//! TO DO: Move helper compartment creation to a tool class like ACE_Carrying_Tools
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		vector userPos = pUserEntity.GetOrigin();
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		pOwnerEntity.GetWorldTransform(params.Transform);
		vector bestTransform[4];
		
		// Get best orientation to perform CPR
		for (int i = 0; i < CPR_PERFORMER_ANGLES.Count(); i++)
		{
			vector transform[4] = params.Transform;
			vector angles = Math3D.MatrixToAngles(params.Transform);
			angles[0] = angles[0] + CPR_PERFORMER_ANGLES[i];
			Math3D.AnglesToMatrix(angles, transform);
			transform[3] = transform[3] + CPR_PERFORMER_OFFSETS[i].Multiply3(transform);
			
			if (vector.DistanceSqXZ(userPos, transform[3]) < vector.DistanceSqXZ(userPos, bestTransform[3]))
				bestTransform = transform;
		}
		
		params.Transform = bestTransform;
		
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
