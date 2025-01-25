#pragma once 

//#define		ACCOUNT_IP_Aeria	"account-aeria.dmo.joymax.com"
//#define		ACCOUNT_IP			"account.dmo.joymax.com"

#define		ACCOUNT_IP_Local	"127.0.0.1"

#ifdef VERSION_QA
#define		ACCOUNT_IP			"127.0.0.1"
#else
#define		ACCOUNT_IP			"127.0.0.1"
#endif

#define ACCOUNT_IP "127.0.0.1"
#define		ACCOUNT_PORT		7029

#define	NAME_MIN_LEN				3
#define NAME_MAX_LEN				12
#define NAME_GUILD_MAX_LEN			14
#define NAME_FRIEND_COMMENT_LEN		23		//cCreateName::Init M_nmaxLengthbyte também deve ser modificado juntos
#define NAME_BLOCK_COMMENT_LEN		23		//cCreateName::Init M_nmaxLengthbyte também deve ser modificado juntos
#define NAME_GUILDRANK_COMMENT_LEN	21		//cCreateName::Init M_nmaxLengthbyte também deve ser modificado juntos
#define VERSION_FILE				"LauncherLib\\vGDMO.ini"
//	#define NET_VERSION_ACCOUNT			52304112//VERSION.ini Modificado para verificar o código da versão verificado com um arquivo chu8820
//	#define NET_VERSION_GAME			20140911//VERSION.ini Modificado para verificar o código da versão verificado com um arquivo chu8820

#define CHAT_LIMITE_SHOUTING_LV		20	// nível de limite de grito


#include "_Lan_English.h"
#define USE_ID_SAVE

#define TARGET_WIN_MULTYLINE				//  Janela de destino 2 linha
//#define PLAY_PANELTY						//	BOT 2nd -nalty por tempo
#define CHEAT_MASTER						//	Se deve usar o mestre trapaça
#define VERSION_ERROR_ROOLBACK				//	Rolução de erro de versão
#define ADD_ACHIEVE_COSTUME					//	Realizações adicionais para figurinos

#define GM_COSTUME							//	GM cos 츔 -Effect Effect
//#define INVENSORT							//  Inventário
#define GUILD_INVITATION					//	Rejeição de convite da guilda

#define GM_COSTUME							//	Traje GM - Efeito Efeito
//#define INVENSORT							//  Limpeza de inventário	
#define GUILD_INVITATION					//	Rejeitar Convite da Guilda
#define GUILD_MARK_DOWN					// Baixar Marca da Guilda
#define BUFFINFO_FIX						//2014.04.30(chu8820)	Informações de buff (ícone de buff) correção da interface do usuário
#define ITEM_LIMIT_STRING_BUG				//2012.07.27(chu8820)	Corrigido o problema em que o valor do nível estava oculto no texto do nível limite "Tamer Limitation Lv" na dica de ferramenta

/********** 13.08.27 Patch (Firecracker) ****************************************/
//#define FIREWORK							//2013.05.01(khc0729)	Firecracker

/********** 13.10.10 Patch (Indulgência adicionada) ***********************************/
#define TRICK_ORTREAT						//2012.10.15(vf00)		Adicione o Halloween Evand


/********** 13.12.24 Patch (reflita a modificação de insetos domésticos) *************************/
#define CHEAT_ITEMRANK						//2013.11.21(chu8820)	Adicionar itens rank -chinet Key
//Adicionar renovação do mapa Dotbon/Yokohama
//#define TUTORIAL_DISABLE					//2013.11.05(vf00)		Tutorial desativado

/********** 14.01.28 Patch (acessórios, adicionando simplificação) ***********************/
#define SIMPLEFICATION						//2013.02.27(khc0729)	Simplificação


/********** 14.03.25 fragmento ***********************************************/
//Sealmaster, recompra, slot rápido, safira milagrosa, adição
//#define CARD_MASTER							//2013.12.30(vf00)	Mestre do cartão
#define REPURCHASE							//2014.01.08(chu8820)	Armazenar itens de vendas Função de recompra
#define	USE_CENTER_BAR						// 센터바
#define LEVELUP_EFFECT_LOOP					//2014.01.23(lks007)	Experiência bombas (?) Mostra o efeito de nível -Up tanto quanto o nível que aumentou.
//Adicionado o vestido dos Jogos Olímpicos de Inverno de 2014 (classificação de gênero)
#define COSTUME_GENDER						//2014.02.11(kimhoshok)	Masculino, mulheres -apenas figurinos 2604, 2605
//Modificação de insetos -Add Desert Mapa Raid Monster AI
#define MONSTER_SKILL_GROWTH				//2013.12.18(chu8820)	Habilidade de Monstro :: Adicionar crescimento
/* Solicitação -gm item transparente */
//#define GM_CLOCKING						//2013.02.20(chu8820)	Transparente


//월드컵 아바타 추가 관련
#define COSTUME_TAMER_ONLY					//2014.04.29(kimhoshok)	Traje exclusivo do Tamer adicionado
//버그수정 - 버프아이콘 UI 수정
//업적 아이콘_02 추가

