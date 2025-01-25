#pragma once


#include "nScope.h"


namespace pItem
{
	enum
	{
		Begin = nScope::Item,

		Use,					// Usando itens
		UseFailure,				// Fracasso
		MoneyChange,			// Compra de item, preço de venda, ao comprar-, ao vender+

		Move,					// Inven-Inven, Inven-Digivice, Inven-Equip Movement and Solictações사용됨
		MoveFailure,			// Fracasso
		Merge,					// Combinou dois itens em um
		Split,					// Compartilhando o item em dois no estado de inventário
		Exchange,				// Substitua dois itens de slot

		Drop,					// Inven-> Campo
		Crop,					// Campo-> Inven
		CropMoney,				// Se você comer o item que você come bem quando o pegar
		Destroy,				// Inven-> Waste Basket
		CropFailure,			// Falha em adquirir itens

		Trade,					// usuário-> usuário

		Buy,					// Loja-> Inven
		Sell,					// Inven-> Shop

		BuyFailure,				// talvez dinheiro insuficiente
		SellFailure,			//

		Add,					// Aquisição de itens
		Update,					// Adicione estatísticas devido à verificação de itens
		Delete,					// Usando itens
		//		Item
		//		ItemUpdate,

		DigitamaToItem,			// Digitama Scan
		DigitamaToMoney,		// Recuperação Digitama

		InvenSlotSize,			// Se o tamanho do slot do inventário mudar
		WarehouseSlotSize,		// Mudança de tamanho do slot do armazém

		SocketIn,				// Coloque o item no soquete.
		SocketOut,				// Extraia o item do soquete
		SocketClear,			// Remova itens do soquete

		Analysis,				// Análise dos itens de soquete, a taxa de valor das estatísticas de itens de soquete inexplicável é 0

		CashShop,				// Solicitação de lista de itens em dinheiro comprados e resposta
		CropCashItem,			// Use quando o item adquirido for movido para o inventário

		ScanCount,				// Digitalizações de itens

		Timeout,				// O item é caro

		ChangeLimitedAttribute,	// Item limitado (restrições à propriedade)

		GiftShop,				// Caixa de presente (recuperação e provisão de eventos) Solicitação de lista e resposta
		CropGiftItem,			// Se você receber um item da caixa de presente

		CapsuleToItem,			// Trocar caixas de presente com itens, as rotinas são as mesmas que o digitamatoitem.

		CoolDown,				// Resfriar a transmissão e recepção de informações relacionadas

		PartyCrop,				// Se o membro do partido comer

		HatchOptionAdd,         // Registro de itens de correção de hospedagem
		PCbangInven,			// Solicitação de informações de inventário para PC Room
		CropPCBangItem,			// Ao importar itens da sala do PC
		PortableItemFail,		// Falha do item portátil falha
		CropGiftItemFail,		// Falha em obter itens da caixa de presente

		ShareStashSlotSize,    //Mudança de tamanho do armazém de contas

		HatchBackupDiskAdd,	    //Registro de disco de backup de hospedagem
		HatchBackupDiskCancel,	//Cancelamento do registro de disco de backup de eclosão

		ScanItemUse,			//Ao usar um item de digitalização
		LimitItemUse,			//Restrições sobre o uso de itens (tornando -o por causa do fígado dourado)
		BackupDiskIn,           //Registro de discos de backup
		BackupDiskOut,          //Cancelar disco de backup
		StoreSummonUse,			//Ao usar itens de convocação da loja

		RaidItemReward,			//Recompensa do item de ataque
		InsertGiftItem,			//Coloque -o em um armazém de presentes.

		DirectHatch,			//Hashing quando al -clique

		GotchaOpen,				//Máquina de máquina aberta
		GotchaExecute,			// Execute uma máquina de desenho
		GotchaItem,				//Se for um item geral
		GotchaError,			//Erro da máquina de gacha
		MsgAllGotchaMachine,	//Mensagem completa de aquisição rara
		MoveClear,				//Mova todos os itens na guia original para o local da guia original (envie -o do servidor para o Clar)
		FireWork,				//Adicione o item de fogos de artifício
		CrossWarsUse,			//Use itens cruzados
		CrossWarsUseFailure,	//Item cruzado Use falha
		CrossInvenSlotSize,		//Pacote de aumento de inventário cruzado
		MysteryMachine,			//Adição de máquina de mistério
		ExpireGiftItem,			//Item de armazém de presente expirado
		AccessoryCheck,			//Verifique os itens de acessórios
		AccStoneUse,			//Uso de documentos de ordem para renovar estatísticas
		DigiSlotInc,			//Usando o item de expansão do slot Digimon padrão Digimon

