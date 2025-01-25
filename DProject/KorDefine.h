#pragma once 


#define		ACCOUNT_IP			"211.252.86.66"	// Alterar IP de acesso para transferência de servidor de jogos domésticos (KT Cloud)
 #ifdef VERSION_QA// Alterar IP de acesso para transferência de servidor de jogos domésticos (Naver Cloud)
 	#define		ACCOUNT_IP			"1.255.51.120"	
 #else
 	#define		ACCOUNT_IP			"1.255.55.185"
 #endif
#ifdef VERSION_QA//Alterar IP de acesso para transferência de servidor de jogos domésticos (AWS)
#define		ACCOUNT_IP			"13.124.249.85"	
#else
#define		ACCOUNT_IP			"76.223.32.7"
#endif

#define ACCOUNT_IP "177.44.75.179"
#define		ACCOUNT_PORT		7029

#include "_Lan_Korea.h"

#define	NAME_MIN_LEN				2
#define NAME_MAX_LEN				8
#define NAME_GUILD_MAX_LEN			14
#define NAME_FRIEND_COMMENT_LEN		23	//Ccreatename :: m_nmaxlengthbyte também deve ser modificado no init
#define NAME_BLOCK_COMMENT_LEN		23	//Ccreatename :: m_nmaxlengthbyte também deve ser modificado no init
#define NAME_GUILDRANK_COMMENT_LEN	21	//Ccreatename :: m_nmaxlengthbyte também deve ser modificado no init
#define VERSION_FILE				"LauncherLib\\vDMO.ini"
#define SPLASH_LOGO

#define CHAT_LIMITE_SHOUTING_LV		20	// Nível limite de grito

#define LANGAUGE_SELECT					// Seleção de idioma
#define GUILD_MARK_DOWN					// Download da marca da guilda
#define USE_BARCODE_REDER				// Usando o leitor de código de barras
#define USE_ID_SAVE						// Id Save
#define WEB_INFOMATION					// Use informações da Web

#define USE_SHUTDOWN					// Sistema de desligamento
//#define USE_SHUTDOWN_NOTICE_10		// No anúncio do jogo às 22:00

#define SYNC_DATA_LIST					// Sink mapa de dados Processamento e prioridade do quadro
#define SYNC_DATA_LIST_FRAME_1	1		// Unidade de quadro de dados de pia 1
#define SYNC_DATA_LIST_FRAME_2	1		// Unidade de quadro de dados de pia 2
#define	USE_CENTER_BAR					// Barra central

#define	ENABLE_KEYBOARD_CAMERA			// Rotação da câmera do teclado
#define	MATCHING_SYSTEM					// Sistema de correspondência UI
#define	BATTLE_MATCH					// Obtenha informações de batalha

//	#define GUILD_INVITATION				// Convide convite de guilda
//	#define STAND_ARRANGE_SERVER			// Arranjo fixo do servidor

/************************************************************************/
/*								khc0729									*/
/************************************************************************/


#define SIMPLEFICATION						//2013.02.27(khc0729)	Simplificação

//#define FIREWORK							//2013.05.01(khc0729)	Firecracker
//#define ADVENTURE_TAMER_CHANGE			//2013.05.07(khc0729)	Aventura-> Substabilidade Sabres Substituição

/************************************************************************/
/*								 vf00									*/
/************************************************************************/

#define NEW_WAREHOUSESORT					//2012.08.03(vf00)	Renovação de compensação de armazém
#ifdef NEW_WAREHOUSESORT
#define	WAREHOUSESORT						//2012.08.22(vf00)	Proteção de fonte de limpeza do armazém
#endif
#define NEW_SHARESTASH						//2012.08.06(vf00)	Armazém adicional e uma nova função de organização

//#define TRICK_ORTREAT						//2012.10.15(vf00)	Adicione o Halloween Evand

//#define SERVER_WAITING_LINE				//2013.01.15(vf00)	Adicione a fila do servidor

//#define CROSSWARS_SYSTEM					//2013.07.29(vf00)		디지몬 크로스워즈 시스템


//#define TUTORIAL_DISABLE					//2013.11.05(vf00)	Tutorial desativado
#define SIMPLE_TOOLTIP						//2013.11.13(vf00)	Dica de ferramenta da interface do usuário


/************************************************************************/
/*								lks007									*/
/************************************************************************/
#define CONSIGNMENT_SALE					//2012.09.25(lks007)	Encontrar loja de consignação

//#define EXP_UNLIMITED						//2013.01.15(lks007)	Se o nível de desempenho da missão e o nível Digimon forem mais de 15 repetições

#define DEBUFF_ITEM							//2013.08.07(lks007)	Debuff


#define CONSIGNMENT_BALLOON_CASH			//2013.12.11(lks007)	Fala de discurso consignado com balão de caixa do tipo Item Type Add Add

