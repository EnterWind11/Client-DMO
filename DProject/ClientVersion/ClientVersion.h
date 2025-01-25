
// Construa após a atualização do SVN através do Jenkins e configure o número da revisão do Build.
// o arquivo clientversion.h é usado para não construído com Jenkins
// Apague o conteúdo do arquivo clientversion.h ao criar com Jenkins
// coloque -o em clientversion.h depois de definir o Rebby do conteúdo aqui.

#ifdef VERSION_USA
	#define COUNTRY_CODE		1	// Us (GSP/Steam) Código Nacional 34936
#else 
	#define COUNTRY_CODE		82	// Código nacional coreano
#endif

#ifdef VERSION_STEAM
#define CHANNELING_TYPE		1		// canalização tipo 0: padrão, 1: vapor
#else
#define CHANNELING_TYPE		0		// canalização tipo 0: padrão, 1: vapor
#endif

#ifdef _GIVE
	#ifdef VERSION_QA
		#define SERVICES_TYPE		1	// 0: teste, 1: QA, 2: ao vivo
	#else
		#define SERVICES_TYPE		2	// 0: teste, 1: QA, 2: ao vivo
	#endif
#else
	#define SERVICES_TYPE		0		// 0: teste, 1: QA, 2: ao vivo
#endif

#define SVN_REVNUMBER		34936		// Construa o número da revisão SVN

