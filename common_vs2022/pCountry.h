#pragma once

#define LANGUAGE_SUPPORT

//Veja outro código nacional iso_3166-1_alpha-2
#define DEFAULT_COUNTRY_CODE			"ZZ"

namespace Language
{

namespace pCountry
{

#ifdef VERSION_HK

	const int CHARACTER_SET = 65001;

	enum eSize	// Tamanho, esquema DB, buffer de cordas
	{
		SizeID				= 21,
		SizePass			= 120,
		SizeAccessToken		= 255,
		SizeCharName		= 42,
		SizeGuildName		= 50,
		SizeGuildNotice		= 255,
		SizeGuildClassName	= 32,
		SizeGuildClassNameDesc = 72,
		SizeGuildMemo		= 64,
		SizeShopName		= 128,
		SizeBuddyMemo		= 255,
#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
		SizeInvenMax		= 150, //Inven, tamanho do armazém 255 limite
#else
		SizeInvenMax		= 84,
#endif
		SizeSecondPass		= 33,
		SizeFirstInven      = 21,
		SizeFirstWarehouse  = 21,
		SizeFirstShareStash = 14,
		SizeDigimonBaseSlot = 4,
	};

	enum eMinLen	// Carta gil agora
	{
		MIN_LenID				= 2,
		MIN_LenPass				= 2,
		MIN_LenCharName			= 2,
		MIN_LenGuildName		= 2,
		MIN_LenGuildClassName	= 2,
		MIN_LenGuildClassNameDesc = 2,
		MIN_LenGuildNotice		= 0,
		MIN_LenGuildMemo		= 0,
		MIN_LenShopName			= 1,
		MIN_LenBuddyMemo		= 2,
		MIN_LenMsg				= 1,
	};

	enum eMaxLen	// Letra gil agora, o comprimento real da string
	{
		MAX_LenID				= 14,
		MAX_LenPass				= 14,
		MAX_LenCharName			= 8,
		MAX_LenGuildName		= 14,
		MAX_LenGuildClassName	= 8,
		MAX_LenGuildClassNameDesc = 21,	
		MAX_LenGuildNotice		= 60,
		MAX_LenGuildMemo		= 21,		// No caso de 31 letras, é maior que o tamanho do buffer, por isso é limitado a 21 caracteres.
		MAX_LenShopName			= 15,
		MAX_LenBuddyMemo		= 23,
		MAX_LenMsg				= 250,
	};

	enum eData
	{
		TamerLevelMax		= 99,
		DigimonLevelMax		= 99,		
		EvoStep				= 9,
		MaxAttrInchantLevel = 15,
		MaxInchantLevel =	60,
		MAX_CHANNELING		= 5,
		MAX_ItemSkillDigimon = 2,
		DropItem			= 35,
		MAX_DIGIMONSKILLLevel	= 15,
	};

#elif defined VERSION_TW

	const int CHARACTER_SET = 65001;

	enum eSize	// Tamanho, esquema de banco de dados, buffer
	{
		SizeID				= 21,
		SizePass			= 120,
		SizeAccessToken		= 255,
		SizeCharName		= 42,
		SizeGuildName		= 50,
		SizeGuildNotice		= 255,
		SizeGuildClassName	= 32,
		SizeGuildClassNameDesc = 72,
		SizeGuildMemo		= 64,
		SizeShopName		= 128,
		SizeBuddyMemo		= 255,
#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
		SizeInvenMax		= 150, //Inven, tamanho do armazém 255 limite
#else
		SizeInvenMax		= 84,
#endif
		SizeSecondPass		= 33,
		SizeFirstInven      = 21,
		SizeFirstWarehouse  = 21,
		SizeFirstShareStash = 14,
		SizeDigimonBaseSlot = 4,
	};