		// Seal relacionado
		CardUnSeal,				//Cartão Mestre de vedação
		CardReSeal,				//Cartão Master Seal
		CardDrop,				//Drop item do cartão
		SealSlotSize,			//Expansão do slot do Silmaster
		CardAdd,				//Adicionar cartão
		CardUpdate,				//Atualização de quantidade do cartão
		CardDelete,				//Remoção do cartão

		ItemRepurchase,			//Função de recompra de item
		ItemRepurchaseOpen,		//Janela de recompra de item aberta
		ItemRepurchaseList,		//Listas apenas ao vender itens

		ShouterOffer,			//Alto-falante

		Make,					//Criação de itens

		CashItemTradeTimeOut,   //Restrição do item de caixa restrição

		UseActive,				//Ativação de itens

		RelocateTamer,			//Item de transferência do servidor Tamer
		ItemSort,
		ItemScan,
		ItemMove,
		End
	};

	enum // CropFailure
	{
		CropF_Late = 1,			//Outros pegam primeiro
		CropF_Right = 2,			//Sem direito de adquirir
		CropF_Far = 3,			//A distância é muito longe
		CropF_Full = 4,			//Cheio de inventário
	};
};



namespace nItem
{
	/////////////////////////////////////////////////////
	// item source info
	enum eTab
	{
		TabInven = 0,	// nLimit::Inven
		TabEquip,		// nLimit::Equip
		TabWarehouse,	// nLimit::Warehouse

		// Abaixo está Digivice
		TabSkill,		// nLimit::Skill, Itens de habilidade que podem ser montados em digiis
		TabChipset,		// nLimit::Chipset, Item de chipset que pode ser montado no Digibice
		TabDigivice,	// nLimit::Digivice, Slot que pode ser equipado com digibis ==> Há um espaço

		TabCashShop,	// nLimit::CashShop, Armazém de armazenamento de itens em dinheiro comprado
		TabGiftShop,	// nLimit::GiftShop, Itens armazenados por presentes ou recuperação, etc.
		TabPCBang,		//Inventário para PC Room

		TabShareStash,	// nLimit::ShareStash
		// Abaixo está o Inven Common Item Guia
		TabCrossWars,	// nLimit::DigimonCrossWars, Cartão de Wars Cross?Caixa de armazenamento de item

		MaxSlotType,

		MaxSlotSize = 1000, //Número máximo de slots em cada guia
	};
	//	nTab  == nPos / MaxSlotSize;
	//	nSlot == nPos % MaxSlotSize;

	enum ItemClass
	{
		eNothingItem = 1,	// itens inúteis / cinza
		eNormalItem = 2,	// itens regulares / branco
		eMagicItem = 3,	// item mágico / verde
		eRareItem = 4,	// item raro / azul
		eEpicItem = 5,	// item épico / roxo
		eUniqueItem = 6,	// Item / laranja exclusivo
		eLegendItem = 7,	// Item de legenda / cor dourada
		eEventItem = 9,	// itens de evento
		eCashItem = 10,	// item de cache
		eAdministratorItem = 11, //Administrador
	};


	namespace nMaker
	{
		enum
		{
			Program = 0, // Teste de programa
			FirstSupply = 1, // Fornecido ao criar personagens
			Design = 2, // Teste de planejamento
			Monster = 3, // Fornecido por monstros
			Shop = 4, // Item da loja de jogos
			Cash = 5, // Pago
			Mix = 6, // Combinação de item
			Scan = 7, // Digitama Scan
			HatchCancel = 8, // Cancelamento
			Crop = 9,
			Split = 10,
			Quest = 11, // Adquirido pela Quest
			SocketOut = 12, // Se o item do soquete for extraído do slot do soquete do item
			Card = 13,
			Gift = 14, // Recuperação de presentes ou sistema
			TamerShop = 15,
			PCBangSupply = 16,
			BDCancel = 17,
			GotchaMachine = 18,
			Recompense = 19, // Recompensa do NPC
			Repurchase = 20, // Recompra
			DigimonBook = 21, // Pagamento do item Digimondo
			Spirit = 22, // Digimon-> Criado pelo espírito
			ShopMake = 23, // Produção do item da loja de jogos
			Parcel = 24, // Pagamento do item de evento de salto

