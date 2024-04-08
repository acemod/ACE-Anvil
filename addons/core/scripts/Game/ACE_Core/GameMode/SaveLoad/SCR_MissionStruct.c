
[BaseContainerProps()]
modded class SCR_MissionStruct: SCR_JsonApiStruct 
{
	protected ref SCR_JsonApiStruct m_ACE_EditorStruct;
	
	void SCR_MissionStruct()
	{
		RegV("m_ACE_EditorStruct");
		
		if (!m_ACE_EditorStruct)
			m_ACE_EditorStruct = new ACE_EditorStruct();
		
		m_aStructs.Insert(m_ACE_EditorStruct);
		m_aStructTypes.Insert(m_ACE_EditorStruct.Type());
	}
}