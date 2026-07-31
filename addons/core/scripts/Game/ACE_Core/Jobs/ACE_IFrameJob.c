//------------------------------------------------------------------------------------------------
//! Interface for frame jobs
//! Can be registered as standalone jobs to ACE_FrameJobSystem or used in custom systems
//! To Do: Implement templated context once you can inherit from primary templates
class ACE_IFrameJob : Managed
{
	private bool m_bShouldBeStopped = false;
	private bool m_bShouldBeUpdated = true;
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
	//! To turn on/off OnUpdate
	void ToggleUpdate(bool enable)
	{
		m_bShouldBeUpdated = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Method for job manager to check whether it should call OnUpdate
	bool ShouldBeUpdated()
	{
		return m_bShouldBeUpdated;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true when the job is running. Is independent of `ShouldBeUpdated`.
	bool IsRunning()
	{
		return m_bIsRunning;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Forces termination of job
	void Stop()
	{
		m_bShouldBeStopped = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Method for job manager to check whether it should terminate the job
	bool ShouldBeStopped()
	{
		return m_bShouldBeStopped;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Copies contents of `from` job to this job
	void CopyFrom(ACE_IFrameJob from);
}
