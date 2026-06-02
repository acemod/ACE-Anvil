class ACE_Carrying_PullUserAction : ACE_Carrying_BaseUserAction {
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		//If user is already carrying something
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController || !userCharController.ACE_CanCarry(user,m_sCannotPerformReason))
			return false;
		//If you're not carrying something, you are allowed to pull something :)
		return true;
	
	
	}
	override bool CanBeShownScript(IEntity user)
	{
		//---Code from super, reused to maximize efficency (tm)---//
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar || ownerChar.IsInVehicle())
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController || ownerCharController.GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return false;
		
		//---New code---//
		//---If I am not ragdolling// or i am not in water
		if (!ownerCharController.GetAnimationComponent().IsRagdollActive() || !ChimeraWorldUtils.TryGetWaterSurfaceSimple(GetGame().GetWorld(),ownerChar.GetOrigin()))
			return false;

		return true;
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		vector ownerEntityPos = pOwnerEntity.GetOrigin();//Ignore Y component
		vector userEntityPos  = pUserEntity.GetOrigin();
		
		vector distanceVector = userEntityPos-ownerEntityPos;
		float distanceFloat = vector.Distance(ownerEntityPos,userEntityPos);
		
		float travelDistance = Math.Min(1,distanceFloat)-0.1; // Minimum buffer distance of a bit, cap 1 meter per drag
		
		Print(ownerEntityPos);
		Print(userEntityPos);
		Print(distanceVector);
		Print(distanceFloat);
		Print(travelDistance);
		
		float travelDistanceNormalized = travelDistance/distanceFloat; //Normalize by distance
		vector newOwnerPos = vector.Lerp(ownerEntityPos,userEntityPos,distanceFloat);
		
		array<string> matrixValues = {"","","",""};
		
		
		string.Format("1 0 0 %1,0 1 0 %2,0 0 1 %3,0 0 0 1",newOwnerPos[0],newOwnerPos[1],newOwnerPos[2]).Split(",",matrixValues,false);

		vector mat[4] = {matrixValues[0].ToVector(),matrixValues[1].ToVector(),matrixValues[2].ToVector(),matrixValues[3].ToVector()};
        SCR_EditableCharacterComponent.Cast(pOwnerEntity.FindComponent(SCR_EditableCharacterComponent)).SetTransform(mat);
        
		
	}
}