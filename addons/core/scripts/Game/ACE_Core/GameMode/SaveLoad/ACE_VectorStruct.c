//------------------------------------------------------------------------------------------------
//! JSON helper struct for storing vectors in an array
class ACE_VectorStruct : SCR_JsonApiStruct
{
	// short name for more efficient serialization
	protected vector v;
	
	//------------------------------------------------------------------------------------------------
	void ACE_VectorStruct(vector vec)
	{
		RegV("v");
		v = vec;
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetVector()
	{
		return v;
	}
}
