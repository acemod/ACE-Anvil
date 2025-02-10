//------------------------------------------------------------------------------------------------
class ACE_SurfaceHelper
{
	protected static ref const array<ResourceName> DIGGABLE_PHYSMAT_RES_NAMES = {
		"{99CE88C5B1865216}Common/Materials/Physics/dirt.physmat",
		"{2B4B89135A4F0637}Common/Materials/Physics/grass.physmat",
		"{8AEE78000E7E1346}Common/Materials/Physics/gravel.physmat",
		"{C2004DDEB7A62E05}Common/Materials/Physics/sand.physmat",
		"{EFF6CC81E77B1860}Common/Materials/Physics/snow.physmat",
		"{B0F2C4F345C4894C}Common/Materials/Physics/soil.physmat"
	};

	//------------------------------------------------------------------------------------------------
	//! Returns the when the terrain surface at the given position is diggable
	static bool IsDiggableSurface(notnull SurfaceProperties props)
	{
		ResourceName resName;
		props.Get("Physics material", resName);
		
		foreach (ResourceName diggablePhysMatResName : DIGGABLE_PHYSMAT_RES_NAMES)
		{
			if (diggablePhysMatResName == resName)
				return true;
		}
		
		return false;
	}
}
