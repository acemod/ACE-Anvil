//------------------------------------------------------------------------------------------------
class ACE_FrameJobScheduler_IObjectContext<Class TObject>
{
	TObject m_pObject;
	float m_fLastUpdateTime;
	
	//------------------------------------------------------------------------------------------------
	void ACE_FrameJobScheduler_IObjectContext(TObject object)
	{
		m_pObject = object;
		m_fLastUpdateTime = GetGame().GetWorld().GetWorldTime();
	}
}
