class ACE_Carrying_PullUserAction : ScriptedUserAction {
	float m_fApplicationDeltaTime;
	float m_fApplicationInterval=0.1;
	float movementFactor = 0.4;
	bool currentlyPulled;
	SCR_CharacterAnimationComponent ownerAnimationComponent;
	SCR_CharacterControllerComponent userControllerComponent;
	vector boneMatrix[4];
	[Attribute(desc:"0 crotch, 1 upper chest, 2 head, 3 left arm,4 right arm,5 left wrist,6 right wrist,7 left leg,8 left knee,9 right leg,10 right knee")]
	int boneIndex;

	override void PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		//Make the user entity crouch, if they are not swimming
		if (!userControllerComponent.IsSwimming())
		{
			userControllerComponent.ForceStance(ECharacterStance.CROUCH);//Put them into crouch position
		}
		//Enforce one call per interval
		if (m_fApplicationDeltaTime<m_fApplicationInterval)
		{
			m_fApplicationDeltaTime+=timeSlice;
			return;
		}
		m_fApplicationDeltaTime-=m_fApplicationInterval;
		
		ownerAnimationComponent.GetRagdollBone(boneIndex,boneMatrix);//Get the ragdoll bone
		vector boneCoordinates = {boneMatrix[3][0],boneMatrix[3][1],boneMatrix[3][2]};//Find it's location
		
		vector userEntityPos  = pUserEntity.GetOrigin();

		vector impulseDirection = (userEntityPos-boneCoordinates).Normalized();//Find the direction that impulse should be applied in
		float m_fDistance  = Math.Min(vector.Distance(boneCoordinates,userEntityPos),2); //Cap it, so no infinite force
		float impulsePower = Math.Pow(m_fDistance,2)*40-20;
		ownerAnimationComponent.AddRagdollEffectorDamage(pOwnerEntity.CoordToLocal(boneCoordinates),impulseDirection,impulsePower,0.2,m_fApplicationInterval);//Apply a force to it
	}

	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		currentlyPulled=true;
		
		if (!ownerAnimationComponent)//The owner never changes, so it is safe to create once
			ownerAnimationComponent = SCR_CharacterAnimationComponent.Cast(GetOwner().FindComponent(SCR_CharacterAnimationComponent));
		
		userControllerComponent = SCR_CharacterControllerComponent.Cast(pUserEntity.FindComponent(SCR_CharacterControllerComponent));
		userControllerComponent.OverrideMaxSpeed(movementFactor);
	}

	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.OnActionCanceled(pOwnerEntity, pUserEntity);
		currentlyPulled=false;
		userControllerComponent.OverrideMaxSpeed(1);
		userControllerComponent.ForceStance(0);//Let them stand back up
	}
	
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		//If user is already carrying something
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController || !userCharController.ACE_CanCarry(user,m_sCannotPerformReason))
			return false;
		
		if (currentlyPulled && (userCharController != userControllerComponent))//if the current puller != this person, don't let them pull
			return false;
		//If you're not carrying something, you are allowed to pull something :)
		return true;
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		//---Code from ACE_Carrying_BaseUserAction, reused to maximize efficency (tm)---//
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar || ownerChar.IsInVehicle())
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController || ownerCharController.GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return false;
		//---New code, check if not ragdolling
		if (!ownerCharController.GetAnimationComponent().IsRagdollActive())
			return false;
		
		return true;
	}

	
	
	override float GetActionProgressScript(float fProgress, float timeSlice)
	{
		if (currentlyPulled)
			return 998;
		return 0;//Bar empty when not pulled
	}

}