	enum eMinLen	// Carta gil agora
	{
		MIN_LenID				= 2,
		MIN_LenPass				= 2,
		MIN_LenCharName			= 2,
		MIN_LenGuildName		= 2,
		MIN_LenGuildClassName	= 2,
		MIN_LenGuildClassNameDesc = 2,
		MIN_LenGuildNotice		= 0,
		MIN_LenGuildMemo		= 0,
		MIN_LenShopName			= 1,
		MIN_LenBuddyMemo		= 1,
		MIN_LenMsg				= 1,
	};

	enum eMaxLen	//Carta gil agora
	{
		MAX_LenID				= 14,
		MAX_LenPass				= 14,
		MAX_LenCharName			= 8,
		MAX_LenGuildName		= 14,
		MAX_LenGuildClassName	= 8,
		MAX_LenGuildClassNameDesc = 21,
		MAX_LenGuildNotice		= 60,
		MAX_LenGuildMemo		= 21,		// Se você tiver 31 letras, é maior que o tamanho do buffer, por isso é limitado a 21 caracteres
		MAX_LenShopName			= 15,
		MAX_LenBuddyMemo		= 255,
		MAX_LenMsg				= 250,
	};

	enum eData
	{
		TamerLevelMax		= 99,
		DigimonLevelMax		= 99,		
		EvoStep				= 9,
		MaxAttrInchantLevel = 15,
		MaxInchantLevel =	60,
		MAX_CHANNELING		= 5,
		MAX_ItemSkillDigimon = 2,
		DropItem			= 35,
		MAX_DIGIMONSKILLLevel	= 15,
	};

#elif defined VERSION_USA

	const int CHARACTER_SET = 949;

	enum eSize	// Tamanho, esquema de banco de dados, buffer
	{
#ifdef VERSION_AERIA
		SizeID				= 21,		
#else
		SizeID				= 30,
#endif
		SizePass			= 120,
		SizeAccessToken		= 255,
		SizeCharName		= 32,
		SizeGuildName		= 48,
		SizeGuildNotice		= 255,
		SizeGuildClassName	= 32,
		SizeGuildClassNameDesc = 72,
		SizeGuildMemo		= 64,
		SizeShopName		= 128,
		SizeBuddyMemo		= 255,
#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
		SizeInvenMax		= 150, //Inven, tamanho do armazém 255 limite
#else
		SizeInvenMax		= 84,
#endif
		SizeSecondPass		= 33,
		SizeFirstInven      = 30,
		SizeFirstWarehouse  = 21,
		SizeFirstShareStash = 14,
		SizeDigimonBaseSlot = 4,
	};

	enum eMinLen	// Carta gil agora
	{
#ifdef VERSION_AERIA
		MIN_LenID				= 2,		
#else
		MIN_LenID				= 6,
#endif
		MIN_LenPass				= 2,
		MIN_LenCharName			= 3,
		MIN_LenGuildName		= 3,
		MIN_LenGuildClassName	= 3,
		MIN_LenGuildClassNameDesc = 3,
		MIN_LenGuildNotice		= 0,
		MIN_LenGuildMemo		= 0,
		MIN_LenShopName			= 1,
		MIN_LenBuddyMemo		= 2,
		MIN_LenMsg				= 1,
	};

	enum eMaxLen	// Carta gil agora
	{
		MAX_LenID				= 24,
		MAX_LenPass				= 16,
		MAX_LenCharName			= 12,
		MAX_LenGuildName		= 14,
		MAX_LenGuildClassName	= 12,
		MAX_LenGuildClassNameDesc = 21,
		MAX_LenGuildNotice		= 120,
		MAX_LenGuildMemo		= 31,
		MAX_LenShopName			= 17,
		MAX_LenBuddyMemo		= 23,
		MAX_LenMsg				= 250,
	};

	enum eData
	{
		TamerLevelMax		= 120,
		DigimonLevelMax		= 120,
		EvoStep				= 9, // 최대 8단계 진화(0~7), 8번째는 퇴화
		MaxAttrInchantLevel = 15,
		MaxInchantLevel =	60,
		MAX_CHANNELING		= 5,
		MAX_ItemSkillDigimon = 2,
		DropItem			= 35,
		MAX_DIGIMONSKILLLevel	= 30,
	};

#elif defined VERSION_TH

