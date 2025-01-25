//---------------------------------------------------------------------------
//
// 파일명 : GameEventCode.h
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
namespace EVENT_CODE
{
	enum GAME_EVENT_CODE
	{
		RESET_LOGIN_TIME_SEQ = 0,
		START_RESOURCECHECKER,		// 20180504 Evento de início de inspeção de integridade de recursos
		TIMEOUT_RESOURCECHECKER,		// 20180504 Evento de início de inspeção de integridade de recursos
		//NOTMATCH_MAPNAME_RESOURCECHECKER,
		RECV_LOGIN_WAIT_TIME,
		RECV_USE_BARCODE_FALSE,		//2017-07-11-nova

		RECV_LOGIN_REQUEST,

		RECV_SERVERSELECT_CLUSTER,	// Resultados para solicitações de acesso ao servidor

		RECV_PLAYER_DATA_LOAD_COMPLETE,

		RECV_SHUTDOWNSYSTEM_DATA,	// Receber dados de desligamento.
		RECV_WEEK_SHUTDOWN_DATA,	// Receba dados de desligamento seletivo.

		RECV_SECONDPASS_CHECK,		// Resultados para confirmação
		RECV_SECONDPASS_REGISTER,	// Resultados para a oferta secundária registro
		RECV_SECONDPASS_CHANGE,		// Resultados para mudar a segunda ofensa

		RECV_CHECK_DOUBLE_NAME,		// É um nome usado para receber um pacote do servidor
		RECV_SERVER_LIST,

		RECV_CHAR_SELETECT_LIST,	// Receba informações de caracteres da janela de seleção de caracteres.

		GET_SELETECT_LIST_EMPTY_SLOT_NUMBER,	// Obtenha o número de slot vazio da janela de seleção de caracteres.

		RECV_ADD_NEW_CHARACTER,		// Recebi as informações recém -criadas do personagem
		RECV_CHAR_CREATE_RESULT,	// Resultados da criação de personagens
		RECV_CHAR_DELETE_RESULT,	// Resultados da exclusão de caracteres

		RECV_CHAR_SELECT_RESULT,	// Sucesso de seleção de personagens

		OPEN_CHARDEL_PASSWORD_WINDOW,

		SEND_PORTAL_MOVE,			// Envie um pacote para mover o portal para o servidor
		LOADING_START,				// Starting de carregamento
		LOADING_PROGRESS_VALUE,		// Medidor de carregamento
		LOADING_COMPLETE,			// Carregando conclusão
		TABLE_LOAD_SUCCESS,			// Tabela de tabela de jogo final

		//////////////////////////////////////////////////////////////////////////

		OPEN_ITEM_PRODUCTION_SHOP,	// Item Production Store aberto
		CLOSE_ITEM_PRODUCTION_SHOP,	// Fechar uma loja de produção de itens

		SEND_ITEM_PRODUCTION_RUN,		//Criação de itens.execução
		RECV_ITEM_PRODUCTION_RESULT,	//Criação de itens.resultado

		SET_ITEM_PRODUCTION_ASSIST_ITEM_REG,

		//////////////////////////////////////////////////////////////////////////

		CHANGE_ITEM_COUNT,				// Criação de itens.Mudança de itens
		CHANGE_HAVE_MONEY,				// Criação de itens.Quantidade de alteração


		DIGITAMA_SELL_COMPLETE,				// Evento de sucesso de recuperação Digitama
		RECV_HATCHOUT_COMPLETE,				// Digitama Hatching Success Event
		RECV_ENCHANT_COMPLETE,				// Evento de sucesso de aprimoramento Digimon

		EVENT_CHAT_PROCESS,					// Conversasse o evento de bate -papo.
		EVENT_SYSTEM_PROCESS,				// Processe o evento de log do sistema.
		EVENT_BATTLE_PROCESS,				// O evento de log de batalha é processado.
		EVENT_ITEMCROP_PROCESS,				// Processos de eventos de log de aquisição de itens.

		CASHSHOP_TOGGLE,					// Cash Shop Open
		CASH_TOGGLE,						// 2017-03-28-NOVA Cash Warehouse

		CASHWAREHOUSE_TOGGLE,				// Cash Warehouse aberto
		CASHWAREHOUSE_ITEM_SET,				// Configuração do item do armazém em dinheiro
		CASHWAREHOUSE_ITEM_SET_REFRESH_DISABLE,	// Cash Warehouse Nova atualização

		CASHITEMPAY_TOGGLE,					// Janelas de pagamento de item aberto
		CASHITEMPAY_REFRESH,
		CASHITEMPAY_POSTITEM_SET,			
		CASHITEMPAY_REWARDITEM_SET,		
 
		MAIL_SET_DATA,							// Carta


		//////////////////////////////////////////////////////////////////////////
		// Exp Event
		WAREHOUSE_TOGGLE,					// Open Warehouse, feche
		WAREHOUSE_SET_NORMAL,
		WAREHOUSE_SET_SHARE,
		WAREHOUSE_ITEMREMOVE_NOSERVER,

		WAREHOUSE_NORMAL_DATA_DECREASE,
		WAREHOUSE_SHARE_DATA_DECREASE,
		WAREHOUSE_NORMAL_SLOTCOUNT,			// Número de armazém geral
		WAREHOUSE_SHARE_SLOTCOUNT,			// Número de armazéns compartilhados
		WAREHOUSE_GETDATA,
		WAREHOUSE_RESET,				
		WAREHOUSE_ENDTIME,
		WAREHOUSE_GET_SLOTCOUNT,
		WAREHOUSE_GET_EVENTTIME,
		WAREHOUSE_TRADELIMITED_ITEM_TIMEOUT,