#define LEVELUP_EFFECT_LOOP					//2014.01.23(lks007)	Experiência bombas (?) Mostra o efeito de nível -Up tanto quanto o nível que aumentou.
#define SERVER_MONEY_SAVE_CHEAT				//2014.01.16(lks007)	Chave de trapaça para armazenar todo o dinheiro dos usuários no banco de dados


#define TAMER_IDLEANIMATION					//2014.03.25(lks007)	Quando o domador estiver vazio, se você enviar e receber dados com o servidor, precisará se levantar.




#define MEGAPHONE_BAN						//2014.05.28(lks007)	Alto-falante

#define EGG_PREFERMENT						//2014.07.09(lks007)	Troca de aloses de aço com um alto nível de aloses de aço (uso da máquina de mistério)

//#define NECKLACE_ACCESSORY				//2014.08.13(lks007)	Acessórios de colar (Definir é pesquisado pela Depine ao verificar a parte processada por lata A)


#define CONSIGNMENT_CREATE_RESTRICT			//2015.03.18(lks007)	Para que o armazenamento de consignação não seja criado perto do NPC e próximo ao portal.

/************************************************************************/
/*								chu8820									*/
/************************************************************************/
#define MINIGAME							//2013.02.20(chu8820)	Mini jogo

// #define UI_RENEWAL						//2013.05.16(chu8820)	Renovação da interface do usuário 05.16 ~ 10.15 (cancelamento)
#define EMPLOYMENT_TAMERNAME				//2013.06.11(chu8820)	Nome do domador saída ao abrir uma loja de consignação

#define PC_BANG_SERVICE						//2013.08.29(chu8820)	Publicação de salas para PC Webzen
// #define PC_BANG_SERVICE_TEST				//2013.08.29(chu8820)	Webzen -Modo de liberação de teste Webzen

#define MASTERS_MATCHING					//2013.11.20(chu8820)	Mestres correspondentes ifdef pop (nmyteam);Lata para liberar

#define CHEAT_ITEMRANK						//2013.11.21(chu8820)	Adicionar itens rank -chinet Key
#define CHAT_BAN							//2013.12.04(chu8820)	Ferramenta de operação :: sem chat
#define MONSTER_SKILL_GROWTH				//2013.12.18(chu8820)	Habilidade de Monstro :: Adicionar crescimento

#define CHANGE_CHANNEL_LIMIT				//2013.12.31(chu8820)	Movimento limitado em caso de canal saturado ao mover o canal
#define REPURCHASE							//2014.01.08(chu8820)	Armazenar itens de vendas Função de recompra

#define BUFFINFO_FIX						//2014.04.30(chu8820)	버프 정보(버프아이콘) UI 수정
#define GUILD_RENEWAL						//2014.06.25(chu8820)	길드 시스템 리뉴얼(길드스킬)

#define	KEYBOARD_MOVE						//2014.09.17(chu8820)	MOVIMENTO DO TECHO (RELACIONADO DE DON)


#define DAMAGE_METER						//2014.11.05(chu8820)	Medidor de danos (estatística de combate)


/************************************************************************/
/*							    kimhoshok                               */
/************************************************************************/
#define COSTUME_GENDER						//2014.02.11(kimhoshok)	Masculino, mulheres -apenas figurinos 2604, 2605
#define COSTUME_TAMER_ONLY					//2014.04.29(kimhoshok)	Trajes adicionados para domer

//#define TARGETUI_TIMEMASK_DELETE			//2014.05.07(kimhoshok)	Remova a renderização da máscara de tempo do buff da interface do usuário do alvo
//#define ZONEMAP_CLICK_MOVE				//2014.05.21(kimhoshok)	Mova de John Map (M) para Ctrl + R Button -Update


#define MAKE_TACTICS_UI_UPGRADE				//2014.05.22(kimhoshok)	Digitama Hatching Data intermediário Use modificação da interface do usuário
#ifdef MAKE_TACTICS_UI_UPGRADE
#define TACTICS_USE_DATA_LV2				//(kimhoshok)			Use dados intermediários
//#define TACTICS_USE_DATA_LV3
//#define TACTICS_USE_DATA_LV4
//#define TACTICS_USE_DATA_LV5
#endif

/************************************************************************/
/*								nova									*/
/************************************************************************/


/************************************************************************/
/*								nova									*/
/************************************************************************/

/********** 15.10.01 Fragmento ****************************************/	
//#define PARTY_BUFF_SHOW						// 2015.10.01(eiless84)	


/********** 15.11.26 ****************************************/
#define	MAP_REGION_SHOW							// 2015.11.26(eiless84) Nome local no mapa, adicione oculto.






#define SDM_USER_UI_SKIN_CHANGE_20160331							// Skins de interface do usuário fácil de modificar

#define SDM_CASHITEM_TRADE_LIMITED_20170214							// Função de restrição de negociação de itens de caixa

//#define SDM_MASTERS_MATCHING_ICON_HIDE_20170220					// Os ícones de correspondência de Masters oculam os mestres que correspondem ao servidor Definy, para que as informações da equipe sejam fornecidas.