	const int CHARACTER_SET = 874;

	enum eSize	// Tamanho, esquema de banco de dados, buffer
	{
		SizeID				= 21,
		SizePass			= 120,
		SizeAccessToken		= 255,
		SizeCharName		= 31,
		SizeGuildName		= 32,
		SizeGuildNotice		= 255,
		SizeGuildClassName	= 32,
		SizeGuildClassNameDesc = 72,
		SizeGuildMemo		= 64,
		SizeShopName		= 128,
		SizeBuddyMemo		= 255,
#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
		SizeInvenMax		= 150, //Inven, tamanho do armazém 255 limite
#else
		SizeInvenMax		= 84,
#endif
		SizeSecondPass		= 33,
		SizeFirstInven      = 21,
		SizeFirstWarehouse  = 21,
		SizeFirstShareStash = 14,
		SizeDigimonBaseSlot = 4,
	};

	enum eMinLen	// Carta gil agora
	{
		MIN_LenID				= 2,
		MIN_LenPass				= 2,
		MIN_LenCharName			= 2,
		MIN_LenGuildName		= 3,
		MIN_LenGuildClassName	= 3,
		MIN_LenGuildClassNameDesc = 3,
		MIN_LenGuildNotice		= 0,
		MIN_LenGuildMemo		= 0,
		MIN_LenShopName			= 1,
		MIN_LenBuddyMemo		= 3,		//Conclusão (condição anterior: o comprimento do calor do personagem do pacote muda para a variável u2)
		MIN_LenMsg				= 1,
	};

	enum eMaxLen	// Carta gil agora
	{
		MAX_LenID				= 10,
		MAX_LenPass				= 15,
		MAX_LenCharName			= 12,
		MAX_LenGuildName		= 14,
		MAX_LenGuildClassName	= 12,
		MAX_LenGuildClassNameDesc = 21,
		MAX_LenGuildNotice		= 120,
		MAX_LenGuildMemo		= 31,
		MAX_LenShopName			= 36,
		MAX_LenBuddyMemo		= 23,
		MAX_LenMsg				= 250,
	};

	enum eData
	{
		TamerLevelMax		= 99,
		DigimonLevelMax		= 99,
		EvoStep				= 9, // Até 9 níveis de evolução (0 ~ 7), 8º é degeneração
		MaxAttrInchantLevel = 15,
		MaxInchantLevel =	60,
		MAX_CHANNELING		= 3,
		MAX_ItemSkillDigimon = 2,
		DropItem			= 35,
		MAX_DIGIMONSKILLLevel	= 25,
	};

#elif defined VERSION_KOR

	const int CHARACTER_SET = 949;

	enum eSize	// Tamanho, esquema de banco de dados, buffer
	{
		SizeID				= 21,
		SizePass			= 128,
		SizeAccessToken		= 255,
		SizeCharName		= 32,
		SizeGuildName		= 48,
		SizeGuildNotice		= 255,
		SizeGuildClassName	= 32,
		SizeGuildClassNameDesc = 72,
		SizeGuildMemo		= 64,
		SizeShopName		= 128,
		SizeBuddyMemo		= 255,
#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
		SizeInvenMax		= 150, //Inven, tamanho do armazém 255 limite
#else
		SizeInvenMax		= 84,
#endif
		SizeSecondPass		= 33,
		SizeFirstInven      = 30,
		SizeFirstWarehouse  = 21,
		SizeFirstShareStash = 14,
		SizeDigimonBaseSlot = 4,
	};

	enum eMinLen	// Carta gil agora
	{
		MIN_LenID				= 3,
		MIN_LenPass				= 3,
		MIN_LenCharName			= 2,
		MIN_LenGuildName		= 2,
		MIN_LenGuildClassName	= 2,
		MIN_LenGuildClassNameDesc = 2,
		MIN_LenGuildNotice		= 0,
		MIN_LenGuildMemo		= 0,
		MIN_LenShopName			= 1,
		MIN_LenBuddyMemo		= 2,
		MIN_LenMsg				= 1,
	};