		WAREHOUSE_RECV_ITEMSORT_END,
		//////////////////////////////////////////////////////////////////////////

		ACHIEVE_CHECKTYPE,
		ACHIEVE_FIRSTPOSTLOAD,
		ACHIEVE_SETDISPTITLE,
		ACHIEVE_SETRANKACHIEVE,

		OPEN_CARD_EVENT_UI,					// Janela do jogo de cartas aberta
		RECV_CARD_EVENT_RESULT,				// Resultados do jogo de cartas.
		RECV_CARD_EVENT_END,				// Card Game End

		OPEN_TRADE_SELECT,					// Abra a janela de seleção de troca de dados.Ou volte para a janela opcional na janela Exchange.
		START_TRADE_DATA,					// Execução da troca de dados.

		RECV_TRADE_DATA_SCALE,				// Receba informações de troca para dados de tamanho.
		RECV_TRADE_DATA_UPGRADE,			// Receber dados de reforço.
		RECV_TRADE_DATA_EVOLUTION,			// Dados evolutivos são recebidos.
		RECV_TRADE_DATA_FAILURE,			// Falha na troca de dados

		// [5/13/2016 hyun] Relacionado ao evento de recomendação de amigos
		RECV_RECOMMEND_EVENT_INFO,			// Receba informações relacionadas ao evento de recomendação de amigos.

		SEND_RECOMMEND_ADD_ANSWER_INFO,		// Quando alguém me recomenda, envio minha resposta ao servidor.
		RECV_FRIENDLY_ADD_ANSWER_INFO,		// Evento que recebe os resultados da recomendação que enviei
		RECV_FRIENDLY_MARKITEM_INFO,		// Informações que você recebe quando pressiona o item de recompensa

		RECV_FRIENDLY_CHANGE_NAME,			// Alteração do nome do domador
		RECV_FRIENDLY_ADD_INFO,				// Quando alguém me recomendou
		RECV_FRIENDLY_MARK_INFO,			// Informações relacionadas quando um amigo alcançou uma meta
		RECV_FRIENDLY_CHANGESTATE_INFO,		// Quando as informações do evento de recomendação do amigo são alteradas
		// @@ Até aqui

		// [8/24/2016 hyun] 상점
		OPEN_STORE_WINDOW,					// Loja
		SEND_STORE_ITEMBUY,					// Compra de item
		SEND_STORE_ITEMSELL,				// Vendas de itens

		STORE_APPLY_DISCOUNT,				// Aplicação de taxa de desconto

		RECV_STORE_BUY_SUCCESS,				// ITEM COMPRA DE COMPRA
		RECV_STORE_SELL_SUCCESS,			// Sucesso de vendas de itens


		REQUEST_STORE_REPURCHASE_OPEN,		// Solicitação de janela de compra de compra
		RECV_STORE_REPURCHASE_SET,			// Representando a configuração da janela
		SEND_STORE_REPURCHASE,				// Solicitação de compra de itens
		RECV_STORE_REPURCHASE_SUCCESS,		// Resposta de recompra do item
		RECV_STORE_REPURCHASE_FAILED,		// Falha na recompra do item

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// Exp Event
		RECV_EXP_EVENT_DATA,				// Receber dados de eventos de experiência.
		DISABLE_EXP_EVENT_UI,				// Excluir o evento de experiência da experiência
		HIDE_EXP_EVENT_UI,					// Evento de experiência na interface do usuário escondido
		SHOW_EXP_EVENT_UI,					// Experience Event UI Show
		GET_EXP_EVENT_NEXT_EXPRATE,			// No dia seguinte, traga uma experiência de acesso
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Evento relacionado à transação
		SEND_TRADE_REQUEST_TAMERNAME,			// Pedido de transações (solicitado pelo nome do domador)
		SEND_TRADE_REQUEST_TAMEROBJECTIDX,		// Solicitação de transação (solicitada por Tamer Object Idx)
		SEND_TRADE_REQUEST_CANCEL,				// Cancelamento de solicitações de transação.
		SEND_TRADE_REQUEST_RESULT,				// Enviando o resultado de solicitações de transação
		SEND_TRADE_MONEY_CHANGE,				// Altere o valor da transação.
		SEND_TRADE_ITEM_REGIST,					// Registre itens no inventário.

		RECV_TRADE_REQUEST_RESULT,				// O resultado da solicitação de transação.
		RECV_TRADE_RECEIVE_REQUEST,				// Recebeu uma solicitação de transação.
		RECV_TRADE_ITEM_REGIST,					// Registro de itens de negociação
		RECV_TRADE_ITEM_UNREGIST,				// Remoção de itens de negociação
		RECV_TRADE_MONEY_CHANGE,				// Mudar dinheiro da transação
		RECV_TRADE_INVEN_LOCK_UNLOCK,			// Mudança na preparação para transações
		RECV_TRADE_STANDBY_RESULT,				// Terreno de conclusão da transação
		RECV_TRADE_FINAL_RESULT,				// Resultados finais da transação

		//////////////////////////////////////////////////////////////////////////
		//Relacionado à interface do usuário de mudança de canal
		RECV_CHANNEL_LIST_INFO,					//Informações do canal.
		RECV_CURRENT_CHANNELINDEX,				//Canal atual
		SEND_CHANGE_CHANNEL,					//Canal Alterar transmissão de pacotes e conjunto de mensagens de bate -papo
		SEND_CHANGE_CHANNEL_BYMASTER,			//Send_change_channel chamada pelo mesmo GM

