#pragma once
#include "..\nScope.h"

// Tamer server transfer (relocation) packets
namespace pRelocateTamer
{
	enum
	{
		Begin = nScope::Relocate,

		CheckTamerName,
		RelocateTamer,

		/* Append - Rebuild */

		//------------------------------------------------------------
		// Broker Server -> Core Server: Login request
		//
		// {
		//     WORD   Type            // Simple request
		// }
		//
		//------------------------------------------------------------
		dfREQ_SERVER_LOGIN,    // Request packet for core server connection matching

		//------------------------------------------------------------
		// Core Server -> Broker Server: Login response
		//
		// {
		//     WORD Type
		//     int  iServerNO;   // Responding server cluster number
		// }
		//
		//------------------------------------------------------------
		dfRES_SERVER_LOGIN,    // Response packet for core server connection matching


		//------------------------------------------------------------
		// Core Server -> Broker Server: Tamer name duplication check request
		//
		// {
		//     WORD   Type
		//     uint   iAccountIDX;        // User’s web account index
		//     int    iServerNO;          // Server number to check for duplication
		//     char   szCheckName[32];    // Tamer name to check for duplication
		// }
		//
		//------------------------------------------------------------
		dfREQ_CHECK_NAME,      // Tamer name duplication check

		//------------------------------------------------------------
		// Broker Server -> Core Server: Tamer name duplication check request
		//
		// {
		//     WORD Type
		//     int  iOldServer;           // Previous server index
		//     uint iAccountID;           // User’s web account index
		//     char szCheckName[32];      // Tamer name to check for duplication
		// }
		//
		//------------------------------------------------------------
		// dfREQ_CHECK_NAME,      // (Shared protocol) Tamer name duplication check

		//------------------------------------------------------------
		// Core Server -> Broker Server: Tamer name duplication check response
		//
		// {
		//     WORD Type
		//     int  iOldServer;   // Previous server index
		//     uint iAccountID;   // User’s web account index
		//     int  iResult;      // Result
		// }
		//
		//------------------------------------------------------------
		dfRES_CHECK_NAME,      // Tamer name duplication check result

		//------------------------------------------------------------
		// Broker Server -> Core Server: Tamer name duplication check response
		//
		// {
		//     WORD Type
		//     uint iAccountID;   // User’s web account index
		//     int  iResult;      // Result
		// }
		//
		//------------------------------------------------------------
		// dfRES_CHECK_NAME,      // (Shared protocol) Tamer name duplication check result


		//------------------------------------------------------------
		// Core Server -> Broker Server: Tamer creation request
		//
		// {
		//     WORD   Type
		//     uint   iAccountID;     // User’s web account index
		//     int    iOldServer;     // Previous server index
		//     int    iNewServer;     // New server index
		//     __int64 iOldTamerIDX;  // Old tamer index
		//     char   szName[32];     // Requested new tamer name
		// }
		//
		//------------------------------------------------------------
		dfREQ_CREATE_TAMER,    // Tamer creation request

		//------------------------------------------------------------
		// Broker Server -> Core Server: Tamer creation request
		//
		// {
		//     WORD   Type
		//     int    iOldServerID;   // Old server number
		//     uint   iAccountID;     // User’s web account index
		//     __int64 iSessionID;    // Internal session ID
		//     char   szName[32];     // Requested new tamer name
		// }
		//
		//------------------------------------------------------------
		// dfREQ_CREATE_TAMER,    // (Shared protocol) Tamer creation request

		//------------------------------------------------------------
		// Core Server -> Broker Server: Tamer creation response
		//
		// {
		//     WORD   Type
		//     int    iOldServerID;   // Old server number
		//     __int64 iSessionID;    // Internal session ID
		//     int    iResult;        // Result code
		//     __int64 iNewTamerIDX;  // New tamer index
		// }
		//
		//------------------------------------------------------------
		dfRES_CREATE_TAMER,    // Tamer creation result

		//------------------------------------------------------------
		// Broker Server -> Core Server: Tamer creation response
		//
		// {
		//     WORD Type
		//     uint iAccountID;   // User’s web account index
		//     int  iResult;      // Result code
		// }
		//
		//------------------------------------------------------------
		// dfRES_CREATE_TAMER,    // (Shared protocol) Tamer creation result


		//------------------------------------------------------------
		// Request to update tamer information
		//
		// {
		//     WORD                  Type;
		//     __int64               iSessionID;  // Internal session ID
		//     stRelocateTamerInfo*  pTamer;      // Complete tamer info
		// }
		//
		//------------------------------------------------------------
		dfREQ_UPDATE_TAMER,

		//------------------------------------------------------------
		// Response to updating tamer information
		//
		// {
		//     WORD   Type;
		//     __int64 iSessionID;    // Internal session ID
		//     int    iResult;        // Ignored unless there's an error, which stops further progress
		// }
		//
		//------------------------------------------------------------
		dfRES_UPDATE_TAMER,

		//------------------------------------------------------------
		// Digimon creation request (list-based)
		//
		// {
		//     WORD       Type;
		//     __int64    iSessionID;      // Internal session ID
		//     __int64    iNewTamerIDX;    // Newly created tamer index
		//     int        iCount;          // Number of Digimon to create
		//     stDIGIMONLIST stDigimon[iCount]; // Array of Digimon data
		// }
		//
		//------------------------------------------------------------
		dfREQ_CREATE_DIGIMON,   // Digimon creation request

