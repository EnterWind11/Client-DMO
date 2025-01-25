#pragma once

typedef enum eContentsType
{
	E_CT_LOGO,
	E_CT_LOGIN,
	E_CT_SECOND_PASSWORD,
	E_CT_SERVER_SELECT,
	E_CT_CHARACTER_SELECT,
	E_CT_CHARACTER_CREATE,
	E_CT_LOADING,
#ifdef USE_SHUTDOWN
	E_CT_SHUTDOWN_SYSTEM,
#endif
	E_CT_INVENTORY_CONTENTS,
	E_CT_WAREHOUSE,
	E_CT_MAIL_SYSTEM,
	E_CT_ITEMPRODUCTION,			// Criação de itens
	E_CT_CHATTING_STANDARDIZATION,	// Janela de bate -papo (Sistema, Log de Batalha) Padronização
	E_CT_CARD_EVENT,				// Sistema de eventos de cartão
	E_CT_MAP_INTERFACE,				// Conteúdo do mapa
	E_CT_DATA_TRADE_SYSTEM,			// Sistema de troca de dados
	E_CT_ITEM_REWARD,				// Armazém de pagamento do item.
	E_CT_FRIENDRECOMMEND_EVENT,		// [5/12/2016 HYUN] Evento de recomendação de amigos
	E_CT_STORE_SYSTEM,				// [23/08/2016 HYUN] Sistema de loja

	E_CT_CHANELLSELECT,				// Selecionar canal

	E_CT_DATS_CENTER,
	E_CT_DAMAGE_METER,

	E_CT_EVENT_CONTENTS,
	E_CT_TRADE,						// Conteúdo de negociação de itens

	E_CT_FRIENDLIST,				//Lista de amigos e lista de bloqueio

	E_CT_DIGITAMA_CONTENTS,			//2017-03-22-NOVA Impan and Recovery

	E_CT_MAKETACTICS_CONTENTS,		//2017-04-11-NOVA MERCENÁRIO DIGIMON Hosting / Incubay터

	E_CT_PERSONSTORE_CONTENTS,		//2017-04-27-NOVA PRIVADA SOPER
	E_CT_UNIONSTORE_CONTENTS,		//2017-04-19-NOVA Store de consignação
	E_CT_ENCYCLOPEDIA_CONTENTS,		//2017-05-10-NOVA Digimondo
	E_CT_ACHIEVE_CONTENTS,			//2017-06-08-NOVA ACIVERAÇÃO
	E_CT_BATTLE_SYSTEM,				// Conteúdo do sistema de batalha
	E_CT_PARTYSYSTEM,	
	E_CT_RESOURCEINTEGRITYCHECKER,
	E_CT_SEALMASTER,				//Mestre do selo
	
	E_CT_COMMUNITY,					//Guilda

	E_CT_MACROPROTECT,				//Macro Protect
	E_CT_SERVERCHANGE_CONTENTS,		//Alteração do servidor Conteúdo
	E_CT_TUTORIAL_CONTENTS,			//Sistema tutorial
	E_CT_INFINITEWAR_CONTENTS,		//Guerra Infinita
	E_CT_INFINITEWAR_RANKREWARD_CONTENTS,		// Compensação infinita de guerra
	
	E_CT_DIGIMON_ARCHIVE_CONTENTS,
	E_CT_MAINFRAME_CONTENTS,
	E_CT_NEON_DISPLAY_CONTENTS,		// Painel publicitário

	E_CT_NEW_CASHSHOP,
	E_CT_EVOLUTION_STORE_CONTENTS,	// Loja evolutiva
#ifdef SDM_VIP_SYSTEM_20181105
	E_CT_VIPMEMBERSHIP_CONTENTS,
#endif
	E_CT_GOTCHAMACHINE_CONTENTS,	// Máquina de gacha
	E_CT_ELEMENTITEM_CONTENTS,		// Propriedades
	E_CT_ENCHANT_OPTION_CONTENTS,	// Sentimento / reforço opcional do item
	E_CT_QUEST_PROGRESS_CONTENTS,	// Busca
	E_CT_END,
}E_CONTENTS_TYPE;

namespace CONTENTS_EVENT
{
	typedef enum eContentsEvent
	{
		EEvt_None = 0,
		
		EEvt_PrepareDestroy,
		EEvt_CharacterCreate_Success,
		EEvt_Party_New,				// Eu entrei em uma nova festa ou fiz uma festa
		EEvt_Party_Leave,			// Eu deixei a festa.
		EEvt_Party_Banish,			// Eu sou uma festa na festa?
		EEvt_BattleLoading_Start,	
		EEvt_MacroProtect_Start,	// A questão da macro protetor veio.
		EEvt_EndEvent,
	}E_CONTENTS_EVENT;

	typedef enum eContentsStreamEvent
	{
		EStreamEvt_SecondPassword_Type,
		EStreamEvt_PartyMember_Digimon_Change,	// Digimon Mudança dos membros do partido
		EStreamEvt_PartyMember_Map_Change,		// Movendo o mapa dos membros do partido.
		EStreamEvt_PartyMember_Info_Change,		// Mudar a informação do partido um.
		EStreamEvt_PartyMember_Leave,			// O membro do partido saiu
		EStreamEvt_PartyMember_Banish,			// Membro do Partido Estrada 榮?
		EStreamEvt_PartyMember_New,				// Um novo membro do partido entrou
		EStreamEvt_PartyMember_Faraway,			// Os membros vão embora ou venham
		EStreamEvt_Guild_Channel,				// Configuração de informações do canal da guilda
		//EStreamEvt_VipMemberShip_Change,		// Mudança de associação VIP
		EStreamEvt_Inventory_NewItem,			// Informações de novo item de inventário
		EStreamEvt_Inventory_UseItem,			// Use itens ao clicar com a direita no inventário
		EStreamEvt_Cashshop_ShowProductItem,	// Evento de seleção de produtos da loja de dinheiro
		EStreamEvt_NotMatch_MapNameResource,	// O nome do mapa de recursos é diferente.
		EstreamEvt_MainFrame_SetTarget,			// Alvejando
		EstreamEvt_MainFrame_ReleaseTarget,		// Alvejando

	}E_CONTENTS_STREAM_EVENT;
}
