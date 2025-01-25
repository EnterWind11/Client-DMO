

#pragma once

#include "nScope.h"


namespace nGame
{
	enum
	{
		ExpEvent = 0x00000001,	// Evento de experiência em andamento

	};
};

namespace nLoginType
{
	enum
	{
		DigiTalic	= 0x00000001,
		PcBang		= 0x00000002,
		PlayNet		= 0x00000004,
		TooNiland	= 0x00000008,
		IMBC		= 0x00000010,
		MGAME		= 0x00000020,
		ZAM			= 0x00000040,
		STEAM		= 0x00000080,
		HANGAME		= 0x00000100,
	};

	enum EVENT_TARGET
	{
		EVENT_DIGITALIC	= 0,
		EVENT_PLAYNET	,
		EVENT_TOONILAND	,
		EVENT_IMBC,
		EVENT_MGAME,
		EVENT_STEAM,
		EVENT_HANGAME,
		EVENT_TARGET_MAX,
	};

	inline bool is_DigiTalic(int compareVal)	{	return ( DigiTalic == (compareVal & DigiTalic) ); }
	inline bool is_PcBang(int compareVal)		{	return ( PcBang == (compareVal & PcBang) ); }
	inline bool is_PlayNet(int compareVal)		{	return ( PlayNet == (compareVal & PlayNet) ); }
	inline bool is_TooNiland(int compareVal)	{	return ( TooNiland == (compareVal & TooNiland) ); }
	inline bool is_IMBC(int compareVal)			{	return ( IMBC == (compareVal & IMBC) ); }
	inline bool is_MGAME(int compareVal)		{	return ( MGAME == (compareVal & MGAME) ); }
	inline bool is_ZAM(int compareVal)			{	return ( ZAM == (compareVal & ZAM) ); }
	inline bool is_STEAM(int compareVal)		{	return ( STEAM == (compareVal & STEAM) ); }
	inline bool is_HANGAME(int compareVal)		{	return ( HANGAME == (compareVal & HANGAME) ); }
};

namespace nPlayTime
{
	enum
	{
		CASH_PLAYTIME = 0,
		NORMAL_PLAYTIME,
	};

	enum
	{
		NO_PANELTY	= 1,
		LEVEL1_PANELTY,
		LEVEL2_PANELTY,

		MAX_PENALTY = 3,
		USE_CASH	= 100,
	};
};

namespace GameServiceCode
{
	enum
	{
		SERVICECODE_COMMISSIONSHOP = 0,
		SERVICECODE_NEW_BUFF,

		MAX_SERVICE = 2,
	};
};


namespace pGame
{
	enum
	{
		Begin = nScope::Game,

		Ready,				// Completamente pronto para começar o jogo
		SvrInfo,			// Processamento de informações relacionadas ao servidor de jogo

		InitData,			// Suas próprias informações no começo

		MoveTo,				// Mova para o ponto de destino e depois vá para o próximo ponto de alvo
		NewPath,			// Altere o ponto alvo em movimento

		Sync,
		HpRate,				// Tratamento para conhecer o HPRATE circundante, não enviado a si mesmo

		Chat,
		Whisper,
		WhisperFailure,		// Som quando os cavalos falham

		MsgAll,				// Toda a transmissão do servidor, item (tipo: 159) necessária
		MsgSystem,			// Mensagem do sistema, para todo o pessoal do cluster

		Attack,				// ataque de A a B
//		AttackShape,		// Ataque, apenas aparência
//		AttackDmgInfo,		// Ataque, apenas níveis de dano
		AttackMiss,			// Ataque Miss A a B

		Skill,				// Habilidade é de A a B

		SetTarget,			// Ele permite adquirir informações sobre a configuração de destino, destino.
		TargetInfo,			// HP da Target, sincronize as informações do DS

		GetExp,
		LevelUp,

		Die,
		SkillDie,				// Em caso de morte por habilidade
		DieConfirm,				// O cliente solicita a solicitação de confirmação da morte (posteriormente transferida para a vila.)

		HealUp,					// No futuro, HP e DS são todos processados.
		FsChange,				// Aumento da intimidade

		EvoCard,				// Equipado com cartão evolutivo
		EvoCrest,				// Equipado com frase evolutiva
		EvoOpen,				// Abertura do slot evolutivo
		Evolution,				// Evolução, degeneração
		EvoFailure,				// Evolução, falha de degeneração

		StatUp,					// Statup
		ChangeStatValue,		// Ao alterar os resultados das estatísticas

		Stop,					// Pare por 3 segundos.
		StopAttack,				// Pare o ataque.

		BattleOn,				// O modo de batalha começa
		BattleOff,				// Modo de batalha final

