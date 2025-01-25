

#pragma once

#include <dbghelp.h>

class cDumper
{
public:
	enum eLevel	// or ��������.. �ߺ� ���� ����,
    {
		LEVEL_0  = MiniDumpNormal,		// ����� ������ ���ٸ�, LEVEL_0 �� ����ϼ���.
		LEVEL_1  = MiniDumpWithDataSegs,
		LEVEL_2  = MiniDumpWithFullMemory,
/*
		LEVEL_3  = MiniDumpWithHandleData,
		LEVEL_4  = MiniDumpFilterMemory,
		LEVEL_5  = MiniDumpScanMemory,
		LEVEL_6  = MiniDumpWithUnloadedModules,
		LEVEL_7  = MiniDumpWithIndirectlyReferencedMemory,
		LEVEL_8  = MiniDumpFilterModulePaths,
		LEVEL_9  = MiniDumpWithProcessThreadData,
		LEVEL_10 = MiniDumpWithPrivateReadWriteMemory,
		LEVEL_11 = MiniDumpWithoutOptionalData
//*/
    };


private:
//	cDumper(int lv = LEVEL_0 | LEVEL_1 | LEVEL_2);
	cDumper(int lv = LEVEL_0);
   ~cDumper(void);

public:
	HANDLE NewFileHandle(void);
	int GetDumpLevel(void) { return m_DumpLevel; }

private:
    // ���ܿ� ���� ������ �޾Ƽ�, �̴� ���� ������ �����Ѵ�. 
	static LONG WINAPI TopLevelFilter(EXCEPTION_POINTERS* pExPtrs);

    // ������ ���ο� ���� ���ڿ��� ��ȯ�Ѵ�.
	const char *GetFaultReason(EXCEPTION_POINTERS* pExPtrs);


private:
	int m_DumpLevel;
	char m_szAppName[MAX_PATH];	// ���� ���� �̸� ���ڿ�.
	char m_szFaultReason[2048];	// ������ �߻��� ����



public:
	static cDumper &GetInstance(void)
	{
		static cDumper s_Dumper;
		return s_Dumper;
	}
};

#define g_Dumper cDumper::GetInstance()




