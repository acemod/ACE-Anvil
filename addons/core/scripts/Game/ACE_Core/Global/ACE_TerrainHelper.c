//------------------------------------------------------------------------------------------------
class ACE_TerrainHelper
{
	//------------------------------------------------------------------------------------------------
	//! Returns the surface properties of the terrain at the given position
	static SurfaceProperties GetSurfaceProperties(vector pos)
	{
		pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
		TraceParam params = new TraceParam();
		params.Flags = TraceFlags.WORLD;
		params.Start = pos + 0.01 * vector.Up;
		params.End = pos - 0.01 * vector.Up;
		GetGame().GetWorld().TraceMove(params, null);
		return params.SurfaceProps;
	}
}