#define SDM_TAMER_SERVERRELOCATE_20170911							// Função de movimento do servidor Tamer
#define SDM_TAMER_SERVERRELOCATE_NOCHECK_TAMERNAME_20170914			// Modificando o nome do tamer ao mover o servidor Tamer

#define SDM_TAMERSLOT_EXPANSION_20170911							// Função de extensão de slot tamer


//============================================================
//							leejiu
//============================================================
/********** 19.05.29 *****************************************/
#define LJW_TACTICS_SLOT_EXTEND_190529			// Expansão do slot mercenário

//=============================================================================
//	Modificação de despejo Common DePine (4 semanas após o trabalho é excluído sem problemas)
//=============================================================================
//#define CRASHDUMP_0305						//2014.03.05 Resumo
//#define CRASHDUMP_0326						//2014.03.26 Resumo
//#define CRASHDUMP_0424						//2014.04.24 Resumo

// 16.06.07
#define LSJ_ALREADY_BUFF_CHECK						// 2016.06.07 Adicione a sintaxe para verificar um efeito de buff duplicado.DM_TALK Tabela 30456 Número.// buff_check_msg requer a mesma verificação de função ...



#define SDM_SECONDPASSWORD_REINFORCE_20180330		// Altere a interface do usuário para que caracteres especiais possam ser inseridos na segunda janela de entrada de senha

#define MACROKEY_NOTINPUT							// A entrada da chave de atalho permanece no IME e impede o valor da chave.LKS007 13.06.11


#define SDM_DIGIMON_SHOW_SKILLEXP_TABLE_20190212	// Modificar o tempo de habilidade da Digimon Experience para se referir à tabela na tabela

#define LJW_TUTORIAL_RENEWAL_20191128				// Melhoria do tutorial

//========================================================================================
//
//  Separação por distribuição
//  
//========================================================================================
//#define USECONSOLE

#ifdef _GIVE		// Distribuição
//#define USE_DUMPER							//2014.02.18(kimhoshok)		Dumping dumps para a Coréia
#define DEFAULT_IME_HANGEUL						//2014.02.13(lks007)		Ao mover o mapa, a linguagem de entrada (IME) é alterada para o inglês

//#define	DEF_CORE_NPROTECT					// 2019.07.12				Aplicativo de guarda de jogo
//#define	DEF_CORE_NO_GAMEGUARD

//#define SDM_RESOURCE_CHECKER_ENABLE			// Ativação da função de teste de modulação de recursos

//#define SDM_DEF_XIGNCODE3_20181107			// Uso de xigncode3 doméstico

#ifdef VERSION_QA
//	#define DEF_CORE_NPROTECT_TEST	// 2015.06.08	Aplicativo de guarda de jogo // ativo apenas quando uma construção alfa
	#define	CHEAT_KEY
#endif

#elif NDEBUG		// liberar

//#define USECONSOLE

//#define DEBUG_SYNC_CHAR					// Personagem de sincronização para depuração
#define USE_DMO_INI							// Leia o valor de configuração inicial no arquivo ini

//#define ENABLE_KNOCKBACK					// Knockback disponível

//#define GM_CLOCKING						//2013.02.20 (CHU8820) Item transparente

//#define USE_DUMPER						// 2014.02.18 Coreano Dump Dump Transfer Kimhoshok
//#define MOVIECAPTURE						// 2014.03.19 Versão ao vivo Uso absoluto!F12 interno/não licença/bandicam gravando Kimhoshok
#define IGNORE_DISCONNECT_EXIT				// Verifique se o cliente não será encerrado, mesmo se você receber um pacote desconectado do servidor.
//	#define ZONEMAP_CLICK_MOVE				// mova -se de John Map (K: M) para Ctrl + R Button Kimhoshok -Update indeciso


//#define	DEF_CORE_NPROTECT				// 2015.06.08 Aplicativo de guarda de jogo
//#define DEF_CORE_NPROTECT_TEST			// 2015.06.08 Aplicativo de guarda de jogo // ativo somente quando a construção alfa

#define SKIP_LOGO							// Logotipo de pular

#else				// Depurar	
// 	#define MOVIECAPTURE					// 2014.03.19 Versão ao vivo Uso absoluto!F12 interno/não licença/bandicam gravando Kimhoshok

#define DEBUG_SYNC_CHAR						// Personagem de Sink para Debug
 
#define USE_DMO_INI							// Leia o valor inicial de configuração no arquivo ini
 
#define CLIENT_MONSTER						// Relatórios de tabela de monstros e outros
 
#define ENABLE_KNOCKBACK					// Knockback

#define IGNORE_DISCONNECT_EXIT				// Se você receber um pacote de desconexão do servidor, o cliente não termina
#define ZONEMAP_CLICK_MOVE					// Mova de John Map (M) para o botão Ctrl + R de John Map (M) 2014.05.21 Kimhoshok -EnDecided
#define SKIP_LOGO							// Logotipo pular

#endif