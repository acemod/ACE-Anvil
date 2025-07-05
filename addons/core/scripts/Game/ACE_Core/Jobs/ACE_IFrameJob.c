//------------------------------------------------------------------------------------------------
//! Interface for frame jobs
//! Can be registered as standalone jobs to ACE_FrameJobSystem or used in custom systems
//! To Do: Implement templated context once you can inherit from primary templates
class ACE_IFrameJob : Managed
{
	private bool m_bShouldBeStopped = false;
	private bool m_bIsRunning = false;
	
	//------------------------------------------------------------------------------------------------
	void OnStart()
	{
		m_bIsRunning = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnUpdate(float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	void OnStop()
	{
		m_bIsRunning = false;
		m_bShouldBeStopped = false;
	}
	
	//------------------------------------------------------------------------------------------------
	void ScheduleStop()
	{
		m_bShouldBeStopped = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void CancelStop()
	{
		m_bShouldBeStopped = false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Method for job manager to check whether it should terminate the job
	bool ShouldBeStopped()
	{
		return m_bShouldBeStopped;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsRunning()
	{
		return m_bIsRunning;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Copies contents of `from` job to this job
	void CopyFrom(ACE_IFrameJob from);
}
