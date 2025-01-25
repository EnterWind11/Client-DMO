#ifndef _ANTICPX_SERVER_H_
#define _ANTICPX_SERVER_H_

// 유효하지 않은 핸들을 의미한다
#define ANTICPX_INVALID_HANDLE_VALUE		(0)
#define ANTICPXSVR_H_VERSION				01003201

//////////////////////////////////////////////////////////////////////////
// 핵쉴드 동작 상태 확장기능 세부 에러 코드
//////////////////////////////////////////////////////////////////////////
#define MAX_ANTICPXSVR_STATUSEX_NUM									2 // 기능 갯수
// 목적지 포트 검사에 실패 하였습니다. 
#define	ERROR_ANTICPXSVR_STATUSEX_CHECK_REMOTE_PORT_FAIL 			0x1
// StandAlone Check 기능 검사에 실패 하였습니다. 
#define	ERROR_ANTICPXSVR_STATUSEX_CHECK_STANDALONE_FAIL 			0x2


//////////////////////////////////////////////////////////////////////////
// 에러 코드
//////////////////////////////////////////////////////////////////////////

#define ANTICPXSVR_BASECODE_ERROR							0xE9040000

// 파일에 접근 권한이 없습니다
#define ERROR_ANTICPXSVR_FILE_ACCESS_DENIED					(ANTICPXSVR_BASECODE_ERROR + 0x1)
// 파일을 찾을 수가 없습니다
#define ERROR_ANTICPXSVR_FILE_NOT_FOUND						(ANTICPXSVR_BASECODE_ERROR + 0x2)
// 입력값이 올바르지 않습니다
#define ERROR_ANTICPXSVR_INVALID_PARAMETER					(ANTICPXSVR_BASECODE_ERROR + 0x3)
// 올바른 포멧이 아닙니다
#define ERROR_ANTICPXSVR_BAD_FORMAT							(ANTICPXSVR_BASECODE_ERROR + 0x4)
// 요청에 대한 응답을 받지 않았습니다
#define ERROR_ANTICPXSVR_NOT_YET_RECEIVED_RESPONSE			(ANTICPXSVR_BASECODE_ERROR + 0x5)
// 대기하고 있지 않습니다
#define ERROR_ANTICPXSVR_NO_WAITING							(ANTICPXSVR_BASECODE_ERROR + 0x6)
// 메모리가 부족합니다
#define ERROR_ANTICPXSVR_NOT_ENOUGH_MEMORY					(ANTICPXSVR_BASECODE_ERROR + 0x7)
// 송수신된 메시지가 올바르지 않습니다
#define ERROR_ANTICPXSVR_BAD_MESSAGE						(ANTICPXSVR_BASECODE_ERROR + 0x8)
// 패킷 분석을 위한 재전송 공격이 감지되었습니다
#define ERROR_ANTICPXSVR_REPLY_ATTACK						(ANTICPXSVR_BASECODE_ERROR + 0x9)
// 핵쉴드 모듈 변조가 감지되었습니다
#define ERROR_ANTICPXSVR_HSHIELD_FILE_ATTACK				(ANTICPXSVR_BASECODE_ERROR + 0xA)
// 클라이언트 파일 변조가 감지되었습니다
#define ERROR_ANTICPXSVR_CLIENT_FILE_ATTACK					(ANTICPXSVR_BASECODE_ERROR + 0xB)
// 메모리 변조가 감지되었습니다
#define ERROR_ANTICPXSVR_MEMORY_ATTACK						(ANTICPXSVR_BASECODE_ERROR + 0xC)
// 구 버전 클라이언트 지원이 중지되었습니다
// (서버 중지 없이 HSB 파일을 업데이트하였을 경우 HSB 정책에 구 버전 클라이언트를 지원하지 않도록
// 설정되어 있을 경우 이 오류가 발생할 수 있다. 정책에 의한 것이므로 명백한 오류는 아니다.)
#define ERROR_ANTICPXSVR_OLD_VERSION_CLIENT_EXPIRED			(ANTICPXSVR_BASECODE_ERROR + 0xD)
// HSB 파일을 생성할 때 지정한 핵쉴드 모듈과 짝이 맞지 않습니다
#define ERROR_ANTICPXSVR_UNKNOWN_CLIENT						(ANTICPXSVR_BASECODE_ERROR + 0xE)
// V3 Small Engine 파일의 변조가 감지되었습니다.
#define ERROR_ANTICPXSVR_V3SENGINE_FILE_ATTACK				(ANTICPXSVR_BASECODE_ERROR + 0xF)
// Nano Engine 파일의 변조가 감지되었습니다.
#define ERROR_ANTICPXSVR_NANOENGINE_FILE_ATTACK				(ANTICPXSVR_BASECODE_ERROR + 0x10)
// HackShield Version이 지원되지 않는다.
#define ERROR_ANTICPXSVR_INVALID_HACKSHIELD_VERSION			(ANTICPXSVR_BASECODE_ERROR + 0x11)
// Engine Version이 지원되지 않는다.
#define ERROR_ANTICPXSVR_INVALID_ENGINE_VERSION				(ANTICPXSVR_BASECODE_ERROR + 0x12)
// 서버 오브젝트 생성시 예외발생.
#define ERROR_ANTICPXSVR_CREATE_SVROBJ_EXCEPTION			(ANTICPXSVR_BASECODE_ERROR + 0x13)
// 요구메세지 생성시 예외발생.
#define ERROR_ANTICPXSVR_MAKEREQ_EXCEPTION					(ANTICPXSVR_BASECODE_ERROR + 0x14)
// 응답메세지 확인시 예외발생.
#define ERROR_ANTICPXSVR_VERIFY_EXCEPTION					(ANTICPXSVR_BASECODE_ERROR + 0x15)
//Tracing Information 작성 중에 예외발생.
#define ERROR_ANTICPXSVR_TRACE_EXCEPTION					(ANTICPXSVR_BASECODE_ERROR + 0x16)
//CreateClientObject 작성 중에 예외 발생.
#define ERROR_ANTICPXSVR_CREATE_CLIENT_OBJECT_EXCEPTION		(ANTICPXSVR_BASECODE_ERROR + 0x17)
//핵쉴드 동작 상태가 정상적이지 않습니다.
#define ERROR_ANTICPXSVR_ABNORMAL_HACKSHIELD_STATUS			(ANTICPXSVR_BASECODE_ERROR + 0x18)
//핵쉴드 동작 상태가 정상적이지 않습니다.
#define ERROR_ANTICPXSVR_DETECT_CALLBACK_IS_NOTIFIED		(ANTICPXSVR_BASECODE_ERROR + 0x19)
// 응답메세지_EX 확인시 예외발생.
#define ERROR_ANTICPXSVR_VERIFYEX_EXCEPTION					(ANTICPXSVR_BASECODE_ERROR + 0x1A)
// 핵쉴드 동작 상태 강화 검사 정상적이지 않습니다.
#define ERROR_ANTICPXSVR_ABNORMAL_HACKSHIELD_XSTATUS		(ANTICPXSVR_BASECODE_ERROR + 0x1B)
// 서버연동 강화 기능이 적용되지 않는 구 버전의 클라이언트가 접속하였습니다.
#define ERROR_ANTICPXSVR_OLD_HACKSHIELD_VERSION				(ANTICPXSVR_BASECODE_ERROR + 0x1C)
// HackShield Initialize Option 변조가 감지되었습니다.
#define ERROR_ANTICPXSVR_HACKSHIELD_OPTION_ATTACK			(ANTICPXSVR_BASECODE_ERROR + 0x1D)
// 핵쉴드 동작 상태 확장기능 검사결과가 정상적이지 않습니다.
#define ERROR_ANTICPXSVR_STATUSEX_ABNORMAL					(ANTICPXSVR_BASECODE_ERROR + 0x1E)
// 해킹툴 제작자로 의심됩니다.
#define ERROR_ANTICPXSVR_STATUSEX_SUSPICIOUS				(ANTICPXSVR_BASECODE_ERROR + 0x1F)
// .env File Version이 지원되지 않는다.
#define ERROR_ANTICPXSVR_INVALID_ENVFILE_VERSION			(ANTICPXSVR_BASECODE_ERROR + 0x20)
// .env File의 변조가 감지되었습니다.
#define ERROR_ANTICPXSVR_HSENV_FILE_ATTACK					(ANTICPXSVR_BASECODE_ERROR + 0x21)
// HEI 버전이 지원되지 않는다.
#define ERROR_ANTICPXSVR_INVALID_HEI_VERSION				(ANTICPXSVR_BASECODE_ERROR + 0x22)
// HEI 변조가 감지되었습니다.
#define ERROR_ANTICPXSVR_HEI_ATTACK							(ANTICPXSVR_BASECODE_ERROR + 0x23)
// 핵쉴드 주요 쓰레드가 동작하지 않음이 감지되었습니다.
#define ERROR_ANTICPXSVR_HS_THREAD_IS_NOT_RUNNING			(ANTICPXSVR_BASECODE_ERROR + 0x24)
// 정의되지 않은 에러
#define ERROR_ANTICPXSVR_UNKNOWN							(ANTICPXSVR_BASECODE_ERROR + 0xFF)

