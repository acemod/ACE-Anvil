//------------------------------------------------------------------------------------------------
//! JSON helper struct for storing vectors in an array
class ACE_VectorStruct : SCR_JsonApiStruct
{
	protected vector m_vVector;
	
	//------------------------------------------------------------------------------------------------
	void ACE_VectorStruct(vector v)
	{
		RegV("m_vVector");
		m_vVector = v;
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetVector()
	{
		return m_vVector;
	}
}
