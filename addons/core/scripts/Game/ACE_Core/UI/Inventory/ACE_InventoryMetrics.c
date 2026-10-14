//------------------------------------------------------------------------------------------------
class ACE_InventoryMetrics
{
	//------------------------------------------------------------------------------------------------
	static string Volume(float cubicCentimetres)
	{
		if (cubicCentimetres < 0)
			return "";
		return VolumeNumber(cubicCentimetres) + " L";
	}

	//------------------------------------------------------------------------------------------------
	protected static string VolumeNumber(float cubicCentimetres)
	{
		if (cubicCentimetres > 0 && cubicCentimetres < 1)
			return "<0.001";

		float litres = cubicCentimetres / 1000;
		return CompactNumber(litres, 3);
	}

	//------------------------------------------------------------------------------------------------
	protected static string CompactNumber(float value, int decimals)
	{
		string text = value.ToString(-1, decimals);
		while (text.EndsWith("0"))
			text = text.Substring(0, text.Length() - 1);

		if (text.EndsWith("."))
			text = text.Substring(0, text.Length() - 1);

		return text;
	}

	//------------------------------------------------------------------------------------------------
	protected static void GetCompartments(BaseInventoryStorageComponent storage, out array<BaseInventoryStorageComponent> compartments)
	{
		if (!storage || storage.GetOwner().FindComponent(BaseWeaponComponent))
			return;

		// Prefer the bound storage's capacity over auxiliary attachment compartments.
		if (!ClothNodeStorageComponent.Cast(storage) && storage.GetMaxVolumeCapacity() > 0)
		{
			compartments.Insert(storage);
			return;
		}

		array<BaseInventoryStorageComponent> attached = {};
		bool clothing = ClothNodeStorageComponent.Cast(storage) != null;
		if (clothing)
			storage.GetOwnedStorages(attached, 1, false);
		else
			storage.GetOwnedStorages(attached, 0, false);

		foreach (BaseInventoryStorageComponent compartment : attached)
		{
			if (!compartment || compartment == storage || compartments.Contains(compartment))
				continue;

			// Slot-attached bags are not compartments of the outer container.
			if ((!clothing && compartment.IsCompartment()) || (clothing && SCR_UniversalInventoryStorageComponent.Cast(compartment)))
				compartments.Insert(compartment);
		}

	}

	//------------------------------------------------------------------------------------------------
	static string Container(BaseInventoryStorageComponent storage)
	{
		array<BaseInventoryStorageComponent> compartments = {};
		GetCompartments(storage, compartments);
		if (compartments.IsEmpty())
			return "";

		float remaining;
		foreach (BaseInventoryStorageComponent compartment : compartments)
		{
			float capacity = compartment.GetMaxVolumeCapacity();
			float used = compartment.GetOccupiedSpace();
			if (capacity <= 0)
				return "";
			if (used < 0)
				return "";

			remaining += Math.Max(0, capacity - used);
		}

		return Volume(remaining);
	}

	//------------------------------------------------------------------------------------------------
	static int CompartmentCount(BaseInventoryStorageComponent storage)
	{
		array<BaseInventoryStorageComponent> compartments = {};
		GetCompartments(storage, compartments);
		return compartments.Count();
	}

	//------------------------------------------------------------------------------------------------
	static string Capacity(BaseInventoryStorageComponent storage)
	{
		array<BaseInventoryStorageComponent> compartments = {};
		GetCompartments(storage, compartments);
		if (compartments.IsEmpty())
			return "";

		float total;
		foreach (BaseInventoryStorageComponent compartment : compartments)
		{
			float capacity = compartment.GetMaxVolumeCapacity();
			if (capacity <= 0)
				return "";
			total += capacity;
		}

		return Volume(total);
	}
}
