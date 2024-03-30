//------------------------------------------------------------------------------------------------
//! JSON helper struct for storing vectors in an array
class ACE_VectorStruct : SCR_JsonApiStruct
{
	protected vector m_vVector;
	
	//------------------------------------------------------------------------------------------------
	void ACE_VectorStruct()
	{
		RegV("m_vVector");
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetVector()
	{
		return m_vVector;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVector(vector v)
	{
		m_vVector = v;
	}
}
