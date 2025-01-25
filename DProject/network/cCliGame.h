
#pragma once

#include "nlib/cClient.h"
//#include "cNetwork.h"

#include "common_vs2022/pDebug.h"
#include "common_vs2022/pType.h"
#include "common_vs2022/pSync.h"
#include "common_vs2022/pLength.h"
#include "common_vs2022/pTrade.h"


class CPlayer;
class CDigimon;
class CMonster;

class cCliGame : public cClient
{
public:
	cCliGame(void);
   ~cCliGame(void);

public:
	bool start(void);
	void send(void);

public:
	void OnConnected(void);
	void OnExecute(void);
	void OnDisconnected(char *msg);

public:
	char ip[Language::pLength::ip];
	uint port;
	uint m_nConnectedTickCount;
	uint m_nIsChangeServer;

	TCHAR m_szTamerName[Language::pLength::name+1];

	CsC_AvObject*	m_pTargetObject;

public:
	bool m_bPortalProcessing;
	bool m_bPortalRequesting;
	bool m_bIsReadyCompleted;
/////////////////////////////////////////////////////////
// receive
private:
	void RecvVersionError(void);
	void RecvStartMessage(void);

	void RecvInitGameData(void);
	void RecvBossGenInfo(void);

	void RecvLocalPortalSuccess(void);
	void RecvSelectPortalSuccess(void);
	void RecvSelectPortalFailure(void);
	void RecvChangeServer(void);	// Ao usar o portal, você pode se conectar ao próximo servidor.

	void RecvTing(void);			// Se uma multidão de antecessor me encontrou
	void RecvSyncData(void);
	void RecvCropFailure(void);

	void RecvGameEvent(void);		// Folga
	void RecvAttendance(void);		// Reconhecimento de participação + Detalhes do pagamento do item

	void RecvNtfHotTimeEvent(void);
	void RecvHotTimeGetItemResult(void);

	void RecvAdvertBossCount(void);
	void RecvAdvertBossDie(void);
	void RecvAdvertBossCreate(void);

	void RecvAdvertBurstMode(void);

	void RecvItemTimeout(void);
	void RecvItemBuy(void);
	void RecvItemSell(void);
	void RecvItemCropSuccess(void);
	void RecvCropMoney(void);
	void RecvItemCropParty(void);

	void RecvItemUseSuccess(void);
	void RecvItemUseFailure(void);
#ifdef CROSSWARS_SYSTEM
	void RecvCrossItemUseSuccess(void);
	void RecvCrossItemUseFailure(void);
	void RecvCrossInvenSlotSize(void);
#endif
	void RecvItemMoveSuccess(void);
	void RecvItemMoveFailure(void);
	void RecvChangeItemLimitedAttribute(void);
	void RecvRaidItemReward(void);
	void RecvAttack( bool bDie );
	void RecvAttackMiss(void);
	void RecvTargetInfo(void);

	void RecvSkillDie(void);

	void RecvGetExp(void);
	void RecvLevelUp(void);

	void RecvHealUp(void);
	void RecvFsChange(void);	// Mudança de intimidade

	void RecvCreateMonsterFailure(void);
	void RecvSetMoveSpeed(void);

	void RecvAllStat(void);
	void RecvEquipItem(void);

	void RecvHpRate(void); // Não é enviado para si mesmo, apenas em torno
	void SetHpRate(u4 nUID, u1 nHpRate);

	void RecvDiedAlready(void);
	void RecvItemBuySuccess(void);
	
	void RecvItemMake(void);

	void RecvItemSellSuccess(void);
	void RecvDigitamaSellResult(void);

	void RecvUserMessage(void);
	void RecvTamerMessage(void);

public:	// Attendance
	void SendAttendance(void);		// Verificação de presença

public: // card scanner
	void SendUserMessageConfirm(void);
	void SendTamerMessageConfirm(void);
	void RecvTamerMessageConfirm(void);

	void SendScannerRegister(wchar* szCode);
	void SendScannerRelease(wchar* szCode);

	void RecvScannerRegister(void);
	void RecvScannerRelease(void);
	void RecvHandshake487(void);
	bool SendScannerEquip(wchar* szCode);	// Ao montar um scanner
	void SendScannerOn(wchar* szCode);		// Quando o mapa é concluído enquanto o scanner é instalado
	void SendScannerOff(void);

	void RecvScannerEquip(void);
	void RecvScannerOn(void);
	void RecvScannerOff(void);

	void RecvCardScan(void);
	void SendCardScan(wchar* szCode, nSync::Pos &pos);

	void UpdateScannerState(n4 nScannerState);
	void RecvScannerError(void);

	void Send_ItemMake(	DWORD const& dwNpcIDX,				 // Produção NPC IDX
						int const& nMakeCount,				// Produção
						int const& nMakeItemIDX,			// Item idx para produzir
						DWORD const& dwRateItemIDX,			// Probabilidade IDX de item de assistência
						DWORD const& dwProtectItemIDX);		// Item de proteção IDX

	void SendHotTimeGetItemRequest(int const& nTableIDX);

public: // item
	void RecvItemAdd(void);			// Adicionar itens
	void RecvItemUpdate(void);		// Modificação de informações do item
	void RecvItemDelete(void);		// Remover itens

	void RecvItemSplit(void);		// Separação de itens
	void RecvItemMerge(void);		// Combinado
	void RecvItemMove(void);		// Movimento do item
	void RecvItemExchange(void);	// Substituição do item

public: // skill
	void RecvSkill(void);
	void RecvSkillApply(void);
	void RecvSkillApplyAround(void);
	void RecvWideSkillApply(void);
	void RecvAroundSkillAni(void);
	void RecvAroundSkillDmg(void);
	void _RecvWideSkillApplyAround_ExistHitter(void);

	void RecvSkillAction(void);
	void RecvSkillHit(void);

	void _RecvSkillApplyAround_ExistHitter(void);
	void _RecvSkillApplyAround_NoneHitter(void);
	void _RecvSkillApplyME_NoneHitter(void);

	// Habilidade de área ampla Digimon
	void _RecvDigimonSkill_ApplyAround(void);

	void RecvSkillRepeat(void);
	void RecvSkillPointUp(void); // Ponto de habilidade para cima, +2 pontos de habilidade adquiridos cima, +2 pontos de habilidade adquiridos

	void RecvSkillError(void);	// Skill error

	void RecvTamerBuffStart(void);
	void RecvTamerBuffEnd(void);
	void RecvTamerBuffInfo(void);

	void RecvTamerBuffActive(void);
	void RecvTamerBuffInactive(void);

	void RecvTamerSkillSuccess(void);
	void SendTamerActiveSkill(u4 nSkillIdx);
	void SendTamerActiveSkillEnd(u4 nSkillIdx );
	void SendTamerSummonSkill( TCHAR const* szName, u4 nIndex, u1 nCase );
	void SendTamerSummonAccept(u1 nCase, u1 nValue);
	void SendTamerSummon_StateCheck(u1 nCase, u1 nValue, u1 nOK );
	void SendTamerSummon_StateCheck2(u1 nCase, u1 nValue, u4 nSkillCode );
	void RecvTamerSummon();

	void RecvTamerCashSkillUseResult(void);				// Item de habilidade de dinheiro domer depois de usar o valor do resultado
	void RecvTamerCashSkillExpire(void);				// Tamer Cash Skill expira
	void RecvTamerCashSkillRemove(void);				// Ao excluir habilidades de dinheiro dométer
	void SendTamerCashSkillDelete( u4 nSkillIndex );	// Ao excluir o código de habilidade de dinheiro Tamer

	void RecvDotSkillSuccess(void);						// Habilidade do ponto
	void RecvStunSkillSuccess(void);					// Habilidade de atordoamento
		