/********** 14.06.24 패치 ***********************************************/
//데미지 자료형 수정
#define DEBUFF_ITEM							//2013.08.07(lks007)	item de debuff
//국내 수정사항 적용
#define TAMER_IDLEANIMATION					//2014.03.25(lks007)	Quando o domador estiver ausente e trocando dados com o servidor, ele deve se levantar.
#define GUILD_RENEWAL
/********** 14.07.22 패치 ***********************************************/
//중급데이터
#define MAKE_TACTICS_UI_UPGRADE				//2014.05.22(kimhoshok)	Digitama Incubator Intermediate Data Usage UI Modificada
#define TACTICS_USE_DATA_LV2				//(kimhoshok)			dados intermediários
//#define TACTICS_USE_DATA_LV3
//#define TACTICS_USE_DATA_LV4
//#define TACTICS_USE_DATA_LV5


/********** 14.11.25 fragmento ***********************************************/
//Movimento do teclado, atualização do escudo nuclear, sobreposição de texto (aumento da largura do texto)
#define	KEYBOARD_MOVE						//2014.09.17(chu8820)	MOVIMENTO DO TECHO (RELACIONADO DE DON)




/************************************************************************/
/*							    eiless84                                */
/************************************************************************/

/********** 15.10.01 Fragmento ****************************************/	
//#define PARTY_BUFF_SHOW						// 2015.10.01(eiless84)	

/********** 15.12.02 ****************************************/

#define MASTERS_MATCHING					//2013.11.20(chu8820)	Mestres correspondentes ifdef pop (nmyteam);Lata para liberar

// 2015.12.22
#define	MAP_REGION_SHOW						// 2015.11.26(eiless84) Nome local no mapa, adicione oculto.

#define NEW_SHARESTASH						//2012.08.06(vf00)		Armazém adicional e uma nova função de organização
#define NEW_WAREHOUSESORT					//2012.08.03(vf00)		Renovação de compensação de armazém
#ifdef NEW_WAREHOUSESORT
#define	WAREHOUSESORT						//2012.08.22(vf00)		Proteção de fonte de limpeza do armazém
#endif

#define CONSIGNMENT_BALLOON_CASH			//2013.12.11(lks007)	Fala de discurso consignado com balão de caixa do tipo Item Type Add Add
#define CONSIGNMENT_SALE					//2012.09.25(lks007)	Encontrar loja de consignação
//#define EXP_UNLIMITED						//2013.01.15(lks007)	Se o nível de desempenho da missão e o nível Digimon forem mais de 15 repetições, o 98 representante수정


#define MEGAPHONE_BAN						//2014.05.28(lks007)	Alto-falante



#define EMPLOYMENT_TAMERNAME				// [2/25/2016 hyun]	Nome do domador saída ao abrir uma loja de consignação

// 16.08.16
#define LSJ_ALREADY_BUFF_CHECK				// 2016.06.07 Adicione a sintaxe para verificar se há efeitos de buff duplicados.DM_TALK Tabela 30456 Número.// buff_check_msg requer a mesma verificação de função ...

// [8/23/2016 hyun] Adicione Hyunwook.
#define MINIGAME							//2013.02.20(chu8820)	Mini jogo



#define SDM_CASHITEM_TRADE_LIMITED_20170214	// Função de restrição de negociação de itens de caixa
#define SDM_VIP_SYSTEM_20181105
//#define SDM_MASTERS_MATCHING_ICON_HIDE_20170220			// Os ícones de correspondência de Masters oculam os mestres que correspondem ao servidor Definy, para que as informações da equipe sejam fornecidas.


// 2017.05.30 fragmento
#define DAMAGE_METER						//2014.11.05(chu8820)	Medidor de danos (estatística de combate)


#define SDM_SECONDPASSWORD_REINFORCE_20180330		// Altere a interface do usuário para que caracteres especiais possam ser inseridos na segunda janela de entrada de senha

#define SDM_SERVER_STATE_SHOW_20181011				// Melhoria da interface do usuário para exibir o status de saturação do servidor

#define SDM_DIGIMON_SHOW_SKILLEXP_TABLE_20190212	// Modificar o tempo de habilidade da Digimon Experience para se referir à tabela na tabela


// 2019-3-19
#define CHAT_BAN							//	ferramentaDeOperação ::SemChat


#define LJW_TACTICS_SLOT_EXTEND_190529	// 19.07.03 Expansão do slot mercenário
#define LJW_TUTORIAL_RENEWAL_20191128	// Melhoria do tutorial

//========================================================================================
//
//  Separação por distribuição
//
//========================================================================================


#ifdef _GIVE

//#define SDM_DEF_XIGNCODE3_20181107
//#define SDM_RESOURCE_CHECKER_ENABLE	// Ativar recurso de verificação de adulteração de recursos

#ifdef VERSION_QA
	#define CHEAT_KEY
#endif

#elif NDEBUG
	#define USE_DMO_INI						//	Leia as configurações padrão do arquivo ini
	#define IGNORE_DISCONNECT_EXIT			// Evita que o cliente desligue mesmo quando um pacote de desconexão é recebido do servidor
	#define SKIP_LOGO						// pular logotipo
	#define CHEAT_KEY
#else //_DEBUG
	#define USE_DMO_INI						//	Leia as configurações padrão do arquivo ini
	#define IGNORE_DISCONNECT_EXIT			// Evita que o cliente desligue mesmo quando um pacote de desconexão é recebido do servidor
	#define SKIP_LOGO						// pular logotipo
	//#define PASSWORD_ENCODE
#endif