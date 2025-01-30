//------------------------------------------------------------------------------------------------
class ACE_CarriableEntityComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_CarriableEntityComponent : ScriptComponent
{
	[Attribute(desc: "Info for the attachment point on the carrier")]
	protected ref PointInfo m_AttachmentPoint;
	
	[RplProp(onRplName: "OnCarrierChanged")]
	protected RplId m_iCarrierID;
	protected IEntity m_pCarrier;
	
	//------------------------------------------------------------------------------------------------
	//! Compartment gets attached to the carrier
	void Carry(notnull IEntity carrier)
	{
		if (m_pCarrier)
			return;
				
		RplComponent carrierRpl = RplComponent.Cast(carrier.FindComponent(RplComponent));
		if (!carrierRpl)
			return;
		
		m_iCarrierID = carrierRpl.Id();
		OnCarrierChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
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
		RplComponent carrierRpl = RplComponent.Cast(Replication.FindItem(m_iCarrierID));
		if (carrierRpl)
		{
			m_pCarrier = carrierRpl.GetEntity();
			m_AttachmentPoint.Init(m_pCarrier);
			m_pCarrier.AddChild(GetOwner(), m_AttachmentPoint.GetNodeId());
			vector transform[4];
			m_AttachmentPoint.GetLocalTransform(transform);
			GetOwner().SetLocalTransform(transform);
		}
		else
		{
			GetOwner().GetParent().RemoveChild(GetOwner(), true);
		}
	}
}