	void RecvBuffSkill();								// Efeito de buffer adicional quando o gatilho de habilidade
		
	void RecvActiveSkill();								// Habilidade ativa (efeito instantâneo)
		
	void RecvDigimonAddSkillSuccess();					// Habilidade de memória Digimon Adicionar sucesso
	void RecvDigimonSkillRemove();						// Digimon Memory Skill Exclua RECV
	void RecvDigimonMemorySkill_UseSuccess();			// Habilidades de memória Digimon Use o sucesso
	void RecvMemorySkill_EffectSync();					// Digimon Memory Skill Effect Sync Sync

	void SendDigimonSkillDelete( u4 nSkillCode );		// Habilidade de memória Digimon Excluir Enviar
	void SendUseDigimonMemorySkill( u4 nDigimonUID, u1 nEvoStep, u4 nSkillCode, u4 nTargetUID );	// Digimon Memory Skill Use Enviar

	bool RecvMonsterSkill( int nRecvType, int nIdx = 0 );	// Ao lançar habilidades de monstro
	void RecvMonsterSkill_Use( int nIdx );					// Ao usar habilidades de monstro (sementes, ampliação de habilidades-> Remoção dos efeitos do sinal e criação de piso)
	void RecvRaidChainSkill(void);
	void RecvMonsterSkillSync_PersistEnd();					// Quando o piso expirar

	//////////////////////////////////////////////////////////////////////////
	// Função relacionada ao protetor macro
	//////////////////////////////////////////////////////////////////////////
private:
	void RecvMacroResult(void);
	void RecvMacroFailure(void);
	void RecvMacroSuccess(void);
	void RecvMacroQuestion(void);

public:
	void SendMacroQuestion(std::wstring const& szTamerName);

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	void SendMacroAnswer( std::wstring const& wsAnswer);
#else
	void SendMacroAnswer( n4 const& nRandNo, n4 const& nAnswer );
#endif
 	void SendMacroAnswer(void);
 	void SendMacroAnswerFailure(void);
	//////////////////////////////////////////////////////////////////////////


	void SendAwayTime(void);			// Solicitação de mudança no estado de esvaziar
	
	
public: // evolution	
	void RecvEvoOpen(void);	
	void RecvEvolution(void);	
	void RecvEvoFailure(void);	
	
public: // Recebendo processamento de transações
	void RecvTradeRequest(void);		// Solicitado para transações
	void RecvTradeImpossible(void);		// Sem precedente
	void RecvTradeApproval(void);		// A outra pessoa aprova a transação
	void RecvTradeReject(void);			// Recusa
	void RecvTradeAddItem(void);		// Adicionar itens
	void RecvTradeCancelItem(void);		// Cancelamento de itens adicionais
	void RecvTradeAddMoney(void);		// Adicione o dinheiro da Gem

	void RecvTradeInvenLock(void);		// Ben Lock
	void RecvTradeUnInvenLock(void);	// Negociação na liberação de bloqueio -ben
	void RecvTradeComplete(void);		// Conclusão de negociação
	void RecvTradeCompleteFinal(void);	// Transação final concluída

	void SendSetMasterCardLeader( u2 nCardID );		// Solicitação de seleção do líder do selo
	void SendUnSetMasterCardLeader( u2 nCardID );	// Solicitação de liberação de líderes do selo

	void RecvSetMasterCardLeader();					// Resultados de seleção do líder do selo
	void RecvUnSetMasterCardLeader();				// Resultado de liberação do líder do selo


	void SendCardUnSeal(u2 nInven_SealIdx);	// Pedido de liberação do selo de vedação (índice de vedação de inventário, uso de vedação)
	void RecvCardUnSeal(void);
	
	void SendReCardSeal(u2 nCardID, u4 nCardAmount);		// Solicitação de vedação de vedação (número do cartão, quantidade de cartão)
	void SendCardDrop( short nInvenPos, u2 nCount );

	void RecvCardInfoAll(void);
	void SendCardBookmark(u2 nCardCode, u1 nBookmark);
	void RecvCardBookmark(void);

public:	// Processamento de loja pessoal Tamer
	void SendTamerShopReady(n4 nRequestType, u4 nSlot = 0);										// Preparação para a abertura de uma loja pessoal (1: Preparação da abertura da loja, 2: Solicitação para mudar para a loja 3: Cancelamento do Open 4: Consignment Award점 준비 요청 5: 위탁 상점 준비상태 취소 )
	void SendTamerShopOpen(wchar const* szTitle, n4 nItemNumber, nTrade::Item *pSellItemList);	// Solicitação de abrir uma loja privada
	void SendTamerShopClose(void);																// Solicitação de fechamento de lojas pessoais
	void SendTamerShopItemList(u4 nTargetTamerUID, uint nType = 0);								// Solicitação de lista de lojas pessoais (Tipo 0: loja pessoal 1: loja de consignação)
	void SendTamerShopBuy(u4 nTargetTamerUID, uint nIndex, cItemData *pItem, n8 Price);			// Compre itens da loja (Target UID, Índice de itens da loja, número de compras, tipo de item, preço individual);

	void RecvTamerShopItemList(void);		// Resultados da solicitação da lista de itens da loja Tamer
	void RecvTamerShopReadyResult(void);	// Resultados da preparação para a abertura de lojas pessoais
	void RecvTamerShopOpenResult(void);		// Resultados de abertura da loja
	void RecvTamerShopBuy(void);			// Resultados da compra do item
	void RecvTamerShopSellResult(void);		// Resultados de vendas de itens
	void RecvTamerShopCloseResult(void);	// Resultados de perto da loja pessoal
public:	//Processamento da loja de consignação
	void SendCommissionShopOpen(nSync::Pos pos, float nFloat, wchar const* szTitle, n8 nMoney, uint nItemCount, nTrade::Item *item );	// Solicitação de abertura de uma loja de consignação	
	void SendCommissionShopClose(void);															// Solicitação de fechamento de lojas de consignação
	void SendCommissionShopBuy(u4 nShopUID, uint nIndex, cItemData *pItem, n8 nUnitPrice);		// Compra de itens de loja de consignação (Target UID, Índice de itens da loja, número de compras)
	void SendCommissionShopSell(u4 nShopUID, uint nIndex, cItemData *pItem, n8 nUnitPrice);		// Proibição de uso -Sale a itens pessoais lojas de consignação (Target UID, Índice de itens da loja, número de vendas)	
	void SendCommissionShopItemWithDraw();														// Encontre itens e dinheiro da Union Merchant NPC
	void SendCommissionShopNpcItemList();														// Item & Don List solicitada pelo Union Merchant NPC
	void RecvCommissionShopCloseResult();

	void RecvCommissionShopOpenResult(void);			// Resultados de lojas de consignação
	void RecvCommissionShopItemWithDrawResult(void);	// Resultados da coleta de lojas de consignação
	void RecvCommissionShopItemListResult(void);		// Resultados da solicitação de itens de armazenamento de consignação
	void RecvCommissionShopNpcItemList(void);			// Union Merchants Item List Solicite Resultados
	void RecvCommissionShopSellResult(void);			// Vendas de itens resulta em lojas de consignação
	void RecvCommissionShopBuyResult(void);				// Resultados de compra do item da loja de consignação
public: // Hatching
	void SendHatchLostGageProgress(u4 nRequestType, n4 npcIDX, u4 ItemType);				// Uso do medidor de correção de perda e processamento de carregamento (tipo de solicitação, npcidx, tipo de item)
	void SendHatchLostGageProgressResult(void);

	void RecvHatchOptionItemAddResult(void);

public: // Processamento relacionado ao cache do item
	void RecvCashShop(void);
	void SendReqCashShop(void);					// Solicitação de renovação do armazém em dinheiro