		GET_CHANNELINFO,						//Traga informações do canal
		OPEN_CHANGE_CHANNEL_POPUP,				//Confirmação de mudança de canal pop -up aberto
		DISPLAY_CHANGE_CHANNEL,					//Indicador de mudança de canal
		CHANGE_CHANNEL_FAIL,					//Falha na mudança de canal
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Daily Event
		RECV_DAILY_EVENT_DATA,					// Dados do evento de conexão
		RECV_MONTHLY_EVENT_DATA,				//2017-03-07-nova Evento de acesso mensal
		//////////////////////////////////////////////////////////////////////////
		//Relacionado à interface do usuário de mudança de canal
		RECV_FRIENDLIST,						// Receba uma lista de amigos
		RECV_SHUTOUTLIST,						// Lista de bloqueio
		RECV_FRIEND_ON_OFF,						// Friend Login
		RECV_CHANGE_NAME_NEARBY,				// Perto da mudança de nome
		RECV_CHANGE_NAME_FRIEND_SHUTOUT,		// Amigos/nomes de bloqueio
		RECV_ADD_FRIEND,						// Sucesso adicional de amigos
		RECV_ADD_SHUTOUT,						// Bloquear sucesso adicional
		RECV_NOT_FOUND,							// Não consigo encontrar o domador (todas as adições de amigos/bloqueio)

		SEND_FRIENDLIST,						// Pedido para listar amigos
		SEND_FRIEND_SHUTOUT_MEMO,				// Escreva amigos/memorando de bloqueio
		SEND_ADD_FRIEND_SHUTOUT,				// Amigos/bloquear pacotes adicionais enviam e encaminhamento

		GET_FRIENDLIST,							// Traga uma lista de amigos
		GET_SHUTOUTLIST,						// Traga a lista de bloqueio
		GET_CURRENT_LISTINDEX,					// Índice da lista de importação da lista atual.

		CHECK_ACHIEVE_TYPE,						// Verificação do tipo Archif
		DELETE_FRIEND_SHUTOUT,					// Excluir amigos/lista de bloqueio
		TOGGLE_FRIEND_SHUTOUT_UI,				// UI 토글 0 차단 탭이면 친구 탭으로 변경 토글, 1 단순 토글

		REWARD_ITEM_TOGGLE,						// 2017-03-08-nova  Armazém de pagamento do item
		REWARD_UI_TOGGLE,						// 2017-03-08-nova  Sistema de recompensa de conexão
		REWARD_RECEIVE_GIFT,					// 2017-03-09-nova	Receba itens de presente
		REWARD_RECEIVE_REWARD,					// 2017-03-09-nova	Obtenha um item de compensação
		REWARD_SEND_ACCPET_DATA,				// 2017-03-15-nova	Envie um presente, um item de recompensa para receber um servidor
		
		DIGITAMA_SCAN_ADD_DATA,					//2017-03-28-nova
		DIGITAMA_SCAN_SUCCESS,					//2017-03-28-nova
		DIGITAMA_SELL_SUCCESS,					//2017-03-28-nova

		DIGITAMA_OPEN_NPC,
		DIGITAMA_OPEN_PORTABLE,
		DIGITAMA_CLOSE,
		DIGITAMA_SCAN_START,
		DIGITAMA_SELL_START,
		DIGITAMA_SETITEM,
		DIGITAMA_ALLSCAN,

		ENCYCLOPEDIA_OPEN_DATA,					//2017-05-11-nova
		ENCYCLOPEDIA_USE_DECK,					//2017-05-15-nova
		ENCYCLOPEDIA_USE_GETDECK,				//2017-05-29-nova
		ENCYCLOPEDIA_SET_EVOLINFO,				//2017-05-15-nova
		ENCYCLOPEDIA_RECV_SERVER,				//2017-05-15-nova
		ENCYCLOPEDIA_INCHANTRESULT,				//2017-05-15-nova
		ENCYCLOPEDIA_GROUPINFOCON,				//2017-05-15-nova
		ENCYCLOPEDIA_LV_CHECK,					//2017-05-16-nova
		ENCYCLOPEDIA_EVOL_UPDATE,				//2017-05-16-nova
		ENCYCLOPEDIA_BISREWARD,					//2017-05-16-nova
		ENCYCLOPEDIA_SCALE,						//2017-05-16-nova
		ENCYCLOPEDIA_LOGOUT,					//2017-05-16-nova
		GET_ENCYCLOPEDIA_USE_DECK,				//2017-05-15-nova
		GET_ENCYCLOPEDIA_BISRECV,				//2017-05-15-nova
		GET_ENCYCLOPEDIA_NULL_CHECK,			//2017-05-15-nova
		GET_ENCYCLOPEDIA_CONDITION,				//2017-05-16-nova
		GET_ENCYCLOPEDIA_NATTYPE,				//2017-05-16-nova
		GET_ENCYCLOPEDIA_PROB,					//2017-05-16-nova
		GET_ENCYCLOPEDIA_OPTION,				//2017-05-16-nova
		GET_ENCYCLOPEDIA_VAL,					//2017-05-16-nova
		GET_ENCYCLOPEDIA_TIME,					//2017-05-16-nova
		