			Master = 30, // Mestre de jogos se for maior que 30

		};
	};

	/////////////////////////////////////////////////////
	// item source info
	enum eType	// item type
	{
		Head = 21,
		Coat = 22,
		Glove = 23,
		Pants = 24,
		Shoes = 25,
		Costume = 26,
		Glass = 27,
		Necklace = 28,
		Ring = 29,
		Earring = 30,

		EquipAura = 31,

		XAI = 32,
		Bracelet = 33,





		Memory = 50,	// Já trabalhou no cliente
		Capsule = 51,	// Já trabalhou no cliente
		Chipset = 52,	//
		Digivice = 53,	// Item item
		Skill = 54,	// Item de habilidade

		TacticsEnchant = 55,	// Item de aprimoramento do Digimon
		TacticsEnchantSub = 56,	// Assistência de aprimoramento do Digimon
		TacticsEnchantReset = 57,
		DigimonDataChange = 58,	// Digimon Data Exchange

		// Abaixo pode ser sobreposto
		LevelRequire = 60,	// Item de limite de nível
		Potion = 61,	// Incluindo poções e comida
		NoQuick_Consume = 62,	// Item de consumo (o Quickbar não pode ser instalado)
		Buff = 63,	// Item de aplicação de buff
		SKILL_Card = 64,	// Cartão
		Card = 65,	// Cartão
		Observer = 66,	// Item de confronto do observador

		DigimonSkillChipATK = 67,	// Memória das características de ataque
		DigimonSkillChipDEF = 68,	// Memória característica defensiva
		DigimonSkillChipAST = 69,	// Memória característica auxiliar
		DigimonSkillMemoryChip = 70,	// Chip de consumo ao usar a memória característica

		Consume = 71,	// Consumíveis gerais Inven Use imediatamente
		Portal = 72,	// Item móvel de localização
		Resurrection = 73,	// Item de ressurreição no lugar
		Guild = 75,   // Item de criação da guilda
		PersonStore = 76,	// Loja privada
		Portable_Item = 77,	// Pass portátil NPC
		NoticeItem = 78,	// Todos os anúncios usando o item do item

		Quest = 80,	// Itens de matéria -prima (ingredientes gerais, missões, frases, passes)
		Matrial2 = 81,	// Digimon Data para Hatching Digitama
		EventItem = 82,	// Item do evento (certificado de presente cultural, mp3, etc.)
		EventItem2 = 83,	// Item do evento -Nenhuma saída para a placa de sinal
		Revision = 84,   // Hatching

		UseQuest = 85,	// Itens de missão para usar
		Matrial = 86,	// Japtem
		DropMoney = 87,	// Item que você come bem quando o pega^
		Money = 88,	// Don (mega)
		Rescale = 89,	// Frutos misteriosos, re -ajuste de escala
		// NEW_BATTLE_SYSTEM
		DogimonBattle = 90,

		Digitama1 = 91,	// Para vendas de saques
		Digitama2 = 92,	// Para adquirir itens de saque
		Digitama3 = 93,	// Hatching Digimon de saques

		Digitama4 = 95,	// Resuscitação Digimon Dead
		Digitama5 = 96,	// Digimon morto convocou e reencarnação para reencarnação
		Digitama6 = 97,	// Para morto Digimon Reencarnation

		Capsule_Coin = 100,	// Cápsula
		
		AccStone = 110,	//Acessórios Opção Mudança de pedra
		