	enum eMaxLen	// Carta gil agora
	{
		MAX_LenID				= 12,
		MAX_LenPass				= 16,
		MAX_LenCharName			= 8,
		MAX_LenGuildName		= 14,
		MAX_LenGuildClassName	= 8,
		MAX_LenGuildClassNameDesc = 21,
		MAX_LenGuildNotice		= 120,
		MAX_LenGuildMemo		= 31,
		MAX_LenShopName			= 17,
		MAX_LenBuddyMemo		= 23,
		MAX_LenMsg				= 250,
	};

	enum eData
	{
		TamerLevelMax		= 120,
		DigimonLevelMax		= 120,
		EvoStep				= 9, // Até 9 níveis de evolução (0 ~ 7), 8º é degeneração
		MaxAttrInchantLevel = 15,
#ifdef SERVER_KOREANDNS_DIGIMON_TRANSCENDENCE_190412
		MaxInchantLevel =	75,
#else
		MaxInchantLevel =	60,
#endif
		MAX_CHANNELING		= 5,
		MAX_ItemSkillDigimon = 2,
		DropItem			= 35,
		MAX_DIGIMONSKILLLevel	= 30,
	};

#else

	const int CHARACTER_SET = 949;

	enum eSize	// Tamanho, esquema de banco de dados, buffer
	{
		SizeID				= 21,
		SizePass			= 128,
		SizeAccessToken		= 255,
		SizeCharName		= 32,
		SizeGuildName		= 32,
		SizeGuildNotice		= 255,
		SizeGuildClassName	= 32,
		SizeGuildClassNameDesc = 72,
		SizeGuildMemo		= 64,
		SizeShopName		= 128,
		SizeBuddyMemo		= 255,
#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
		SizeInvenMax		= 150, //Inven, tamanho do armazém 255 limite
#else
		SizeInvenMax		= 84,
#endif
		SizeSecondPass		= 33,
		SizeFirstInven      = 21,
		SizeFirstWarehouse  = 21,
		SizeFirstShareStash = 14,
		SizeDigimonBaseSlot = 4,
	};

	enum eMinLen	// Carta gil agora
	{
		MIN_LenID				= 3,
		MIN_LenPass				= 3,
		MIN_LenCharName			= 2,
		MIN_LenGuildName		= 2,
		MIN_LenGuildClassName	= 2,
		MIN_LenGuildClassNameDesc = 2,
		MIN_LenGuildNotice		= 0,
		MIN_LenGuildMemo		= 0,
		MIN_LenShopName			= 1,
		MIN_LenBuddyMemo		= 2,
		MIN_LenMsg				= 1,
	};

	enum eMaxLen	// Carta gil agora
	{
		MAX_LenID				= 12,
		MAX_LenPass				= 16,
		MAX_LenCharName			= 8,
		MAX_LenGuildName		= 14,
		MAX_LenGuildClassName	= 8,
		MAX_LenGuildClassNameDesc = 21,
		MAX_LenGuildNotice		= 120,
		MAX_LenGuildMemo		= 31,
		MAX_LenShopName			= 17,
		MAX_LenBuddyMemo		= 23,
		MAX_LenMsg				= 250,	
	};

	enum eData
	{
		TamerLevelMax		= 99,
		DigimonLevelMax		= 99,
		EvoStep				= 9, // Até 9 níveis de evolução (0 ~ 7), 8º é degeneração
		MaxAttrInchantLevel = 15,
		MaxInchantLevel =	60,
		MAX_CHANNELING		= 5,
		MAX_ItemSkillDigimon = 2,
		DropItem			= 35,
		MAX_DIGIMONSKILLLevel	= 15,
	};

#endif
}

};