#if defined (WIN32) || defined(WIN64)
#ifdef AHNHS_EXPORTS
#define AHNHS_API  __declspec(dllexport)
#else
#define AHNHS_API  __declspec(dllimport)	
#endif
#else
#define IN
#define OUT
#define __stdcall
#define AHNHS_API
//typedef unsigned long * ULONG_PTR;
#endif


#if defined (WIN32) || defined(WIN64)
typedef PVOID AHNHS_SERVER_HANDLE;
typedef PVOID AHNHS_CLIENT_HANDLE;
#else
typedef void * AHNHS_SERVER_HANDLE;
typedef void * AHNHS_CLIENT_HANDLE;
#endif

#ifndef __ANTICPX_TRANS_BUFFER__
#define __ANTICPX_TRANS_BUFFER__

#define ANTICPX_TRANS_BUFFER_MAX			600

#define ANTICPX_RECOMMAND_CLOSE_SESSION		101
#define ANTICPX_RECOMMAND_KEEP_SESSION		102

// Client-side CallBack Code
#define AHNHS_ACTAPC_DETECT_UNSUPPORTED_PLATFORM	0x010103		// 지원하지 않는 플랫폼입니다.
#define AHNHS_ACTAPC_DETECT_AUTOMACRO				0x010104		// 오토메크로 행위가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_HARDWARE_AUTOMACRO		0x010106		// 하드웨어 오토매크로 행위가 감지되었습니다.
			