		// Evento relacionado à festa
		RECV_PARTY_INFO_INIT,						// Receba informações de renovação de informações da parte do servidor ao mover o mapa.
		RECV_PARTY_INFO,							// Obtenha informações da festa.
		RECV_PARTY_INVITE_RESULT,					// Resultados da solicitação da parte
		SEND_PARTY_INVITE_FROM_TAMERNAME,
		SEND_PARTY_INVITE_FROM_UNIQUE_ID,			// Enviando um pedido de convite de festa
		RECV_PARTY_INVITE_ASK,						// A parte é solicitada.
		SEND_PARTY_INVITE_ANSWER_ACCEPT,			// Resposta convidada quando a festa foi convidada.
		SEND_PARTY_INVITE_ANSWER_REJECT,

		RECV_PARTY_MAKE_ME,							// Eu crio uma festa
		RECV_PARTY_ADD_NEW_MEMBER,					// Novo Party One Information
		RECV_PARTY_MEMBER_LEAVE,					// O membro do partido sai.
		SEND_PARTY_LEAVE,							// Enviando uma retirada de parte
		RECV_PARTY_LEAVE,							// Enviando uma retirada de parte
		RECV_PARTY_MEMBER_BANISH,					// Os membros do partido estão lotados.
		SEND_PARTY_MEMBER_BANISH,					// Enviar membros do partido
		RECV_PARTY_CHANGE_MASTER,					// Mude o salão de festas.
		SEND_PARTY_CHANGE_MASTER,					// Enviando um salão de festas
		RECV_PARTY_DESTROY,							// Desmantelamento da parte
		RECV_PARTY_ITEM_LOOTING_INFO_CHANGE,		// Alterar informações sobre o item da festa Informações de enraizar

		NTF_PARTY_MEMBER_MAP_CHANGE,				// Movimento de um mapa de um mapa
		NTF_PARTY_MEMBER_INFO_CHANGE,				// Mudança de informação da parte um
		NTF_PARTY_MEMBER_DIGIMON_CHANGE,			// Partido Um Digimon Mudança
		NTF_PARTY_MEMBER_ONOFFLINE,					// Partido Um em Informações de OFF
		NTF_PARTY_MEMBER_BUFF_CHANGE,				// Mudança de informação do partido um
		NTF_PARTY_MEMBER_POSITION_UPDATE,			// Renovação de informações de um local da parte um
		NTF_PARTY_EXP_UPDATE,						// Experiência adicional adquirida quando uma parte
		NTF_PARTY_MEMBER_LOOT_ITEM,					// Aquisição de um item da parte um
		OPEN_PARTY_LOOTING_WINDOW,					// Abra o Windows para configurar o método de distribuição de itens de festa
		OPEN_PARTY_ITEMRATE_WINDOW,					// Abra o Windows para configurar itens ao distribuir itens da festa

		GET_PARTY_UIDX,								// Obtenha a festa IDX.
		GET_PARTY_MEMBER_NAME,						// Obtenha o nome do membro do partido
		GET_PARTY_LOOTING_INFO,						// Obtendo as informações atuais de enraizar a parte
		IS_PARTYMEMBER_FROM_TAMERUIDX,				// Verificação do membro do partido
		IS_PARTYMEMBER_FROM_DIGIMONUIDX,			// Verificação do membro do partido

		//////////////////////////////////////////////////////////////////////////
		// Eventos relacionados ao duelo
		SEND_DUAL_REQUEST_OBJIDX,					// Solicitação de aplicação de duelo como número de objeto
		SEND_DUAL_REQUEST_TAMERNAME,				// Em nome de uma solicitação de aplicação de duelo, o nome
		SEND_DUAL_REQUEST_ANSWER,					// Resposta ao pedido de aplicação de duelo.
		RECV_DUAL_REQUEST_RESULT,					// Resultados da aplicação de duelo
		RECV_DUAL_REQUEST_ASK,						// Solicitação de duelo.
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Evento de Hatching
		MAKETACTICS_DIGITAMA_DATA,				//2017-04-25-nova
		MAKETACTICS_DIGITAMA_LV,				//2017-04-25-nova
		MAKETACTICS_DIGITAMA_SC_LV,				//2017-04-25-nova

		GET_MAKETACTICS_DIGITAMA_COUNT,			//2017-04-26-nova
		GET_MAKETACTICS_DIGITAMA_ISENABLE,		//2017-04-26-nova
		GET_MAKETACTICS_DIGITAMA_TYPE,			//2017-04-26-nova
#ifdef MINIGAME
		// 미니 게임
		RECV_MAKETACTICS_DIGITAMA_START_MINIGAME,
		RECV_MAKETACTICS_DIGITAMA_CLICK_RESULT,
		RECV_MAKETACTICS_DIGITAMA_TIME_OUT,
		RECV_MAKETACTICS_DIGITAMA_ERROR_MESSAGE,
		RECV_MAKETACTICS_DIGITAMA_END_MINIGAME,
		RECV_MAKETACTICS_MINIGAME_INIT,

		SEND_MINIGAME_END,
		SEND_MINIGAME_TIMEOUT,
		SENT_MINIGAME_CLICKBTN,
#endif
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		SET_TARGET_MARK,
		RELEASE_TARGET_MARK,
		//////////////////////////////////////////////////////////////////////////
		SEALMASTER_UI_OPEN_CLOSE,					// Ui Heat/Fechar