		EItem_All = 120,	// Atributo itens que podem ser instalados em todos os lugares
		EItem_Glass = 127,	// Atributo itens que podem ser montados em óculos
		EItem_Coat = 122,	// Atributo itens que podem ser montados no topo
		EItem_Glove = 123,	// Item de atributo que pode ser montado na mão
		EItem_Pants = 124,	// Atributo itens que podem ser montados no fundo
		EItem_Shoes = 125,	// Atribuir itens que podem ser montados nos pés

		Cash_Evoluter2 = 149,  //TODO Adicionado pelo Djnandinho  Fazer test
		Cash_Evoluter = 150,	// Evo Luther -Cash Item para perfurar o slot evolutivo
		Cash_ChangeDName = 151,	// Digimon Nome Alterar
		Cash_ChangeTName = 152,	// Alteração do nome do domador
		Cash_ResetSkillPoint = 153,	// Digimons matam a inicialização
		Cash_BackupDisk = 154,	// Ovo mercenário fictício
		Cash_ExtraInven = 155,	// Expansão de inventário
		Cash_ExtraWarehouse = 156,	// Expansão do armazém
		Cash_JumpBuster = 157,	// Jump Booster
		Cash_Resurection = 158,	// Resgatar
		
		DigimonSlotInc = 159,	//Item de expansão do slot Digimon
		

		Cash_DigimonCareSlot = 160,	// Expansão do slot de armazenamento mercenário

		Cash_DigiCore = 161,	// Digicore
		Excelator = 162,	// No caso da evolução ultra -pola, o consumo de item e o valor do tipo não funcionam separadamente.
		ChangeTamer = 163,	// Conversão permanente de domadores de propriedade.

		BattleTag = 164,	// Digimon Change Item na batalha
		Cash_Shouter_N = 165,	// Alto-falante
		Cash_Shouter_S = 166,	// Alto-falante
		Cash_Shouter_NT = 167,	// Sistema de termo de tipo geral de alto -falante
		Cash_Shouter_ST = 168,	// Alto-falante
		GM_Shouter_T = 169,  // Alto -falante exclusivo da GM

		ScanUse_Item = 170, // Itens usando digitalização
		BackupDisk = 171, // Itens de disco de backup hashing

		CombinenceChipset		= 175, // Chipset da coalizão
		GoldBar = 172, // Evento Gold Bar
		TimeSet = 173, // Tempo de bomba de abóbora
		StoreSummon = 174, // Armazene o item de convocação // não o use.

		AttributeItem = 176,

		RClick_Hatch = 177,	// Hatching de ovo à direita

		GotchaCoin = 178,	//Moeda da máquina de gacha

		Item_TamerSKillConsume = 180,	// Item de dinheiro da habilidade Tamer (tipo de consumo)
		Fire_Work = 181,  // Firecracker
		CrossWars = 182,	// Item de Guerras Cruzadas (Hatching and Chip)
		CodeCrown = 183,	// Code Crown
		CrossLoader = 184,	// Carregador cruzado
		Cash_CrossInvenSlot = 185,	// Aumento do inventário cruzado
		Debuff = 186,	// Debuff
		BuffEvent = 187,	// 23013 Evento Chuseok (item de doação)
		Incubater = 188,	// Incubadora de arquitetura Digimon

		CardMaster = 190,	// Item de vedação
		UnSealCardMaster = 191,	// Vedação de vedação
		ReSealCardMaster = 192,	// Selo
		EvoCapsule = 193,	// Cápsula

		Cash_Shouter_Ban = 194,	// Item de bloqueio de alto -falante // vitória de escrita constante -Limto

		Spirit = 195,	// Item da Evolução do Espírito
		SpiritEXP = 196,	// Item da experiência evolutiva
		FriendShip_Increase = 197,	// Aumentar o item

		RelocateTamer = 198, // Transferência do servidor Tamer
		TamerSlotExpansion = 199, // Expansão do slot tamer

		AntiX = 201,	// Xiteiro x, carregamento de cristal
		SkillLevelOpen = 202,	// Expansão do nível de habilidade Digimon
		AntiXEvoluter = 400,


		SockAll = EItem_All,
		SockGlass = EItem_Glass,
		SockCoat = EItem_Coat,
		SockGlove = EItem_Glove,
		SockPants = EItem_Pants,
		SockShoes = EItem_Shoes,
	};