#define AHNHS_ACTAPC_DETECT_HOOKFUNCTION			0x010301		// 보호 API에 대한 후킹 행위가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_DRIVERFAILED			0x010302		// 해킹 차단 드라이버가 로드되지 않았습니다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK				0x010303		// 스피드핵류의 프로그램에 의해 시스템 시간이 변경되었습니다.
#define AHNHS_ACTAPC_DETECT_KDTRACE					0x010307		// 디버거 트래이싱이 발생했다.(커널 디버거 활성화, 이후 브레이크 포인터 처리)

#define AHNHS_ENGINE_DETECT_GAME_HACK				0x010501		// 게임 해킹툴의 실행이 발견되었습니다.
#define AHNHS_ENGINE_DETECT_WINDOWED_HACK			0x010503		// 창모드 해킹툴	
#define AHNHS_ACTAPC_DETECT_MULTI_LOADING			0x010504		// 클라이언트 멀티 로딩 감지.

#define AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS	0x010601		// 코드 패치 시도가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_WRITE	0x010602		// 메모리에 Write 시도가 감지되었습니다.

#define AHNHS_ACTAPC_DETECT_NANOENGINEFAILED		0x010702		// 나노엔진이 정상동작하지 않습니다.
#define AHNHS_ACTAPC_DETECT_CODEMISMATCH			0x010703		// HookFreeCode에 대한 변조가 감지 되었습니다.
#define AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP		0x010705		// 보호 모듈에 대해서 메모리 변조가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_LMP_FAILED				0x010706		// 핵쉴드 로컬 메모리 보호 기능이 정상적이지 않습니다.
#define AHNHS_ACTAPC_DETECT_RMEM_MODIFY_FROM_LMP	0x010709		// rdata(.rsrc) section에 VirtualProtect를 통해 메모리에 write하려고 함.
#define AHNHS_ACTAPC_DETECT_HEI_MISMATCH			0x01070A		// HEI 변조가 감지되었습니다

