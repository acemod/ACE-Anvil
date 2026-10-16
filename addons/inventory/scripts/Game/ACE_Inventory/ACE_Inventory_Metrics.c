//------------------------------------------------------------------------------------------------
class ACE_Inventory_Metrics
{
	//------------------------------------------------------------------------------------------------
	protected static bool HasDimensions(vector dimensions)
	{
		return dimensions[0] > 0 && dimensions[1] > 0 && dimensions[2] > 0;
	}

	//------------------------------------------------------------------------------------------------
	protected static vector SortedDimensions(vector dimensions)
	{
		float swap;
		if (dimensions[0] < dimensions[1])
		{
			swap = dimensions[0];
			dimensions[0] = dimensions[1];
			dimensions[1] = swap;
		}
		if (dimensions[1] < dimensions[2])
		{
			swap = dimensions[1];
			dimensions[1] = dimensions[2];
			dimensions[2] = swap;
		}
		if (dimensions[0] < dimensions[1])
		{
			swap = dimensions[0];
			dimensions[0] = dimensions[1];
			dimensions[1] = swap;
		}
		return dimensions;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool LargerDimensions(vector candidate, vector current)
	{
		vector sorted = SortedDimensions(candidate);
		vector bestSorted = SortedDimensions(current);
		for (int axis = 0; axis < 3; axis++)
		{
			if (sorted[axis] != bestSorted[axis])
				return sorted[axis] > bestSorted[axis];
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------
	static string Dimensions(vector dimensions, bool includeUnit = true)
	{
		if (!HasDimensions(dimensions))
			return "";

		string unit;
		if (includeUnit)
			unit = " cm";

		if (dimensions[0] == dimensions[1] && dimensions[1] == dimensions[2])
			return DimensionNumber(dimensions[0]) + "³" + unit;

		return DimensionNumber(dimensions[0]) + "x" + DimensionNumber(dimensions[1]) + "x" + DimensionNumber(dimensions[2]) + unit;
	}

	//------------------------------------------------------------------------------------------------
	protected static string DimensionNumber(float centimetres)
	{
		if (centimetres < 0.001)
			return "<0.001";
		return CompactNumber(centimetres);
	}

	//------------------------------------------------------------------------------------------------
	static string ItemSize(InventoryItemComponent item)
	{
		if (!item)
			return "";

		ItemPhysicalAttributes physical = ItemPhysicalAttributes.Cast(item.FindAttribute(ItemPhysicalAttributes));
		if (!physical)
			return "";

		return Dimensions(physical.GetDimensions());
	}

	//------------------------------------------------------------------------------------------------
	static string AvailableSize(BaseInventoryStorageComponent storage, bool includeLargestVolumePocket = false)
	{
		array<BaseInventoryStorageComponent> compartments = {};
		GetCompartments(storage, compartments);
		vector best;
		vector largestVolumeDimensions;
		float largestRemaining = -1;
		foreach (BaseInventoryStorageComponent compartment : compartments)
		{
			float capacity = compartment.GetMaxVolumeCapacity();
			float used = compartment.GetOccupiedSpace();
			if (capacity <= 0 || used < 0)
				return "";
			if (used >= capacity)
				continue;

			vector dimensions = compartment.GetMaxDimensionCapacity();
			if (!HasDimensions(dimensions))
				return "";

			// Rank real pockets by longest axis, then use the other axes to break ties.
			if (LargerDimensions(dimensions, best))
			{
				best = dimensions;
			}
			if (includeLargestVolumePocket)
			{
				float remaining = capacity - used;
				if (remaining > largestRemaining || (remaining == largestRemaining && LargerDimensions(dimensions, largestVolumeDimensions)))
				{
					largestRemaining = remaining;
					largestVolumeDimensions = dimensions;
				}
			}
		}
		string bestText = Dimensions(best, false);
		if (bestText.IsEmpty())
			return bestText;
		if (includeLargestVolumePocket)
		{
			string largestVolumeText = Dimensions(largestVolumeDimensions, false);
			if (bestText != largestVolumeText)
				bestText += "(" + largestVolumeText + ")";
		}
		return bestText + " cm";
	}

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
		return CompactNumber(litres);
	}

	//------------------------------------------------------------------------------------------------
	protected static string CompactNumber(float value)
	{
		string text = value.ToString(-1, 3);
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

		bool clothing = ClothNodeStorageComponent.Cast(storage) != null;
		// Prefer the bound storage's capacity over auxiliary attachment compartments.
		if (!clothing && storage.GetMaxVolumeCapacity() > 0)
		{
			compartments.Insert(storage);
			return;
		}

		array<BaseInventoryStorageComponent> attached = {};
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
	static string Container(BaseInventoryStorageComponent storage, bool includeLargestVolumePocket = true)
	{
		array<BaseInventoryStorageComponent> compartments = {};
		GetCompartments(storage, compartments);
		if (compartments.IsEmpty())
			return "";

		float remaining;
		float largestRemaining;
		foreach (BaseInventoryStorageComponent compartment : compartments)
		{
			float capacity = compartment.GetMaxVolumeCapacity();
			float used = compartment.GetOccupiedSpace();
			if (capacity <= 0 || used < 0)
				return "";

			float free = Math.Max(0, capacity - used);
			remaining += free;
			largestRemaining = Math.Max(largestRemaining, free);
		}

		string totalText = VolumeNumber(remaining);
		string largestText = VolumeNumber(largestRemaining);
		if (includeLargestVolumePocket && totalText != largestText)
			return totalText + "(" + largestText + ") L";

		return totalText + " L";
	}

	//------------------------------------------------------------------------------------------------
	static int CompartmentCount(BaseInventoryStorageComponent storage, bool availableOnly = false)
	{
		array<BaseInventoryStorageComponent> compartments = {};
		GetCompartments(storage, compartments);
		if (!availableOnly)
			return compartments.Count();

		int available;
		foreach (BaseInventoryStorageComponent compartment : compartments)
		{
			float capacity = compartment.GetMaxVolumeCapacity();
			float used = compartment.GetOccupiedSpace();
			if (capacity <= 0 || used < 0)
				return -1;
			if (used < capacity)
				available++;
		}
		return available;
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