		HatchIn,				// Hospedando registro Digitama
		HatchUp,				// Hashing Digimon Data Entrada
		HatchOut,				// Verifique a eclosão
		HatchCancel,			// Cancelamento
		HatchFailure,			// Hatching

		ChangePartner,			// Alteração do Digimon mercenário
		DeletePartner,			// Remoção do mercenário

		AllStat,
		MoveBossMonster,		// Boss Monster Move
		WalkBossMonster,

		RefreshScreen,
		StartMessage,			// Primeira mensagem de conversa no início do jogo

		MsgAllHatchOut,			// Quando eclodir sucesso
		MsgAllCropEventItem,	// Aquisição de certificados de presente cultural, etc.

		ChangeChannel,			// Solicitação de mudança de canal e resposta
		OpenRegion,				// Ao entrar em uma nova área

		DiedAlready,			// Já morto

		Ting,					// O monstro antecessor se encontra

		ChangeState,			// O valor do estado atual do jogo

		OpenEvoSlot,			// Use o item de abertura do Evolutor ou Burst no modo

		MsgShout,				// Gerado

		JumpGate,				// Uso do portão de salto

		Emoticon,				// Emoticon -Sinnchronização de emoções

		AdvertBossCount,		// Número de monstros chefes até que eles nascem
		AdvertBossCreate,		// Notificação do pessoal do mapa para criar um chefe
		AdvertBossDie,			// Notificação da morte de monstro -chefe

		Event,					// Sincronização de eventos que só podem ser pulverizados na tela atual

		OpenRideMode,			// Função de pilotagem Digimon aberta

		MacroQuestion,			// Solicite uma confirmação de macro de jogo e remova um item macro.
		MacroAnswer,			// Resultados de confirmação da macro do jogo (resposta a perguntas macro)
		MacroFailure,			// Processamento relacionado a falhas de processamento de macro (Falha ao voar para o assunto usando itens)
		MacroSuccess,			// Solicitação de confirmação de macro bem -sucedida (perguntas de macro bem -sucedidas para metas usando itens)
		MacroResult,			// Resultados de confirmação de macro (sucesso se o alvo for macro ou falhar)

		AwayTime,				// Solicitação de status de esvaziamento
		SyncCondition,			// O estado atual do domador

		MsgAllFailure,			// O servidor grito -Error

		HatchGage,				// Processamento do medidor de correção de perda
		ShopInfo,				// Solicitação de transmissão de informações do armazenamento de consignação (jogo <-> núcleo)

		MsgAllByItem,			// Gerado
		DigimonInchant,			// Tentativas de fortalecer o Digimon (neste momento, as informações de material registradas são entregues ao cliente.)
		SaveClientOption,		// Salvar informações da opção do cliente

		reserve_1077,			// reserva

		PlayTimeCheck,			// Mudança de tempo de reprodução e cheque
		PlayTimePaneltyChange,	// Mudança de penalidade de tempo de jogo

		QueryMoneyInven,		// Informações de inventário e informações de reprodução de re -transmissão (usado apenas para teste)
		ReloadData,				// Renovação do arquivo de dados
		GMHatch,				// Hatching de ovos

		DecreaseDigimonInchant,	// Informações de aprimoramento D ++ Digimon
		ChangeStat,				// Mudança de estatística
		KnockBack,				// Posição de knockback
		BattleEvolPoint,		// Ponto de evolução da batalha

//#pragma todo("Pacote de batalha será excluído")
		//BattleMatchInfo,			// Informações relacionadas à batalha
		//BattleMatchRequest,		// Inscrição
		//		
		//BattleMatchCancel,		// Cancelamento da participação de batalha

		//BattleRoundResult,		// resultado
		//BattleUpdateRoundInfo,	// Atualização redonda

		//BattleMatchFail,			// Falha de batalha
		//BattleMatchReady,			// Batalha
//#pragma todo("Exclua pacote de batalha")
		ClientEventSave,		// Informações do evento do cliente Salvar solicitação
		ViewEventSeen,			// Assista à cena do evento
		DotDie,					// Transferência para morreu por ponto

//#pragma todo("Pacote de batalha será excluído")
		//DualBattleRequest,		// Confronto
		//DualBattleResponse,		// Confronto
		//DualBattleFail,			// Confronto
//#pragma todo("Exclua pacote de batalha")

/************************************************************************/
/* Proibição de pacotes adicionais                                                  */
/************************************************************************/
/************************************************************************/
/* Proibição de pacotes adicionais                                                  */
/************************************************************************/
/************************************************************************/
/* Proibição de pacotes adicionais                                                  */
/************************************************************************/