	void SendCashShopCart(void);				// Solicitação de lista de carrinhos de compras
	void RecvCashShopCart(void);				// Receba uma lista de carrinhos de compras
	void SendCashShopCartSave(int *nItemTypes);	// Salvar uma lista de lojas de carrinho de compras

	void RecvCropCashItemSuccess(void);
	void SendCropCashItem(n2 nCashSlot);

	void SendCashShopBuyRequest_Steam(n4 nTotalPrice, std::list<DWORD> const& nItemNo);
	void SendCashShopBuyRequest_Steam(int nCashType, n1 nItemCount, n4 nTotalPrice, n4 *nItemNo);
	void RecvBuyCashItemRequest_Steam(void);

	void SendBuyCashItem( int nCashType, n1 nItemCount, n4 nTotalPrice, n4 *nItemNo, uint64 ui64OrderID = 0);
	void SendBuyCashItem( n4 const& nTotalPrice, std::list<DWORD> const& nItemNo, uint64 ui64OrderID = 0);
	void RecvBuyCashItem(void);

	void SendGiftCashItem(n4 const& nPrice, std::list<DWORD> const& nItemNo, std::wstring const& szPeerTamerName, uint64 ui64OrderID = 0);
	void RecvGiftCashItem(void);

	void SendCashShopBalance(void);
	void RecvCashShopBalance(void);
	void SendCashShopBuyHistory(void);
	void RecvCashshopBuyhistory(void);

public:
	void RecvGiftShop(void);
	void SendReqGiftShop(void);

	void RecvCropGiftItemSuccess(void);
	void SendCropGiftItem(n2 nGiftSlot);
	void SendCropGiftItemAll(void);
public: // Mercenário
	void SendDigimonCareSlotList(u4 nNpcType, u4 nInvenIdx = 0, bool bVip = false);	// Solicitação de Mercenary Lista de Mercenários
	void RecvDigimonCareSlotList(void);	// Lista de mercenários de armazenamento mercenário recebe
	void RecvDigimonCareSlotSize(void); // Resposta ao uso de itens de expansão de armazenamento mercenário

	void SendDigimonMoveTo(int nSlot1, int nSlot2, u4 nNpcType, bool bVip);
	void SendDigimonMoveBetweenArchive(int Src, int Dest, u4 nNpcType, bool bVip);
	// O valor do slot de mercenário padrão é 0 ~ nlimit :: DigimonBasesLot
	// O valor do slot de armazenamento mercenário é usado como 0 ~ nlimit :: DigimoCareslot Valor + 100

public: // Transmissão
	void SendTradeRequest(u4 nTargetTamerUID);		// Solicitar uma transação da outra parte
	void SendTradeApproval(u4 nTargetTamerUID);		// Aprovação do pedido de transação
	void SendTradeReject(u4 nTargetTamerUID);		// Notificar que a transação foi recusada
	void SendTradeImpossible(u4 nTargetTamerUID);	// Notificar que a transação está indisponível

	void SendTradeAddItem(u2 nInvenPos, u2 nCnt);
	void SendTradeCancelItem(n1 nPos);
	void SendTradeAddMoney(u4 nMoney); 			// Negociando mais de 4 bilhões de bits

	void SendTradeInvenLock( bool bLock );		// Pacote de bloqueio/liberação com uma janela de transação invente
	void SendTradeCompleteFinal(void);			// Pacote de confirmação de transação final

public:	// option 처리
	void SendTamerOption(u4 nOption);
	void RecvTamerOption(void);
	void SendTamerAchievement(u2 nAchievement);
	void RecvTamerAchievement(void);

public:
	void RecvChangeStatValue(void);

	void RecvDigitamaScanResult(void);
	void RecvCapsuleToItem(void);


public: // game debug command	
	void RecvLoseTarget(void);
	void RecvSyncInfo(void);
	void RecvInsufficientRange(void);

public: // game master command
	void RecvChannelInfo(void);
	void SendChannelInfo(void);
	void RecvCheatLevel(void);

	void RecvHatchTry();

	void RecvUserInfo(void);
	void RecvUserList(void);
	void RecvUserInfoNotFound(void);
	void RecvConnCountInfo(void);

#ifdef CHAT_BAN
	void RecvChatBanUser(void);
#endif
	void RecvBanUser(void);
	void RecvDiscUser(void);
	void RecvInitSkillStat(void);
	void RecvInitDigimonSkillPoint(void);
	
	void RecvInsertItemToShop(void);
	void RecvInit2ndPass(void);
public:
	void RecvBattleOn(void);
	void RecvBattleOff(void);

public: // Related Hatching Related
	void RecvHatchIn(void);		// Hospedando resultados de registro Digitama
	void RecvHatchUpSuccess(void);
	void RecvHatchUpFailure(void);
	void RecvHatchOut(void);	// Resultados de confirmação de hospedagem
#ifdef CROSSWARS_SYSTEM
	void RecvCrossWarsHatch(void);
	void RecvCrossWarsEnergize(void);
	void RecvCrossNotReg(void);
#endif
	void RecvHatchBackupDisk(void);
	void RecvHatchBackupDiskCancel(void);	//Cancelamento do registro do disco de backup

public: // Mudança mercenária
#ifdef CROSSWARS_SYSTEM
	void RecvCrossChangePartner2(void);
	void RecvCrossChangePartner(void);
#endif
	void RecvChangePartner(void);		// Mudança mercenária
	void RecvBattleTagSuccess(void);	// Mudança mercenária sucesso na batalha
	void RecvBattleTagFailure(void);	// Falha em mudar de mercenários em batalha
	void RecvChangePartnerScale(void);	// Alteração do tamanho do mercenário

public:
	void RecvItemAnalysis(void);		// Item (para soquete) Análise completa
	void RecvInvenSlotSize(void);		// Ao mudar o tamanho do inventário
	void RecvWarehouseSlotSize(void);	// Ao alterar o tamanho do armazém

public:
	void RecvUpdateDailyQuest(void);	// Inicialize a missão diária.
	void RecvQuestCancel(void);			// Pacote


public: // Processo de mudança de nome
	void RecvChangeTamerName(void);
	void SendChangeTamerName(int nInvenSlotNo, wchar const * name);
	void SendChangeDigimonName(int nInvenSlotNo, wchar const * name);

public: // party system
	void RecvPartyCreated(void);

	std::wstring m_szPartyInviteTargetTamerName;
	void SendPartyInvite(std::wstring const & szTamerName);
	void RecvPartyInvite(void);
	void RecvPartyInviteResult(void);

	void SendPartyInviteResult( int const& nResultType, std::wstring const& targetName );
 	void SendPartyInviteAccept(void);	// Sztamername ==> candidatos convidados, ou seja, a transmissão de nomes de domer da outra parte
 	void SendPartyInviteReject(void);
 	void SendPartyInviteImpossible(void);	// Em outro trabalho, a resposta à solicitação de convite não é possível

	void RecvPartyJoin(void);				// Um novo membro do partido se move
	void RecvPartyMemberList(void);			// Ao se inscrever, a recepção das informações dos membros do partido existente
		
	void SendPartyLeave(void);				// Se você sair da festa você mesmo
	void RecvPartyLeave(void);				// Outro membro do partido sai do partido

	void RecvPartyKick(void);
	void SendPartyKick(std::wstring const& szTamerName);	// Se o líder do partido retirar forçosamente o membro

	void RecvPartyChat(void);					// Recepção de bate -papo da festa
	void SendPartyChat(TCHAR const*szMsg);		// Bate -papo de festa

	void RecvPartyDie(void);			// Quando os membros do partido morrem
	void RecvPartyExp(void);			// Aquisição de experiência do partido
	void RecvPartySyncInfo(void);		// HP, MHP, DS, MDS Sincronização
	void RecvPartySyncPosition(void);	// Sincronização da localização