		//------------------------------------------------------------
		// Digimon creation response (list-based)
		//
		// {
		//     WORD       Type;
		//     int        iResult;            // Response code (ignore further data if fail)
		//     __int64    iSessionID;         // Internal session ID
		//     int        iCount;             // Number of Digimon created
		//     stIDX_RES  stDigimon[iCount];  // Array of result data
		// }
		//
		// struct stIDX_RES
		// {
		//     __int64 iIdx;     // Broker server table index
		//     __int64 iNewIDX;  // Newly created Digimon or item index
		// };
		//
		//------------------------------------------------------------
		dfRES_CREATE_DIGIMON,   // Digimon creation result

		//------------------------------------------------------------
		// Item creation request (list-based)
		//
		// {
		//     WORD       Type;
		//     __int64    iSessionID;        // Internal session ID
		//     __int64    iNewTamerIDX;      // Newly created tamer index
		//     int        iCount;            // Number of items to create
		//     stITEMLIST stItem[iCount];    // Array of item data
		// }
		//
		//------------------------------------------------------------
		dfREQ_CREATE_ITEM,      // Item creation request

		//------------------------------------------------------------
		// Item creation response (list-based)
		//
		// {
		//     WORD       Type;
		//     int        iResult;            // Response code (ignore further data if fail)
		//     __int64    iSessionID;         // Internal session ID
		//     int        iCount;             // Number of items created
		//     stIDX_RES  stItemList[iCount]; // Array of result data
		// }
		//
		//------------------------------------------------------------
		dfRES_CREATE_ITEM,      // Item creation result

		//------------------------------------------------------------
		// Seal creation request (list-based)
		//
		// {
		//     WORD       Type;
		//     __int64    iSessionID;       // Internal session ID
		//     __int64    iNewTamerIDX;     // Newly created tamer index
		//     int        iCount;           // Number of seals to create
		//     stSEALLIST stSeal[iCount];   // Array of seal data
		// }
		//
		//------------------------------------------------------------
		dfREQ_CREATE_SEAL,      // Seal master creation request

		//------------------------------------------------------------
		// Seal creation response (list-based)
		//
		// {
		//     WORD       Type;
		//     int        iResult;             // Response code (ignore further data if fail)
		//     __int64    iSessionID;          // Internal session ID
		//     int        iCount;              // Number of seals created
		//     stIDX_RES  stSealList[iCount];  // Array of result data
		// }
		//
		//------------------------------------------------------------
		dfRES_CREATE_SEAL,      // Seal master creation result

		//------------------------------------------------------------
		// Incubator creation request (list-based)
		//
		// {
		//     WORD       Type;
		//     __int64    iSessionID;       // Internal session ID
		//     __int64    iNewTamerIDX;     // Newly created tamer index
		//     int        iCount;           // Number of incubators to create
		//     stINCULIST stIncList[iCount];// Array of incubator data
		// }
		//
		//------------------------------------------------------------
		dfREQ_CREATE_INCU,      // Incubator creation request

		//------------------------------------------------------------
		// Incubator creation response (list-based)
		//
		// {
		//     WORD       Type;
		//     int        iResult;             // Response code (ignore further data if fail)
		//     __int64    iSessionID;          // Internal session ID
		//     int        iCount;              // Number of incubators created
		//     stIDX_RES  stIncList[iCount];   // Array of result data
		// }
		//
		//------------------------------------------------------------
		dfRES_CREATE_INCU,      // Incubator creation result

		//------------------------------------------------------------
		// Evolution information creation request (list-based)
		//
		// {
		//     WORD       Type;
		//     __int64    iSessionID;       // Internal session ID
		//     __int64    iNewTamerIDX;     // Newly created tamer index
		//     int        iCount;           // Number of Digimon to set evolution info for
		//     stEVOLIST  stEvoList[iCount];// Array of evolution data
		// }
		//
		//------------------------------------------------------------
		dfREQ_CREATE_EVO,       // Request to create evolution (evolution info)

		//------------------------------------------------------------
		// Evolution information creation response (list-based)
		//
		// {
		//     WORD       Type;
		//     int        iResult;             // Response code (ignore further data if fail)
		//     __int64    iSessionID;          // Internal session ID
		//     int        iCount;              // Number of Digimon for which evolution info was created
		//     stIDX_RES  stEvoList[iCount];   // Array of result data
		// }
		//
		//------------------------------------------------------------
		dfRES_CREATE_EVO,       // Result of creating evolution (evolution info)

		//------------------------------------------------------------
		// Tamer transfer completed (send the tamer index to update the consignment store)
		//
		// {
		//     WORD    Type;
		//     __int64 iTamerIDX;   // Tamer index
		// }
		//
		//------------------------------------------------------------
		dfREQ_SUCCESS_RELOCATE,  // Sent to the Core when Tamer relocation is finished
		dfRES_SUCCESS_RELOCATE,  // Once receiving the relocation completion response, copy to History

		/* End append */

		End
	};

	enum eTAMER_RELOCATE_STATE
	{
		eRELOCATE_TAMER_NONE = 0,
		eRELOCATE_TAMER_COPY = 1
	};
}
