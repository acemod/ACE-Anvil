
[BaseContainerProps()]
modded class SCR_MissionStruct: SCR_JsonApiStruct 
{
	protected ref SCR_JsonApiStruct m_ACE_EditorStruct;
	
	void SCR_MissionStruct()
	{		
		m_ACE_EditorStruct = new ACE_EditorStruct();
		
		m_aStructs.Insert(m_ACE_EditorStruct);
		m_aStructTypes.Insert(m_ACE_EditorStruct.Type());
		
		RegV("m_ACE_EditorStruct");
	}
}