	void SendPartyChangeCropType(int nCropType, n1 nItemClass, n1 nItemDispGrade);	// pParty.h ==> nCrop::Base, nCrop::Free, nCrop::Order, nCrop::RareRate
	void RecvPartyChangeCropType(void);

	void RecvPartyChangeMap(void);
	void RecvPartyChangeDigimon(void);	// Mudança mercenária

	void SendPartyChangeMaster(int nNextMasterSlotNo);
	void RecvPartyChangeMaster(void);

	void RecvPartyDisconnected(void);
	void RecvPartySyncBuff(void);			// Você precisa obter as informações de buff excluídas posteriormente.Você também deve obter informações de buff excluídas.

public: // CBT2 EVENT Protocolo relacionado
	void RecvMsgAllHatchOut(void);			// Se você tiver sucesso em eclodir mercenário
	void RecvMsgAllCropEventItem(void);		// Em caso de adquirir itens de evento (certificados de presente cultural, etc.)

public: // Evento de verificação de atendimento 1
	void RecvChangeGameState(void);			// Valor do estado do jogo
	void Send_NTF_ExpEventTimeCheck(void);	// Verifique a atualização do tempo da experiência para o servidor sempre (envie apenas para o servidor)

	void RecvEventDailyCheck(void);			// Verificação de presença
	void SendEventDailyCheck(void);			// Comparecimento
	
	void RecvDailyEventInfo(void);			// Informações diárias sobre eventos
	void RecvMonthlyEventInfo(void);		// 2017-02-16-NOVA Informações de eventos mensais
	
public:	
	void RecvSocketInSuccess(void);			// Sucesso para colocar itens no soquete
	void RecvSocketOutSuccess(void);		// Itens de sucesso de soquetes
	void RecvSocketClearSuccess(void);		// Itens de sucesso de soquetes

//////////////////////////////////////////////////////////
// sync
	void SyncNewObject(void);		// Se você precisar fazer um novo
	void SyncDeleteObject(void);	// Remova imediatamente sem movimento

	void SyncWalkObject(void);		// Quando outros OBJs se movem (caminhando)
	void SyncMoveObject(void);		// Quando outros OBJs se movem (salto)

	void SyncInObject(void);		// Se outro obj entrar na visão
	void SyncOutObject(void);		// Se outro obj desaparecer da vista

	void SyncCondition(void);		// Tratamento ao mudar o estado
	void RecvSyncCondition(void);	// Sua condição

