 
#include "LuaProfilerBackend.h"
#include "Protocol.h"

LuaProfilerBackend *  LuaProfilerBackend::s_instance = nullptr;
static bool   decodePDULength(char* data, int size, int& len)
{

	if (size < 4)
		return false;
	else
	{
		len = (int)*data;
		return size>=len;
	}
	return false;

}

static void  RecvMsg(int ChannalId,char*)
{

}

LuaProfilerBackend::LuaProfilerBackend()
{
	m_SocketChannel = new Client();
	mBuff = new char[MAXBUFF];
	InitBuff();
}

void LuaProfilerBackend::InitBuff()
{
	index = sizeof(int);
 
}

void LuaProfilerBackend::ConnectClient(int port)
{
	m_SocketChannel->Connect("127.0.0.1",port, decodePDULength, RecvMsg);
}

LuaProfilerBackend::~LuaProfilerBackend()
{
	if (m_SocketChannel)
	{
		delete m_SocketChannel;
	}

	if (mBuff)
	{
		delete mBuff;
	}
}

 
void LuaProfilerBackend::AddOnePF(LuaPF&  onePF)
{
	LuaPF* CurPos = (LuaPF*)(mBuff + index);
	CurPos->CMD = onePF.CMD;
	CurPos->Line = onePF.Line;
	CurPos->FileNameID = onePF.FileNameID;
	CurPos->FuncId = onePF.FuncId;
	CurPos->CurrentMem = onePF.CurrentMem;
	CurPos->CureentTime = onePF.CureentTime;
	index += sizeof(LuaPF);
 
}


void LuaProfilerBackend::AddOneLuaFunc(int CMD,int Line, int FileNameID, int FuncId, long long Time, int CurrentMem)
{

	LuaPF* CurPos = (LuaPF*)(mBuff + index);
	CurPos->CMD         = CMD;
	CurPos->Line        = Line;
	CurPos->FileNameID  = FileNameID;
	CurPos->FuncId      = FuncId;
	CurPos->CurrentMem = CurrentMem;
	CurPos->CureentTime = Time;
	index += sizeof(LuaPF);

}
 
void LuaProfilerBackend::SendDataToFrontEnd()
{
	int * Len = (int *)mBuff;
	*Len = index;
	m_SocketChannel->Send(mBuff, index);
}


void LuaProfilerBackend::TriggleOneLuaFrame()
{
	SendDataToFrontEnd();
	InitBuff();
}