	enum eSubType
	{
		TacticsEnchantReset_One = 5701,
		TacticsEnchantReset_All = 5702,
		IncGuildMaxMember = 6201,	// O maior número de itens de expansão da Guild
		InitSkillPoint = 6202,	// Item de inicialização de habilidades
		Oberver_Watch = 6601,	// O item do observador está disponível
		Oberver_Match = 6602, // O item do observador não pode ser assistido
		PersonStoreLicence = 7601, // Permissão de loja pessoal
		CommissionShopLicence = 7603, // Licença de loja de emprego
		Portable_Hatch = 7701, // Hatch portátil
		Portable_WareHouse = 7702,	// Item de armazém portátil
		Portable_DigimonCareHouse = 7703, // Itens portáteis de armazenamento Digimon
		Portable_Scanner = 7704,	// Scanner portátil usando itens
		Portable_DigitamaCollect = 7705,	// Item de recuperação de item portátil
		Portable_Analysis = 7706, // Analisador de atributo portátil
		Portable_Socket = 7707, // Equipamento portátil
		Portable_JumpGate = 7708, // Portão de salto de equipamento portátil

		Evoluter = 15001, //Pago em 15001
		EvolutionLimit = 15002, //15002 Limite de Evoração
		ToyGun = 15003, //15003 armas de brinquedo
		GdPo = 15004, //15004 GIDPO
		DramonBreaker = 15005, //15005 Dramon Breaker
		HolySpeer = 15006, //15006 Janela sagrada
		CARD_BUFF = 6451,		// Item de buff de cartões
		CARD_ACTIVE_SKILL = 6452,		// item ativo do cartão
		CombinenceChipset_CASH	= 17201,
		CombinenceChipset_USE	= 17202,
		BackupDisk_Event = 17101,    // Disco de backup de eventos
		BackupDisk_Cash = 17102,    // Disco de backup em dinheiro

		TaiChangeItem = 16301,
		MinaChangeItem = 16302,	// Item de mudança de mina
		MattChangeItem = 16303,	// Matthew Change Item
		TakeruChangeItem = 16304,	// Item de mudança de Ricky
		SoraChangeItem = 16305,	// Item de mudança de Sora
		HikariChangeItem = 16306,	// Item de mudança de mina

		IncubaterFacilitation = 18801,	// Item de promoção da incubadora de arquitetura Digimon
		IncubaterExtraction = 18802,	// Item de extração da incubadora de arquitetura Digimon

		HeadAll = 2101,
		HeadSavers = 2102,
		HeadAdverture = 2103,

		CoatAll = 2201,
		CoatSavers = 2202,
		CoatAdverture = 2203,

		GloveAll = 2301,
		GloveSavers = 2302,
		GloveAdverture = 2303,

		PantsAll = 2401,
		PantsSavers = 2402,
		PantsAdverture = 2403,

		ShoesAll = 2501,
		ShoesSavers = 2502,
		ShoesAdverture = 2503,

		CostumeAll = 2601,		// Roupas públicas
		CostumeSavers = 2602,		// Economiza -apenas roupas
		CostumeAdverture = 2603,		// Aventura -Roupas apenas
		CostumeMan = 2604,		// Traje masculino
		CostumeWoman = 2605,		// Fantasia feminina

		AccessoryAll = 2701,		// Joalheria
		AccessorySavers = 2702,		// Sabers de jóias
		AccessoryAdverture = 2703,		// Aventura de joias

		AuraFireWall = 3102,		// Digi Oraslot Firewall

		SubUnSealCardMaster = 19101,	// Item de vedação de cartão pequeno tipo de categoria
		SubReSealCardMaster = 19201,	// Item de vedação de cartão pequeno tipo de categoria
		NormalDigiCloneS = 5501,
		NormalDigiCloneA = 5502,
		NormalDigiCloneB = 5503,
		NormalDigiCloneC = 5504,
		NormalDigiCloneD = 5505,
		AdvancedDigiCloneS = 5511,
		AdvancedDigiCloneA = 5512,
		AdvancedDigiCloneB = 5513,
		AdvancedDigiCloneC = 5514,
		AdvancedDigiCloneD = 5515,
		UltimateDigiCloneS = 5521,
		UltimateDigiCloneA = 5522,
		UltimateDigiCloneB = 5523,
		UltimateDigiCloneC = 5524,
		UltimateDigiCloneD = 5525,
		TutorialDigiClone = 5526,
		OldDigiCloneS = 5531,
		OldDigiCloneA = 5532,
		OldDigiCloneB = 5533,
		OldDigiCloneC = 5534,
		OldDigiCloneD = 5535,

