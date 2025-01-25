#pragma once

#include "nScope.h"

namespace pLogin
{
	enum
	{
		Begin = nScope::Login,

		Request,                // Request login
		Success,                // Login successful
		Failure,                // Login failed

		OnTheAnvil,             // Under preparation

		WaitTime,               // Waiting time for login processing

		FailureGSP,             // GSP: Authentication failure
		RequestAeriaChannel,    // Aeria channeling request
		AccountBan,             // Send account ban information
		Birthday,               // Send birthdate for Korea's shutdown regulation

		End
	};

	enum
	{
		ErrID,                  // Error in ID
		ErrPass,                // Error in password

		Online,                 // Already in use
		Waiting,                // Processing authentication
		Blocking,               // Account is suspended

		ErrAge,                 // Under 12 years old

		NotOpen,                // Server is not open yet
		Ban,                    // Access restricted

		HttpError,              // GSP: Authentication server error
		AccoutDBError,          // GSP: User Agreement DB error
		NotAgreement,           // GSP: User agreement not accepted
		IPBlock,                // GSP: IP blocked
		PlayTimeOut,            // Playtime expired
		PopErrMsg,              // Pop-up error message

		SystemError,            // System error
		UNKNOWN_NICK,           // Unknown nickname
		UNKNOWN_EMAIL,          // Unknown email address

		ErrLoginPass,           // ID/password incorrect (specific to North America)
		EMPTY_TOKEN,            // Empty token
		UNKNOWN_TOKEN,          // Unknown token
		UNKNOWN_TOKENTIME,      // Token time unknown
	};
};