#define AHNHS_ACTAPC_DETECT_ABNORMAL_FUNCTION_CALL	0x010801

#define AHNHS_ACTAPC_DETECT_ANTIFREESERVER			0x010901		// Game이 올바른 서버로 접속을 하지 않았습니다.

#define AHNHS_ACTAPC_DETECT_ABNORMAL_HACKSHIELD_STATUS	0x010A01	// HackShield 동작 상태 확인 결과, 정상 정보가 아님.

#define AHNHS_ACTAPC_DETECT_HS_THREAD_IS_NOT_RUNNING	0x010A03	// 핵쉴드의 주요 쓰레드가 멈춰있는 경우



//[AntiCpXSvr.dll 1.0.0.10 이후 부터 적용]
// 게임사에서 기본 bytealigment를 사용하지 않으면 핵쉴드 서버연동 모듈 적용 후 게임 
// bytealignment가 변경되므로 기존 정보를 스택에 저장.
#pragma pack(push) //기존 alignment 저장
#pragma pack(8)

typedef struct _AHNHS_TRANS_BUFFER
{
	unsigned char byBuffer[ANTICPX_TRANS_BUFFER_MAX/* 송수신 패킷의 최대 크기 */];
	unsigned short nLength;
} AHNHS_TRANS_BUFFER, *PAHNHS_TRANS_BUFFER;

#pragma pack(pop) // 기존 alignment 복구.

#endif // __ANTICPX_TRANS_BUFFER__

typedef struct _AHNHS_SERVER_EXTRAINFO
{
	unsigned int uiServerPort;
	unsigned int uiReserved1;
} AHNHS_SERVER_EXTRAINFO, *PAHNHS_SERVER_EXTRAINFO;

typedef struct _AHNHS_CLIENT_EXTRAINFO
{
	unsigned int uiSessionId;
	unsigned int uiReserved1;
} AHNHS_CLIENT_EXTRAINFO, *PAHNHS_CLIENT_EXTRAINFO;

