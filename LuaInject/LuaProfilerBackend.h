#pragma once
#include "Singleton.h"
#include "Client.h"
#include "Protocol.h"


class LuaProfilerBackend 
{
public:
	LuaProfilerBackend();
	~LuaProfilerBackend();
	void AddOneLuaFunc(int CMD, int Line, int SourceId, int FuncId, long long Time, int GC);
	void AddOnePF(LuaPF& onePF);
	void TriggleOneLuaFrame();
	void InitBuff();
	void ConnectClient(int port );
	void SendDataToFrontEnd();
	static LuaProfilerBackend * s_instance;
public:
 
	static LuaProfilerBackend* Get() {
		if(s_instance ==nullptr)
			s_instance = new LuaProfilerBackend();
		return s_instance;
	}

private:
	Client* m_SocketChannel;
	int index;
	char * mBuff;
};

