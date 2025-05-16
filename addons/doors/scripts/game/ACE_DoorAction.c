class ACE_AdjustDoorAction : SCR_AdjustSignalAction
{
	protected DoorComponent m_doorComponent;
	protected float m_lastStep;
	
	protected override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		
		m_doorComponent = DoorComponent.Cast(pOwnerEntity.FindComponent(DoorComponent));
	}
	
	override protected bool OnLoadActionData(ScriptBitReader reader)
	{
		bool result = super.OnLoadActionData(reader);
		if(Replication.IsServer())
			HandleAdjust();
		return result;
	}
	
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		
		RplComponent rpl = RplComponent.Cast(pUserEntity.FindComponent(RplComponent));
		if(!rpl || !rpl.IsOwner())
			return;
		
		float control = m_doorComponent.GetControlValue();
		SetSignalValue(control);
		m_fTargetValue = control;
		m_lastStep = control;
	}
	
	protected void HandleAdjust()
	{
		if(!Replication.IsServer())
			return;
		
		float currentStep = SCR_GetCurrentValue();
		if(currentStep == m_lastStep)
			return;
		
		m_lastStep = currentStep;
		m_doorComponent.SetControlValue(currentStep);
	}
}