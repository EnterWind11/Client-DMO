#pragma once

#include "nScope.h"

namespace pSvr
{
	enum
	{
		Begin = nScope::Svr,

		ClusterList,           // Cluster list
		SelectCluster,         // Select a cluster

		Change,                // Change Game server
		GoBackGate,            // Return from game server to gate server
		GoBackAccount,         // Return from gate server to account server

		AccessCode,            // AccessCode is generated upon connecting to the account server
		// This value is used for authentication on the core server

		KillGate,              // Terminate gate server
		KillSession,           // Terminate session by UserIDX

		SelectPortal,          // Request and success for portal selection
		SelectPortalFailure,   // Portal selection failure

		LocalPortal,           // Local portal success

		TryLogin,              // If already logged in, a second login attempt will request the core server
		// to disconnect the existing session

		ChannelInfo,           // Send channel list and channel status

		Pause,                 // Temporarily pause the server
		Resume,                // Resume server operations

		PrimiumStart,          // PC bang (cyber cafe) service info
		AlramExpire,           // Alert 5 minutes before PC bang service ends
		TimeExpire,            // End of PC bang service
		PrimiumChange,         // Change PC bang service tier
		SelectCharacter,       // Select a character to start the game

		KoreaNumberCheck,      // Verify Korean identification number

		SMS,                   // SMS service

		MoveInstGame,          // Move to an instance game server

		TEST_GameServerLog,    // Test game server logging
		ChangeVersionCheck,    // Version check for updates
		PCBangServerDown,      // Used to remove PC bang effects for all users when the PC bang support server goes down

		SkinUser,              // Skin-related user operation
		KillSession2,          // Terminate session by TamerIDX

		EncryptionControl,     // Control encryption operations

		TimeServerUserKick,    // Kick users due to time-restricted map closure
		TimeMapClose,          // Close a time-restricted map
		TimeMapOpen,           // Open a time-restricted map

		ResourceHashReload,    // Reload resource hash

		End
	};
};
