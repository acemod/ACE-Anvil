//------------------------------------------------------------------------------------------------
//! Same as ACE_IFrameJob, but adds a context for storing data
//! TO DO: Move to ACE_IFrameJob once templates can be properly subclassed
class ACE_IContextFrameJob<Managed TContext> : ACE_IFrameJob
{
	protected ref TContext m_pContext = null;
	
	//------------------------------------------------------------------------------------------------
	void SetContext(TContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	TContext GetContext()
	{
		return m_pContext;
	}
}