#if defined(__cplusplus)
extern "C"
{
#endif

// MACRO Version

#if defined (_NO_HACKSHIELD)
	
	#define	AHNHS_NOT_NULL	(1)	// 포인터값을 리턴하는 경우, NOT NULL로 처리

	#define	_AhnHS_CreateServerObject(arg1)		AHNHS_NOT_NULL
	#define	_AhnHS_CreateServerObjectEx(arg1,arg2)		AHNHS_NOT_NULL
	#define	_AhnHS_CloseServerHandle(arg1)		;
	#define	_AhnHS_CreateClientObject(arg1)		AHNHS_NOT_NULL
	#define	_AhnHS_CreateClientObjectEx(arg1,arg2)		AHNHS_NOT_NULL
	#define _AhnHS_CloseClientHandle(arg1)		;
	#define _AhnHS_MakeRequest(arg1,arg2)					ERROR_SUCCESS
	#define _AhnHS_VerifyResponseEx(arg1,arg2,arg3,arg4)	ERROR_SUCCESS
	#define _AhnHS_VerifyResponseEx_WithInfo(arg1,arg2,arg3,arg4,arg5)	ERROR_SUCCESS

#else	//!_NO_HACKSHIELD

/**
	서버 핸들(Server Handle)을 생성한다

	@param
		const char *pszFilePath ; [입력값] HackShield Briefcase (.hsb) 파일 경로

	@retval
		서버 핸들(Server Handle)

	@remarks
		서버핸들을 올바르게 생성하지 못했을 경우 NULL 값을 리턴한다. 보통 HackShield Briefcase
		(.hsb) 파일 경로가 올바르지 않을 경우나 시스템 리소스(메모리)가 부족할 경우 발생한다.

	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
AHNHS_SERVER_HANDLE
__stdcall
_AhnHS_CreateServerObject (
	IN const char *pszFilePath
	);


AHNHS_API
AHNHS_SERVER_HANDLE
__stdcall
_AhnHS_CreateServerObjectEx (
	IN const char *pszFilePath,
	IN AHNHS_SERVER_EXTRAINFO stServerExtraInfo
	);

/**
	서버 핸들(Server Handle)을 닫는다

	@param
		AHNHS_SERVER_HANDLE hServer ; [입력값] 서버 핸들(Server Handle)

	@retval
		없음

	@remarks
		생성한 서버핸들은 서버 프로세스나 서비스 프로세스가 종료할 때 닫아주어야
		서버핸들 생성에 사용되었던 메모리나 시스템 자원을 해제하게된다.

	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
void
__stdcall
_AhnHS_CloseServerHandle (
	IN AHNHS_SERVER_HANDLE hServer
	);


/**
	클라이언트 핸들(Client Handle)을 생성한다

	@param
		AHNHS_SERVER_HANDLE hServer ; [입력값] 서버 핸들(Server Handle)

	@retval
		클라이언트 핸들(Client Handle)

	@remarks
		서버 핸들을 입력 받아 클라이언트 핸들을 생성한다. 클라이언트 핸들은
		클라이언트가 접속할 때 마다 생성하며, 세션이 유지되는 동안 핸들을 닫지 않고
		재사용한다.

	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
AHNHS_CLIENT_HANDLE
__stdcall
_AhnHS_CreateClientObject (
	IN AHNHS_SERVER_HANDLE hServer
	);


/**
	클라이언트 핸들(Client Handle)을 생성한다

	@param
		AHNHS_SERVER_HANDLE hServer					: [입력값] 서버 핸들(Server Handle)
		AHNHS_CLIENT_EXTRAINFO stClientExtraInfo	: [입력값] extrainfo에 대한 정보 

	@retval
		클라이언트 핸들(Client Handle)

	@remarks
		서버 핸들을 입력 받아 클라이언트 핸들을 생성한다. 클라이언트 핸들은
		클라이언트가 접속할 때 마다 생성하며, 세션이 유지되는 동안 핸들을 닫지 않고
		재사용한다.

	@author
		AhnLab, Inc., HackShield Team.

    @see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
AHNHS_CLIENT_HANDLE
__stdcall
_AhnHS_CreateClientObjectEx (
	IN AHNHS_SERVER_HANDLE hServer,
	IN AHNHS_CLIENT_EXTRAINFO stClientExtraInfo
	);


/**
	클라이언트 핸들(Client Handle)을 닫는다

	@param
		AHNHS_CLIENT_HANDLE hClient ; [입력값] 클라이언트 핸들(Client Handle)

	@retval
		없음

	@remarks
		생성한 클라이언트 핸들은 클라이언트 세션이 종료될 때 해제되어야 한다.
		이때 클라이언트 핸들 생성에 사용되었던 메모리나 시스템 자원을 해제하게된다.

	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
void
__stdcall
_AhnHS_CloseClientHandle (
	IN AHNHS_CLIENT_HANDLE hClient
	);


/**
	클라이언트의 해킹 시도를 감지하기 위해 요청(메시지)를 생성한다.

	@param
		AHNHS_CLIENT_HANDLE hClient ; [입력값] 클라이언트 핸들
		PAHNHS_TRANS_BUFFER pRequestBuffer ; [출력값] 보낼 데이터 버퍼/길이

	@retval
		ERROR_ANTICPXSVR_xxx 형식으로 정의되어 있는 에러 코드 

	@remarks
		현재 세션에 맞는 클라이언트 핸들을 입력하여 요청 메시지를 생성한다.
		요청 메시지는 AHNHS_TRANS_BUFFER 구조체 형태로 출력되며, 멤버 변수 값은
		다음과 같다.

		typedef struct _AHNHS_TRANS_BUFFER
		{
			unsigned short nLength;
			unsigned char byBuffer[ANTICPX_TRANS_BUFFER_MAX]; // 송수신 패킷의 최대 버퍼 크기
			
		} AHNHS_TRANS_BUFFER, *PAHNHS_TRANS_BUFFER;

		nLength ; 요청 메시지 생성에 사용된 버퍼 길이
		byBuffer ; 요청 메시지 생성에 사용될 수 있는 최대 바이트 버퍼

		주의 사항!!! byBuffer는 요청 메시지 생성에 사용될 수 있는 최대 버퍼 크기이므로 네트워크로 
		전송할 때 nLength 만큼만 전송해야한다.

		Example:
		
			AHNHS_TRANS_BUFFER stReqTransBuf;

			ulRet = _AhnHS_MakeRequest ( hClient, &stReqTransBuf );

			if ( ulRet != ERROR_SUCCESS )
				return ulRet;

			bytesSent = send( ConnectSocket, stReqTransBuf.byBuffer, stReqTransBuf.nLength, 0 );

			...

	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
unsigned long
__stdcall
_AhnHS_MakeRequest (
	IN AHNHS_CLIENT_HANDLE hClient,
	OUT PAHNHS_TRANS_BUFFER pRequestBuffer
	);


/**
	클라이언트의 응답 메시지를 검증하여 클라이언트 세션 종료여부를 
	리턴한다. 

	@param
		AHNHS_CLIENT_HANDLE hClient ; [입력값] 클라이언트 핸들
		unsigned char *pbyResponse ; 클라이언트로 부터 받은 데이터 버퍼
		unsigned long nResponseLength ; 클라이언트로 부터 받은 데이터 길이
		unsigned long *pnErrorCode	; 오류 코드 
	@retval
		ANTICPX_RECOMMAND_CLOSE_SESSION : 클라이언트 접속 세션 종료 권장
		ANTICPX_RECOMMAND_KEEP_SESSION  : 클라이언트 접속 세션 유지 권장
	@remarks
	
	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
unsigned long
__stdcall
_AhnHS_VerifyResponseEx (
	IN AHNHS_CLIENT_HANDLE hClient,
	IN unsigned char *pbyResponse,
	IN unsigned long nResponseLength,
	OUT unsigned long *pulErrorCode
	);


/**
	클라이언트의 응답 메시지를 검증하여 클라이언트 세션 종료여부를 
	리턴한다. 

	@param
		AHNHS_CLIENT_HANDLE hClient ; [입력값] 클라이언트 핸들
		unsigned char *pbyResponse ; 클라이언트로 부터 받은 데이터 버퍼
		unsigned long nResponseLength ; 클라이언트로 부터 받은 데이터 길이
		unsigned long *pulErrorCode	; 오류 코드 
		unsigned long *pulSpecificErr ; pnErrorCode 오류 값을 보충하는 에러코드 
	@retval
		ANTICPX_RECOMMAND_CLOSE_SESSION : 클라이언트 접속 세션 종료 권장
		ANTICPX_RECOMMAND_KEEP_SESSION  : 클라이언트 접속 세션 유지 권장
	@remarks
	
	@see
		확장 서버연동 Quick Guide를 참고해 주세요.
*/
AHNHS_API
unsigned long
__stdcall
_AhnHS_VerifyResponseEx_WithInfo(
	IN AHNHS_CLIENT_HANDLE hClient,
	IN unsigned char *pbyResponse,
	IN unsigned long nResponseLength,
	OUT unsigned long *pulErrorCode,
	OUT unsigned long *pulSpecificError
	);


#endif	// _NO_HACKSHIELD

#if defined(__cplusplus)
}
#endif

#endif // _ANTICPX_SERVER_H_
