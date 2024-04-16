//------------------------------------------------------------------------------------------------
class ACE_Testing_Debug_Display : SCR_InfoDisplayExtended {
	protected BaseWorld m_World;
	protected TextWidget m_wHeader;
	protected RichTextWidget m_wText;
	
	protected ref ACE_Testing_extensionRunner m_extRunner;

	protected float m_tick = 0;
	const float tickRate = 0.1;
	
	
	//------------------------------------------------------------------------------------------------
	override void DisplayInit(IEntity owner) {
		super.DisplayInit(owner);
		Print("ACE_Testing_Debug_Display::DisplayInit()");
		m_World = GetGame().GetWorld();
		m_extRunner = new ACE_Testing_extensionRunner();
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner) {
		super.DisplayStartDraw(owner);
		Print("ACE_Testing_Debug_Display::DisplayStartDraw()");
		m_wHeader = TextWidget.Cast(m_wRoot.FindWidget("frame.header"));
		m_wText = RichTextWidget.Cast(m_wRoot.FindWidget("frame.text"));
	}

	//------------------------------------------------------------------------------------------------
	override protected void DisplayUpdate(IEntity owner, float timeSlice) {
		super.DisplayUpdate(owner, timeSlice);
		if (!m_bShown) { return; }

		float worldTime = GetGame().GetWorld().GetWorldTime();
		m_tick += timeSlice;

		if (m_extRunner.readyToSend()) {
			IEntity player = GetGame().GetPlayerController().GetControlledEntity();
			//if (!player) { return };
			vector pos = player.GetOrigin();
						vector m[4];
			GetGame().GetWorld().GetCurrentCamera(m);
			
			m_extRunner.send("x=" + pos[0].ToString() + "&y="+ pos[2].ToString() + "&q=" + m[0][0].ToString());
		}
		

		if (m_tick > tickRate) {
			m_tick -= tickRate;
			m_wHeader.SetText(worldTime.ToString() + " [" + (timeSlice * 1000).ToString() + "ms]");
			
			string output = "request: " + m_extRunner.lastRequest();
			output += "\nresponse: " + m_extRunner.lastData();
			
			array<string> values = {};
			m_extRunner.lastData().Split(",", values, false);
			float a = -1;
			float b = -1;
			float c = -1;
			if (values.IsIndexValid(0)) { a = values[0].ToFloat(); }
			if (values.IsIndexValid(1)) { b = values[1].ToFloat(); }
			if (values.IsIndexValid(2)) { c = values[2].ToFloat(); }
			output += "\nparsed: [" + a + ", " + b + ", " + c + "]";
			
			m_wText.SetText(output);
		}
	}
	
	
}


class ACE_Testing_extensionRunner {
	protected ref ACE_Testing_extensionCallback m_callback = new ACE_Testing_extensionCallback();
	protected string m_lastRequest = "xNever";
	protected string m_lastData = "xNever";

	void checkForNewData() {
		if (m_callback.dataReady()) {
			m_lastData = m_callback.m_data;
			m_callback.reset();
		}
	}
	bool readyToSend() {
		checkForNewData();
		return m_callback.notBusy();
	}
	void send(string input) {
		if (!m_callback.notBusy()) return;
		m_lastRequest = input;
		string contextURL = "http://127.0.0.1:5000/";
		RestContext context = GetGame().GetRestApi().GetContext(contextURL);
		context.GET(m_callback, "test?" + input);
		context = null;
		m_callback.send();
	}
	string lastData() {
		checkForNewData();
		return m_lastData;
	}
	string lastRequest() {
		return m_lastRequest;
	}
}

enum ACE_Testing_extensionCallback_state { READY, SENT, RECIEVED }
class ACE_Testing_extensionCallback : RestCallback {
	string m_data = "";
	ACE_Testing_extensionCallback_state m_state = ACE_Testing_extensionCallback_state.READY;
	float m_debugSendtime = -1;

	void reset() {
		m_data = "";
		m_state = ACE_Testing_extensionCallback_state.READY;
	}
	void send() {
		m_debugSendtime = GetGame().GetWorld().GetWorldTime();
		m_state = ACE_Testing_extensionCallback_state.SENT;
	}
	bool notBusy() { return m_state != ACE_Testing_extensionCallback_state.SENT; }
	bool dataReady() { return m_state == ACE_Testing_extensionCallback_state.RECIEVED; }
	string get() { return m_data; }
	
	override void OnError(int errorCode) {
		Print("ACE_Testing_extensionCallback::OnError()");
		m_state = ACE_Testing_extensionCallback_state.READY;
	}
	override void OnTimeout() {
		Print("ACE_Testing_extensionCallback::OnTimeout()");
		m_state = ACE_Testing_extensionCallback_state.READY;
	}
	override void OnSuccess(string data, int dataSize) {
		// Print("ACE_Testing_extensionCallback::OnSuccess() - data size = " + dataSize + " bytes");
		m_data = data;
		m_state = ACE_Testing_extensionCallback_state.RECIEVED;

		float rTime = GetGame().GetWorld().GetWorldTime();
		//Print("rtt " + (rTime - m_debugSendtime).ToString());
	}
}


