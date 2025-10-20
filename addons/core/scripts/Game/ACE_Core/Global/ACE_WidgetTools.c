//------------------------------------------------------------------------------------------------
class ACE_WidgetTools
{
	//------------------------------------------------------------------------------------------------
	static void SetDirectChildrenVisible(Widget root, bool show)
	{
		Widget child = root.GetChildren();
		while (child)
		{
			child.SetVisible(show);
			child = child.GetSibling();
		}
	}
}