		RECV_SEALMASTER_SEALINFO,					// Receber informações de vedação
		RECV_SEALMASTER_FAVORITE,					// Receber informações favoritas
		RECV_SEALMASTER_LEADER,						// Líder definido selo
		RECV_SEALOPEN,								// Receber selo
		RECV_RESEALING,								// Receber focas
		SEND_SEALMASTER_LEADER,						// Verificação de configuração do líder
		CHECK_SEALMASTER_UNSEAL,					// Verifique se é possível antes do selo une
		

		RE_SEALING_UISTART,							// Faça o selo novamente e comece a interface do usuário

		GET_SEALMASTER_SELECTSEALID,				// Importando a escolha atualmente Sealid
		GET_SEALMASTER_PLUS_STATE,					// Traga as estatísticas adicionadas

		//////////////////////////////////////////////////////////////////////////
		SEND_GUILD_LEAVE,							// Deixe a guilda (você mesmo)
		SEND_GUILD_INVITE,							// Enviar convite da guilda
		SEND_GUILD_CREATE,							// Enviar guildas
		SEND_GUILD_DESTROY,							// Fechamento da guilda enviando
		SEND_GUILD_BAN,								// Retirada da guilda (apenas guildas podem estar disponíveis)
		SEND_GUILD_CHANGECLASSNAME,					// Envie a alteração do nome da nota Guild
		SEND_GUILDRANK_SET,							// Enviar compromisso no Grau Guild
		SEND_GUILDMEMO,								// Enviar notas da guilda

		RECV_GUILDINFO,								// Recebimento de informações da guilda (movimento/acesso ao mapa)
		RECV_GUILD_MEMBERDELETE,					// Membros da guilda
		RECV_GUILDMEMBER_MAXINC,					// Recebeu o círculo máximo da guilda
		RECV_MAP_NO,								// Número do mapa
		RECV_GUILD_LEAVE,							// Deixando a guilda (você mesmo)
		RECV_GUILD_CREATE,							// Abertura da guilda
		RECV_GUILDNOTICE_CHANGE,					// Mudança de notificação da guilda
		RECV_GUILD_HISTORY,							// Registros de atividade da guilda
		RECV_GUILD_LEVELUP,							// Guilda de nível
		RECV_GUILD_MEMBER_LEVELUP,					// Guild One Level Up
		RECV_GUILD_DESTORY_BC,						// Toda a exclusão da guilda é recebida (BC Broadcast)
		RECV_GUILD_ADD,								// Adicionar guilda
		RECV_GUILD_INVITEFAIL,						// Falha no convite da guilda
		RECV_GUILD_REJECT,							// Recusa de convite da guilda
		RECV_GUILDMEMBER_DELETE,					// Excluir membro da guilda
		RECV_GUILD_TAMERNAME_CHANGE,				// É aplicado ao nome Change Guild
		RECV_GUILDRANK_SET,							// Nomeação de grau de guilda
		RECV_GUILD_ON,								// Guilda em
		RECV_GUILD_OFF,								// Guild offs
		RECV_GUILD_CHANGE_CLASSNAME,				// Mudança de classificação da guilda
		RECV_GUILDSKILL_SET,						// Se deve usar habilidades de guilda
		RECV_GUILD_SKILLPOINT,						// Pontos de habilidade da guilda
		RECV_GUILD_CHANGEMEMO,						// Notas do membro da guilda recebem
		RECV_GUILD_SUMMON,							// Gilma (Bukilma) convocou
		RECV_GUILD_SKILL_RESET,						// 2017-06-29-NOVA

		CHECK_INCREASE_GUILDMEMBERMAX,				// Inspeção antes do uso
		CHECK_GUILD_DESTROY,						// Declaração de material de exclusão da guilda
		SET_GUILD_PERSONLEVEL,						// Mudança de um nível da guilda

		GET_GUILDINFO,								// Importar informações da guilda
		GET_GUILD_MYINFO,							// Importar minhas informações da guilda
		GET_GUILD_PERSON_INFO,						// Importar Guild One Information
		GET_USINGSKILL_MAP,							// Mapa de importação armazenado com a habilidade em uso

		GET_SET_PASIVESKILL_COLOR,					// Defina a cor da habilidade passiva da guilda e importe -a.
		GET_USING_SKILL,							// Importação de habilidades em uso
		GET_GUILDRANK_NAME,							// Traga nomes de classificação da guilda
		GET_GUILDRANK_DECRIPT,						// Traga a descrição da classificação da guilda

		GET_ISACTIVESKILL,							// Trazendo a propriedade de habilidades ativas
		GET_USINGSKILLVALUE,						// Traga o valor da mudança devido à habilidade em que está sendo aplicada


		//////////////////////////////////////////////////////////////////////////
		NTF_USER_MESSAGE,
		//////////////////////////////////////////////////////////////////////////

		TOGGLE_DAMAGEMETER_WINDOW,					// Alternação da janela do medidor de dano
		ADD_DAMAGEMETER_DATA,						// Conjunto de dados para danificar o medidor

		DIGIMON_BATTLE_ON,
		DIGIMON_BATTLE_OFF,
		//////////////////////////////////////////////////////////////////////////
		// @@ Até aqui

		PERSON_ITEM_SELL_ADD,						//2017-05-02-nova
		PERSON_ITEM_BUY,							//2017-05-02-nova
//		PERSON_SET_STORENAME,						//2017-05-08-nova
//		PERSON_GET_STORENAME,						//2017-05-08-nova
//		PERSON_GET_VAULETYPE,						//2017-05-23-nova
		REQ_PERSON_OPEN,							// 개인/위탁 상점을 열겠다고 서버에 요청함.
		