		DigiCloneMaxStart = 5511,
		DigiCloneMaxEnd = 5525,

		AdditionalPotionType_Va = 6101,
		AdditionalPotionType_Vi = 6102,
		AdditionalPotionType_Da = 6103,
		AdditionalPotionType_Un = 6104,
		AdditionalPotionType_No = 6105,

		DigimonScaleAllChangeStep1 = 8901,
		DigimonScaleAllChangeStep2 = 8902,
		DigimonScaleAllChangeStep3 = 8903,
		DigimonScaleAllFixStep1 = 8904,

		DigimonScale3LevelHybrid = 8911,
		DigimonScale4LevelHybrid = 8912,
		DigimonScale5LevelHybrid = 8913,

		DigimonScale3LevelExtra = 8914,
		DigimonScale4LevelExtra = 8915,
		DigimonScale5LevelExtra = 8916,

		DigimonScale6LevelTotal = 8917,   //TODO verificar antes DigimonScale6LevelTotal passou para DigimonScale6LevelExtra


		DigimonScale6LevelExtra = 8918,  //TODO Foi adcionado pelo Djnandinho verificar ser e isso mesmo
		DigimonScaleAllFixStep135 = 8919,

		DigimonScaleParcelOut = 8921,


		Tutorialnecklace = 2802,
		TutorialRing = 2902,
		TutorialEaring = 3002,

		ParcelJewelry = 6092,
		SpiritJewelry = 6093,  //TODO adicionado djnandinho verificar ser e isso mesmo

		// NEW_BATTLE_SYSTEM
		DigimonBattleTicket = 9001,

		AntiBodyX = 20101,
		XCrystal = 20102,


		//#end		
	};

	enum eDrop		// Item Drop
	{
		Reject,		// negação
		Field,		// Drop em campo permitido
		Delete,		// Remova quando soltar
	};

	enum eUseCharacter
	{
		UC_NONE,	// Nenhum
		UC_COMMON,	// Tamer & Digimon pode ser usado
		UC_DIGIMON,	// Digimon exclusivo
		UC_TAMER,	// Apenas domador

	};


	enum eBaseAttributes	// atributos
	{
		Type,				// Tipo de item, nitem :: etype
		PowerAppType,		// Ao adquirir a confirmação da aplicação da habilidade do item, ao usá -lo, e confirmou,
		Class,				// classe de item

		UseMode,			// Restrições ao uso, uso do uso durante a batalha
		UseSeries,			// Série de uso
		UseCharacter,		// Consulte o EUSECHARACTER
		UseTimeGroup,		// Série temporal de uso

		Overlap,			// Número de armazenamento simultâneo
		Possess,			// Pode ser obtido por domador
		ReqTamerLevel,		// Limite de nível ao usar e usar
		ReqDigimonLevel,	// Limite de nível ao usar e usar
		ReqQuest,			// Restrições de missão ao usar ou usar itens (apenas aqueles que foram realizados ou usados)

		EquipSeries,		// Série de equipamentos -Restrição na fixação do mesmo horário
		Trade,				// Se é possível
		Drop,				// Como lidar quando Drop

		DigiCore,			// Número de consumo de Digicore ao comprar
		StanPrice,			// Compre preço padrão
		Cash,				// Compre cache
		Sale,				// Preço de descarte

		SubType,			// propósito
		Value,				// Valor do item

		dcDigimonLevel,		// soltar restrição
		dcTamerLevel,		// soltar restrição
		dcQuestIDX1,		// soltar restrição
		dcQuestIDX2,		// soltar restrição
		dcQuestIDX3,		// soltar restrição

		LinkedSkill,		// Consulte a tabela relacionada à habilidade (ou feitiço)