	void SyncInTamer(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInTamer(nSync::Pos& pos, int Handle, int ModelType, bool bNew = false);
	void SyncInTamerAround(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInDigimon(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInDigimon(nSync::Pos& pos, int Handle, int ModelType, bool bNew = false);
	void SyncInDigimonAround(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInMonster(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInMonster(nSync::Pos& pos, int Model, int Handle, bool bNew);
	void SyncInItem(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInCommissionShop(nSync::Pos &pos, cType &type, bool bNew=false);

	void SyncNewTamer(nSync::Pos &pos, cType &type);
	void SyncNewDigimon(nSync::Pos  &pos, cType &type);
	void SyncNewMonster(nSync::Pos &pos, cType &type);
	void SyncNewItem(nSync::Pos &pos, cType &type);
	void SyncNewCommissionShop(nSync::Pos &pos, cType &type);

	void SyncMoveTamer(u4 nIDX, nSync::Pos &pos);
	void SyncMoveDigimon(u4 nIDX, nSync::Pos &pos);
	void SyncMoveMonster(u4 nIDX, nSync::Pos &pos,bool bWalk);

	void SyncMsgChat(void);
	void SyncMsgWhisper(void);
	void SyncMsgAll(void);
	void SyncMsgSystem(void);
	void SyncMsgShout( bool bGuild = false );	// Verifique se o guilda grita
	void SyncMsgAllByItem(void);

 	void SyncState(void);
 	void SyncState(ushort nUID);

	// Afundar
	void SyncInBuff(void);
	void _SyncInBuffObject(int nObjectCnt);


/////////////////////////////////////////////////////////
// send
private:
	void SendAccessCode(void);	

public:
	void SendSelectPortal(uint nPortalNo);
	void SendJumpGate(u2 nInvenPos, u2 nOrderIDX, bool bVip =false);	// Ninevenpos: Inven Localização do item a ser usado, norderidx: quantas funções do item serão usadas (0 ~ n)

	void SendChangeServer(void); // game server -> game server
	void SendOpenRegion(int nOpenRegionIDX); // Se você entrar em uma nova região, registre que você entrou mais de uma vez nessa área.

	void SendGoBackGate(void); // game server -> gate server

	void SendReady(nSync::Pos &DigimonStartPos); // Relate a conclusão de carregamento do jogo
	void SendDieConfirm(void); // forçou a vila.

	void SendTamerJumpGate( u2 nItemID, u2 nMapID );

public:
	void SendEmoticon(int nType, int nVal = -1);
	void RecvEmoticon(void);
	void SendChangeTamer(int nTamerType, int nInvenSlotNo);
	void RecvChangeTamer(void);
#ifdef ADVENTURE_TAMER_CHANGE
	void RecvChangeTamerAround(void);
#endif
public:
	void SendChatMsg(wchar* msg);
	void SendMoveTo(u4 uid, nSync::Pos tp, float fDirect);	
	void SendMoveTo(u4 uid, nSync::Pos sp, nSync::Pos tp, float fDirect);
	void SendMoveToKnockBack(u4 uid, nSync::Pos tp);

	void SendMsgAllByItem(wchar* msg, n4 nItemSlotNo);	// Transmissão em todo o servidor atual (Luche Sup Bay, Riva Sup Bay)
	void RecvMsgAllByItemFailure(void);	// Falha em usar gritos

	void SendMsgWhisper( TCHAR const* name, TCHAR const* msg );	// transfere apenas para usuários do oponente

	void SendMsgShout(wchar const* msg);		// Gerado
	void RecvMsgShoutFailure(void);				// Gritar

public: // game master command
 	void SendJump(int x, int y);
 	void RecvSetMoney(void);

 	void SendChangeChannel(uint nChannelIDX);

 	void SendQueryMoneyInven();
 	void RecvQueryMoneyInven();
 	void RecvKillMonster();
 	void RecvGmQuest();
 	void RecvGmSetSkillPoint();
 	void RecvGMCoolReSet();
 	void RecvGuildSkillReSet();		//2017-06-28-nova

public: // Digitama Hatching Order
	void SendHatchIn(uint nInvenSlotNo, n4 nNpcIDX, bool bVip = false);	// Hospedando registro Digitama

	void SendHatchBackupDisk(uint nInvenSlotNo, n4 nNpcIDX, bool bVip = false);

	void SendHatchUp(n4 nNpcIDX, n1 nDataLv = 0, bool bVip = false);	// Hashing Digimon Data Entrada

	void SendHatchCancel(n4 nNpcIDX, bool bVip = false);

	void SendHatchBackupDiskCancel(n4 nNpcIDX, bool bVip = false);

	void SendHatchOut(wchar* szName, n4 nNpcIDX, u4 nInvenPortablePos = 0, bool bVip = false);	// Hatching

#ifdef CROSSWARS_SYSTEM
	void SendCrossWarsHatch(u2 IDX, wchar* szName);	// Hatching

	void RecvCrossWarsRelease(void);
	void RecvCrossWarsCRRelease(void);

	void RecvCrossWarsCrownHeal(void);
	void RecvCrossWarsLoader(void);
	void SendCrossWarsRecover(void);
	void RecvCrossWarsRecover(void);
	void SendCrossChangePartner(int nArrIDX);
	void SendCrossChangePartner2( int nArrIDX, int nArr[]);
#endif

	void SendChangePartner(u1 nArrIDX);
	void SendDeletePartner(u1 nArrIDX, char *pass2); // Remova esse digimon
	void RecvDeletePartner(void);

public: // inventory command
	void SendItemMove(n2 nSrcPos, n2 nDstPos, bool bCheckBelonging = true );
	void SendItemSplit(n2 nSrcPos, n2 nDstPos, u2 nCount);
	void SendItemDrop(n2 nInvenPos, nSync::Pos &FieldPos, u2 nCount);	

	void SendItemCrop(n4 nItemIDX);
	void SendItemUse(u4 nUID, n2 nInvenPos);
#ifdef CROSSWARS_SYSTEM
	void SendItemUseCross(u4 nUID, n2 nInvenPos);
#endif
	void SendItemAnalysis(u4 nNpcIDX, u2 nInvenPos, u4 nInvenPortablePos = 0, bool bVip = false);

public: // shop command
	void SendBuy( u4 NpcFTID, n4 ShopSlotIndex, u2 ItemCount );
	void SendSell( u4 NpcFTID, u1 InvenSlotIndex, u2 ItemCount );
	void SendPropertyBuy( u4 NpcFTID, n4 ShopSlotIndex, u2 ItemCount, u2 ItemIndex );


public: // Digitama Confirmation e Item Return New Version
	void SendDigitamaScanItem( bool bIsVip, u4 uPortableIdx, u4 uNpcIdx, u4 uInvenIdx, u2 uScanCount );	// new digitama scan
	void SendDigitamaToItem(u4 nNpcIDX, u4 nInvenPos, u4 nInvenPortablePos = 0, bool bVip = false);	// scan digitama
	void SendDigitamaToMoney(u4 nNpcIDX, u4 nInvenPos, u4 nInvenPortablePos = 0, bool bVip = false );	// sell digitama

	void SendCapsuleToItem(u4 nNpcIDX, u4 nInvenPos);	// scan capsule

public: // Montagem e liberação do soquete
	void SendSocketIn(u4 nNpcIDX, u2 nInvenSrcPos, u2 nInvenDstPos, u1 nSocketOrder, u4 nInvenPortablePos = 0, bool bVip = false);
	// O item de soquete no local do NinVensRCPOS é montado no item do equipamento no local do NINTSTPOS.
	// Os itens da localização do NinVensRCPOS são removidos.

	void SendSocketOut(u4 nNpcIDX, u2 nInvenSrcPos, u2 nInvenDstPos, u1 nSocketOrder, u4 nInvenPortablePos = 0, bool bVip = false);
	// itens de soquete presos na localização do nsocketorder (esquerda-> direita, 0 ~ nlimit :: socketslot) a partir do item de equipamento do local Ninvensrcpos
	// Crie itens extraídos do local Ninvendstpos

	void SendSocketClear(u4 nNpcIDX, u2 nInvenPos, u1 nSocketOrder);


public:
	void SendSetTarget(u4 nHitterUID, u4 nTargetUID);
	void SendAttack(u4 nHitterUID, u4 nTargetUID);
	void SendStopAttack(u4 nUID);
	bool SendSkill(u1 nSkillArrIDX, u4 nHitterUID, u4 nTargetUID);
	void SendSkill(u1 nSkillArrIDX, u2 nHitterUID, nSync::Pos &TargetPosition);
	void SendActiveSkill(u4 nSkillArrIDX, u4 nTargetUID);

public: // evolution
	void SendEvolution(uint nDigimonUID, uint nSlotIDX);
	void SendEvoCard(uint nDigimonUID, uint nInvenSlot);
	void SendEvoCrest(uint nDigimonUID, uint nInvenSlot);


public: // quest
	void SendQuestRequire(uint nQuestIDX);	// Busca
	void SendQuestCancel(uint nQuestIDX);	// Cancelar missão
	void SendQuestComplete(uint nQuestIDX);	// Conclusão da missão

	void RecvQuestChangeInfo(void);
	void SendQuestChangeInfo(uint nQuestIDX, uint nCondIDX, uint nValue);

	void SendQuestAchievement(uint nQuestIDX);

public: // buddy
	void SendBuddyList(void);	// Solicitação de amigos e listas de bloqueio
	void SendBuddyAdd(std::wstring const& szTamerName);
	void SendBuddyDelete(std::wstring const& szTamerName);
	void SendBuddyMemo(std::wstring const& szTamer, std::wstring const& szMemo);
	void SendBuddyBan(std::wstring const& szTamerName);	// Processado com a mesma mensagem na lista anterior de amigos

	void RecvBuddyOn(void);
	void RecvBuddyOff(void);

	void RecvBuddyList(void);			// Amigos e listas de bloqueio
	void RecvBuddyAdd(void);			// Sucesso de registro de amigos
	void RecvBuddyBan(void);			// Registro bem -sucedido
	void RecvBuddyNotFound(void);		// Amigos, falha do registro de bloqueio -Não há mais domínio com esse nome.
	void RecvBuddyChangeName(void);		// Mudar o nome do amigo

public: // guild
	void SendGuildCreate(TCHAR const*title, u2 nInvenSlot, u4 nNpcIDX);	// Criando uma nova guilda, inventário de itens usados ​​para criar guildas Ninvenslot
	void SendGuildClose(void);	// Fechando sua guilda, a conclusão fechada é desnecessária.

	void SendGuildAllow(TCHAR const*szTamerName, u4 nCertifiedCode);	// Pguild :: Reqadd's Response -permission
	void SendGuildReject(TCHAR const*szTamerName, u4 nCertifiedCode);	// Pguild :: Resposta de Reqadd -rejeção

	void SendGuildLeave(void);	// Se você se deixar da guilda
	void SendGuildDelete(std::wstring* szTamerName);	// Ele é capaz de assumir seu oponente (ele deve ser uma guilda)

	void RecvGuildChangeNotice(void);
	void RecvGuildCreate(void);								// Sucesso da geração da guilda

	bool SendGuildReqAdd(TCHAR const*szTamerName);			// Registro do domador relevante
	void RecvGuildReqAdd(void);								// Solicitação de registro do domador na guilda

	void SendGuildReqAddFailure(TCHAR const*szTamerName);
	void RecvGuildReqAddFailure(void);						// Solicitação de registro de falha da guilda

	void RecvGuildReject(void);		
	void RecvGuildAllowFailure(void);						// Falha na conclusão da assinatura da guilda

	void RecvGuildClose(void);								// Desmantelamento da guilda
							
	void SendGuildInfo(void);								// Não solicitamos informações da guilda e lista de membros.
	void RecvGuildInfo(void);								// Informações da guilda e lista de membros

	void SendGuildHistory(void);
	void RecvGuildHistory(void);

	void SendGuildChat(TCHAR *msg);
	void RecvGuildChat(void);

	void RecvGuildOn(void);
	void RecvGuildOff(void);

	void RecvGuildRankChange(void);

	void RecvGuildAdd(void);		// O domador é adicionado à guilda
	void RecvGuildLeave(void);		// O domador se retira da guilda
	void RecvGuildDeleted(void);	// O domador está arruinado da guilda

	void RecvGuildToMaster(void);
	void RecvGuildToSubMaster(void);
	void RecvGuildToMember(void);
	void RecvGuildToSubMember(void);
	void RecvGuildToDatsMember(void);

	void SendGuildToMaster(std::wstring* szNextMasterTamerName);
	void SendGuildToSubMaster(std::wstring* szNextSubMasterTamerName);
	void SendGuildToMember(std::wstring* szNextMemberTamerName);
	void SendGuildToSubMember(std::wstring* szNextMemberTamerName);
	void SendGuildToDatsMember(std::wstring* szNextMemberTamerName);

	void RecvGuildDeletedTamer(void);
	void RecvGuildChangeTamerName(void);

	void RecvGuildLevelUp(void);
	void RecvGuildMemberLevelUp(void);

	bool SendChangeGuildNotice(wchar const* msg);

	void SendGuildChangeClassName(int nClass, wchar const* szName, wchar const* szDesc);

 	void SendGuildChangeClassName2(int nClass, wchar* szName, wchar* szDesc, bool bClass);

	void RecvGuildChangeClassName(void);

	void RecvGuildIncMember(void);

#ifdef GUILD_RENEWAL
	void RecvGuildSkillPoint();									// Ponto de habilidade da guilda (GSP) quando você muda (somente quando chega?) O
	void SendGuildMemoAdd( TCHAR const* szMemo );				// Guild One Memo (Self) armazenamento
	void SendUseGuildSkill( int nType, void* pData = NULL );	// Usar habilidades da guilda (habilidade de habilidade, código de habilidade)
	void RecvUseGuildSkill();									// Habilidade da guilda Use RECV
	void RecvRemoveUseGuildSkill();								// O período das habilidades da guilda expira ou ao excluir sub -skills usando habilidades mais altas
			
	void SendGuildSkill_Summon( u4 nSkillCode );				// Enviar quando o membro da guilda aceitar a convocação
			
	void RecvGuildSkill_Summon();								// O membro da guilda recebe quando o escritor da guilda escreveu
	void RecvGuildSkillError();									// Receber quando um erro ocorre habilidade
	void RecvGuildChangeMemo();									// Tudo quando o memorando da guilda é modificado.#Endif
#endif
public: // stat and skill
	void SendStatUp(uint nUID, uint nStat);
	void SendDigimonSkillLevelUp(uint nUID, uint nEvoUnitArrIDX, uint nSkillArrIDX);
	void SendOpenEvoSlot(uint nEvoUnitArrIDX, n2 nInvenIdx = -1);
	void SendOpenRideMode(uint nEvoUnitArrIDX, int nItemType);

	void SendRideOn(void);		// Solicitação de embarque
	void RecvRideOn(void);		// a bordo

	void SendRideOff(void);		// Solicitar
	void RecvRideOff(void);		// Sair
public:	// Relacionado à sala do PC
	void SendPCBangInvenRequest();										 	// Solicitação de informações de inventário para PC Room
	void SendCropPCBangInvenItem(uint nSlot, uint nType, uint nCount);		// Item de inventário adquirido para PC Room
	void RecvPCBangItemCropResult(); 				// Aquisição de itens do inventário de salas de PC
	void RecvPCBangInven(void);						// Informações de inventário de salas de PC
	void RecvPreminum(void);						// Usuário PC Room Premium Service Start
	void RecvPreminumEnd(void);						// Usuário PC Room Premium Service Expiration
	void RecvPartyPCBangExpBuff(void); 				// Receba uma taxa de experiência adicional com o número de usuários de salas de PC para a festa

public:
	void SendDigimonInchant(uint nType, uint ItemSlot, int RecoverDisk); // Tipo de reforço, slot de clone digital, slot de item de recuperação
	void RecvDigimonInchantResult();

	void SendDecreaseDigimonInchant(const UINT& nType, const int& nItemSlot); //enfraquecimento
	void RecvDecreaseDigimonInchantResult();

public:
	void SendClientOptionSave(u4 nOption);	
	void SendViewEventSeen();

private:
	void Recv_NProtectCheck(void);	// Nprotect Game Guard Check

public:
	void ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize );
	void NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize);

public: // option
	void SendRefreshScreen(void);
public:
	void RecvPortableItemFail(void);
	void RecvDigimonMoveResult(void);
	void RecvDigimonMoveResultInArchive(void);
public: // PlayTime Set
	void RecvPlayTime();
	void RecvPlayTimePaneltyChange();
public:
	void RecvBuffCreate();		// Geração de buff
	void RecvBuffChange();		// Mudança de buff
	void RecvStatChange();		// Mudança de estatística
	void RecvBuffClear();		// Processo de expiração do buff

	void SendManualRemove(u1 nType,u2 nBuffIDX);	// Remoção manual do buff
	void RecvBuffApplyFail();

	void _RecvDotSkillAround();
	void RecvBuffSyncInit();				// Informações antecipadas sobre o coletor
	void RecvDotDie();
#ifdef MONSTER_SKILL_GROWTH
	void RecvMonBuffSet();					// Conjunto de habilidades de crescimento de monstros
	void RecvMonBuffRelease();				// Desbloquear habilidades de crescimento de monstros (final da batalha)
#endif

public: // Master Cheat Result
	void RecvMasterConnCount();
	void RecvMasterPayCash();

public:	// Spirit Evolution
	void SendSpirit_ItemToDigimon( int nModelID, std::wstring const&, int nNpcID );
	void SendSpirit_DigimonToSpirit( u1 nDelSlot, char const* pass2, int nNpcID );
	void SendSpirit_AncientSpiritEvolution( u1 nDelSlot, u1 nDelSlot2, char* pass2, TCHAR* szName, int nNpcID );

	void RecvSpirit_SpritToDigimon();
	void RecvSpirit_DigimonToSpirit();
	void RecvSpirit_AncientSpiritEvolution();

	//=============================================================================================================================
	//
	//		InsMap
	//
	//=============================================================================================================================
public:
	void	SetInsMap_DungeonRestoreFlag();
	void	SetInsMap_DungeonMSG(uint nPortalNo);

	void	SendInsMap_DungeonIN(uint nPortalNo);
	void	RecvInsMap_DungeonFail();
	void	RecvBattleEvoPoint();


/////////////////////////////////////////////////////////
// 
	static void SetAttackInfo(	ushort nHitterUID, ushort nTargetUID, int nDamage,CsC_AttackProp::eHIT_TYPE ht,CsC_AttackProp::eDAMAGE_TYPE dt,int nEffectType );			// Defina informações de ataque

	static void SetAttackInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack );			// Defina informações de ataque

	static void SetSkillInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack );				// Definindo informações de habilidade
	static void SetSkillInfo2( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack, u4 nTargetUID, int nSkillSpeed, int nSkillAceel );				// Definindo informações de habilidade
	void	RecvRaidRankingList();

	static void SetDamage( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo);

	static void SetSkillInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, CsC_AvObject* pTargetMark, bool bLastAttack, bool &bMaxCount, bool bRecvComp );	// Digimon Wide Area Skill Informações
	static void SetDotInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, int nBuffCode, bool bLastAttack );				// Configuração de informações de ponto