		OPEN_PERSONSTORE,							//2019-01-04-leejiu
		OPEN_COMMISSIONSTORE,						//2019-01-04-leejiu
		CLOSE_PERSONSTORE_MSGBOX,
		REGIST_PERSONSTORE_ITEM,
		
		RECV_STORE_READY_RESULT,					//2019-01-23-leejiu
		RECV_OPEN_PERSONSTORE,						//2019-01-23-leejiu
		RECV_OPEN_COMMISSIONSTORE,					//2019-01-23-leejiu
		RECV_BUY_ITEM_RESULT,						//2019-01-23-leejiu
		RECV_PERSONSTORE_ITEMLIST,
		RECV_COMMISSIONSTORE_ITEMLIST,
		RECV_CLOSE_PERSONSTORE,
		RECV_CLOSE_COMMISSIONSTORE,

		UNION_ITEM_ADD_DATA,						//2017-05-02-nova
		UNION_MONEY_SET_DATA,						//2017-05-02-nova
		UNION_SET_SHOPDATA,							//2017-05-08-nova
		UNION_SET_MODELID,							//2017-05-08-nova
		UNION_RELEASE_DATA,							//2017-05-08-nova
		UNION_INITNET_OFF,							//2017-05-08-nova
		UNION_DELETE_DATA,							//2017-05-08-nova
		UNION_GET_CHAR_MAPID,						//2017-05-08-nova
		UNION_GET_CHAR_POS,							//2017-05-08-nova
		UNION_GET_CHAR_MODELID,						//2017-05-08-nova
		RECV_UNION_SHOP_WITHDRAW,					

		ACHIEVE_SET_DISPTITLE,						//2017-06-08-nova
		ACHIEVE_GET_DISPTITLE,						//2017-06-08-nova
		ACHIEVE_SET_RANKACHIEVE,					//2017-06-12-nova
		ACHIEVE_SET_CHECKTYPE,						//2017-06-12-nova

		//////////////////////////////////////////////////////////////////////////

		USING_MACROPROTECT,							// Macro Protect é inspecionado pelo alvo do alvo
		USING_MACROPROTECT_TARGET_UIDX,				// Macro Protect é inspecionado pelo Target UIDX
		SEND_MACROPROTECT_QUESTION,					// Envie uma solicitação de proteção de macro para um usuário específico no servidor
		NTF_MACROPROTECT_QUESTION_REQUEST,			// Peço uma macro proteger.
		RECV_MACROPROTECT_QUESTION_RESULT,			// O resultado do jogador que solicitei para Macro Protect.
		RECV_MACRO_PLAYER_RESULT,					// O resultado de um macro player ou não
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		// Eventos relacionados ao sistema de correspondência de batalha
		OPEN_BATTLEMATCH_REGIST_WINDOW,				// Abra a janela do aplicativo de batalha
		RECV_BATTLEMATCH_MY_INFO,					// As informações de correspondência de batalha foram recebidas do servidor.
		RECV_BATTLEMATCH_REGISTER_RESULT,			// Resultados de aplicativos de correspondência de batalha do servidor.
		RECV_BATTLEMATCH_REGISTER_CANCEL,			// Resultados de cancelamento de aplicativos de correspondência de batalha do servidor.
		NTF_BATTLE_MATCH_READY_REQUEST,				// Solicitações do servidor se você pode participar da partida de batalha.
		RECV_BATTLEMATCH_READY_STATE,				// O resultado do status de preparação de batalha
		NTF_BATTLEMATCH_READY_STATE,				// O resultado do status de preparação de batalha de outros jogadores
		NTF_BATTLEMATCH_CANCEL,						// Cancelamento e outras correspondências na preparação de batalha de outros jogadores.
		NTF_BATTLEMATCH_COMPLETE,					// Pacote de sucesso correspondente
		NTF_BATTLE_START,							// Battle Start
		NTF_BATTLE_END,								// Battle End
		NTF_BATTLE_REWARDS,							// Compensação de batalha
		IS_BATTLEMATCH_REGISTED,					// Está se candidatando a correspondência de batalha
		IS_MY_BATTLE_TEAM,							// É a mesma equipe de batalha como eu?
		IS_BATTLE_PLAYING,							// Você está em batalha?
		RECV_ADD_BATTLE_POINT,						// Pontos de batalha obtidos usando ingressos de batalha
		//////////////////////////////////////////////////////////////////////////
		OPEN_SERVER_RELOCATE_PAGE_UI,					// Alteração do servidor Calor da interface do usuário
		RECV_SERVER_RELOCATE_CHECK_TAMERNAME_RESULT,	// O resultado de se o nome pode ser criado no servidor que você deseja transferir
		RECV_SERVER_RELOCATE_ITEMUSE_RESULT,			// O resultado do pacote de solicitação no servidor para transferir
		IS_SERVERR_RELOCATE_PAGE_SHOW,					// Verifique se você está assistindo a página de transferência do servidor
		//////////////////////////////////////////////////////////////////////////