		End
	};

}; // namespace pGame

namespace pGameTwo
{
	enum
	{
		Begin = nScope::GameTwo,

		RecompenseList,		// Solicitação de lista de compensação de inspeção do NPC
		RecompenseGain,		// Aquisição de compensação de inspeção do NPC
		RecompenseGainFail,	// Falha na aquisição de recompensa
		RecompenseChange,	// Renovação devido a atualizações de lista anteriores
		RecompenseRemove,	// Remoção da lista de recompensa
		AllMsg,				// Recém -notado
		DoorObjectOpen,		// De porta em porta
		HackShildCheck,		// Cheque de Shield Hack
		GameGuardCheck,		// Cheque de guarda de jogo

		SkillAction,		// Monster_skill_renewal_20150715 Separação de pacotes de habilidade
		SkillHit,			// Monster_skill_renewal_20150715 Separação de pacotes de habilidade

		ErrorMessage,		// Falha
		SvrInfoFailed,		// Falha na operação do servidor
//#pragma todo("Pacote de batalha será excluído")
		//DualBattleSucc,			// Confronto		
//#pragma todo("Exclua pacote de batalha")

		DigimonInchantData,		// Notificação de informações após dedução

		BurningEventAddExpRate,	// Notificação de experiência adicional (proporção) para eventos de queima

		MacroBanCountSaveNotice,// Notificação do armazenamento de inimigos de Macrobannu

		CheckTamerName,
		CheckTamerResult,

		Result,	// Usado em comum em relação à transferência do servidor Tamer

		//Servidor servidor
		ArenaRankingReqTopCur,
		ArenaRankingReqTopOld,
		ArenaRankingReqFame,

		//Servidor-CLA
		ArenaRequestRank,
		ArenaRequestOldRank,
		ArenaRequestFame,

		ArenaRequestFameList,

		SkillHitEffect,

		DieLog,
		
		ResourceIntegrity,

		//X Relacionado ao anticorpo
		XGToCrystal,	//X Gauge-> cristal
		CrystalToXG,	//Crystal-> x Gentro
		XGuageUpdate,	//X Gauge yang renovação
		XGuageMaxUpdate,//Calibre x, a quantidade máxima de cristais

		NeonDisplay,

		XignCodeCheck,
		RegionBuff,		//Buff-local-local entre mapas, temporário
		MacroCheckOtherMap,
		InventoryInfo,
		DigimonTranscendenceChargeEXP,
		DigimonTranscendence,

		End // = 20000 // Espero que possa ser usado até 200000
	};
};

namespace nErrorMessage
{
	enum eErrorMessage
	{
		eErrorMessage_Make_CountOver = 0,				// Erro de solicitação de produção Erro
		eErrorMessage_Make_ItemCountOver,				// Item de produção Número de erros do item de produção
		eErrorMessage_Make_NoneResource,				// Sem recurso da tabela de produção
		eErrorMessage_Item_NoneResource,				// Nenhum recurso de tabela de itens
		eErrorMessage_Make_NotEnoughInvenSlot,			// Falta de produção no espaço Ben
		eErrorMessage_Make_NotEnoughMoney,				// Falta de produção
		eErrorMessage_Make_NotEnoughMaterialItem,		// Falta de materiais de produção
		eErrorMessage_Repurchase_NotEnoughInvenSlot,	// Falta de espaço Ben no item de recompra
	};
};

namespace pMiniEvent
{
	enum
	{
		Begin = nScope::MiniEvent,
		PiemonGameStart,		// Piemon Card Game Start Mensagem
		PiemonGameEnd,			// End of Piemon Card Game
		//PiemonGameFail,		// Piemon Card Game termina após o final do trabalho (2012 10 08 ~
		End
	};
};//(2012 10 09)

namespace pMiniGame
{
	enum
	{
		Begin = pMiniEvent::End+1,
		MiniGameStart,		// Minigame Start
		MiniGameClickBtn,	// Clique no botão de parada durante o mini jogo
		MiniGameTimeOut,	// Deve ter sido com o tempo sem clicar no botão
		MiniGameEnd,		// Mini Game End
		MiniGameLimit,		// Restrição

		MiniGameIsStartReq,	// Solicitação para iniciar um mini jogo
		MiniGameIsStartAcc, // Se o mini -jogo começa é bom
		End
	};
};

#define d_nWhisperNotConnected	(u1)0	// Não estar conectado
#define d_nWhisperMapChange		(u1)1	// Mapa no mapa
#define d_nWhisperNotFound		(u1)2	// Usuários que não existem
#define d_nWhisperSuccess		(u1)3	// Transmissão e recebimento bem -sucedidos