	static void	SetDieInfo( CsC_AvObject* pTarget );

	//////////////////////////////////////////////////////////////////////////
	// Função de correspondência de batalha
	//////////////////////////////////////////////////////////////////////////
	// Recver
	void RecvMyBattleMatchInfo();			// Minhas informações de batalha
	void RecvBattleRegistRequest();			// Pacote de resultado da solicitação de registro de batalha
	void RecvBattleRegistCancel();			// Pacote de resultado de registro de batalha
	void NtfBattleRegistReadyInfo();
	void RecvBattleRegistReadyResult();
	void NtfBattleRegistReadyResult();
	void NtfBattleMatchCancel();
	void NtfBattleMatchComplete();
	void NtfBattleStart();
	void NtfBattleEnd();
	void NtfBattleReward();
	void NtfBattleMemberSync();
	void NtfBattleMatchSync();

	// Sender
	void SendMyBattleMatchInfo();			// Peça ao servidor minhas informações de batalha.
	void SendBattleMatchRequest( n1 const& nMainMode, n1 const& nSubMode, u4 const& nNpcID, u4 const& nMapID );// Pedido de batalha
	void SendBattleMatchCancel();			// Cancelamento de batalha
 	void SendBattleReady( n1 nIs, n4 const& nBattleIdx );			// responder

