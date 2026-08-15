//------------------------------------------------------------------------------------------------
class ACE_Math3D
{
	//------------------------------------------------------------------------------------------------
	//! Return angle between two vectors
	//! \param[in] v0 First vector
	//! \param[in] v1 Second vector
	//! \return Angle in radians
	static float ComputeAngle(vector v0, vector v1)
	{
		return Math.Atan2(SCR_Math3D.Cross(v0, v1).Length(), vector.Dot(v0, v1));
	}
}
