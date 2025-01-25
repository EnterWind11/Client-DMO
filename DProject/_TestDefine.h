
#pragma once 

#define _ONLY_KOR(x)       __T(x)			// Usado apenas na versão coreana atual
#define _ONLY_ENG(x)       __T(x)			// Usado apenas na versão atual em inglês
#define _CUR_ENG(x)       __T(x)			// Já em inglês
#define _ERROR_MSG(x)       __T(x)			// Mensagem de erro -Current em coreano
#define _DEBUG_MSG(x)       __T(x)			// Para mensagem de depuração -indicada em inglês


#define FONT_WHITE		NiColor( 0.98f, 0.98f, 0.98f )
#define FONT_RED		NiColor( 0.98f, 0.1f, 0.1f )
#define FONT_GREEN		NiColor( 0, 0.98f, 0 )
#define FONT_YELLOW		NiColor( 0.98f, 0.98f, 0 )
#define FONT_GOLD		NiColor( 247/255.0f, 211/255.0f, 153/255.0f )
#define FONT_BLACK		NiColor( 0.02f, 0.02f, 0.02f )
#define FONT_GLAY		NiColor( 0.5f, 0.5f, 0.5f )
#define FONT_NEWGOLD	NiColor( 222/255.0f, 204/255.0f, 48/255.0f )
#define FONT_DARKBLUE	NiColor(0.5f, 0.8f, 1.0f)

#define DEFAULT_NAMECOLOR NiColor( 139.0f/255.0f, 248.0f/255.0f, 8.0f/255.0f )
#define PARTY_NAMECOLOR	NiColor( 28/255.0f, 102/255.0f, 193/255.0f )

#define BATTLE_SPEED_NORMAL				1.4f				// Achatando
#define BATTLE_SPEED_SKILL				1.4f				// habilidade
#define BATTLE_SPEED_RANGE				1.1f				// Projétil

#define DIMA_OAUTH_KEY_VALUE		"DiMaOAuthKey.value"

#ifdef VERSION_USA
	#include "common_vs2022/Define/Define_USA.h"
	#include "EngDefine.h"

#elif VERSION_TH
	#include "common_vs2022/Define/Define_TH.h"
	#include "ThaiDefine.h"

#elif VERSION_TW
	#include "common_vs2022/Define/Define_TW.h"
	#include "TaiwanDefine.h"

#elif VERSION_HK
	#include "common_vs2022/Define/Define_HK.h"
	#include "HongKongDefine.h"

#else
	#include "common_vs2022/Define/Define_KOR.h"
	#include "KorDefine.h"

#endif

#ifdef NEW_ATTAND_EVENT
	#define REWARD_SYSTEM_UI				//2017-03-08-nova	Sistema de compensação de acesso
#endif

#ifdef ITEM_TIME_PASS_ACTIVE
	#define	ITEM_USE_TIME_PASS				//2017-04-04-nova	Tempo
#endif

#ifdef SERVER_KSW_DIGIMON_ARENA_RANK_20180312
	#define SDM_INFINITEWAR_RANKING_SYSTEM_20180312		// Sistema de Classificação Digimon Arena
#endif

#ifdef SERVER_KOREANDNS_VIPMEMBERMODE_181109
	#define SDM_VIP_SYSTEM_20181105						// Adicionar sistema de conteúdo VIP
#endif

#ifdef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
	#define SDM_DIGIMONBOOK_EXTEND_20181126				//Aumento do número de subsistemas na Enciclopédia Digimon
#endif

#ifdef SERVER_KSW_SKILL_LV_EXPEND_181205
	#define SDM_DIGIMONSKILL_LV_EXPEND_20181206		// Expansão do nível de habilidade Digimon
#endif

#ifdef SERVER_KSW_MACRO_RENEW_181214
	#define SDM_MACRO_PROTECT_RENEW_20181217		// Renovação do protetor de macro
	#ifdef SERVER_KSW_MACRO_QUESTION_190517
		#define SDM_MACRO_PROTECT_QUESTION_20190524
	#endif
#endif

#ifdef SERVER_KSW_XEVOLUTION_1_180615
	#define SDM_TAMER_XGUAGE_20180628					// Definir medidor de anticorpos X e uso de itens
#endif

#ifdef SERVER_KSW_ADD_BRACELET_181106
	#define SDM_TAMER_EQUIP_ADD_BRACELET_20181031			// Tamer Ao adicionar uma nova pulseira de equipamento, ela deve ser adicionada aos 8 dias definidos da biblioteca Cs GBChar.
#endif

#ifdef SERVER_KOREANDNS_DIGIMON_TRANSCENDENCE_190412
	#define SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507	// Atualização relacionada à transcendência de Digimon
#endif

#ifdef SERVER_KOREANDNS_UIRENEWAL_ITEMSCANMODE_190318
	#define UI_ITEMSCAN_RENEWAL					// Melhorias na verificação de itens
#endif
#if defined (SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325) && defined (SERVER_KOREANDNS_UIRENEWAL_ITEMSORTMODE_190226)
	#define UI_INVENTORY_RENEWAL				// Melhorias no inventário
	#define WAREHOUSE_SERVER_SORT				// Organize seu armazém no servidor
#endif

#ifdef SERVER_KOREANDNS_LINKEDACCESSORY_190822
	#define LJW_ENCHANT_OPTION_DIGIVICE_190904		// Adicionar opção de encantamento Digivice - Cs GBChar - deve ser adicionado para definir
#endif

#ifdef SERVER_KSW_ITEM_TIME_CHARGE_190716
	#define LJW_EQUIPSYSTEM_190724					// Equipamento, integração de sintetizador de propriedade + sistema de carregamento de equipamento adicionado
#endif

//Todos os países se aplicam
#define SDM_CHET_ULIST_LOG_FILE_SAVE_20190110		// Adicionado para ser salvo como um arquivo de log quando a chave de truque ulist é usada

#ifdef SERVER_KSW_DAILYCHECKEVENT_191014
	#define LJW_DAILYCHECKEVENT_191030				// Evento do Livro Diário
#endif

#define TOOLTIP_SKILL_ADD_BASEATT				// Melhoria de dano de habilidade - habilidade + poder de ataque normal = aplicado a todos os países

#ifdef SERVER_KSW_DIGIMON_EFFECT_CHANGE_191230
	#define SDM_DIGIMON_PARTSSYSTEM_20200115	// Item de troca de skin do Digimon adicionado
#endif

#ifdef _DEBUG
#define USECONSOLE 1
#endif

#include "_LanConvert.h"