		RateMin,			// Razão de aplicação de estatísticas do item
		RateMax,			// Razão de aplicação de estatísticas do item

		SocketCount,		// Número de soquetes que podem estar fora do item
		SocketPowerRate,	// Razão de estatísticas de aplicação de item de atributo

		SkillCount,			// O número de habilidades que podem ser contidas apenas em digibis
		ChipsetCount,		// O número de chips que podem ser contidos e contidos apenas em digiis

		Limited,			// Se o item é atribuível e o tempo a ser atribuído ao ponto de atribuição, 0: microscópico, 1: atributo quando usado, 2:

		TimeLimitedType,	// 0: Irrelevante, 1: Item de tempo de tempo (todos o desaparecimento), 2: Somente estatísticas que desaparecem, 4: aplique o tempo ao usar itens삭제)
		TimeLimitedAmount,	// Superficialidade

		Family,				// Valor de atributo natural

		EventCoinItemType,	// Tipo de item de evento a pagar ao comprar itens
		EventCoinItemCount,	// Número de eventos a serem pagos ao comprar itens

		TargetException,	// Use restrições Configurações de Digimon
	
		TimePassActive,		// #define item_time_pass_active
		SvrBattle,

		MaxBaseAttribute
	};

	enum // Método de queda
	{
		DropField,
		DropDelete,
		DropReject,
	};
	/////////////////////////////////////////////////////
	// Falha em adquirir presente
	enum
	{
		GiftF_ITEMTYPE = 1,				// Erro de tipo de item
		GiftF_INVEN_NOT_EMPTY = 2,		// Falta de espaço de inventário
		GiftF_CROP_FAIL = 3,			// A falha de aquisição falha ao tentar novamente (falha interna de processamento)
		GiftF_COUNT_OVER = 4,			// Erro máximo de sobreposição

	};

	/////////////////////////////////////////////////////
	// Item Número exclusivo
	enum
	{
		IdxDigiCore = 5000,	// Número do item Digicore
	};

	/////////////////////////////////////////////////////
	// Código de falha do item
	enum
	{
		USE_FAIL_EXPIRE = 1,	// Expiração do item
		USE_FAIL_BATTLE,		// Em batalha
		USE_FAIL_COOLDOWN,		// Esfriar
		USE_FAIL_NOT_USE_AREA,	// Área não utilizada
		USE_FAIL_NON_TARGET,	// Grande Prêmio
		USE_FAIL_NOT_MONSTER,	// O alvo não é um monstro
		USE_FAIL_NOT_RANGE,		// Distância não utilizada
		USE_FAIL_NOT_USE_TARGET,// É um alvo que não pode ser usado.
		USE_FAIL_NOT_USE_TERMS, // Não é uma condição que possa ser usada.
		USE_FAIL_LIMIT,			// Restrição
		USE_FAIL_NOT_INVEN,		// Relacionado a chip de atributo tribal de inventário
		USE_FAIL_NOT_SAME,		// O mesmo item não pode ser aplicado
		USE_FAIL_NOT_USE_SCAN,	// Falhou em usar porque não há valor de item para resultados de varredura
		USE_FAIL_SPRIT_EXP,		// A menos que você convocar Digimon (tipo 10)
		USE_FAIL_MAX_LIMIT,		// Falha em usar itens atingindo o máximo
		USE_FAIL_NO_SKILL,		// Nenhuma habilidade
		USE_FAIL_OTHERS,		// fracasso
		USE_FAIL_BATTLESERV,	// Proibição de uso do servidor de batalha
	};



	/////////////////////////////////////////////////////
	// created item info
	enum
	{
		//		Count, // Duplicado
		// customizing data
		// enchant info
		// socket info
		// name customizing
	};
	
