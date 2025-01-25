


#pragma once

#include "nScope.h"

namespace pGuild
{
	enum
	{
		Begin		= nScope::Guild,

		Create,			// Criação da guilda
		Close,			// Fechamento da guilda

		Allow,			// Adicione a permissão de associação
		AllowFailure,	//
		Reject,			// Rejeição de membros adicionais

		Delete,			// Excluir membro (acima da aposentadoria)
		Leave,			// secessão

		Add,			// Adicionou o domador à guilda
		ReqAdd,			// Pedido adicional
		ReqAddFailure,	// O oponente já se juntou à guilda.
						// ncode == 1;==> O oponente já pertence a outra guilda.
						// ncode == 2;==> um oponente que não está conectado ou um oponente que não existe
						// ncode == 3;==> A outra pessoa está em tarefas diferentes
						// ncode == 4;==> Assinantes da guilda cheios

		On,				// Observe que está online e você precisa dizer qual mapa está online.
		Off,			// Notificação de estar offline

		Info,			// Informações da guilda e solicitação de lista de membros e resposta

		Chat,			// Bate -papo entre membros da guilda

		ToSubMember,	// Membro associado
		ToMember,		// Relegado a um membro geral
		ToDatsMember,	// Ponto
		ToSubMaster,	// Promoção para Bukilma
		ToMaster,		// Transfira para o assento mestre para outro

		ChangeMap,		// Ao mudar o mapa ou canal
		LevelUp,		// Guilda de nível

		MemberLevelUp,	// Sincronização necessária para os membros da guilda de nível para cima

		ChangeTName,	// Alteração do nome do domador
		DeletedTamer,	// Tamer é removido da janela de seleção de personagens

		AddExp,			// Aquisição da experiência da guilda

		ChangeNotice,	// Mudança de notificação da guilda

		RankChange,		// Mudança de classificação
		History,		// História da guilda,

		ChangeClassName,	// Alterar o nome da classe (mestre, Bumaster ...).

		IncMember,		// O maior número de guildas por item
		GSPLevelUp,		// Ponto de habilidade da guilda Nível para cima
		GSPUse,			// Use pontos de habilidade da guilda
		Memo,			// Memorando da guilda
		Shout,			// Loudspeaker da guilda
		Summon,			// Convocar guild um grupo
		GSkill,			// Use habilidades da guilda
		GSkillRemove,	// Liberação de habilidades da guilda
		SkillError,		// Falha do uso da habilidade da guilda

		ChangeMemo,		// Mudança de memorando do membro da guilda

		End
	};
};



namespace nGuild
{
	enum
	{
		Master		= 1,	// mestre
		SubMaster	= 2,	// Boo Master

		DatsMember	= 3,	// Membro DOTZ
		Member		= 4,	// membro
		SubMember	= 5,	// Membro associado
	};

	namespace History
	{
		enum
		{
			ToMaster		= 1,	// Mestre da guilda
			ToSubMaster		= 2,	// Bulgilma
			ToDatsMember	= 3,	// Dotz
			ToMember		= 4,	// Em geral
			ToSubMember		= 5,	// Membro associado

			Create			= 100,	// Fundação

			Add				= 101,	// Junte-se
			Leave			= 102,	// secessão
			Delete			= 103,	// Pendente

			Close			= 104,	// Fechamento da guilda
		};
	};	

	namespace GetPoint // Como obter contribuições da guilda
	{
		const int MONSTER_HUNT_COUNT = 100;		// Se você pegar 100 monstros 4

		enum
		{
			RaidHunt		= 1,	// Caça a monstros de ataque
			CompQuest		= 2,	// Conclusão da missão
			HatchEgg		= 3,	// Alfabético
			MonsterHunt		= 4,	// Se você pegar n monstro general n
		};
	};
	namespace SkillCode
	{
		enum{
			GuildShout		= 2600001,	// Loudspeaker da guilda
			NameColor		= 2600004	// Código de habilidade de mudança de cor da guilda
		};
	};

};


struct stGuildSkill
{
	u4 nSkillCode;					// Código de habilidade
	u4 nGSP;						// Consumo GSP
	u2 nStep;						// Estágio de habilidade
	u2 nLevel;						// Nível de requisito
	u2 nGroup;						// ID do grupo de habilidades
	u2 nValue;						// Habilidade aumenta o valor
	u2 nTimeS;						// Duração [segundos]
	u2 nTimeM;						// Duração [minutos]
	u2 nCoolS;						// Tempo legal [segundos]
	u2 nCoolM;						// Recarga [min]
	u1 nSection;					// Classificação de habilidade (0 se for 0, 1 se 1)
};
struct stGuildSkillCore
{
	u4 nSkillCode;					// Habilidade no gatilho
	u4 nStartTime;					// Tempo de habilidade
};