		RECV_QUEST_INFO_INIT,							// Recebi informações de missão do servidor.
		TUTORIALQUEST_ASK_CANCEL,						// Relacionado ao abandono da missão do tutorial
		TUTORIALQUEST_ACCEPT,							// Aceitação da missão do tutorial
		TUTORIALQUEST_CANCEL,							// Abandono da missão do tutorial
		TUTORIALQUEST_COMPLETE,							// Conclusão da missão do tutorial
		TUTORIALQUEST_PROCESS_ALL_COMPLETE,				// Todas as condições completas do tutorial de progresso estão concluídas 瑛?
		TUTORIALQUEST_PROCESS_UPDATE,					// Atualização na missão do tutorial em andamento
		TUTORIALQUEST_DIGIVICE_CHECK,					// Chamado quando você recebe a missão Digiis.
		TUTORIALQUEST_SKIP_ACCEPT,						// Abandono da missão do tutorial
		GET_IS_TUTORIAL_PLAYING,						// Traga -o em um tutorial.
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		OPEN_INFINITEWAR_START_WINDOW,						// Abra as janelas começando em guerra infinita
		OPEN_INFINITEWAR_GIVEUP_WINDOW,						// Abra o fim da guerra infinita
		OPEN_INFINITEWAR_NOTICEBOARD_WINDOW,				// Abra as janelas infinitas do quadro de guerra

		RECV_INFINITEWAR_CHALLENGESTATECLEAR,				// Estágio infinito de guerra claro
		RECV_INFINITEWAR_CHALLENGETRY_NEXTSTAGE,			// Guerra Infinita no próximo estágio
		RECV_INFINITEWAR_CHALLENGE_GIVEUP,					// Guerra Infinita
		RECV_INFINITEWAR_MAPMOVE_FALSE,						// Falha do mapa infinito daejeon

		RECV_INFINITEWAR_NOTICEBOARD_DATA,					// Receba informações do quadro de avisos.
		RECV_INFINITEWAR_NOTICEBOARD_POINT_PUT_RESULT,		// Resultados para entrada de pontos
		NTF_INFINITEWAR_NOTICEBOARD_DATA_UPDATE,			// Renovação de informações do quadro de avisos

		RECV_INFINITEWAR_NOTICEBOARD_RANKER_LIST,			// Lista de estrelas semanal, mensal e da temporada
		RECV_INFINITEWAR_NOTICEBOARD_PRE_RANKER_LIST,		// Lista semanal, mensal e de temporada
		RECV_INFINITEWAR_NOTICEBOARD_FAME_LIST,				// Lista sazonal do Hall of Fame Ranker
		RECV_INFINITEWAR_SEASON_FAME_LIST,					// Lista de fama sazonal
		//////////////////////////////////////////////////////////////////////////

		FADE_IN_END,
		FADE_OUT_END,

		//////////////////////////////////////////////////////////////////////////
		ON_CANCEL_INCUBATING,

		REQ_START_INCUBATING,
		REQ_STOP_INCUBATING,
		REQ_UNREGISTER_INCUBATED_DIGIMON,
		REQ_REGISTER_DIGIMON_TO_ACTIVATE_INCUSLOT,
		REQ_CHANGE_DIGIMON_IN_ACTIVATE_INCUSLOT,
		REQ_INCUBATOR_ITEM_IN_INVENTORY,
		REQ_SHOWVIEWER_BY_NPC,
		REQ_SHOWVIEWER_BT_ITEM,
		REQ_CHANGE_DIGIMON,
		REQ_HIDE_VIEWER,

		RECV_ARCHIVE_OPENCNT,
		RECV_ARCHIEVE_DIGIMONS,
		RECV_END_ARCHIEVE_DIGIMONS,
		RECV_INIT_ARCHIVE_INFO,
		RECV_ON_EXTENT_ARCHIVE,
		RECV_INCUBATOR_INFO,
		RECV_CHANGE_DIGIMONS_ARCHIVE,
		RECV_MOVE_DIGIMONS_IN_ARCHIVE,
		RECV_CHANGE_INCUBATOR,
		RECV_START_INCUBATING_INFO,
		RECV_STOP_INCUBATING_INFO,
		RECV_ON_EXTENT_ACCOMPANY_SLOT,
		RECV_ON_CHANGE_PARTNER_DIGIMON,
		//////////////////////////////////////////////////////////////////////////
		CHANGE_EQUIP,
		Recv_Ntf_XGuage_Update,
		Recv_XGToCrystal_Result,
		Recv_CrystalToXG_Result,
		Recv_Ntf_XGuage_Max_Update,

		CHANGE_QUICK_EXPAND,

		SET_BALLOON_TITLE,

		//////////////////////////////////////////////////////////////////////////
		NTF_NEON_DISPLAY,	//  Receba um pacote de Billboard.

		//////////////////////////////////////////////////////////////////////////
		REQ_SHOW_NEWCASHSHOP,
		REQ_HIDE_NEWCASHSHOP,

		RECV_CASHSHOP_STORAGE,
		//RECV_CASHSHOP_CART,
		SEND_CASHSHOP_ITEM_CROP,
		RECV_CASHSHOP_ITEM_CROP,
		SEND_CASHSHOP_ITEM_PRESENT,
		RECV_CASHSHOP_ITEM_PRESENT,
		CLOSE_CASH_ITEMBUY_WINDOW,

		REQUEST_STEAM_CASHITEM_PURCHASE,
		RECV_REQUEST_STEAM_CASHITEM_PURCHASE,	// Resultados intermediários Ao comprar dinheiro a vapor.