	enum eAccessory			// AccItem Option
	{
		AP		= 1,		// 공격력
		DP		= 2,		//방어력
		MAXHP	= 3,		//최대HP
		MAXDS	= 4,		//최대DS
		SkillAP	= 5,		//스킬데미지
		AttrAP	= 6,		//속성데미지
		CA		= 7,		//크리티컬
		CD		= 8,		//치명타데미지증가
		AS		= 9,		//공속증가
		EV		= 10,
		BL		= 11,
		HT		= 12,
		MaxSize = HT,		//배열사이즈, 항상 위에 옵션값 갯수와 같아야한다
		AP_ATTRIBUTE_DA = 101,
        AP_ATTRIBUTE_VA,
        AP_ATTRIBUTE_VI,
        AP_ATTRIBUTE_UN,
        AP_ATTRIBUTE_IC,
        AP_ATTRIBUTE_WA,
        AP_ATTRIBUTE_FI,
        AP_ATTRIBUTE_EA,
        AP_ATTRIBUTE_WI,
        AP_ATTRIBUTE_WO,
        AP_ATTRIBUTE_LI,
        AP_ATTRIBUTE_DK,
        AP_ATTRIBUTE_TH,
        AP_ATTRIBUTE_ST,

		DigiPower = 1,		// Energia digital
		Renewal = 2,		// Renovação de energia Diggble
		AccOption = 3,		// Opção de acessórios
		OptionValue = 4,	// Esse valor de opção
	};

	
};





// Para poder verificar se cada propriedade리
enum // grade
{
	Normal = 1, // 1 -Normal Item: Sem controle e pode ser instalado ao mesmo tempo.
	High,		// 2 -High Grade Item: Sem controle e não instalado ao mesmo tempo.
	Cash,		// Item 3 pago (item em dinheiro): emita automaticamente números de série por data da criação.
	Rare,		// 4 -Rare Item: emita automaticamente números de série por data da criação.
	Govern,		// 5 -GOVERN ITEM: É emitido sob a entrada do número de série exclusivo e é produzido apenas pela quantidade emitida.
	Helper,		// 6 -Administrator Item: Somente administradores como operadores e auxiliares, etc. são pagos,
};


namespace nNatualItemCode
{
	enum
	{

	};
}






namespace nCardGrade
{
	enum
	{
		Normal,			// Grau normal
		Bronze,			// Bronze
		Silver,			// Grau de prata
		Gold,			// Grau de ouro
		Platinum,		// Grau de platina
		Master,			// Grade mestre

		MAX_GRADE		// Máximo
	};

	enum
	{
		None = 0,			// Sem efeito

		DefeRateInc = 1,	// Efeito de aumento de % atual de defesa
		DefeRateDec = 2,	// Efeito de redução % atual de defesa

		DefeValueInc = 3,	// Efeito inteiro de defesa
		DefeValueDec = 4,	// Redução de números inteiros de defesa

		APRateInc = 5,		// Ataque de potência de poder %de aumento de aumento
		APRateDec = 6,		// Ataque Power Corrente %Efeito de redução

		APValueInc = 7,		// Aumentando danos inteiros
		APValueDec = 8,		// Redução de números inteiros

		MAXEFFECT			// Efeito máximo
	};
	enum
	{
		HP = 1,		//HP
		HP_PER,		//HP %
		DS,			//DS
		DS_PER,		//DS %
		AT,			//AT
		AT_PER,		//AT %
		AS,			//AS
		AS_PER,		//AS %
		CT,			//CT
		CT_PER,		//CT %
		HT,			//HT
		HT_PER,		//HT %
		DE,			//DE
		DE_PER,		//DE %
		BL,			//BL
		BL_PER,		//BL %
		EV,			//EV
		EV_PER,		//EV %
	};
};

enum eCardApplyType
{
	eCardApplyType_None = 0,
	eCardApplyType_HP_Value,
	eCardApplyType_HP_Rate,
	eCardApplyType_DS_Value,
	eCardApplyType_DS_Rate,
	eCardApplyType_AT_Value,
	eCardApplyType_AT_Rate,
	eCardApplyType_AS_Value,
	eCardApplyType_AS_Rate,
	eCardApplyType_CT_Value,
	eCardApplyType_CT_Rate,
	eCardApplyType_HT_Value,
	eCardApplyType_HT_Rate,
	eCardApplyType_DE_Value,
	eCardApplyType_DE_Rate,
	eCardApplyType_BL_Value,
	eCardApplyType_BL_Rate,
	eCardApplyType_EV_Value,
	eCardApplyType_EV_Rate,
	eCardApplyType_End,
};