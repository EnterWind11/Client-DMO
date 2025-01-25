namespace eLOGINRESULT
{
	const int SUCCESS = 0;

	const int SERVER_ERROR = 10034; // Server error
	const int AUTH_FAILED = 10035; // Authentication failed
	const int SERVER_NOT_OPEN = 10015; // Account server is not open
	const int ACCOUNT_BANNED = 10052; // Account is banned
	const int ALREADY_LOGIN = 10014; // Already logged in
	const int IPBLOCKED = 10036; // IP blocked
	const int MACBLOCKED = 10037; // MAC blocked
	const int VIRTUALMACHINE = 10038; // Access from virtual machine
	const int NO_AVAILABE_CORE = 10039; // No available core server
	const int VERSION_NOT_MATCH = 19999; // Version mismatch
	const int PASS_NOT_MATCH = 20052; // Second password incorrect

	const int HTTP_ERROR = 10054; // HTTP authentication error
	const int NOT_AGREEMENT = 10055; // No user agreement
	const int BLOCKING = 10056; // Account blocked
	const int ERROR_ID = 10058; // Error during authentication
	const int ERROR_LOGINPASS = 10057; // Account credentials do not match

	const int NOT_AGREEMENT_EMAIL = 10059; // No email agreement
	const int NOT_AGREEMENT_USER = 10060; // No user agreement
	const int NOT_AGREEMENT_GAME = 10061; // No game agreement

	const int SERVER_IS_MAINTENANCE = 10011; // Server under maintenance
	const int SERVER_CONNECT_USER_FULL = 10012; // Server is full
	const int SERVER_IS_NOT_READY = 10015; // Server not ready

	const int RELOCATE_BANNED = 10120; // Previously used ID is still in progress
}

enum eRESULT
{
	SUCCESS = 0,
	FAILED = 1,
};