		SEND_CASHSHOP_ITEM_PURCHASE,			// Evento de compra de item em dinheiro
		RECV_RESULT_CASHITEM_PURCHASE,			// Resultados da compra do item
		RECV_RESULT_HAVE_CASH_REFRESH,			// Resultados da solicitação de informações em dinheiro
		RECV_CASHSHOP_BUY_HISTORY,			
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		OPEN_EXTRA_EVOLUTION_STORE,
		CLOSE_EXTRA_EVOLUTION_STORE,
		ASK_DIGIMON_TO_ITEM,
		ASK_ITEM_TO_DIGIMON,
		//SEND_DIGIMON_TO_ITEM,
		SEND_ITEM_TO_DIGIMON,
		RECV_ITEM_TO_DIGIMON,
		RECV_DIGIMON_TO_ITEM,
		//////////////////////////////////////////////////////////////////////////
		RECV_VIP_SYSTEM_DATA,
		ON_VIP_SYSTEM_CONTENTS,

		//////////////////////////////////////////////////////////////////////////
		RECV_NTF_HOTTIME_EVENT_INFO,
		RECV_HOTTIME_EVENT_ITEM_GET,

		SET_ACTIONKEY_NPC,
		SET_GIFTBOX_WINDOW,
		UPDATE_GIFTBOX,

		//////////////////////////////////////////////////////////////////////////
		OPEN_INVENTORY,
		CLOSE_INVENTORY,
		SET_ENABLE_INVENTORY,
		GET_POS_DEPENDON_INVEN,
		ADD_NEWITEM_INVENTORY,
		RECV_INVEN_SORT_END,

		RECV_INVEN_REQ_RESULT,
		RECV_WAREHOUSE_REQ_RESULT,

		//////////////////////////////////////////////////////////////////////////
		OPEN_CLOSE_TAMERSTATUS,
		UPDATE_TAMERSTATUS,

		OPEN_CROSSLOADER,
		CHANGE_CROSSINVEN_SIZE,
		CHANGE_CROSSDIGIMON,

		REQUEST_DELETE_DIGIMON,

		USE_GAMESCANNER,
		//////////////////////////////////////////////////////////////////////////
		OPEN_CLOSE_DIGIMONSTATUS,
		UPDATE_STAT_DIGIMONSTATUS,
		UPDATE_SKILL_DIGIMONSTATUS,
		CHANGE_DIGIMON_DIGIMONSTATUS,
		RECV_SKILL_LEVELUP_DIGIMONSTATUS,
		//////////////////////////////////////////////////////////////////////////
		UPDATE_TARGET_HPRATE,
		//////////////////////////////////////////////////////////////////////////
		
		RECV_DIGIMON_TRANSCENDENCE_CHARGE_EXP,
		RECV_DIGIMON_TRANSCENDENCE_HATCH_UP,
		//////////////////////////////////////////////////////////////////////////
		SEND_OPEN_GOTCHAMACHINE,
		RECV_OPEN_GOTCHAMACHINE,
		SEND_GET_ITEM_MACHINE,
		RECV_GET_ITEM_GOTCHAMACHINE,
		RECV_GET_ITEM_MYSTERYMACHINE,
		RECV_ERROR_GOTCHAMACHINE,

		GET_ITEM_GOTCHAMACHINE,
		CLOSE_MACHINE,
		OPEN_MYSTERYMACHINE,
		//////////////////////////////////////////////////////////////////////////
		SEND_ELEMENT_ITEM_CHECK,
		RECV_ELEMENT_ITEM_CHECK,
		RECV_ELEMENT_ITEM_ATTACH,
		RECV_ELEMENT_ITEM_DETACH,

		OPEN_ELEMENT_ITEM_NPC,
		OPEN_ELEMENT_ITEM_PORTABLE,
		CLOSE_ELEMENT_ITEM,

		REGIST_ELEMENT_ITEM_CHECK,
		REGIST_ELEMENT_ITEM_EQUIP,

		//////////////////////////////////////////////////////////////////////////
		OPEN_EQUIPSYSTEM_NPC,
		OPEN_EQUIPSYSTEM_PORTABLE,
		CLOSE_EQUIPSYSTEM,

		RECV_CHARGE_ITEM_LIST,
		RECV_CHARGE_ITEM_NORMAL_RESULT,
		RECV_CHARGE_ITEM_QUICK_RESULT,

		SEND_CHARGE_ITEM_CANCEL,
		RECV_CHARGE_ITEM_CANCEL,

		SEND_CHARGE_ITEM_WITHDRAW,
		RECV_CHARGE_ITEM_WITHDRAW,

		SEND_CHARGE_ITEM_UNLOCK,
		RECV_CHARGE_ITEM_UNLOCK,

		//////////////////////////////////////////////////////////////////////////
		RECV_ENCHANT_OPTION_IDENTIFY_RESULT,
		RECV_ENCHANT_OPTION_ENCHANT_RESULT,
		START_ENCHANT_OPTION_ENCHANT,
		//////////////////////////////////////////////////////////////////////////

		REQ_CHANGETAMER_USINGITEM,
		OPEN_CHANGETAMER_WINDOW,
		SEND_CHANGETAMER_CHANGE,

		//////////////////////////////////////////////////////////////////////////
		UPDATE_DAILYCHECKEVENT_TIMER,
		RECV_DAILYCHEKEVENT_INFO_RESULT,
		RECV_DAILYCHECKEVENT_RESULT_ITEM,
		//////////////////////////////////////////////////////////////////////////
		// Tutorial Event
		SKIP_TUTORIAL_EVENT,
		//////////////////////////////////////////////////////////////////////////
		RECV_QUEST_ALL_INFO,		// Renovação de informações da missão

		MAX_EVENT
	};
}