 	void RecvBattleUpdateRoundInfo();
 	void RecvBattleResult();
 	void RecvBattleMatchFail();				// Falha da partida de batalha
 	void RecvBattleMatchReady();			// Battleship
 	void RecvBattleMemberInfo();			// Informações de Gyeonggi (equipe e nome)
 	void RecvBattleEnd();					// Enviar se o jogo terminar (enviando compensação)
 	void RecvBattleReadyComplete();			// Você pode começar o jogo
 	void RecvBattleJoin();					// No jogo
 	void RecvDualBattleRequest();
 	void RecvDualBattleFail();
 	void RecvDaulBattleSucc();
 	void RecvBattleTeamSync();
 	void SendBattleGiveUp();				// Abandono de batalha
 	void SendBattleCheat(char *cmd);
 
 	void SendDualBattleRequest( TCHAR *szTamerName );
 	void SendDualBattleResponse( n1 nIs );

	//////////////////////////////////////////////////////////////////////////

// Processamento de eventos de convite de amigo
private:
	void RecvAddFriendly();
	void RecvAddFriendlyAnswer();
	void RecvFriendlyMark();
	void RecvFriendlyMarkItem();
	void RecvFriendlyStateChange();
	void RecvFriendlyInfo();
	void RecvFriendlyNameChange();

public:
	void SendAddFriendly(TCHAR const * szName);
	void SendAddFriendlyAnswer(u2 nAnswer, TCHAR const * szName);
	void SendMarkItem(u1 nFriendlyType, u2 nMarkIndex, TCHAR const * szName );
	void SendFriendInfo(void);

	void RecvCropGiftInvenFail();

	void RecvGiftBoxUseSuccess();

	void SendCardEventStart();	// Notificação do evento do cartão para o servidor
	void RecvCardEventStart();	// Receba dados do servidor no início do evento do cartão

	void SendCardEventEnd();
	void RecvCardEventEnd();

	void RecvNatureExp();
	void SendGetNatureItem( n1 nElementType, n1 nNatureType );

	// Alguns ovos (evento) eclodem com o botão direito
	void SendRClickDigimonHatch( int nType, int nInvenSlotNo, wchar const * name );
	void RecvRClickDigimonHatch();

#ifdef MINIGAME
	void SendMinigameStart( u1 nGameKind, u1 nlevel );//Minigame Start
	void RecvMinigameStart();

	void SendMinigameClickBtn( u1 nGameKind, u2 nBarIndex );	
	void RecvMinigameClickBtn();

	void SendMinigameTimeOut();	//Envie o tempo fora após o tempo sem pressionar o botão de parada
	void RecvMinigameTimeOut();

	void SendMinigameEnd( u1 nEndResult);//Mini Game End .. Normal End: 1, rescisão anormal:0
	void RecvMinigameEnd();

	void RecvMinigameLimit();			//Mensagens de erro se você não pode executar um mini jogo


	void SendMiniGameIsStartReq();		// Solicite se for possível jogar um mini -jogo.
#endif

	void SendGotchaOpen(u4 nNpcIDX);// canela de bagunça rara aberta
	void RecvGotchaOpen();

	void SendGotchaGet( u4 nUIDX , u4 nNpcIDX, u4 nInvenIndex, u4 nResetCnt );// Quando você pressiona o botão de desenho
	void RecvGotchaGet();

	void SendGotchaItem( u4 nNpcIDX, u4 nItemID, /*u4 nItemCnt,*/ u4 nAllItemCnt, u4 nInvenSlotCnt, u4* nDecreaseItemSlot/*, n4 nGroupID*/ );//획득한 아이템 뭔지 알려주기(로그용)
 	void RecvGotchaError();//Erro de código

	void RecvMsgAllGotchaMachine();	//Mensagem do Billboard
	void RecvGotchaRefresh();		//Atualizar (não falado)

#ifdef EGG_PREFERMENT
	void SendMysteryGet( u4 nUIDX, u4 nNpcIDX, std::map< int, int >& mapItems, u4 nItemID );// Draw Mystery
#else
	void SendMysteryGet( u4 nUIDX, u4 nNpcIDX, std::map< int, int >& mapItems );// Draw Mystery
#endif
	void RecvMysteryGet();

	void RecvRunGotchaMachine();					// Torne -se para a chave virtual da máquina rara
#ifdef ADVENTURE_TAMER_CHANGE
	void RecvMoveClear();	// Tire a janela do equipamento ao usar o item Tamerchase.
#endif


 	void SendFireWork( u2 nUIDX, u4 nItemID, u2 nInvenPos );		//// Envie o tamer yu  id e use o ID do item.
	void RecvFireWork();

	void RecvSyncEnchantEffect();	// Quando sai após o fortalecimento do ponto, o efeito de fortalecimento é invisível para outros usuários LKS007 13.07.01

	void SendRequestRewardList();
	void RecvRequestRewardList();
	void SendRequestRewardAll();
	void SendRequestRewards( u1 nReqCnt, u1 nReqIdx );
	void RecvRequestRewards();

	void RecvRequestRewardFail();

	void SendMastersMatchOpen( u4 nTamerIDX, u4 nNPCIDX );
	void RecvMastersMatchOpen();

	void SendMastersMatchInsert( u4 nUserID, u4 nNpcID, u4 nSlot, u4 nGiveCnt, u1 nFTNpcIdx/*Npc테이블에 있는 아이템 인덱스*/ );
	void RecvMastersMatchInsert();

	//Acessórios
	void SendAccessoryCheck( u4 nUID, u2 nInvenPos );
	void RecvEnchantItemIdentify();
	//Aprimoramento dos acessórios
	void SendAccessoryEnchant( u4 nUID, u2 nItemPos, u2 nAccPos, int nComboIdx = -1 );
	void RecvEnchantItemEnchant();

#ifdef CHEAT_ITEMRANK
	void RecvItemRank();							// Voltando para a classificação do item virtual
#endif

	void SendIncubaterList();						//Incubadora aberta
	void RecvIncubaterList();						//Incubadora aberta

	void SendIncubatorFacilitation( u1 nIncuSlot, u4 nHouseSlot, u4 nInvenSlot );	//	Promoção -Slot para incubador (0 ~ 4), Slot Divo (1000 ~), Item registrado Ben Slot
	void RecvIncubatorFacilitation();				//	Promoção

	void SendIncubatorExtraction( u1 nIncuSlot );		// Extração (OUT) -Incubator Slot (0 ~ 4)
	void RecvIncubatorExtraction();					//	Extração (Out)

	void RecvIncubatorError();						// Processamento da situação de exceção
	
	void SendIncubaterSlotChange( n4 nIncuSlot, n4 nHouseSlot );	//	Substituição de Digimon na promoção -slot de incubador (0 ~ 4), slot DIBO (1000 ~)
	void RecvIncubaterSlotChange();			// Substituição Digimon na promoção

	void SendTacticsOpen( u4 nUID, n2 nInvenPos );
	void RecvTacticsOpen();

#ifdef SELECT_SERVER_ERROR
	void RecvSelectServer_Er();
#endif
#ifdef REPURCHASE
	void SendItemRepurchaseOpen( u4 nNpcID );				// Re -compra da janela aberta
	void RecvItemRepurchaseOpen();							// Resposta de abertura

	void SendItemRepurchase( u4 nNpcID, u2 nItemIdx );		// Recompra
	void RecvItemRepurchase();									// Resposta à recompra
#endif

	void SendEncyclopediaOpen( );
	void RecvEncyclopediaOpen( );

	void SendEncyclopediaGetItem( u4 nDigimonID );
	void RecvEncyclopediaGetItem( );

	void SendEncyclopediaGetDeck( u4 nDeckIdx );	// Decisão de deck (1001 ~)
	void RecvEncyclopediaGetBuffStat();				// Estatísticas variáveis ​​quando acionado Buff Trigger RECV

	void RecvEncyclopediaBuff();					// Informações sobre o tempo do tipo de gatilho

	void RecvGlobalMsg();							// Mostre uma mensagem para todos os usuários no servidor
	void RecvCapsuleEvol_Result();					// Resultados da tentativa de evolução da cápsula

	void RecvActorObjectFactor_Init();				// Todo o monstro no mapa para a abertura e fechamento do objeto do ator (porta) init
	void RecvActorObjectFactor();					// Recebido quando o monstro do mapa atual para a abertura e fechamento do objeto do ator (porta)

	void SendPinokimonInfoReq();					// Solicitação de renovação de votos de cartão
	void RecvPinokimonInfoReq();					// Resposta ao pedido de renovação

	void SendPinokimonVote( u1 nCardIdx, u4 nVoteCnt, u4 nNpcID );	// Votação no cartão: índice de cartão de votação, número de itens de votação, npcid (resposta ao servidor)
	void RecvPinokimonVote();										// Votação normal: Contagem de slots Inven, Índice de Slot Inven, número de itens de votação (resposta ao servidor esperando)
								
	void RecvRemoveVoteItem();										// Reiniciar o item de direitos de voto
	
	// Digimon Data Exchange
	void SendDigimonDataChange( u4 nNPCIDX, nDigimon::eDataChangeType eType, u1 nSrcArrIDX, u1 nDstArrIDX );
	void RecvDigimonDataChange();

	void RecvErrorMessage();

	void AttackProcesses(	u4 nHitterUID, u4 nTargetUID, n4 nDamage, u4 nBattleOption, n4 hp, n4 Mhp, bool bDie);

	void TamerSkillErrInEpicDungeon();

	void RecvTradeLimitedTimeOut(void);

	void SendItemActive(u2 invenPos);	//2017-04-04-nova	Ativação de itens de tempo parcial

	void RecvItemActive(void);			//2017-04-04-nova	Se o item de tempo parcial for ativado, o valor do resultado


	//////////////////////////////////////////////////////////////////////////
	void Send_ServerRelocate_CheckTamerName( int const& nServerIDX, std::wstring const& wsTamerName );
	void Recv_ServerRelocate_CheckTamerName();
	void Send_ServerRelocate_ItemUse( int const& nInvenPos, int const& nServerIDX, std::wstring const& wsTamerName );
	void Recv_ServerRelocate_ItemUse();

	//////////////////////////////////////////////////////////////////////////
	void Recv_InfiniteWar_ChallengeStageClear();
	void Recv_InfiniteWar_ChallengeTryNextStage();
	void Recv_InfiniteWar_ChallengeGiveup();
	void Recv_InfiniteWar_ChallengePartyAlreadyPlay();

	void Send_InfiniteWar_ChallengeTryNextStage(n4 const& nNpcIdx);
	void Send_InfiniteWar_ChallengeGiveup(n4 const& nNpcIdx);

	void Send_Request_ChallengeData();
	void Recv_ChallengeData();
	void Send_Challenge_PointItem_Put(std::list<nChallengeSt::st_ItemInfo> const& items);
	void Recv_ChallengeData_PointItem_Put();
	void Ntf_ChallengeDailyUpdate();

	void Recv_ArenaRequestRank();		// Pacote de lista de classificação da arena
	void Recv_ArenaRequestOldRank();	// Pacote de lista de pré -ranking da arena
	void Recv_ArenaRequestFame();		// Pacote da lista do Hall da Fama da Arena
	void Recv_ArenaRequestFameSeasonList();
	void Send_ArenaRequestRank(unsigned char const& nRequestRankType);
	void Send_ArenaRequestOldRank(unsigned char const& nRequestRankType);
	void Send_ArenaRequestFame(unsigned short const& nRequestSeasonIdx);
	void Send_ArenaRequestFameSeasonList();
	//////////////////////////////////////////////////////////////////////////

	void Recv_XGToCrystal();
	void Recv_CrystalToXG();
	void Recv_XGuageUpdate();
	void Recv_XGuageMaxUpdate();
	void Send_XGToCrystal();
	void Send_CrystalToXG();
	//////////////////////////////////////////////////////////////////////////

	void Send_VipMemberShipData();
	void RecvVipMemberShipData();

	//////////////////////////////////////////////////////////////////////////
	void RecvDigimonTranscendenceChargeExp();
	void SendDigimonTranscendenceChargeExp( u1 const& nIsVip, u4 const& invenSlotIdx
										   ,DWORD const& dwNpcIDX, u1 const& nChargeType
										   ,u1 const& nChargeDigimonTaticsIdx, std::list<u2> const& listMaterialDigimons
										   ,std::list<stTranscendenceItem> const& nSubMaterialItems);

	void SendDigimonTranscendence(u1 const& nIsVip, u4 const& invenSlotIdx, 
								  DWORD const& dwNpcIDX, u1 const& nDigimonTaticsIdx,
								  u8 const& nNeedMoney);
	void RecvDigimonTranscendence();

	//////////////////////////////////////////////////////////////////////////
	void NTF_NeonDisplay(); // Receba dados do Billboard.
	//////////////////////////////////////////////////////////////////////////

	void SendSkillMaxLvUp( int const& nUseItemInvenPos, int const& nUseItemType, int const& nEvoSlotNum );
	void RecvSkillMaxLvUp();
	void NTF_DigimonSkillCoolTime();
	void RecvDigimonSkillLevelUp();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Xigncode
	void NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize );
	void Recv_XignCodeCheck(void);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Inven Sort
	void SendItemSort( u1 eSortFlag );
	void RecvItemSort();
	void RecvInvenResult();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Time Charge
	// Solicitação de lista / resultado de cobrança
	void SendRequestChargeList();
	void RecvChargeListResult();

	// Solicitação / resultado de cobrança de item
	void SendNormalChargeItem( n2 nSlotPos, n2 nItemPos, n4 nItemType );			// Cobrança geral
	void RecvNormalChargeItemResult();

#if COMMON_LIB_FIXED
	//void SendQuickChargeItem( n2 nItemPos, n4 nItemType, n4 nChargeType, n1 nChargeTimeIndex,
	//							std::list< nsTimeCharge::sChargeItemInfo >& lChargeItems );	// 즉시 충전
#endif

	void RecvQuickChargeItemResult();

	// Cancelar / resultado do carregamento do item
	void SendChargeCancel( n2 nSlotIndex );
	void RecvChargeCancelResult();

	// Recuperação / resultado de itens recarregáveis
	void SendChargeItemWithdraw( n2 nSlotIndex );
	void RecvChargeItemWithdrawResult();

	// Lista de carregamento Desbloqueio / Resultados
	void SendUnlockChargeList( n2 nSlotIndex );
	void RecvUnlockChargeListResult();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Daily Check Event
	void SendDailyCheckEventRequest();
	void RecvDailyCheckEventResult();
	void SendDailyCheckEventItemGet(n4 nGroupID);
	void RecvDailyCheckEventItemGetResult();
	//////////////////////////////////////////////////////////////////////////

	// Receba as informações da missão inteira e renove
	void RecvQuestAllInfo();

	//////////////////////////////////////////////////////////////////////////
	void SendChangeDigimonEffect( u2 nItemPos, u4 nItemType, n2 nItemCount );
	void RecvChangeDigimonEffect();
	void NTF_DigimonEffectChange();

};

