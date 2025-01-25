
#include "stdafx.h"
#include "DataMng.h"

#include "Flow/FlowMgr.h"
#include "Flow/Flow.h"

#include "_Interface/Adapt/AdaptWareHouse.h"
#include "_Interface/Adapt/AdaptDigimonAchive.h"
#include "_Interface/Adapt/AdaptEnchantOption.h"
cDataMng* g_pDataMng = NULL;

#define		SORT_COOLTIME	20.0f

void cDataMng::GlobalInit()
{
	cData_Quest::GlobalInit();

	assert_cs(g_pDataMng == NULL);
	g_pDataMng = NiNew cDataMng;
	g_pDataMng->Init();
}

void cDataMng::GlobalShotdown()
{
	if (g_pDataMng)
		g_pDataMng->Delete();
	SAFE_NIDELETE(g_pDataMng);

	cData_Quest::GlobalShotDown();
}

void cDataMng::Delete()
{
	m_Inven.Delete();
	m_TEquip.Delete();
	m_Digivice.Delete();
	m_PostLoad.Delete();
	m_Quest.Delete();
	m_QuestOwner.Delete();
	m_Tactics.Delete();
	m_MapRegion.Delete();
	m_ServerSync.Delete();
	m_Cash.Delete(); //제거예정
	m_Attendance.Delete();

	GAME_EVENT_ST.OnEvent(EVENT_CODE::UNION_DELETE_DATA, NULL);

#ifdef GUILD_MARK_DOWN
	m_GuildMark.Delete();
#endif

#ifdef CROSSWARS_SYSTEM
	m_InvenCross.Delete();
	m_CrossTatics.Delete();
#endif
}

void cDataMng::Init()
{
	m_Inven.Init();
	m_TEquip.Init();
	m_Digivice.Init();
	m_PostLoad.Init();
	m_Quest.Init();
	m_QuestOwner.Init();
	m_Tactics.Init();
	m_MapRegion.Init();
	m_ServerSync.Init();
	m_Cash.Init();
	m_Attendance.Init();

#ifdef PLAY_PANELTY
	m_nPlay_NormalTime = m_nPlay_CashTime = m_nPlay_BasePaneltyTime = m_nPlay_BaseFullTime = 0;
	m_nPlay_PaneltyType = nPlayTime::NO_PANELTY;
#endif

#ifdef GUILD_MARK_DOWN
	m_GuildMark.Init();
#endif

	m_SortSeq.SetEndTime(SORT_COOLTIME);

#ifdef CROSSWARS_SYSTEM
	m_InvenCross.Init();
	m_CrossTatics.Init();
#endif
	ReleaseItemLock();

	m_MapObject.Init();

	m_pWarehouseAdapt = new AdaptWareHouse;

	if (m_pWarehouseAdapt)
	{
		m_pWarehouseAdapt->SetContents(CONTENTSSYSTEM_PTR->GetContents(E_CT_WAREHOUSE));
	}

	m_pDigimonArchiveAdapt = new AdaptDigimonArchiveSystem;
	if (m_pDigimonArchiveAdapt)
		m_pDigimonArchiveAdapt->SetContents(CONTENTSSYSTEM_PTR->GetContents(E_CT_DIGIMON_ARCHIVE_CONTENTS));

	m_pEnchantOptionAdapt = new AdaptEnchantOption;
	if (m_pEnchantOptionAdapt)
		m_pEnchantOptionAdapt->SetContents(CONTENTSSYSTEM_PTR->GetContents(E_CT_ENCHANT_OPTION_CONTENTS));

	m_nItemTryCount = 0;
}

void cDataMng::_InitNetOff()
{
	m_Inven.InitNetOff();
#ifdef CROSSWARS_SYSTEM
	m_InvenCross.InitNetOff();
	m_CrossTatics.InitNetOff();
#endif
	m_TEquip.InitNetOff();
	m_Digivice.InitNetOff();
	m_PostLoad.InitNetOff();
	m_Quest.InitNetOff();
	m_Tactics.InitNetOff();
	m_MapRegion.InitNetOff();
	m_Cash.InitNetOff();
	m_Attendance.InitNetOff();
	GAME_EVENT_ST.OnEvent(EVENT_CODE::UNION_INITNET_OFF, NULL);

	m_SortSeq.SetEndTime(SORT_COOLTIME);
}

//Chamado no início do carregamento, após retirar as luzes e outras coisas do terreno
void cDataMng::Reset()
{
#ifndef UI_INVENTORY_RENEWAL
	m_Inven.Reset();
#endif
	m_TEquip.Reset();
	m_Digivice.Reset();
	m_PostLoad.Reset();
	m_ServerSync.Reset();
	m_Cash.Reset();

	m_ListItem.clear();
#ifdef CROSSWARS_SYSTEM
	m_CrossListItem.clear();
#endif

	m_nSort = 0;
	m_SortSeq.Start();

	ReleaseItemLock();
	m_nItemTryCount = 0;

	bool bIsEnable = true;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable);

	if (IsNetOff() == true)
		_InitNetOff();
}

void cDataMng::LogOut()
{
	m_MapRegion.LogOut();

	GAME_EVENT_ST.OnEvent(EVENT_CODE::UNION_DELETE_DATA, NULL);

#ifdef PLAY_PANELTY
	m_nPlay_NormalTime = m_nPlay_CashTime = m_nPlay_BasePaneltyTime = m_nPlay_BaseFullTime = 0;
	m_nPlay_PaneltyType = nPlayTime::NO_PANELTY;
#endif

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_LOGOUT, NULL);
}

void cDataMng::Update()
{
	m_QuestOwner.Update();

#ifdef GUILD_MARK_DOWN
	m_GuildMark.Update();
#endif
	m_ServerSync.Update();

	//////////////////////////////////////////////////////////////////////////
	///Código temporário
	//////////////////////////////////////////////////////////////////////////
	//Determina quantas vezes por segundo classificar especificando o número de classificações.
	if (IsSort() == true)
	{
		if (m_NewSort.Update())	// Corra uma vez a cada 0,1 segundos.
			_SendSort();
	}
}

void cDataMng::DiedAlready()
{
	ReleaseItemLock();
	CDigimonUser* pUser;
	for (int i = 0; i < MAX_DIGIMONUSER_COUNT; ++i)
	{
		pUser = g_pCharMng->GetDigimonUser(i);
		if (pUser)
			pUser->ReleaseSrvSendSkill();
	}
}

NiColor cDataMng::GetItemColor(CsItem::sINFO const* pFTItem)
{
	SAFE_POINTER_RETVAL(pFTItem, FONT_WHITE);
	return GetItemColor(pFTItem->s_nClass);
}

NiColor cDataMng::GetItemColor(int nItemClass)
{
	switch (nItemClass)
	{
	case 1:		return NiColor(167 / 255.0f, 167 / 255.0f, 167 / 255.0f);
	case 2:		return FONT_WHITE;
	case 3:		return NiColor(34 / 255.0f, 255 / 255.0f, 73 / 255.0f);
	case 4:		return NiColor(28 / 255.0f, 229 / 255.0f, 255 / 255.0f);
	case 5:		return NiColor(255 / 255.0f, 41 / 255.0f, 178 / 255.0f);
	case 6:		return NiColor(225 / 255.0f, 118 / 255.0f, 72 / 255.0f);
	case 7:		return NiColor(255 / 255.0f, 212 / 255.0f, 72 / 255.0f);
		//case 8:	return NiColor( 255/255.0f, 102/255.0f, 255/255.0f );
	case 9:		return NiColor(255 / 255.0f, 0, 87.0f / 255.0f);
	case 10:	return NiColor(255 / 255.0f, 240 / 255.0f, 0);
	case 11:	return FONT_WHITE;
	case 12:	return FONT_WHITE;
	case 13:	return NiColor(255 / 255.0f, 240 / 255.0f, 0);
	}
	assert_csm1(false, _T("Item Class Color Error = %d"), nItemClass);
	return FONT_WHITE;
}

void cDataMng::ItemAllLock()
{
	int nCount = GetInven()->GetInvenSlotCount();
	for (int i = 0; i < nCount; ++i)
	{
		ItemLock(TO_INVEN_SID(i));
	}
}

void cDataMng::ItemLock(int nLockIndex)
{
	int nCount = m_vpItemLockIndex.Size();
	for (int i = 0; i < nCount; ++i)
	{
		// Se houver o mesmo, é devolvido
		if (m_vpItemLockIndex[i] == nLockIndex)
		{
			//assert_cs( false );
			return;
		}
	}
	m_vpItemLockIndex.PushBack(nLockIndex);
}

void cDataMng::ItemUnlock(int nLockIndex)
{
	m_vpItemLockIndex.DeleteElement_FromData(nLockIndex);
}

bool cDataMng::IsItemLock(int nLockIndex)
{
	if (m_vpItemLockIndex.Size() == 0)
		return false;
	return m_vpItemLockIndex.IsExistData(nLockIndex);
}

bool cDataMng::_IsEnableItemMove(int nSrcSrvID, int nDestSrvID)
{
	assert_cs(IsItemLock(nSrcSrvID) == false);
	assert_cs(IsItemLock(nDestSrvID) == false);

	switch (TO_CONSTANT(nSrcSrvID))
	{
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		if (TO_ID(nSrcSrvID) >= (int)GetInvenCross()->GetInvenSlotCount())
		{
			// Dest deve estar chuvoso.
			switch (TO_CONSTANT(nDestSrvID))
			{
			case SERVER_DATA_INVENCROSS_CONSTANT:
				if (g_pDataMng->GetInvenCross()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
			default:
				assert_cs(false);
			}
		}
		break;
#endif
	case SERVER_DATA_INVEN_CONSTANT:
		if (TO_ID(nSrcSrvID) >= (int)GetInven()->GetInvenSlotCount())
		{
			// Dest deve estar chuvoso.
			switch (TO_CONSTANT(nDestSrvID))
			{
#ifdef CROSSWARS_SYSTEM
			case SERVER_DATA_INVENCROSS_CONSTANT:
				if (g_pDataMng->GetInvenCross()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
#endif
			case SERVER_DATA_INVEN_CONSTANT:
				if (g_pDataMng->GetInven()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				//				if( g_pDataMng->m_pWarehouseAdapt->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
				if (m_pWarehouseAdapt->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
#ifdef NEW_SHARESTASH
			case SERVER_DATA_SHARESTASH_CONSTANT:
				if (m_pWarehouseAdapt->GetShareData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
#endif
			case SERVER_DATA_TEQUIP_CONSTANT:
				if (g_pDataMng->GetTEquip()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
			case SERVER_DATA_MEMORY_CONSTANT:
				if (g_pDataMng->GetTactics()->GetTactics(TO_ID(nDestSrvID))->s_Type != 0)
					return false;
			case SERVER_DATA_CHIPSET_CONSTANT:
				if (g_pDataMng->GetDigivice()->GetChipset(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			default:
				assert_cs(false);
			}
		}
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		if (TO_ID(nSrcSrvID) >= (int)m_pWarehouseAdapt->GetSlotCount())
		{
			// Dest deve estar chuvoso.
			switch (TO_CONSTANT(nDestSrvID))
			{
			case SERVER_DATA_INVEN_CONSTANT:
				if (g_pDataMng->GetInven()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				if (m_pWarehouseAdapt->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
#ifdef NEW_SHARESTASH
			case SERVER_DATA_SHARESTASH_CONSTANT:
				if (m_pWarehouseAdapt->GetShareData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
#endif
			case SERVER_DATA_TEQUIP_CONSTANT:
				if (g_pDataMng->GetTEquip()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
			case SERVER_DATA_MEMORY_CONSTANT:
				if (g_pDataMng->GetTactics()->GetTactics(TO_ID(nDestSrvID))->s_Type != 0)
					return false;
			case SERVER_DATA_CHIPSET_CONSTANT:
				if (g_pDataMng->GetDigivice()->GetChipset(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			default:
				assert_cs(false);
			}
		}
		break;



#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
		if (TO_ID(nSrcSrvID) >= (int)m_pWarehouseAdapt->GetShareSlotCount())
		{
			// Dest deve estar chuvoso.
			switch (TO_CONSTANT(nDestSrvID))
			{
			case SERVER_DATA_INVEN_CONSTANT:
				if (g_pDataMng->GetInven()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				if (m_pWarehouseAdapt->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			case SERVER_DATA_SHARESTASH_CONSTANT:
				if (m_pWarehouseAdapt->GetShareData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			case SERVER_DATA_TEQUIP_CONSTANT:
				if (g_pDataMng->GetTEquip()->GetData(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
			case SERVER_DATA_MEMORY_CONSTANT:
				if (g_pDataMng->GetTactics()->GetTactics(TO_ID(nDestSrvID))->s_Type != 0)
					return false;
			case SERVER_DATA_CHIPSET_CONSTANT:
				if (g_pDataMng->GetDigivice()->GetChipset(TO_ID(nDestSrvID))->IsEnable() == true)
					return false;
				break;
			default:
				assert_cs(false);
			}
		}
		break;
#endif

	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
	case SERVER_DATA_DIGIVICE_CONSTANT:
		break;
	default:
		assert_cs(false);
	}

	switch (TO_CONSTANT(nDestSrvID))
	{
	case SERVER_DATA_INVEN_CONSTANT:
		if (TO_ID(nDestSrvID) >= (int)GetInven()->GetInvenSlotCount())
			return false;
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		if (TO_ID(nDestSrvID) >= (int)m_pWarehouseAdapt->GetSlotCount())
			return false;
		break;
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
		if (TO_ID(nDestSrvID) >= (int)m_pWarehouseAdapt->GetShareSlotCount())
			return false;
		break;
#endif
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		if (TO_ID(nDestSrvID) >= (int)GetInvenCross()->GetInvenSlotCount())
			return false;
		break;
#endif
	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
	case SERVER_DATA_DIGIVICE_CONSTANT:
		break;
	default:
		assert_cs(false);
	}

	return true;
}

bool cDataMng::IsItemUse(int nSrvIndex)
{
	if (FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_LOADING)
		return false;

	if (FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_DATSCENTER ||
		FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_DATATRADECENTER)
	{
		cPrintMsg::PrintMsg(10004);
		return false;
	}

	//=============================================================================================================
	//	Tutorial
	//=============================================================================================================

	bool bTutorialPlaying = false;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_IS_TUTORIAL_PLAYING, &bTutorialPlaying);
	if (bTutorialPlaying)
	{
		cPrintMsg::PrintMsg(30412);
		return false;
	}

	//=============================================================================================================
	//	Morte
	//=============================================================================================================
	if (g_pCharMng->GetDigimonUser(0)->IsServerDie() == true)
	{
		return false;
	}

	//=============================================================================================================
	//	Presença
	//=============================================================================================================
	cItemInfo* pItem = SrvID2ItemInfo(nSrvIndex);
	if (pItem == NULL)
		return false;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItem->m_nType);
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	//=============================================================================================================
	//	Verifique a presença ou ausência de uso na batalha
	//=============================================================================================================
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		if (pFTItem->GetInfo()->s_nUseBattle == 0)
		{
			cPrintMsg::PrintMsg(30455);
			return false;
		}
	}

	//=============================================================================================================
	//	Verifique a condição anormal
	//=============================================================================================================
	if (g_pCharMng->GetDigimonUser(0)->GetBuff()->IsUnusualCond(cBuffData::UNUSUAL_ITEM) == true)
	{
		cPrintMsg::PrintMsg(30455);
		return false;
	}

	//=============================================================================================================
	//	Verifique quando o uso
	//=============================================================================================================
	switch (pFTInfo->s_nUseMode)
	{
	case 0:				// A qualquer momento
		break;
	case 1:				// Em batalha
		if (g_pCharMng->GetTamerUser()->IsBattle() == true)
		{
			cPrintMsg::PrintMsg(16002);
			cDigimonTalk::Print(30011);
			return false;
		}
		break;
	case 2:
		assert_cs(false);
		if (g_pCharMng->GetTamerUser()->IsBattle() == false)
		{
			// Quantas vezes ??????
			cPrintMsg::PrintMsg(16002);
			return false;
		}
		break;
	default:
		assert_cs(false);
	}

	//=============================================================================================================
	//	alvo
	//=============================================================================================================
	CsC_AvObject* pTarget = NULL;
	switch (pFTInfo->s_nUseCharacter)
	{
	case 0:		// Nenhum
	case 1:		// Ambos usam
		pTarget = g_pCharMng->GetTamerUser();
		break;
	case 2:		// Digimon disponível
		pTarget = g_pCharMng->GetDigimonUser(0);
		break;
	case 3:		// Tamer disponível	
		pTarget = g_pCharMng->GetTamerUser();
		break;
	default:
		assert_cs(false);
	}

	//=============================================================================================================
	//nível
// ============================================= === ============================================ ======= =====
// Se você o remover da janela do equipamento, não verificará o nível.
	if (nItem::TabEquip != (nSrvIndex / 1000))
	{
		switch (pFTInfo->s_nType_L)
		{
		case nItem::Chipset:		break;// O chipset não é uma verificação nivelada
		case nItem::Head:// equipamento
		case nItem::Coat:
		case nItem::Glove:
		case nItem::Pants:
		case nItem::Shoes:
		case nItem::Costume:
		case nItem::Glass:
		case nItem::Necklace:
		case nItem::Earring:
		case nItem::Ring:
		case nItem::EquipAura: //Adição de aura chu8820
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
		case nItem::Bracelet:
#endif
		{
			/*Verifique se você sentir*/
			if (pItem->m_nRate == 0)
				break;
		}
		default:
		{
			if (!_CheckItemUseLevel(pItem))
				return false;
		}break;
		}
	}

	//=============================================================================================================
	//	busca
	//=============================================================================================================
	if (pFTInfo->s_nQuestRequire)
	{
		if (g_pDataMng->GetQuest()->IsCompleate(pFTInfo->s_nQuestRequire) == false)
		{
			if (nsCsFileTable::g_pQuestMng->IsQuest(pFTInfo->s_nQuestRequire))
			{
				cPrintMsg::PrintMsg(14008, nsCsFileTable::g_pQuestMng->GetQuest(pFTInfo->s_nQuestRequire)->m_szTitleText);
			}
			else
			{
				assert_csm1(false, _T("아이템 요구 퀘스트 %d가 존재하지 않습니다"), pFTInfo->s_nQuestRequire);
			}
			return false;
		}
	}


	//=============================================================================================================
	//	Esfriar
	//=============================================================================================================
	if (pFTInfo->s_nUseTimeGroup != 0)
	{
		if (pFTInfo->GetCoolTimeSeq()->IsCoolTime())
		{
			// É recarga.
			cPrintMsg::PrintMsg(13002);
			return false;
		}
	}

	//=============================================================================================================
	//	tempo de uso
	// =============================================================================================================
	// se não for um item de equipamento
	switch (pFTInfo->s_nType_L)
	{
		// equipamento
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::Necklace:
	case nItem::Earring:
	case nItem::Ring:
	case nItem::EquipAura: //Adição de aura chu8820
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
		break;
	default:
	{
		if (cItemInfo::IsUseItemTime(pFTInfo))
		{
			if (pItem->m_nEndTime == ITEMINFO_INVALIDE_TIME)
			{
				cPrintMsg::PrintMsg(30362);
				return false;
			}
		}
	}
	}

	return true;
}

cItemInfo* cDataMng::SrvID2ItemInfo(int nSrvID)
{
	switch (TO_CONSTANT(nSrvID))
	{
	case SERVER_DATA_INVEN_CONSTANT:
		if (GetInven()->IsExistItem(TO_ID(nSrvID)) == false)
			return NULL;
		return GetInven()->GetData(TO_ID(nSrvID));

	case SERVER_DATA_TEQUIP_CONSTANT:
		if (GetTEquip()->IsExistItem(TO_ID(nSrvID)) == false)
			return NULL;
		return GetTEquip()->GetData(TO_ID(nSrvID));

	case SERVER_DATA_CHIPSET_CONSTANT:
		if (GetDigivice()->GetChipset(TO_ID(nSrvID))->IsEnable() == false)
			return NULL;
		return GetDigivice()->GetChipset(TO_ID(nSrvID));

	case SERVER_DATA_DIGIVICE_CONSTANT:
		if (GetTEquip()->GetDigiviceItem()->IsEnable() == false)
			return NULL;
		return GetTEquip()->GetDigiviceItem();

	case SERVER_DATA_WAREHOUSE_CONSTANT:
		if (m_pWarehouseAdapt->GetData(TO_ID(nSrvID))->IsEnable() == false)
			return NULL;
		return m_pWarehouseAdapt->GetData(TO_ID(nSrvID));
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
		if (m_pWarehouseAdapt->GetShareData(TO_ID(nSrvID))->IsEnable() == false)
			return NULL;
		return m_pWarehouseAdapt->GetShareData(TO_ID(nSrvID));
#endif
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		if (GetInvenCross()->GetData(TO_ID(nSrvID))->IsEnable() == false)
			return NULL;
		return GetInvenCross()->GetData(TO_ID(nSrvID));
#endif
	}

	assert_csm1(false, _T("SrvID2ItemInfo : %d 상수 정의 안됨"), TO_CONSTANT(nSrvID));
	return NULL;
}

void cDataMng::SendItemUse(int nSrvID)
{
	if (IsItemLock(nSrvID) == true)
	{
		cPrintMsg::PrintMsg(10004);
		return;
	}

	switch (TO_CONSTANT(nSrvID))
	{
	case SERVER_DATA_INVEN_CONSTANT:
		_SendItemUse_Inven(TO_ID(nSrvID));
		break;
	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
		break;
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		_SendItemUse_InvenCross(TO_ID(nSrvID));
		break;
#endif
	}
}

#ifdef CROSSWARS_SYSTEM
void cDataMng::_SendItemUse_InvenCross(int nInvenIndex)
{
	if (IsItemUse(TO_INVENCROSS_SID(nInvenIndex)) == false)
		return;

	cItemInfo* pItem = GetInvenCross()->GetData(nInvenIndex);
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItem->m_nType);
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	CsC_AvObject* pTarget = NULL;
	switch (pFTInfo->s_nUseCharacter)
	{
	case 0:		// Nenhum
	case 1:		// Ambos usam
		pTarget = g_pCharMng->GetTamerUser();
		break;
	case 2:		// Digimon disponível
		pTarget = g_pCharMng->GetDigimonUser(0);
		break;
	case 3:		// Tamer disponível	
		pTarget = g_pCharMng->GetTamerUser();
		break;
	default:
		assert_cs(false);
	}

	//=============================================================================================================
	//	Separação por tipo
	//=============================================================================================================
	switch (pFTInfo->s_nType_L)
	{

	case nItem::CrossWars:
	{
		// Item
		ItemLock(TO_INVENCROSS_SID(nInvenIndex));

		if (net::game)
			net::game->SendItemUseCross(pTarget->GetUniqID(), TO_INVENCROSS_SID(nInvenIndex));
		else
			ServerCrossItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);


	}
	break;

	default:
		assert_csm1(false, _T("item use Error : type = %d"), pFTInfo->s_nType_L);
	}
}

#endif


bool cDataMng::SkillMemoryUse(CsItem::sINFO* pFTInfo, int InvenIndex)
{
	// Verifique o número máximo de números para aprender habilidades
	cSkill* pDigimonSkill = g_pCharMng->GetDigimonUser(0)->GetSkillMng();
	int		nSkillCount = 0;
	int		nLearnCount = 0;
	bool	bSameSkill = false;
	for (int i = 0; i < pDigimonSkill->GetSkillCount(); ++i)
	{
		CsSkill::sINFO* pSkill = pDigimonSkill->GetSkill(i)->s_pFTSkill->GetInfo();

		// Quantas habilidades você tem?
		if (pSkill->s_dwID == pFTInfo->s_dwSkill)
			nLearnCount++;

		USHORT	MemorySkill = 0;
		MemorySkill = pSkill->s_nMemorySkill;	// Tipo de memória de habilidade

		USHORT	ItemType = pFTInfo->s_nType_L;		// Tipo de item.

		if (MemorySkill != nSkill::UNKNOWN_MEMORY_SKILL_TYPE)	// Em caso de habilidade de memória (habilidade de cache)
		{
			nSkillCount++;	// Número de habilidades de memória

			// Teste de duplicado de habilidade do tipo de ataque.
			if (MemorySkill == nSkill::MEMORY_SKILL_ATK && ItemType == nItem::DigimonSkillChipATK)
			{
				bSameSkill = true;
				continue;
			}
			// Teste duplicado de habilidade defensiva.
			if (MemorySkill == nSkill::MEMORY_SKILL_DEF && ItemType == nItem::DigimonSkillChipDEF)
			{
				bSameSkill = true;
				continue;
			}
			// Teste duplicado de habilidade auxiliar.
			if (MemorySkill == nSkill::MEMORY_SKILL_AST && ItemType == nItem::DigimonSkillChipAST)
			{
				bSameSkill = true;
				continue;
			}
		}
	}

	if (g_pCharMng->GetDigimonUser(0)->IsEvolDegener())	// Evolução ou degeneração
	{
		cPrintMsg::PrintMsg(13006);	// Ainda não pode ser usado.
		return false;
	}
	else if (nSkillCount >= nLimit::MAX_ItemSkillDigimon)	// Habilidades máximas
	{
		cPrintMsg::PrintMsg(30681);	// Ele excedeu o número máximo de aprender.
		return false;
	}
	else if (nLearnCount)					// A mesma habilidade
	{
		cPrintMsg::PrintMsg(30682);	// É uma habilidade que já foi adquirida.
		return false;
	}
	else if (bSameSkill)									// Digite a mesma habilidade
	{
		cPrintMsg::PrintMsg(30683, pFTInfo->s_szName);	// O tipo de habilidade de memória não pode mais adquirir.
		return false;
	}

	USHORT	DigimonType = 0;
	DigimonType = pFTInfo->s_nDoNotUseType;
	// Teste de tipo utilizável Digimon
	switch (DigimonType)
	{
	case 0:	// Sem restrições
		break;
	case 1:	// X Jogs de anticorpos, jogadas, tipos de varas cruzadas não estão disponíveis.
	{
		SAFE_POINTER_RETVAL(g_pCharMng, false);
		CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
		SAFE_POINTER_RETVAL(pDigimonUser, false);

		if (pDigimonUser->IsCurJointProgress())
		{
			SAFE_POINTER_RETVAL(nsCsFileTable::g_pBaseMng, false);
			std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_JOINTPROGRESSMODE);
			cPrintMsg::PrintMsg(30684, const_cast<TCHAR*>(name.c_str()));
			return false;
		}
#ifdef CROSSWARS_SYSTEM
		if (nsCsFileTable::g_pDigimonMng->IsCrossDigimon(pDigimonUser->GetBaseDigimonFTID()))
		{
			cPrintMsg::PrintMsg(30684, const_cast<TCHAR*>(UISTRING_TEXT("COMMON_TXT_XROS_WARS").c_str()));
			return false;
		}
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
		if (pDigimonUser->IsSameEvoultionType(nsCsDigimonTable::ET_X_JOGRESS))
		{
			SAFE_POINTER_RETVAL(nsCsFileTable::g_pBaseMng, false);
			std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_X_JOGRESS);
			cPrintMsg::PrintMsg(30684, const_cast<TCHAR*>(name.c_str()));
			return false;
		}

		if (pDigimonUser->IsSameEvoultionType(nsCsDigimonTable::ET_MIX))
		{
			SAFE_POINTER_RETVAL(nsCsFileTable::g_pBaseMng, false);
			std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_MIX);
			cPrintMsg::PrintMsg(30684, const_cast<TCHAR*>(name.c_str()));
			return false;
		}
#endif
	}
	break;
	case 2:	// X-Antibody Jogress, apenas Jogress disponível
	{
		SAFE_POINTER_RETVAL(g_pCharMng, false);
		CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
		SAFE_POINTER_RETVAL(pDigimonUser, false);

#ifdef CROSSWARS_SYSTEM
		if (nsCsFileTable::g_pDigimonMng->IsCrossDigimon(pDigimonUser->GetBaseDigimonFTID()))
		{
			cPrintMsg::PrintMsg(30684, const_cast<TCHAR*>(UISTRING_TEXT("COMMON_TXT_XROS_WARS").c_str()));
			return false;
		}
#endif

		if (!pDigimonUser->IsSameEvoultionType(nsCsDigimonTable::ET_JOINTPROGRESSMODE)
#ifdef SDM_TAMER_XGUAGE_20180628
			&& !pDigimonUser->IsSameEvoultionType(nsCsDigimonTable::ET_X_JOGRESS)
			&& !pDigimonUser->IsSameEvoultionType(nsCsDigimonTable::ET_MIX)
#endif
			)
		{
			SAFE_POINTER_RETVAL(nsCsFileTable::g_pBaseMng, false);
			std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(pDigimonUser->GetEvoType());
			cPrintMsg::PrintMsg(30684, const_cast<TCHAR*>(name.c_str()));
			return false;
		}
	}
	break;
	default:
		assert_cs(false);
		break;
	}


	// Verifique se o Digimon se tornou um nível para registrar a habilidade atual.
	int nDigimonLv = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();
	if (!pFTInfo->IsEnableDigimonLv(nDigimonLv))
	{
		cPrintMsg::PrintMsg(30683, pFTInfo->s_szName);
		return false;
	}

	cPrintMsg::PrintMsg(30680, pFTInfo->s_szName);			// Você quer registrar habilidades Digimon?
	cMessageBox::GetFirstMessageBox()->SetValue1(InvenIndex);	// sobre o slot de inventário do item

	return true;
}

bool cDataMng::_CheckItemUseLevel(cItemInfo const* pItem)
{
	SAFE_POINTER_RETVAL(pItem, false);
	int nTLv = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();
	if (!pItem->IsEnableTamerLv(nTLv))
	{
		int nMinLv = 0; int nMMaxLv = 0;
		pItem->GetTamerReqLevel(nMinLv, nMMaxLv);
		if (nMinLv > nTLv)
			cPrintMsg::PrintMsg(11042, &nMinLv);
		else
			cPrintMsg::PrintMsg(20118, &nMMaxLv);

		return false;
	}

	int nDLv = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();
	if (!pItem->IsEnableDigimonLv(nDLv))
	{
		int nMinLv = 0; int nMMaxLv = 0;
		pItem->GetDigimonReqLevel(nMinLv, nMMaxLv);
		if (nMinLv > nDLv)
			cPrintMsg::PrintMsg(11013, &nMinLv);
		else
			cPrintMsg::PrintMsg(20119, &nMMaxLv);

		return false;
	}

	return true;
}

void cDataMng::_SendItemUse_Inven(int nInvenIndex)
{
	if (IsItemUse(TO_INVEN_SID(nInvenIndex)) == false)
		return;

	cItemInfo* pItem = GetInven()->GetData(nInvenIndex);
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItem->m_nType);
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	CsC_AvObject* pTarget = NULL;
	switch (pFTInfo->s_nUseCharacter)
	{
	case 0:		// Nenhum
	case 1:		// Ambos usam
		pTarget = g_pCharMng->GetTamerUser();
		break;
	case 2:		// Digimon disponível
		pTarget = g_pCharMng->GetDigimonUser(0);
		break;
	case 3:		// Tamer pode ser usado	
		pTarget = g_pCharMng->GetTamerUser();
		break;
	default:
		assert_cs(false);
	}

	//=============================================================================================================
	//	Separação por tipo
	//=============================================================================================================
	switch (pFTInfo->s_nType_L)
	{
		// equipamento
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
#ifdef SDM_TAMER_XGUAGE_20180628
	case nItem::XAI:
#endif
		SendItemMove(TO_INVEN_SID(nInvenIndex), TO_TEQUIP_SID(pFTInfo->s_nType_L - nItem::Head));
		break;
	case nItem::EquipAura: //Adição de aura chu8820
		SendItemMove(TO_INVEN_SID(nInvenIndex), TO_TEQUIP_SID((pFTInfo->s_nType_L - nItem::Head))); //Você precisa ir para 31 sem o tipo de equipamento 30 vezes e tirar mais 1.
		break;
	case nItem::Portal:
	{
		bool bIsBattleRegisted = false;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bIsBattleRegisted);
		if (bIsBattleRegisted)// Se você está se candidatando a uma batalha, o mapa não poderá ser movido.
		{
			cPrintMsg::PrintMsg(40013);	// O inventário não possui espaço livre.
			return;
		}

		// Item
		if (g_pResist->IsEnablePortal())
		{
			ItemLock(TO_INVEN_SID(nInvenIndex));

			if (net::game)
				net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
			else
				ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
		}
	}
	break;
	case nItem::LevelRequire:
	{
		DWORD dwTotalType = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
		switch (dwTotalType)
		{
#if COMMON_LIB_FIXED
			//case nItem::SpiritJewelry:// 스피릿 진화체에만 사용되는 경험치 아이템
			//	{
			//		CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
			//		if( !pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_SPIRIT ) )
			//		{
			//			cPrintMsg::PrintMsg( 31002 );
			//			return;
			//		}
			//	}
			//	break;
#endif
		case nItem::ParcelJewelry:// Poção de EXP que só pode ser usada na venda Digimon
		{
			DWORD dwBaseDigimonFTID = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
			if (!nsCsFileTable::g_pDigimonParcelTBMng->IsParcelDigimon(dwBaseDigimonFTID))
			{
				cPrintMsg::PrintMsg(31002);
				return;
			}
		}
		break;
		default:
			break;
		}

		ItemLock(TO_INVEN_SID(nInvenIndex));

		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
	}
	break;
	// break Não vou usar e vou seguir a rotina abaixo

	// Poções, Cartas, Consumíveis - Use Itens
	case nItem::Potion:
	case nItem::Card:
	case nItem::Consume:
	case nItem::FriendShip_Increase:
#ifdef LEVELUP_EFFECT_LOOP
	case nItem::NoticeItem:
#endif
	case nItem::Fire_Work:
	case nItem::DogimonBattle:
	case nItem::AntiX:
	{
		// item balançando
		ItemLock(TO_INVEN_SID(nInvenIndex));

		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
	}
	break;
	case nItem::SkillLevelOpen:// Use itens de expansão de nível de habilidade Digimon
	{
		SAFE_POINTER_RET(g_pCharMng);
		SAFE_POINTER_RET(nsCsFileTable::g_pBaseMng);

		CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
		SAFE_POINTER_RET(pDigimonUser);

		DWORD dwTotalType = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
		int nEvoType = pDigimonUser->GetEvoType();
		if (!nsCsFileTable::g_pBaseMng->IsEnableSkillMaxLvUpFromEvoType(nEvoType, dwTotalType))
		{
			cPrintMsg::PrintMsg(nsSkillLimitOpenResult::ItemTypeError);// Itens atualmente indisponíveis para Digimon
			return;
		}

		// Verifique se a habilidade da etapa anterior foi aberta
		int nSkillMaxLv = pDigimonUser->GetCurrentEvoMaxSkillLv();
		int nCurrentMaxSkillRanke = nsCsFileTable::g_pBaseMng->GetSkillRankFromSkillMaxLv(nEvoType, nSkillMaxLv);
		int nNeedSkillRank = nsCsFileTable::g_pBaseMng->GetBeforOpenSkillRankeFromItemSubType(dwTotalType);
		if (nCurrentMaxSkillRanke < nNeedSkillRank)// Os níveis de habilidade anteriores não foram abertos.
		{
			cPrintMsg::PrintMsg(nsSkillLimitOpenResult::SkipBeforeLevel);// Itens atualmente indisponíveis para Digimon
			return;
		}

		if (nCurrentMaxSkillRanke > nNeedSkillRank)// O rolo de classificação de habilidade superior já foi aberto.
		{
			cPrintMsg::PrintMsg(nsSkillLimitOpenResult::AlreadyOpen);// Itens atualmente indisponíveis para Digimon
			return;
		}

		int nMaxSkillLv = nsCsFileTable::g_pBaseMng->GetSkillMaxLvFromItemSubType(nEvoType, dwTotalType);
		cPrintMsg::PrintMsg(11048, &nMaxSkillLv);// Itens atualmente indisponíveis para Digimon
		cMessageBox* pMsg = cMessageBox::GetMessageBox(11048);
		if (pMsg)
		{
			pMsg->SetValue1(nInvenIndex);
			ItemUnlock(nInvenIndex);
		}
		return;
	}
	break;
#ifdef FIREWORK
	case nItem::Fire_Work:
	{
		if (net::game)
			net::game->SendFireWork(pTarget->GetUniqID(), pFTInfo->s_dwItemID, nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
	}
	break;
#endif

	case nItem::ScanUse_Item:
	{
		cItemInfo* pTempItem = g_pDataMng->GetInven()->GetData(nInvenIndex);

		int count = nsCsFileTable::g_pItemMng->GetDropCount(pTempItem->m_nType);

		if (nsCsFileTable::g_pItemMng->GetDropCount(pTempItem->m_nType) > g_pDataMng->GetInven()->GetEmptySlotCount())
		{
			cPrintMsg::PrintMsg(11015);
			break;
		}

		// item balançando
		ItemLock(TO_INVEN_SID(nInvenIndex));

		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
	}
	break;
	case nItem::RClick_Hatch:
	{
		cPrintMsg::PrintMsg(30534, pFTInfo->s_szName);
		cMessageBox::GetFirstMessageBox()->SetValue1(nInvenIndex);	// Passe os dados do slot de inventário do item para que o nome do item apareça na caixa de mensagem
		break;
	}
	case nItem::Buff:
	case nItem::SKILL_Card:
	{
		assert_cs(nsCsFileTable::g_pSkillMng->IsSkill(pFTItem->GetInfo()->s_dwSkill) == true);

		for (int i = 0; i < SKILL_APPLY_MAX_COUNT; i++)
		{
			USHORT nBuffID = nsCsFileTable::g_pSkillMng->GetSkill(pFTItem->GetInfo()->s_dwSkill)->GetInfo()->s_Apply[i].s_nBuffCode;

			if (nBuffID == 0)
				break;

			if (SystemBuffCheck(nInvenIndex, pFTItem, pTarget, nBuffID) == true)
				return;

#ifdef LSJ_ALREADY_BUFF_CHECK	//	BUFF CHECK MSG Quase a mesma função Verificação necessária...
			if (AlreadyBuffCheck(nInvenIndex, pFTItem, pTarget, nBuffID) == true)
				return;
#endif

#ifdef BUFF_CHECK_MSG
			CsBuff::sINFO* pFTBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff(nBuffID)->GetInfo();
			cBuffData* pBuff = g_pCharMng->GetTamerUser()->GetBuff();

			std::list< cBuffData::sBUFF_DATA >::iterator it = pBuff->GetBuff()->begin();
			std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pBuff->GetBuff()->end();
			for (; it != itEnd; ++it)
			{
				cItemInfo* pUseItem = g_pDataMng->GetInven()->GetData(nInvenIndex);

				// if( it->s_pBuffTable->GetInfo()->s_dwID == nBuffID )	// comparação original
				if (it->s_pBuffTable->GetInfo()->s_dwID == nBuffID ||	// o mesmo buff
					it->s_pBuffTable->GetInfo()->s_nBuffClass == pFTBuffInfo->s_nBuffClass) // Quando a mesma Classe de Bônus
				{

					{	// 14.03.24 Código adicional kimhoshok
						int CurSkillCode = it->s_pBuffTable->GetInfo()->s_dwSkillCode;// o que está em uso					
						int UseReadySkillCode = pFTItem->GetInfo()->s_dwSkill;// o que você quer usar

						CsSkill::sINFO* pCurInfo = nsCsFileTable::g_pSkillMng->GetSkill(CurSkillCode)->GetInfo();
						CsSkill::sINFO* pReadyInfo = nsCsFileTable::g_pSkillMng->GetSkill(UseReadySkillCode)->GetInfo();

						// Ao tentar usar um buff menor (marcado porque o processamento do cliente é necessário no processamento do Recv do servidor)
						if (pReadyInfo->s_Apply[i].s_nB < pCurInfo->s_Apply[i].s_nB)
						{
							cPrintMsg::PrintMsg(30452);
							return;
						}
					}

					cPrintMsg::PrintMsg(20220);
					cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
					pMBox->SetValue1(pTarget->GetUniqID());
					pMBox->SetValue2(nInvenIndex);

					// item balançando
					ItemLock(TO_INVEN_SID(nInvenIndex));
					return;
				}

			}
#endif
		}

		// item
		ItemLock(TO_INVEN_SID(nInvenIndex));

		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
	}
	break;

	case nItem::Observer:
	{
		// 			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_BATTLE_OBSERVER ) == false )
		// 				g_pGameIF->GetDynamicIF( cBaseWindow::WT_BATTLE_OBSERVER, 0, nInvenIndex );
	}
	break;

	// Não deve entrar quando em uso

	// Ignorar ao usar
	case nItem::DigimonDataChange:
	case nItem::Quest:
	case nItem::Matrial2:
	case nItem::Matrial:
	case nItem::EventItem:
	case nItem::EventItem2:
	case nItem::Capsule_Coin:
	case nItem::Digitama1:
	case nItem::Digitama2:
	case nItem::Digitama3:
	case nItem::Digitama4:
	case nItem::Digitama5:
	case nItem::Digitama6:
	case nItem::Cash_DigiCore:
	case nItem::EItem_All:
	case nItem::EItem_Coat:
	case nItem::EItem_Glove:
	case nItem::EItem_Glass:
	case nItem::EItem_Pants:
	case nItem::EItem_Shoes:
	case nItem::BattleTag:
	case nItem::Excelator:
	case nItem::Cash_Resurection:
	case nItem::Guild:
	case nItem::Cash_ResetSkillPoint:
	case nItem::Cash_BackupDisk:
#ifdef TRICK_ORTREAT
	case nItem::TimeSet:
#endif
	case nItem::BackupDisk:
	case nItem::AttributeItem:
	case nItem::GotchaCoin:
	case nItem::Spirit:
		break;

	case nItem::PersonStore:// Use itens de loja pessoal
	{
		GAME_EVENT_ST.OnEvent(EVENT_CODE::REQ_PERSON_OPEN, &nInvenIndex);
	}
	break;

	case nItem::Digivice:
		SendItemMove(TO_INVEN_SID(nInvenIndex), TO_DIGIVICE_SID(0));
		break;

		// Memória
	case nItem::Memory:
		assert_cs(false);
		break;

		// cápsula
	case nItem::Capsule:
		assert_cs(false);
		break;

		// Chipset
	case nItem::Chipset:
	{
		int nEmptyChipset = GetDigivice()->GetEmptyChipsetSlot();
		if (nEmptyChipset != -1)
		{
			SendItemMove(TO_INVEN_SID(nInvenIndex), TO_CHIPSET_SID(nEmptyChipset));
		}
		else
		{
			cPrintMsg::PrintMsg(cPrintMsg::CHIPSET_FULL);
		}
	}
	break;

	case nItem::Cash_Shouter_N:
	case nItem::Cash_Shouter_S:
	case nItem::Cash_Shouter_NT:
	case nItem::Cash_Shouter_ST:
	case nItem::GM_Shouter_T:
	{
		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_SHOUT_CASH) == false)
			g_pGameIF->GetDynamicIF(cBaseWindow::WT_SHOUT_CASH, 0, nInvenIndex);
	}
	break;

	case nItem::NoQuick_Consume:
	{
		switch (pFTInfo->s_nType_S)	// Use quando houver algo para pré-processar consumíveis gerais
		{
		case 1: // Use itens para aumentar o número de membros da guilda
		{
			std::pair<int, bool> guild;
			guild.first = pFTInfo->s_nTypeValue;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::CHECK_INCREASE_GUILDMEMBERMAX, &guild);
			if (guild.second == true)
				return;
		}
		break;
		case 2:	// Uso de item de redefinição de habilidade Digimon
		{
			cPrintMsg::PrintMsg(30300, g_pCharMng->GetDigimonUser(0)->GetName());
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1(nInvenIndex);
			pMBox->SetValue2(pTarget->GetUniqID());
			return;
		}
		break;
		case 20:		// item de bloqueio de macro
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::USING_MACROPROTECT);
			return;
		}
		break;
		}

		// item balançando
		ItemLock(TO_INVEN_SID(nInvenIndex));

		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);
	}
	break;
	case nItem::TacticsEnchant:
	case nItem::TacticsEnchantSub:
	case nItem::TacticsEnchantReset:
		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_ENCHANT_TACTICS))
		{
			// Vou colocar tanto quanto o número de itens nos slots vazios
			g_pGameIF->GetEnchantTactics()->SetResist(nInvenIndex);
		}
		break;
		// item de redefinição de escala
	case nItem::Rescale:
	{
		DWORD dwBaseDigimonFTID = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
		if (nsCsFileTable::g_pDigimonMng->IsStartDigimon(dwBaseDigimonFTID))
		{	// Parceiro Digimon não pode mudar de tamanho
			cPrintMsg::PrintMsg(30162);
			return;
		}

		DWORD dwTotalType = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;

		// No caso de venda de Digimon, apenas itens de venda podem ser usados			
		bool bParcelDigimon = nsCsFileTable::g_pDigimonParcelTBMng->IsParcelDigimon(dwBaseDigimonFTID);
		if (dwTotalType == nItem::DigimonScaleParcelOut)// Ao usar um item exclusivo para venda Digimon
		{
			if (!bParcelDigimon)
			{
				cPrintMsg::PrintMsg(31000);
				return;
			}
		}
		else
		{
			if (bParcelDigimon)
			{
				cPrintMsg::PrintMsg(31001);
				return;
			}
		}

		cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
		SAFE_POINTER_RET(pPostLoad);
		cData_PostLoad::sDATA* pkDigimonData = pPostLoad->GetDigimonData();
		SAFE_POINTER_RET(pkDigimonData);

		switch (dwTotalType)
		{
#if COMMON_LIB_FIXED
			//case nItem::DigimonScale6LevelExtra:			// 6단 전용 아이템과 조건 동일 - 크기 분포값만 다름
#endif
			//case nItem::DigimonScale6LevelTotal:			// 6단 전용 아이템
			//	{											// 6단 전용 아이템인경우 디지몬이 6단 이상이여만 사용 가능
			//		if( pkDigimonData->s_HatchLevel < 6 )	// 6단 디지몬의 6단 전용 아이템으로만 크기 변경 가능
			//		{
			//			cPrintMsg::PrintMsg( 31000 );
			//			return;
			//		}
			//	}
			//	break;
#if COMMON_LIB_FIXED
			//case nItem::DigimonScaleAllFixStep135:	// 단계 구분 없이 크기 135% 고정 아이템
				break;
#endif
		default:
		{
			// No caso de itens de tamanho normal, Digimons de 6º nível ou superior não podem ser usados.
			if (pkDigimonData->s_HatchLevel >= 6)
			{
				cPrintMsg::PrintMsg(31000);
				return;
			}
		}
		break;
		}

		cPrintMsg::PrintMsg(30160);
		assert_cs(cMessageBox::IsMessageBox());
		cMessageBox::GetFirstMessageBox()->SetValue1(nInvenIndex);
	}
	break;

	// dinheiro
	case nItem::Money:
	{
		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
	}
	break;

	// kash
	case nItem::Cash_ExtraInven:
	{
		if (g_pDataMng->GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_EXTRA_INVEN) == false)
		{
			cPrintMsg::PrintMsg(10004);
			return;
		}
		if (g_pDataMng->GetInven()->GetInvenSlotCount() >= nLimit::Inven)
		{
			cPrintMsg::PrintMsg(20024);
			return;
		}

		cPrintMsg::PrintMsg(20021, pFTInfo->s_szName);
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1(nInvenIndex);
		pMBox->SetValue2(pTarget->GetUniqID());
		pMBox->SetValue3(cData_ServerSync::CASH_EXTRA_INVEN);
	}
	break;
	case nItem::Cash_ExtraWarehouse:
	{
		if (g_pDataMng->GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_EXTRA_WAREHOUSE) == false)
		{
			cPrintMsg::PrintMsg(10004);
			return;
		}
		if (m_pWarehouseAdapt->GetSlotCount() >= nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse)
		{
			cPrintMsg::PrintMsg(20024);
			return;
		}
		cPrintMsg::PrintMsg(20021, pFTInfo->s_szName);
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1(nInvenIndex);
		pMBox->SetValue2(pTarget->GetUniqID());
		pMBox->SetValue3(cData_ServerSync::CASH_EXTRA_WAREHOUSE);
	}
	break;

	case nItem::Cash_DigimonCareSlot:
	{
		if (g_pDataMng->GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_EXTRA_TACTICSHOUSE) == false)
		{
			cPrintMsg::PrintMsg(10004);
			return;
		}
		int nTHouseCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
		if (m_pDigimonArchiveAdapt->GetOpenedArchiveCnt() >= nTHouseCount)
		{
			cPrintMsg::PrintMsg(20024);
			return;
		}  //TODO linha  comentado verificar
		/*if( g_pDataMng->GetTacticsHouse()->GetUseSlotCount() >= nTHouseCount )
		{
			cPrintMsg::PrintMsg( 20024 );
			return;
		}*/
		cPrintMsg::PrintMsg(20021, pFTInfo->s_szName);
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1(nInvenIndex);
		pMBox->SetValue2(pTarget->GetUniqID());
		pMBox->SetValue3(cData_ServerSync::CASH_EXTRA_TACTICSHOUSE);
	}
	break;

	case nItem::Cash_Evoluter:
#if COMMON_LIB_FIXED
	//case nItem::Cash_Evoluter2:
#endif
	case nItem::AntiXEvoluter:
	{
		if (GetServerSync()->IsEmptyRefCount(cData_ServerSync::DIGIMON_EVOLUTION_OPEN) == false)
		{
			cPrintMsg::PrintMsg(10004);
		}
		else
		{
			if (g_pGameIF && !g_pGameIF->IsActiveWindow(cBaseWindow::WT_NEW_DIGIMONSTATUS))
				GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_CLOSE_DIGIMONSTATUS, NULL);
		}
	}
	break;

	case nItem::Cash_ChangeTName:
		if (GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_CHANGE_TNAME) == false)
		{
			cPrintMsg::PrintMsg(10004);
		}
		else
		{
#ifdef UI_INVENTORY_RENEWAL
			int nPointer = 0;
#else
			int nPointer = nInvenIndex / IF_INVENTORY_COUNT;
#endif
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_INVENTORY, nPointer))
			{
				//item balançando
				ItemLock(TO_INVEN_SID(nInvenIndex));
				g_pGameIF->GetInventory(nPointer)->NewInstTamerName(nInvenIndex);
			}
		}
		break;
	case nItem::ChangeTamer:
	{
		if (nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_GAME)
			return;

		if (GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_CHANGE_TAMER) == false)
		{
			cPrintMsg::PrintMsg(10004); // não pode usar.
			return;
		}

		GAME_EVENT_ST.OnEvent(EVENT_CODE::REQ_CHANGETAMER_USINGITEM, &nInvenIndex);
	}
	break;
	case nItem::Cash_ChangeDName:
	{
#ifdef UI_INVENTORY_RENEWAL
		int nPointer = 0;
#else
		int nPointer = nInvenIndex / IF_INVENTORY_COUNT;
#endif
		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_INVENTORY, nPointer))
		{
			// Item
			ItemLock(TO_INVEN_SID(nInvenIndex));
			g_pGameIF->GetInventory(nPointer)->NewInstDigimonName(nInvenIndex);
		}
	}
	break;
	case nItem::Cash_JumpBuster:
	{
		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_JUMPBUSTER) == true)
		{
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_JUMPBUSTER);
		}
		((cJumpBuster*)g_pGameIF->GetDynamicIF(cBaseWindow::WT_JUMPBUSTER, 0, pItem->GetType()))->SetJumpBusterItemInvenPos(nInvenIndex);
	}
	break;

	// itens fixos de termo
	case nItem::Portable_Item:
	{
		if (g_pGameIF->IsOpenNpcWindow(true))
			return;

#ifdef ITEM_USE_TIME_PASS
		if (pItem->m_nEndTime == 0xFFFFFFF0)
		{
			cPrintMsg::PrintMsg(30176, pFTInfo->s_szName);
			cMessageBox::GetFirstMessageBox()->SetValue1(nInvenIndex);
			return;
		}
#endif

		if (g_pCharMng && g_pCharMng->GetTamerUser())
			g_pCharMng->GetTamerUser()->DeletePath();	// Excluir passagem em trânsito.

		if (g_pServerMoveOwner)
			g_pServerMoveOwner->KeyReset();

		switch (pFTInfo->s_nType_S)
		{
		case eHATCH:	// incubadora portátil
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
				return;

			// Não pode ser usado enquanto cavalga ou luta
			SAFE_POINTER_RET(g_pCharMng);
			CTamerUser* pTUser = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET(pTUser);
			if (pTUser->IsRide())
			{
				cPrintMsg::PrintMsg(30014);
				return;
			}
			if (pTUser->IsBattle())
			{
				cPrintMsg::PrintMsg(16002);
				return;
			}

#ifdef ITEM_USE_TIME_PASS
			cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF(cBaseWindow::WT_MAKE_TACTICS, 0, nInvenIndex));
#else
			cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF(cBaseWindow::WT_MAKE_TACTICS));
#endif
			SAFE_POINTER_RET(pWin);
			CsPoint csPos = pWin->GetRootSize();
			GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_POS_DEPENDON_INVEN, &csPos);
			pWin->SetPos(csPos);
			pWin->ResetDevice();
			pWin->SetTarget(NULL, true);
		}
		break;
		case eWARE_HOUSE:	// armazém portátil
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_WAREHOUSE))
				return;

			g_pGameIF->GetDynamicIF(cBaseWindow::WT_WAREHOUSE);
			g_pGameIF->GetWarehouse()->SetTargetPos(g_pCharMng->GetTamerUser()->GetPos2D());
		}
		break;
		case eTATICS_HOUSE:	// Armazenamento Portátil Digimon (Mercenário)
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_TACTICSHOUSE))
				return;

			std::pair<int, NiPoint2> pairData;
			pairData.first = nInvenIndex;
			pairData.second = g_pCharMng->GetTamerUser()->GetPos2D();
			GAME_EVENT_ST.OnEvent(EVENT_CODE::REQ_SHOWVIEWER_BT_ITEM, &pairData);
		}
		break;

		case eSCANNER:	// scanner de mão
		{
			SAFE_POINTER_RET(g_pCharMng);
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET(pTamer);
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SCAN;
			ContentsStream kTmp;
			kTmp << nWindowType << nInvenIndex << pTamer->GetPos2D();
			GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp);
		}
		break;
		case ePAY_BACK:	// recuperador portátil
		{
			SAFE_POINTER_RET(g_pCharMng);
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET(pTamer);
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SELL;
			ContentsStream kTmp;
			kTmp << nWindowType << nInvenIndex << pTamer->GetPos2D();
			GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp);
		}
		break;
#ifdef LJW_EQUIPSYSTEM_190724
		case eANALYSOR:	// analisador portátil
		case eFUSION:	// sintetizador portátil
		{
#ifdef ITEM_USE_TIME_PASS
			int nPortable = nInvenIndex;
#else
			int nPortable = 0;
#endif
			GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_EQUIPSYSTEM_PORTABLE, &nPortable);
		}
		break;
#else // LJW_EQUIPSYSTEM_190724
		case eANALYSOR:	// analisador portátil
		{
#ifdef ITEM_USE_TIME_PASS
			int nPortable = nInvenIndex;
#else
			int nPortable = 0;
#endif
			std::pair< int, int > pairSend(cBaseWindow::WT_ELEMENTITEM_CHECK, nPortable);
			GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend);
		}break;
		case eFUSION:		// sintetizador portátil
		{
#ifdef ITEM_USE_TIME_PASS
			int nPortable = nInvenIndex;
#else
			int nPortable = 0;
#endif
			std::pair< int, int > pairSend(cBaseWindow::WT_ELEMENTITEM_ATTACH, nPortable);
			GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend);
		}break;
#endif // LJW_EQUIPSYSTEM_190724
		default:
			assert_cs(false);
		}
	}
	break;

	// Itens de uso da missão
	case nItem::UseQuest:
	{
		DWORD nQuestID = GetQuest()->UseItem(pFTItem);
		if (nQuestID != 0)
		{
			// item balançando
			ItemLock(TO_INVEN_SID(nInvenIndex));

			// Uma vez que é processado internamente, é processado imediatamente sem enviar um pacote usado separado para o servidor.
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);


			CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();
			CsC_AvObject* pSelectTarget = g_pCharResMng->GetTargetMark();

			// processos separados
			switch (nQuestID)
			{
			case 603:
				pTamer->GetProp_Effect()->AddEffect("_Quest\\Fireworks.nif", 1, 0);
				pTamer->PlaySound("_Quest\\Fireworks.wav");
				break;

			case 707:
				pTamer->SetAnimation(ANI::IDLE_ITEMCROP);
				pTamer->SetPauseAll(true);
				pTamer->PlaySound("_Quest\\water_Get.wav");
#ifndef VERSION_USA// Execução da missão Concluiu a retirada de saída do hangul da peça GSP chu8820
				pTamer->GetProp_Effect()->AddEffect("_Quest\\NormalEffect.nif", 1, 0);
#endif
				break;

			case 661:
				pTamer->SetAnimation(ANI::IDLE_ITEMCROP);
				pTamer->SetPauseAll(true);
				pTamer->GetProp_Effect()->AddEffect("_Quest\\Bomb.nif", 2, 0);
				pTamer->PlaySound("_Quest\\Bomb_Set.wav");
				break;

			case 697:
				if (pSelectTarget)
				{
					pSelectTarget->GetProp_Effect()->AddEffect("_Quest\\SmellBomb.nif", 8.0f, nsEFFECT::SPOS_HALF_THEIGHT);
					pSelectTarget->PlaySound("_Quest\\Smell_Bomb.wav");
				}
				break;

			case 291:
			case 212:		// antena
			{
				NiPoint3 vPos = pTamer->GetDir() * 100.0f + pTamer->GetPos();
				pTamer->GetProp_Effect()->AddEffect("_Quest\\Antenna.nif", 1.0f, 0)->SetWorldPos(vPos);
				pTamer->SetAnimation(ANI::TAKE_OBJECT);
				pTamer->SetPauseTime(CsC_AvObject::PAUSE_PATH, 3.0f);
				pTamer->SetPauseTime(CsC_AvObject::PAUSE_ANIMATION, 3.0f, CsC_AvObject::PTE_ANIMATION, ANI::IDLE_NORMAL);
				pTamer->PlaySound("_Quest\\Antenna.mp3");
			}
			break;

			case 290:
			case 307:		//usar item de água
			{
				NiPoint3 vPos = pTamer->GetDir() * 220.0f + pTamer->GetPos();
				pTamer->GetProp_Effect()->AddEffect("_Quest\\WaterFall0002.nif", 7.0f, 0)->SetWorldPos(vPos);
				pTamer->SetAnimation(ANI::TAKE_OBJECT);
				pTamer->SetPauseTime(CsC_AvObject::PAUSE_PATH, 1.0f);
				pTamer->SetPauseTime(CsC_AvObject::PAUSE_ANIMATION, 1.0f, CsC_AvObject::PTE_ANIMATION, ANI::IDLE_NORMAL);

				//pTamer->PlaySound( "_Quest\\Antenna.mp3" );
				pTamer->PlaySound("_Quest\\Quest_Mission.wav");
			}
			break;

			case 329:
			{
				CTamerUser* pTamer = g_pCharMng->GetTamerUser();
				NiPoint3 vPos = pTamer->GetPos();
				vPos.z -= 200;
				pTamer->GetProp_Effect()->AddEffect("_Quest\\329.nif", 1.7f, 0)->SetPos(vPos);
				pTamer->SetAnimation(ANI::TAKE_OBJECT);
				pTamer->SetPauseTime(CsC_AvObject::PAUSE_PATH, 2.0f);
				pTamer->SetPauseTime(CsC_AvObject::PAUSE_ANIMATION, 2.0f, CsC_AvObject::PTE_ANIMATION, ANI::IDLE_NORMAL);

				pTamer->PlaySound("_Quest\\Quest_Mission.wav");
			}
			break;

			default:
#ifndef VERSION_USA//Missão cumprida Parte de saída Hangul GSP removida chu8820
				pTamer->GetProp_Effect()->AddEffect("_Quest\\NormalEffect.nif", 1, 0);
#endif
				pTamer->PlaySound("_Quest\\Quest_Mission.wav");
			}
		}
		break;
	}
	case nItem::GoldBar:	// Gold Bar	(Calculado como custo de descarte para a loja)
	{
		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);

		// Item
		ItemLock(TO_INVEN_SID(nInvenIndex));
	}
	break;
	case nItem::Item_TamerSKillConsume:
	{
		// Se o número de habilidades em dinheiro for 2 ou mais, não poderá mais ser usado.
		if (g_pCharMng->GetTamerUser()->GetSkillMng()->CountCashSkill() >= 2)
		{
			cPrintMsg::PrintMsg(30582);	// Até 2 habilidades temporárias de domador podem ser registradas.
			break;
		}

		cPrintMsg::PrintMsg(30581, pFTInfo->s_szName);	// Registre sua habilidade domador?
		cMessageBox::GetFirstMessageBox()->SetValue1(nInvenIndex);	// Ignorar slot de inventário de itens
	}
	break;
#ifdef CROSSWARS_SYSTEM
	case nItem::Cash_CrossInvenSlot:
	{
		if (0 == g_pDataMng->GetPostLoad()->GetTamerData()->s_nCrossLoader)
		{
			cPrintMsg::PrintMsg(30589);
			return;
		}
		if (g_pDataMng->GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_EXTRA_CROSSINVEN) == false)
		{
			cPrintMsg::PrintMsg(10004);
			return;
		}
		if ((SHORT)g_pDataMng->GetInvenCross()->GetInvenSlotCount() >= nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven)
		{
			cPrintMsg::PrintMsg(20024);
			return;
		}
		cPrintMsg::PrintMsg(20021, pFTInfo->s_szName);
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1(nInvenIndex);
		pMBox->SetValue2(pTarget->GetUniqID());
		pMBox->SetValue3(cData_ServerSync::CASH_EXTRA_CROSSINVEN);
	}
	break;
	case nItem::CodeCrown:
	{
		// Somente quando o carregador está ativo.


		if (0 == g_pDataMng->GetPostLoad()->GetTamerData()->s_nCrossLoader)
		{
			cPrintMsg::PrintMsg(30589);
			return;
		}
		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);


		// item balançando
		ItemLock(TO_INVEN_SID(nInvenIndex));

	}
	break;

	case nItem::CrossLoader:
	{
		if (net::game)
			net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		else
			ServerItemUseSuccess(pTarget->GetUniqID(), nInvenIndex);


		// item balançando
		ItemLock(TO_INVEN_SID(nInvenIndex));
	}
	break;
	case nItem::CrossWars:
	{
		// Deve ser usado em cross-in-ban.
	}
	break;
#endif

#ifdef DEBUFF_ITEM
	case nItem::Debuff:
	{
		if (g_pCharResMng->GetTargetMark() == NULL)
			return;

		if (g_pCharResMng->GetTargetMark()->GetClass() != nClass::Monster)	// Verifique se o alvo é um monstro
		{
			cPrintMsg::PrintMsg(11101);	// Alvo inválido.
			break;
		}

		bool bEqual = true;

		switch (pFTInfo->s_nSection)
		{
		case 18600:	// MONSTER TARGEN
			if (g_pCharResMng->GetTargetMark()->GetFTID() != pFTInfo->s_nTypeValue)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;

		case 18601:	// Geral Monster Alvo
			if (g_pCharMng->GetMonster(g_pCharResMng->GetTargetMark()->GetIDX())->GetMonsterFT()->GetInfo()->s_nClass != 1)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;

		case 18602:	// alvo de monstro de elite
			if (g_pCharMng->GetMonster(g_pCharResMng->GetTargetMark()->GetIDX())->GetMonsterFT()->GetInfo()->s_nClass != 2)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;

		case 18603:	// Boss Monster Target
			if (g_pCharMng->GetMonster(g_pCharResMng->GetTargetMark()->GetIDX())->GetMonsterFT()->GetInfo()->s_nClass != 3)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;

		case 18604:	// Para monstros chefes de elite
			if (g_pCharMng->GetMonster(g_pCharResMng->GetTargetMark()->GetIDX())->GetMonsterFT()->GetInfo()->s_nClass != 4)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;

		case 18605:	// Especial (Mid -Boss) MONSTER TARGE
			if (g_pCharMng->GetMonster(g_pCharResMng->GetTargetMark()->GetIDX())->GetMonsterFT()->GetInfo()->s_nClass != 5)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;

		case 18606:	// Raid Boss Target
			if (g_pCharMng->GetMonster(g_pCharResMng->GetTargetMark()->GetIDX())->GetMonsterFT()->GetInfo()->s_nClass != 6)
			{
				cPrintMsg::PrintMsg(11101);	// Alvo inválido.
				bEqual = false;
			}
			break;
		}

		if (bEqual == true)
			g_pCharMng->GetTamerUser()->SetDebuffPos(g_pCharResMng->GetTargetMark(), nsCsFileTable::g_pSkillMng->GetSkill(pFTInfo->s_dwSkill)->GetInfo()->s_fAttRange, nInvenIndex);
	}
	break;
#endif
	case nItem::Earring:
	case nItem::Necklace:
	case nItem::Ring:
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
	{
		// se estiver sendo reforçado
		if (m_pEnchantOptionAdapt && m_pEnchantOptionAdapt->IsProcessingEnchant())
			break;

		// Use equipamento se você sentir isso
		if (0 != pItem->m_nRate)
			SendItemMove(TO_INVEN_SID(nInvenIndex), TO_TEQUIP_SID(pFTInfo->s_nType_L - nItem::Head));
	}
	break;
	case nItem::AccStone:
	{
		// se estiver sendo reforçado
		if (m_pEnchantOptionAdapt && m_pEnchantOptionAdapt->IsProcessingEnchant())
			break;
	}
	break;
	case nItem::Incubater:
	{
		// item de incubadora rclick
		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_TACTICSHOUSE))
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::REQ_INCUBATOR_ITEM_IN_INVENTORY, &nInvenIndex);
		}
	}
	break;
	case nItem::CardMaster:// usa o item de selo
	{

		if (GetServerSync()->IsEmptyRefCount(cData_ServerSync::CASH_IDENTIFY_CARD) == false)
		{
			cPrintMsg::PrintMsg(10004);
			return;
		}

		// Antes de abrir o Un Seal UI, feche o Re Seal UI se estiver aberto.
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_CARDRESEAL);
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_CARDIDENTIFY, 0, nInvenIndex);

	}
	break;
	case nItem::UnSealCardMaster:
	case nItem::ReSealCardMaster:	// Quando a janela de registro do selo abrir, cadastre o item.
		break;
	case nItem::DigimonSlotInc:
	{
		int nCurOpenSlot = g_pDataMng->GetTactics()->_GetOpenSlot();	// Estado aberto atual, quantos estão abertos
		if (nCurOpenSlot < nLimit::DigimonBaseSlot - 1)	// Somente quando escrito em um estado não máximo
		{
			// pergunte se o item abre com o item
			cPrintMsg::PrintMsg(20021, pFTInfo->s_szName);
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1(nInvenIndex);
			pMBox->SetValue2(pTarget->GetUniqID());
			pMBox->SetValue3(cData_ServerSync::TACTICS_OPEN);
		}
	}
	break;
	// Ação ao usar item de cápsula de inventário
	case nItem::EvoCapsule:
	{
		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CAPSULE_EVOLUTION))
		{
			cCapsuleEvolution* pCapsuleEvo = g_pGameIF->GetCapsuleEvol();

			if (pCapsuleEvo->IsResist(nInvenIndex))
			{
				pCapsuleEvo->SetResist(nInvenIndex);
			}
		}
	}
	break;

#ifdef MEGAPHONE_BAN
	case nItem::Cash_Shouter_Ban:
	{
		if (g_pCharMng->GetTamerUser()->GetMegaPhoneBan())
		{
			cPrintMsg::PrintMsg(30675);	// Já aplicado.
			return;
		}

		// 			if( g_pDataMng->GetBattle()->IsJoinMatch() )
		// 			{
		// 				cPrintMsg::PrintMsg( 30455 );	// Este recurso não está disponível no momento.
		// 				return;
		// 			}

		if (g_pCharMng->GetTamerUser()->IsBattle())
		{
			cPrintMsg::PrintMsg(16002);	// Não pode ser usado durante o combate.
			return;
		}

		// item balançando
		ItemLock(TO_INVEN_SID(nInvenIndex));

		cPrintMsg::PrintMsg(30676);
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1(pTarget->GetUniqID());
		pMBox->SetValue2(nInvenIndex);
	}
	break;
#endif


	case nItem::DigimonSkillChipATK:	//Memória das características de ataque
	case nItem::DigimonSkillChipDEF:	//Memória característica defensiva
	case nItem::DigimonSkillChipAST:	//Memória característica auxiliar
		SkillMemoryUse(pFTInfo, nInvenIndex);
		break;
	case nItem::DigimonSkillMemoryChip:		// chip de memória	
		break;
	case nItem::RelocateTamer:		// Itens de transferência de servidor usam.
		GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_SERVER_RELOCATE_PAGE_UI, &nInvenIndex);
		break;
	case nItem::TamerSlotExpansion:	// Item de expansão do slot tamer
	{
		if (GLOBALDATA_ST.GetCurrentServerLockSlotCount() <= 0)
		{
			// Você não pode mais expandir o slot tamer.
			cPrintMsg::PrintMsg(10030);
			return;
		}
		else
		{
			ItemLock(TO_INVEN_SID(nInvenIndex));// Item
			if (net::game)
				net::game->SendItemUse(pTarget->GetUniqID(), nInvenIndex);
		}
	}break;

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	case nItem::DigimonEffectChange:
	{
		CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
		SAFE_POINTER_RET(pUser);

		int nTypeS = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
		switch (nTypeS)
		{
		case nItem::EffectChange_Base:		// Liberação do efeito Digimon
		{
			int ePartsIndex = nsDPART::None;
			nsCsFileTable::g_pItemMng->GetPartsIndex(pFTInfo->s_dwItemID, ePartsIndex);

			// Se não houver nada instalado
			if (!pUser->IsPartsApplied(ePartsIndex))
			{
				cPrintMsg::PrintMsg(nsDIGIMON_EFFECT_CHANGE_RES::ALREADY_REMOVED);
				return;
			}
		}
		break;
		case nItem::EffectChange_Special:	// Efeito Digimon equipado
		case nItem::EffectChange_Special_Permanent:	//Item permanente
		{
			int ePartsIndex = nsDPART::None;
			nsCsFileTable::g_pItemMng->GetPartsIndex(pFTInfo->s_dwItemID, ePartsIndex);

			// Se o parceiro digimon pode ser usado
			if (!nsCsFileTable::g_pItemMng->IsEnableDigimonPartsItem(pFTInfo->s_dwItemID, pUser->GetFTID(), ePartsIndex))
			{
				cPrintMsg::PrintMsg(nsDIGIMON_EFFECT_CHANGE_RES::INVALID_DIGIMON);
				return;
			}

			// Se já estiver instalado
			if (pUser->IsPartsApplied(ePartsIndex))
			{
				cPrintMsg::PrintMsg(nsDIGIMON_EFFECT_CHANGE_RES::CHANGE_SAME_EFFECT);
				return;
			}
		}
		break;
		default:
			return;
		}

		if (net::game)
			net::game->SendChangeDigimonEffect(nInvenIndex, pFTInfo->s_dwItemID, pItem->GetCount());
	}
	break;
#endif

	default:
		assert_csm1(false, _T("item use Error : type = %d"), pFTInfo->s_nType_L);
	}
}
#ifdef CROSSWARS_SYSTEM
void cDataMng::ServerCrossItemUseSuccess(uint nTargetUID, int nInvenIndex)
{
	cItemInfo* pItem = GetInvenCross()->GetData(nInvenIndex);
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItem->m_nType);
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	//=============================================================================================================
	//	alvo
	//=============================================================================================================
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(nTargetUID);

	//=============================================================================================================
	//	Esfriar
	//=============================================================================================================
	if (pFTInfo->s_nUseTimeGroup != 0)
	{
		// Aplicação de recarga
		pFTInfo->GetCoolTimeSeq()->Start();
	}

	//=============================================================================================================
	//	som
	//=============================================================================================================
	if (pTarget)
	{
		switch (pFTInfo->s_dwSoundID)
		{
		case 0:		break;
		case 1:		pTarget->PlaySound(SOUND_ITEM_DRINK);		break;
		case 2:		pTarget->PlaySound(SOUND_ITEM_SWALLOW);	break;
		case 3:		pTarget->PlaySound(SOUND_ITEM_FOOD);		break;
		case 4:		pTarget->PlaySound(SOUND_ITEM_DISK);		break;
		case 5:		pTarget->PlaySound(SOUND_ITEM_PORTAL);	break;
		default:	assert_cs(false);
		}
	}

	//=============================================================================================================
	//	efeito
	//=============================================================================================================
	if (pTarget)
	{
		if (pFTInfo->s_dwSkill != 0 && nsCsItemTable::SKILLCODE == pFTInfo->s_nSkillCodeType)
		{
			CsSkill* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill(pFTInfo->s_dwSkill);

			switch (pFTSkill->GetInfo()->s_Apply[0].s_nA)
			{
			case 1:	// HP
				pTarget->GetProp_Effect()->AddEffect(EFFECT_FOOD, 1.0f, nsEFFECT::POS_CHARPOS);
				break;
			case 2:	// DS
				pTarget->GetProp_Effect()->AddEffect(EFFECT_DG, 1.0f, nsEFFECT::POS_CHARPOS);
				break;
			case 15: // EXP
			{
				pTarget->PlaySound(SOUND_LEVEL_UP);
				DWORD dwPlag = nsEFFECT::POS_CHARPOS;
				pTarget->GetProp_Effect()->AddEffect(EFFECT_DIGIMON_LEVELUP, pTarget->GetToolWidth() * 0.02f, dwPlag);
			}
			break;
			}
		}
	}

	//=============================================================================================================
	//	Post -Processing -Client Processing
	//=============================================================================================================
	switch (pFTInfo->s_nType_L)
	{

	case nItem::CrossWars:
	{
		pTarget->PlaySound(SOUND_USECROSSITEM);
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;


	default:
		assert_cs(false);
	}

	ItemUnlock(TO_INVENCROSS_SID(nInvenIndex));
}
#endif

void cDataMng::ServerItemUseSuccess(uint nTargetUID, int nInvenIndex)
{
	cItemInfo* pItem = GetInven()->GetData(nInvenIndex);
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItem->m_nType);
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	//=============================================================================================================
	//	alvo
	//=============================================================================================================
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(nTargetUID);

	//=============================================================================================================
	//	Esfriar
	//=============================================================================================================
	if (pFTInfo->s_nUseTimeGroup != 0)
	{
		// Esfriar
		pFTInfo->GetCoolTimeSeq()->Start();
	}

	//=============================================================================================================
	//	som
	//=============================================================================================================
	if (pTarget)
	{
		switch (pFTInfo->s_dwSoundID)
		{
		case 0:		break;
		case 1:		pTarget->PlaySound(SOUND_ITEM_DRINK);		break;
		case 2:		pTarget->PlaySound(SOUND_ITEM_SWALLOW);	break;
		case 3:		pTarget->PlaySound(SOUND_ITEM_FOOD);		break;
		case 4:		pTarget->PlaySound(SOUND_ITEM_DISK);		break;
		case 5:		pTarget->PlaySound(SOUND_ITEM_PORTAL);	break;
		default:	assert_cs(false);
		}
	}

	//=============================================================================================================
	//	efeito
	//=============================================================================================================
	if (pTarget)
	{
		if (pFTInfo->s_dwSkill != 0 && nsCsItemTable::SKILLCODE == pFTInfo->s_nSkillCodeType)
		{
			CsSkill* pFTSkill = NULL;
			if (pFTInfo->s_nType_L == 180)
			{
				CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill(pFTInfo->s_dwSkill, -1);
				pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill(pTamerSkill->Get_TSkill_Info()->s_dwSkillCode);
			}
			else
				pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill(pFTInfo->s_dwSkill);

			switch (pFTSkill->GetInfo()->s_Apply[0].s_nA)
			{
			case 1:	// HP
				pTarget->GetProp_Effect()->AddEffect(EFFECT_FOOD, 1.0f, nsEFFECT::POS_CHARPOS);
				break;
			case 2:	// DS
				pTarget->GetProp_Effect()->AddEffect(EFFECT_DG, 1.0f, nsEFFECT::POS_CHARPOS);
				break;
			case 15: // EXP
			{
				pTarget->PlaySound(SOUND_LEVEL_UP);
				DWORD dwPlag = nsEFFECT::POS_CHARPOS;
				pTarget->GetProp_Effect()->AddEffect(EFFECT_DIGIMON_LEVELUP, pTarget->GetToolWidth() * 0.02f, dwPlag);
			}
			break;
			}
		}
	}

	//=============================================================================================================
	//	Post -Processing -Client Processing
	//=============================================================================================================
	switch (pFTInfo->s_nType_L)
	{
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::Earring:
	case nItem::Necklace:
	case nItem::Ring:
	case nItem::EquipAura: //Adição de aura chu8820
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
	case nItem::Memory:
	case nItem::Capsule:
	case nItem::Chipset:
	case nItem::Quest:
	case nItem::Matrial2:
	case nItem::Digitama1:
	case nItem::Digitama2:
	case nItem::Digitama3:
	case nItem::Digitama4:
	case nItem::Digitama5:
	case nItem::Digitama6:
	case nItem::EItem_All:
	case nItem::EItem_Coat:
	case nItem::EItem_Glove:
	case nItem::EItem_Glass:
	case nItem::EItem_Pants:
	case nItem::EItem_Shoes:
	case nItem::TacticsEnchant:
	case nItem::TacticsEnchantSub:
		assert_cs(false);
		break;

		// Poção, cartão, consumíveis
	case nItem::LevelRequire:
	case nItem::Potion:
	case nItem::Card:
	case nItem::Portal:
	case nItem::Consume:
	case nItem::UseQuest:
	case nItem::Rescale:
	case nItem::FriendShip_Increase:
#ifdef CROSSWARS_SYSTEM
	case nItem::CodeCrown:
#endif
#ifdef LEVELUP_EFFECT_LOOP
	case nItem::NoticeItem:
#endif
	case nItem::Fire_Work:
	case nItem::RelocateTamer:		// Item de transferência do servidor Tamer
	case nItem::AntiX:
	{
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
	case nItem::TamerSlotExpansion:	// Item de expansão do slot tamer
	{
		cPrintMsg::PrintMsg(10032);
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
#ifdef CROSSWARS_SYSTEM
	case nItem::CrossLoader:
	{
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();

		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_ItemUse;
		recv.nValue1 = pFTInfo->s_dwItemID;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
	}
	break;
#endif
	case nItem::ScanUse_Item:
	{
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	case nItem::DigimonEffectChange:
	{
		int nTypeS = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
		switch (nTypeS)
		{
		case nItem::EffectChange_Base:
		case nItem::EffectChange_Special:
			pItem->DecreaseCount(1);	// Número reduzido de itens
			break;
		case nItem::EffectChange_Special_Permanent:	// Item permanente
			break;
		}

		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}break;
#endif
	case nItem::Buff:
	case nItem::SKILL_Card:
	{
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();

		if (pTarget->GetLeafRTTI() == RTTI_TAMER ||
			pTarget->GetLeafRTTI() == RTTI_TAMER_USER)
		{
			g_pCharMng->GetDigimon(((CTamer*)pTarget)->GetDigimonLink())->GetProp_Animation()->SetAnimation(ANI::ACT_BUFF);
		}
	}
	break;

	case nItem::NoQuick_Consume:
	{
		switch (pFTInfo->s_nType_S)	// Usado quando há algo a ser processado após consumíveis gerais
		{
		case 2: // Use o item de inicialização do Digi Monskill
		{
			float fScale = g_pCharMng->GetDigimonUser(0)->GetScale() * 0.3f;
			DWORD dwMask = nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CAMERA_AXIS_Z | nsEFFECT::SPOS_BOUND_CENTER;
			g_pCharMng->GetDigimonUser(0)->GetProp_Effect()->AddEffect("System\\SkillResetEffect.nif", fScale, dwMask);
			cPrintMsg::PrintMsg(30301, g_pCharMng->GetDigimonUser(0)->GetName());
		}
		break;
		case 30:	// Aplicação de desempenho do item
		{
			GS2C_RECV_CHECKTYPE recv;
			recv.nType = AchieveContents::CA_ItemUse;
			recv.nValue1 = pFTInfo->s_dwItemID;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
		}
		break;
		}

		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;

	// dinheiro
	case nItem::Money:
	{
		g_pDataMng->GetInven()->IncreaseMoney(pItem->GetCount() * 1000, true);

		pItem->Reset();
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
	// Dinheiro
	case nItem::Cash_ExtraInven:
	{
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_EXTRA_INVEN, 0);
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
	}
	break;
	case nItem::Cash_ExtraWarehouse:
	{
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_EXTRA_WAREHOUSE, 0);
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
	}
	break;
#ifdef CROSSWARS_SYSTEM
	case nItem::Cash_CrossInvenSlot:
	{
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_EXTRA_CROSSINVEN, 0);
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		pTarget->PlaySound(SOUND_USECROSSINVENEXTRAITEM);
	}
	break;
#endif
	case nItem::Cash_DigimonCareSlot:
	{
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_EXTRA_TACTICSHOUSE, 0);
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
	}
	break;

	// Isso é CDATA_SERVERSYNC.
	case nItem::Cash_ChangeTName:
		assert_cs(false);
		break;
	case nItem::DogimonBattle:
	{
		DWORD dwAddPoint = pItem->GetCount();
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ADD_BATTLE_POINT, &dwAddPoint);
		pItem->Reset();
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
	case nItem::GoldBar:
	{
		g_pDataMng->GetInven()->IncreaseMoney(pItem->GetCount() * pFTInfo->s_dwSale, true);

		pItem->Reset();
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
	case nItem::Item_TamerSKillConsume:
	{
		// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
#ifdef DEBUFF_ITEM
	case nItem::Debuff:
	{
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;
#endif
	case nItem::DigimonSlotInc:	// Item aberto do slot mercenário
	{
		//Se você fizer vários itens que abrem vários de uma só vez, precisará modificá -lo;Basta adicionar um por um para estabilizar o projeto
		SAFE_POINTER_BEK(g_pDataMng);
		cData_Tactics* pTactics = g_pDataMng->GetTactics();
		SAFE_POINTER_BEK(pTactics);
		int nCurOpenSlot = pTactics->_GetOpenSlot();
		if (nCurOpenSlot < nLimit::DigimonBaseSlot - 1)	//	Somente quando o número de slots abertos, exceto o Partnermon, é menor que o máximo
		{
			pItem->DecreaseCount(1);
			cData_QuickSlot::CheckItemCount_AllUser();
			pTactics->_SetOpenSlot(nCurOpenSlot + 1);
			// 				g_pDataMng->GetTactics()->SetTacticsCount( nCurOpenSlot + 1 );

							//Insira a 3ª habilidade de substituição Digimon
			cSkill::sINFO* pSkillInfo = cSkill::sINFO::NewInstance();
			pSkillInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill(1000005/*1000008*/);
			pSkillInfo->s_eAnimation = ANI::INVALIDE_ID;
			pSkillInfo->s_CoolTime.SetEndTime(1.0f);
			pSkillInfo->s_nTacticsIndex = nCurOpenSlot;
			pSkillInfo->fUseSkill = &cSkill::__BattleTag_Digimon;

			SAFE_POINTER_BEK(g_pCharMng);
			CTamerUser* pTUser = g_pCharMng->GetTamerUser();
			SAFE_POINTER_BEK(pTUser);
			cSkill* pSkillMng = pTUser->GetSkillMng();
			SAFE_POINTER_BEK(pSkillMng);
			pSkillMng->m_vpInfo.PushInsert(nCurOpenSlot + 3, pSkillInfo);

			// Renovação da janela de habilidades tamer
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);

			GS2C_RECV_EXTEND_ACCOMPANY_SLOT kRecvdata;
			kRecvdata.OpenedCnt = nCurOpenSlot + 1;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ON_EXTENT_ACCOMPANY_SLOT, &kRecvdata);
		}
	}
	break;
#ifdef MEGAPHONE_BAN
	case nItem::Cash_Shouter_Ban:
	{
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();

		g_pCharMng->GetTamerUser()->SetMegaPhoneBan(true);

		cPrintMsg::PrintMsg(30677);	// A função de bloqueio do alto -falante foi aplicada.
	}
	break;
#endif

	case nItem::DigimonSkillChipATK:
	case nItem::DigimonSkillChipDEF:
	case nItem::DigimonSkillChipAST:
	{

		// 			char cEffectPath[ MAX_PATH ] = {0, };
		// 			sprintf_s( cEffectPath, MAX_PATH, "system\\Buff\\%s.nif", pFTInfo->s_cNif );
		// 			// Efeito de registro de habilidades de memória Adicionar
		// 			g_pCharMng->GetDigimonUser(0)->GetProp_Effect()->AddEffect( cEffectPath, sqrt( g_pCharMng->GetDigimonUser(0)->GetToolWidth()*0.02f ), nsEFFECT::POS_CHARPOS );

					// reduz o número de itens
		pItem->DecreaseCount(1);
		cData_QuickSlot::CheckItemCount_AllUser();
		g_pDataMng->GetQuest()->CalProcess();
	}
	break;

	default:
		assert_cs(false);
	}

	ItemUnlock(TO_INVEN_SID(nInvenIndex));
}

void cDataMng::ServerItemUseFailed(int nInvenIndex)
{
	ItemUnlock(TO_INVEN_SID(nInvenIndex));
}
void cDataMng::SetChangeItemLimited(u2 Itempos, u1 limit)
{
	n4 nTab = TO_CONSTANT(Itempos);
	n4 nSlot = TO_ID(Itempos);

	cItemInfo* pItemInfo = NULL;

	switch (nTab)
	{
		//case SERVER_DATA_INVEN_CONSTANT:
		//	{
		//		pItemInfo = GetInven()->GetData( nSlot );			
		//	}
		//	break;
		//case SERVER_DATA_WAREHOUSE_CONSTANT:
		//	{
		//		//pDest = m_pWarehouseAdapt->GetData( nSlot ) ;
		//	}
		//	break;
		//case SERVER_DATA_SHARESTASH_CONSTANT:
		//	{
		//		//pDest = m_pWarehouseAdapt->GetShareData( nSlot ) ;
		//	}
		//	break;
	case SERVER_DATA_CHIPSET_CONSTANT:
		pItemInfo = m_Digivice.GetChipset(nSlot);
		break;
	case SERVER_DATA_DIGIVICE_CONSTANT:
		pItemInfo = GetTEquip()->GetDigiviceItem();
		break;
	default:
		assert_cs(false);
	}

	SAFE_POINTER_RET(pItemInfo);

	pItemInfo->m_nLimited = limit;
}


void cDataMng::SendItemMoveInven(int nSrcSrvID, int nCount /* = 0  */)
{
	// O primeiro slot vazio
	int nEmptySlot = GetInven()->FindEmptySlot(-1);
	if (nEmptySlot == cData_Inven::INVALIDE_INVEN_INDEX)
	{
		cPrintMsg::PrintMsg(11015);
		return;
	}
	SendItemMove(nSrcSrvID, TO_INVEN_SID(nEmptySlot));
}

//==============================================================================================
//		O tipo de armazém compartilhado foi contente.
//===============================================================================================
//===============================================================================================
//		O tipo de armazém foi contente.
//==============================================================================================
void cDataMng::InvenSort()
{
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_INVENTORY, 0) == false) // A janela Inven está no estado?
		return;

	if (IsSort() == true) // Existe algum dado na lista de classificação?
	{
		cPrintMsg::PrintMsg(10004);
		return;
	}
#ifndef NEW_SHARESTASH
#ifndef VERSION_HK
	// Desative os grupos Inbanchang.Três
	bool bIsEnable = false;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable);
#endif
#endif
	// Apague a mordida do ícone no cursor
	CURSOR_ST.ReleaseIcon();
	// 63 como o número total de slots de van
	for (int i = 0; i < (int)GetInven()->GetInvenSlotCount(); i++)
	{
		cItemInfo* pCItemInfo = GetInven()->GetData(i);
		SAFE_POINTER_CON(pCItemInfo);

		int nItemIdx = pCItemInfo->GetType();	// Se o item estiver no slot for

		if (nItemIdx == 0)
			continue;

		int nCnt = 0;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		if (cData_Inven::IsTradeLimitedItem(pCItemInfo))
		{
			nCnt = 1;
		}
		else
#endif
		{
			bool bFind = false;
			std::list< DWORD >::iterator it = m_ListItem.begin();	// Vire a lista de itens.
			std::list< DWORD >::iterator itEnd = m_ListItem.end();
			for (; it != itEnd; it++)
			{
				if (*it == nItemIdx)
				{
					bFind = true;
					break;
				}
			}

			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem(nItemIdx)->GetInfo();
#ifdef ITEM_USE_TIME_PASS
			if ((pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime != 0xFFFFFFF0) ||
				(pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime == 0xFFFFFFF0))
				bFind = false;
#endif
			if (bFind == true)
				continue;

			int nTotalItemCount = GetInven()->GetCount_Item_ID(nItemIdx, true);// Excluindo o número de itens com restrições às transações
			int nMaxCount = pFTInfo->s_nOverlap;
			nCnt = nTotalItemCount / nMaxCount; // A quantidade do item.
			if (nTotalItemCount % nMaxCount > 0)
				nCnt++;

#ifdef ITEM_USE_TIME_PASS
			if ((pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime != 0xFFFFFFF0) ||
				(pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime == 0xFFFFFFF0))
				nCnt = 1;
#endif
		}

		for (int n = 0; n < nCnt; n++)
			m_ListItem.push_back(nItemIdx);

	}
	m_ListItem.sort();	// Classificar na ordem dos índices SOT da lista
	m_nSort = 0;

#ifdef NEW_SHARESTASH
	if (m_ListItem.empty() != true)
	{
		// Desative os grupos Inbanchang.Três
		bool bIsEnable = false;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable);
	}
#endif
	//_SendSort();
	m_NewSort.Start();
}

void cDataMng::_SendSort()
{
	std::list< DWORD >::iterator it = m_ListItem.begin();
	// Ao girar a lista de itens no inventário.
	if (m_ListItem.empty() != true)
	{
		int nSrc = _GetMoveInvenSlot(*it); // Sobreposição
		if (nSrc != cData_Inven::INVALIDE_INVEN_INDEX)
		{
			int nCode = SendItemMove(nSrc, m_nSort); // Índice de slot para mover, índice de slot para inspecionar
			if (nCode != 0)
			{
				g_CriticalLog.Log(_T("인벤정렬실패 : Src = %d, Dest = %d, Max = %d, ErrCode = %d"), nSrc, m_nSort, GetInven()->GetInvenSlotCount(), nCode);
			}
		}
		else
		{
			m_ListItem.erase(it);
			_NextSort();
		}
	}
}

int cDataMng::_GetMoveInvenSlot(DWORD dwItemID)
{
	// O valor do índice que corresponde ao slot verificando o índice do item.
	int nSrc = g_pDataMng->GetInven()->GetSlot_Item_ID_ReverseToSlot(m_nSort, dwItemID);	// Quantos slots são os números de slots para verificar o número do slot para verificar?		

	// Posso sobrepor na minha localização atual?
	cItemInfo* pData = g_pDataMng->GetInven()->GetData(m_nSort);// Traga as informações do item do número do slot a serem inspecionadas.

	if (pData->GetType() == dwItemID) // Se o índice de itens corresponder
	{
		int nMaxCount = nsCsFileTable::g_pItemMng->GetItem(pData->GetType())->GetInfo()->s_nOverlap; // A quantidade máxima dada ao slot do item atual
		if (pData->GetCount() == nMaxCount)
		{
			return cData_Inven::INVALIDE_INVEN_INDEX;
		}

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214		    
		if (cData_Inven::IsTradeLimitedItem(pData))	// Se o item estiver localizado no Hanslot ..invalide_inven_index
		{
			return cData_Inven::INVALIDE_INVEN_INDEX;
		}
#endif

#ifdef ITEM_USE_TIME_PASS
		CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem(pData->GetType())->GetInfo();

		if (pFTInfo->s_bUseTimeType == 1 && pData->m_nEndTime != 0xFFFFFFF0)	//Se o item de tempo parcial estiver passando tempo
		{
			return cData_Inven::INVALIDE_INVEN_INDEX;
		}
#endif	

	}
	return nSrc;
}

void cDataMng::_NextSort()
{
	if (m_ListItem.empty() == true)
	{
		_ReleaseSort();
	}
	else
	{
		m_nSort++;
		_SendSort();
	}
}
#ifdef CROSSWARS_SYSTEM
void cDataMng::_ReleaseCrossSort()
{

	m_nSort = 0;
}
#endif

void cDataMng::_ReleaseSort(bool	Check)
{
	m_ListItem.clear();
	m_nSort = 0;
	// Ativação dos grupos Banchang
	bool bIsEnable = true;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable);
	if (net::game && Check)
		net::game->SendQueryMoneyInven();
}


int cDataMng::SendItemMove(int nSrcSrvID, int nDestSrvID, int nCount /*=0*/)
{
	if (0 != m_nItemTryCount)
		return 100;

	if ((IsItemLock(nSrcSrvID) == true) || (IsItemLock(nDestSrvID) == true))
	{
		cPrintMsg::PrintMsg(10004);
		return 100;
	}

	// Passe se você for para o mesmo lugar
	if (nSrcSrvID == nDestSrvID)
		return 1;

	// Verifique se pode se mover
	if (_IsEnableItemMove(nSrcSrvID, nDestSrvID) == false)
	{
		cPrintMsg::PrintMsg(11016);
		return 2;
	}

	// é um item que não pode ser movido para o armazém
	if (TO_CONSTANT(nDestSrvID) == SERVER_DATA_WAREHOUSE_CONSTANT)
	{
		switch (TO_CONSTANT(nSrcSrvID))
		{
		case SERVER_DATA_INVEN_CONSTANT:
		{
			cItemInfo* pItemInfo = GetInven()->GetData(TO_ID(nSrcSrvID));
			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItemInfo->GetType())->GetInfo();
			if (pFTItem->IsPCBangType()) //seForUmItemDaSalaDePc
			{
				cPrintMsg::PrintMsg(20204);
				return 3;
			}
		}
		break;
		}
	}

	if (TO_CONSTANT(nSrcSrvID) == SERVER_DATA_DIGIVICE_CONSTANT)
	{
		if (_SendItemMove_Digivice(nSrcSrvID, nDestSrvID) == false)
			return 4;
	}
	else if (TO_CONSTANT(nDestSrvID) == SERVER_DATA_DIGIVICE_CONSTANT)
	{
		if (_SendItemMove_Digivice(nDestSrvID, nSrcSrvID) == false)
			return 5;
	}
	else if (TO_CONSTANT(nDestSrvID) == SERVER_DATA_TEQUIP_CONSTANT)// exceçãoParaUsarEquipamentos
	{
		if (!_SendItemMove_Equiped(nDestSrvID, nSrcSrvID))
			return 9;
	}

	// Se for um bloco de areia
	if (true == net::game->IsSendBlock())
		return 6;

	// Se você está fortalecendo os acessórios
	if (m_pEnchantOptionAdapt && m_pEnchantOptionAdapt->IsProcessingEnchant())
		return 7;

	// Item
	ItemLock(nSrcSrvID);
	ItemLock(nDestSrvID);

	// Receba o número
	switch (TO_CONSTANT(nSrcSrvID))
	{
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
	{
		m_nItemTryCount = (nCount == 0 ? GetInvenCross()->GetData(TO_ID(nSrcSrvID))->GetCount() : nCount);
		if (net::game)
		{
			if (m_nItemTryCount == GetInvenCross()->GetData(TO_ID(nSrcSrvID))->GetCount())
			{
				net::game->SendItemMove(nSrcSrvID, nDestSrvID);
			}
			else
			{
				// Integração parcial
				m_nItemTryCount = GetInvenCross()->CalItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);

				assert_cs(m_nItemTryCount >= 0);
				if (m_nItemTryCount == 0)
				{
					ItemUnlock(nSrcSrvID);
					ItemUnlock(nDestSrvID);
					return 6;
				}
				net::game->SendItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);

			}
		}
		else
		{
			ServerItemMoveSuccess(nSrcSrvID, nDestSrvID);
		}
	}
	break;
#endif
	case SERVER_DATA_INVEN_CONSTANT:
	{
		m_nItemTryCount = (nCount == 0 ? GetInven()->GetData(TO_ID(nSrcSrvID))->GetCount() : nCount);
		if (net::game)
		{
			if (m_nItemTryCount == GetInven()->GetData(TO_ID(nSrcSrvID))->GetCount())
			{
				net::game->SendItemMove(nSrcSrvID, nDestSrvID);
			}
			else
			{
				// integração parcial
#ifdef NEW_SHARESTASH
				m_nItemTryCount = GetInven()->CalItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);
#else
				m_nItemTryCount = GetInven()->CalItemSplit(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
#endif
				assert_cs(m_nItemTryCount >= 0);
				if (m_nItemTryCount == 0)
				{
					ItemUnlock(nSrcSrvID);
					ItemUnlock(nDestSrvID);
					return 6;
				}
				net::game->SendItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);
			}
		}
		else
		{
			ServerItemMoveSuccess(nSrcSrvID, nDestSrvID);
		}
	}
	break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
	{
		m_nItemTryCount = (nCount == 0 ? m_pWarehouseAdapt->GetData(TO_ID(nSrcSrvID))->GetCount() : nCount);

		if (net::game)
		{
			if (m_nItemTryCount == m_pWarehouseAdapt->GetData(TO_ID(nSrcSrvID))->GetCount())
			{
				net::game->SendItemMove(nSrcSrvID, nDestSrvID);
			}
			else
			{
				// integração parcial
#ifdef NEW_SHARESTASH
				m_nItemTryCount = m_pWarehouseAdapt->CalItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);
#else
				m_nItemTryCount = m_pWarehouseAdapt->CalItemSplit(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
#endif
				assert_cs(m_nItemTryCount >= 0);
				if (m_nItemTryCount == 0)
				{
					ItemUnlock(nSrcSrvID);
					ItemUnlock(nDestSrvID);
					return 7;
				}
				net::game->SendItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);
			}
		}
		else
		{
			ServerItemMoveSuccess(nSrcSrvID, nDestSrvID);
		}
	}
	break;
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
	{
		m_nItemTryCount = (nCount == 0 ? m_pWarehouseAdapt->GetShareData(TO_ID(nSrcSrvID))->GetCount() : nCount);

		if (net::game)
		{
			if (m_nItemTryCount == m_pWarehouseAdapt->GetShareData(TO_ID(nSrcSrvID))->GetCount())
			{
				net::game->SendItemMove(nSrcSrvID, nDestSrvID);
			}
			else
			{
				// integração parcial
				m_nItemTryCount = m_pWarehouseAdapt->CalItemSplit_ShareStash(nSrcSrvID, nDestSrvID, m_nItemTryCount);
				assert_cs(m_nItemTryCount >= 0);
				if (m_nItemTryCount == 0)
				{
					ItemUnlock(nSrcSrvID);
					ItemUnlock(nDestSrvID);
					return 8;
				}
				net::game->SendItemSplit(nSrcSrvID, nDestSrvID, m_nItemTryCount);
			}
		}
		else
		{
			ServerItemMoveSuccess(nSrcSrvID, nDestSrvID);
		}
	}
	break;
#endif
	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
	case SERVER_DATA_DIGIVICE_CONSTANT:
	{
		m_nItemTryCount = 1;
		if (net::game)
		{
			net::game->SendItemMove(nSrcSrvID, nDestSrvID);
		}
		else
		{
			ServerItemMoveSuccess(nSrcSrvID, nDestSrvID);
		}
	}
	break;
	default:
		assert_cs(false);
	}

	return 0;
}

bool cDataMng::_SendItemMove_Equiped(int nTEquipSrvID, int nInvenSrvID)
{
#ifdef SDM_TAMER_XGUAGE_20180628
	if (TO_ID(nTEquipSrvID) != nsPART::XAI)
		return true;//Verifique apenas quando são itens XAI atualmente

	SAFE_POINTER_RETVAL(g_pCharMng, false);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, false);
	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());

	assert_cs(TO_CONSTANT(nInvenSrvID) == SERVER_DATA_INVEN_CONSTANT);
	cItemInfo* pDestInfo = m_Inven.GetData(TO_ID(nInvenSrvID));
	SAFE_POINTER_RETVAL(pDestInfo, false);
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, false);
	TB::sXAIStruct const* pXaiStruct = nsCsFileTable::g_pItemMng->GetXaiItem_Info(pDestInfo->GetType());// 착용 하려는 아이템이 XAI
	SAFE_POINTER_RETVAL(pXaiStruct, false);

	if (pXaiStruct->s_ucMaxCrystal < pUserState->GetCurrentCrystal() || pXaiStruct->s_unMaxXG < pUserState->GetCurrentXGuage())
	{
		cPrintMsg::PrintMsg(32000);
		cMessageBox* pMsg = cMessageBox::GetFirstMessageBox();
		if (pMsg)
		{
			pMsg->SetValue1(nInvenSrvID);
			pMsg->SetValue2(nTEquipSrvID);

			ItemTryCount();

			ItemLock(nInvenSrvID);
			ItemLock(nTEquipSrvID);
		}
		return false;
	}
#endif
	return true;
}

bool cDataMng::_SendItemMove_Digivice(int nTEquipSrvID, int nInvenSrvID)
{
	assert_cs(TO_CONSTANT(nInvenSrvID) == SERVER_DATA_INVEN_CONSTANT);

	cItemInfo* pDestInfo = GetInven()->GetData(TO_ID(nInvenSrvID));
	// Você está tentando substituí -lo pelo inventário do inventário?
	if (pDestInfo->IsEnable() == true)
	{
		CsItem::sINFO* pFTDest = nsCsFileTable::g_pItemMng->GetItem(pDestInfo->GetType())->GetInfo();
		assert_cs(pFTDest->s_nType_L == 53);

		// Chipset
		int nNeedEmptySlot = 0;
		int nChipsetCount = GetDigivice()->GetChipsetCount();
		if (nChipsetCount > pFTDest->s_nDigiviceChipsetSlot)
		{
			for (int i = pFTDest->s_nDigiviceChipsetSlot; i < nChipsetCount; ++i)
			{
				if (GetDigivice()->GetChipset(i)->IsEnable())
					++nNeedEmptySlot;
			}
		}
		int nSKillCount = GetDigivice()->GetTamerSkillCount();
		if (nSKillCount > pFTDest->s_nDigiviceSkillSlot)
		{
			for (int i = pFTDest->s_nDigiviceSkillSlot; i < nSKillCount; ++i)
			{
				if (GetDigivice()->GetTamerSkill(i)->IsEnable())
					++nNeedEmptySlot;
			}
		}

		// Existe um inventário tanto quanto o espaço em branco?
		if (nNeedEmptySlot > GetInven()->GetEmptySlotCount())
		{
			cPrintMsg::PrintMsg(11015);
			return false;
		}

		if (m_nItemTryCount != 0)
		{
			return false;
		}

		// Se você tiver espaço livre suficiente no inventário, vamos levar o item para o inventário.
		int nFindSlot = GetInven()->FindEmptySlot(-1);
		for (int i = pFTDest->s_nDigiviceChipsetSlot; i < nChipsetCount; ++i)
		{
			if (GetDigivice()->GetChipset(i)->IsEnable())
			{
				m_nItemTryCount = 0;
				assert_cs(nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX);
				SendItemMove(TO_CHIPSET_SID(i), TO_INVEN_SID(nFindSlot), 1);
				nFindSlot = GetInven()->FindEmptySlot(nFindSlot);
			}
		}
		for (int i = pFTDest->s_nDigiviceSkillSlot; i < nSKillCount; ++i)
		{
			if (GetDigivice()->GetTamerSkill(i)->IsEnable())
			{
				m_nItemTryCount = 0;
				assert_cs(nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX);
				SendItemMove(TO_TSKILL_SID(i), TO_INVEN_SID(nFindSlot), 1);
				nFindSlot = GetInven()->FindEmptySlot(nFindSlot);
			}
		}
	}
	// Para colocá -lo na área vazia.
	else
	{
		int nNeedEmptySlot = 1;		// O slot digibice é necessário um básico
		int nChipsetCount = GetDigivice()->GetChipsetCount();
		for (int i = 0; i < nChipsetCount; ++i)
		{
			if (GetDigivice()->GetChipset(i)->IsEnable())
				++nNeedEmptySlot;
		}
		int nSKillCount = GetDigivice()->GetTamerSkillCount();
		for (int i = 0; i < nSKillCount; ++i)
		{
			if (GetDigivice()->GetTamerSkill(i)->IsEnable())
				++nNeedEmptySlot;
		}

		// Existe um inventário tanto quanto o espaço em branco?
		if (nNeedEmptySlot > GetInven()->GetEmptySlotCount())
		{
			cPrintMsg::PrintMsg(11015);
			return false;
		}

		// Se você tiver espaço livre suficiente no inventário, vamos levar o item para o inventário.
		int nFindSlot = GetInven()->FindEmptySlot(-1, TO_ID(nInvenSrvID));
		for (int i = 0; i < nChipsetCount; ++i)
		{
			if (GetDigivice()->GetChipset(i)->IsEnable())
			{
				m_nItemTryCount = 0;
				assert_cs(nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX);
				SendItemMove(TO_CHIPSET_SID(i), TO_INVEN_SID(nFindSlot), 1);
				nFindSlot = GetInven()->FindEmptySlot(nFindSlot, TO_ID(nInvenSrvID));
			}
		}
		for (int i = 0; i < nSKillCount; ++i)
		{
			if (GetDigivice()->GetTamerSkill(i)->IsEnable())
			{
				m_nItemTryCount = 0;
				assert_cs(nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX);
				SendItemMove(TO_TSKILL_SID(i), TO_INVEN_SID(nFindSlot), 1);
				nFindSlot = GetInven()->FindEmptySlot(nFindSlot, TO_ID(nInvenSrvID));
			}
		}
	}

	return true;
}

void cDataMng::ServerItemMoveSuccess(int nSrcSrvID, int nDestSrvID)
{
	assert_cs(IsItemLock(nSrcSrvID) == true);
	assert_cs(IsItemLock(nDestSrvID) == true);
	ItemUnlock(nSrcSrvID);
	ItemUnlock(nDestSrvID);

	int nSrcID = TO_ID(nSrcSrvID);
	int nDestID = TO_ID(nDestSrvID);
	int nSrcConstant = TO_CONSTANT(nSrcSrvID);
	int nDestConstant = TO_CONSTANT(nDestSrvID);

	switch (nSrcConstant)
	{
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
	{
		switch (nDestConstant)
		{
		case SERVER_DATA_INVENCROSS_CONSTANT:

			GetInvenCross()->CrossToCross(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
		case SERVER_DATA_INVEN_CONSTANT:

			GetInvenCross()->CrossToInven(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;

		default:
			assert_cs(false);
		}
	}
	break;
#endif
	// No inventário
	case SERVER_DATA_INVEN_CONSTANT:
	{
		switch (nDestConstant)
		{
#ifdef CROSSWARS_SYSTEM
		case SERVER_DATA_INVENCROSS_CONSTANT:
			GetInvenCross()->InvenToCross(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
#endif
			// Inventário
		case SERVER_DATA_INVEN_CONSTANT:
			GetInven()->ItemMove(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
			// Pelo armazém
		case SERVER_DATA_WAREHOUSE_CONSTANT:
			m_pWarehouseAdapt->InvenToWarehouse(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);

			// Verificação da missão
			g_pDataMng->GetQuest()->CalProcess();
			g_pDataMng->GetQuest()->RevQuestCheck();
			// Verifique o slot rápido
			cData_QuickSlot::CheckItemCount_AllUser();

			break;
#ifdef NEW_SHARESTASH
			// como um armazém compartilhado
		case SERVER_DATA_SHARESTASH_CONSTANT:
			m_pWarehouseAdapt->InvenToShareStash(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);

			// Cheque de missão
			g_pDataMng->GetQuest()->CalProcess();
			g_pDataMng->GetQuest()->RevQuestCheck();
			// Verifique o slot rápido
			cData_QuickSlot::CheckItemCount_AllUser();

			break;
#endif
			// com uma janela de equipamento dométer
		case SERVER_DATA_TEQUIP_CONSTANT:
			assert_cs(m_nItemTryCount == 1);
			GetTEquip()->ToInven(TO_ID(nDestSrvID), TO_ID(nSrcSrvID));
			cWindow::PlaySound(cWindow::SD_At1);
			break;
			// como um digital
		case SERVER_DATA_DIGIVICE_CONSTANT:
			GetTEquip()->ToInven_Digivice(TO_ID(nSrcSrvID));
			cWindow::PlaySound(cWindow::SD_At1);
			break;
			// por chipset
		case SERVER_DATA_CHIPSET_CONSTANT:
			//assert_cs( m_nItemTryCount == 1 );
			GetDigivice()->ChipsetToInven(TO_ID(nDestSrvID), TO_ID(nSrcSrvID));
			cWindow::PlaySound(cWindow::SD_At1);
			break;
		default:
			assert_cs(false);
		}
	}
	break;
	// no equipamento
	case SERVER_DATA_TEQUIP_CONSTANT:
	{
		switch (nDestConstant)
		{
			// como um inventário
		case SERVER_DATA_INVEN_CONSTANT:
			assert_cs(m_nItemTryCount == 1);
			GetTEquip()->ToInven(TO_ID(nSrcSrvID), TO_ID(nDestSrvID));
			cWindow::PlaySound(cWindow::SD_Np1);
			break;
		default:
			assert_cs(false);					// Não pode haver substituição para outros lugares.
		}
	}
	break;
	// do item Digice da janela do equipamento
	case SERVER_DATA_DIGIVICE_CONSTANT:
	{
		switch (nDestConstant)
		{
			// como um inventário
		case SERVER_DATA_INVEN_CONSTANT:
			//assert_cs( m_nItemTryCount == 1 );
			GetTEquip()->ToInven_Digivice(TO_ID(nDestSrvID));
			cWindow::PlaySound(cWindow::SD_Np1);
			break;
		default:
			assert_cs(false);					// Não pode haver substituição para outros lugares.
		}
	}
	break;
	// No chipset
	case SERVER_DATA_CHIPSET_CONSTANT:
	{
		switch (nDestConstant)
		{
			// Inventário
		case SERVER_DATA_INVEN_CONSTANT:
			//assert_cs( m_nItemTryCount == 1 );
			GetDigivice()->ChipsetToInven(TO_ID(nSrcSrvID), TO_ID(nDestSrvID));
			cWindow::PlaySound(cWindow::SD_Np1);
			break;
			// Por chipset
		case SERVER_DATA_CHIPSET_CONSTANT:
			//assert_cs( m_nItemTryCount == 1 );
			GetDigivice()->ChipsetToChipset(TO_ID(nSrcSrvID), TO_ID(nDestSrvID));
			cWindow::PlaySound(cWindow::SD_At1);
			break;
		default:
			assert_cs(false);
		}
	}
	break;
	// No armazém
	case SERVER_DATA_WAREHOUSE_CONSTANT:
	{
		switch (nDestConstant)
		{
			// Inventário
		case SERVER_DATA_INVEN_CONSTANT:
			m_pWarehouseAdapt->WarehouseToInven(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);

			// Verificação da missão
			g_pDataMng->GetQuest()->CalProcess();
			g_pDataMng->GetQuest()->RevQuestCheck();
			// Verifique o slot rápido
			cData_QuickSlot::CheckItemCount_AllUser();

			break;
			// Pelo armazém
		case SERVER_DATA_WAREHOUSE_CONSTANT:
			m_pWarehouseAdapt->WarehouseToWarehouse(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
#ifdef NEW_SHARESTASH
		case SERVER_DATA_SHARESTASH_CONSTANT:
			m_pWarehouseAdapt->WarehouseToShareStash(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
#endif
		default:
			assert_cs(false);
		}
	}
	break;
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
	{
		switch (nDestConstant)
		{
			// Inventário
		case SERVER_DATA_INVEN_CONSTANT:
			m_pWarehouseAdapt->ShareStashToInven(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);

			// Verificação da missão
			g_pDataMng->GetQuest()->CalProcess();
			g_pDataMng->GetQuest()->RevQuestCheck();
			// Verifique o slot rápido
			cData_QuickSlot::CheckItemCount_AllUser();

			break;
			// Pelo armazém
		case SERVER_DATA_WAREHOUSE_CONSTANT:
			m_pWarehouseAdapt->ShareStashToWarehouse(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
		case SERVER_DATA_SHARESTASH_CONSTANT:
			m_pWarehouseAdapt->ShareStashToShareStash(TO_ID(nSrcSrvID), TO_ID(nDestSrvID), m_nItemTryCount);
			cWindow::PlaySound(cWindow::SD_At3);
			break;
		default:
			assert_cs(false);
		}
	}
	break;
#endif
	default:
		assert_cs(false);
	}

	m_nItemTryCount = 0;
}

void cDataMng::ResetItemTryCount()
{
	m_nItemTryCount = 0;
}

void cDataMng::ItemTryCount()
{
	m_nItemTryCount = 1;
}

void cDataMng::ServerItemMoveFailed(int nSrcSrvID, int nDestSrvID)
{
	assert_cs(IsItemLock(nSrcSrvID) == true);
	assert_cs(IsItemLock(nDestSrvID) == true);

	m_nItemTryCount = 0;

	ItemUnlock(nSrcSrvID);
	ItemUnlock(nDestSrvID);

	_ReleaseSort(false);

#ifdef NEW_WAREHOUSESORT
	m_pWarehouseAdapt->_ReleaseWareHouseSort(false);
#endif

#ifdef NEW_SHARESTASH
	m_pWarehouseAdapt->_ReleaseShareStashSort(false);
#endif

	if (net::game)
		net::game->SendQueryMoneyInven();	// Acho que estou atualizando o dinheiro no fósforo van ...
}

bool cDataMng::IsTradeItem(cItemInfo* pItemInfo)
{
	// Verifique o status de pilotagem
	if (g_pCharMng->GetDigimonUser(0)->IsRide())
	{
		cPrintMsg::PrintMsg(30014);
		return false;
	}

	// Item de missão
	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem(pItemInfo->GetType())->GetInfo();
	SAFE_POINTER_RETVAL(pFTInfo, false);

	if ((pFTInfo->s_nType_L == nItem::Quest) || (pFTInfo->s_nType_L == nItem::UseQuest))
	{
		cPrintMsg::PrintMsg(TRADE_RESULT_CANTNOT_TRADE_LIMITED_ITEM);
		return false;
	}

	// É um item pertencente
	if (pItemInfo->m_nLimited != 0)
	{
		cPrintMsg::PrintMsg(TRADE_RESULT_CANTNOT_TRADE_LIMITED_ITEM);
		return false;
	}

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	if (cData_Inven::IsTradeLimitedItem(pItemInfo))
	{
		cPrintMsg::PrintMsg(TRADE_RESULT_CANTNOT_TRADE_LIMITED_CASHITEM);
		return false;
	}
#endif
	return true;
}

bool cDataMng::IsDropItem(cItemInfo* pItemInfo)
{
	// Se você estiver vendendo mercadorias ou item de compra
	//if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
	//{
	//	cPersonStore* pUI =	g_pGameIF->GetPersonStore();
 //		switch( pUI->GetValueType() )
 //		{
 //		case PersonStoreContents::VT_RES_PERSON:
 //		case PersonStoreContents::VT_RES_PERSON_START:
 //		case PersonStoreContents::VT_RES_EMPLOYMENT:
 //		case PersonStoreContents::VT_RES_EMPLOYMENT_START:
 //			cPrintMsg::PrintMsg( 30357 );
 //			return false;
 //		}
	//}

	return true;
}

bool cDataMng::IsOpenStore()
{
	// Verifique o status de pilotagem
	if (g_pCharMng->GetDigimonUser(0)->IsRide())
	{
		cPrintMsg::PrintMsg(30014);
		return false;
	}

	// Não é uma negociação na situação atual.(Uso do NPC, batalha durante combate, evolução, substituição de parceiros)
	if (g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser(0)->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon())
	{
		cPrintMsg::PrintMsg(30024);
		return false;
	}

	// Se houver algo flutuando na caixa de mensagem, a transação falhou
	if (cMessageBox::IsMessageBox())
	{
		cPrintMsg::PrintMsg(30024);
		return false;
	}

	//Não esteja cansado
	if (g_pCharMng->GetTamerUser()->GetCondition()->IsCondition(nSync::Fatigue) == true)
	{
		cPrintMsg::PrintMsg(30358);
		return false;
	}

	float fEnableDist = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fStoreDist;
	NiPoint2 ptUser = g_pCharMng->GetTamerUser()->GetPos2D();

#ifdef CONSIGNMENT_CREATE_RESTRICT
	float fNpcDist = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fNpcDist;
	{
		// Inspeção se houver um portal por aí
		if (nsCsGBTerrain::g_pCurRoot->GetPotalCheck(ptUser, fNpcDist))
		{
			cPrintMsg::PrintMsg(30350);
			return false;
		}
	}
#endif

	// Verifique se há outros domadores por perto
	{
		std::map< DWORD, CTamer* >* pVector = g_pCharMng->GetTamerMap();
		std::map< DWORD, CTamer* >::iterator it = pVector->begin();
		for (; it != pVector->end(); ++it)
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON(pTamer);

			if (pTamer->GetCondition()->IsCondition((nSync::eCondition)(nSync::Shop | nSync::ShopReady)) == 0)
				continue;

			if ((pTamer->GetPos2D() - ptUser).Length() < fEnableDist)
			{
				cPrintMsg::PrintMsg(30350);
				return false;
			}
		}
	}

	// NPC ao redor
	{
		CNpcMng::NpcCont* pVector = g_pNpcMng->GetNpcVector();
		CNpcMng::NpcCont::iterator it = pVector->begin();
		for (; it != pVector->end(); ++it)
		{
			CNpc* pNpc = it->second;
			SAFE_POINTER_CON(pNpc);
#ifdef CONSIGNMENT_CREATE_RESTRICT
			if ((pNpc->GetPos2D() - ptUser).Length() < fNpcDist)
#else
			if ((pNpc->GetPos2D() - ptUser).Length() < fEnableDist)
#endif
			{
				cPrintMsg::PrintMsg(30350);
				return false;
			}
		}
	}


	// Inspeção se houver outras lojas consignadas ao redor
	{
		std::map<DWORD, CEmployment*>* pVector = g_pCharMng->GetEmploymentMap();
		std::map<DWORD, CEmployment*>::iterator it = pVector->begin();
		for (; it != pVector->end(); ++it)
		{
			CEmployment* pEmployment = it->second;
			SAFE_POINTER_CON(pEmployment);
			if ((pEmployment->GetPos2D() - ptUser).Length() < fEnableDist)
			{
				cPrintMsg::PrintMsg(30350);
				return false;
			}
		}
	}


	return true;
}

bool cDataMng::IsScanItem(cItemInfo* pItemInfo)
{
	SAFE_POINTER_RETVAL(pItemInfo, false);
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, false);

	bool bIsRank = false;
	bool bIsType = false;

	// Inspeção de classificação do item
	const CsItemRank::MAP* pkRanks = nsCsFileTable::g_pItemMng->GetItemRank();
	CsItemRank::MAP::const_iterator it = pkRanks->find(pItemInfo->GetType());
	if (it != pkRanks->end())
		bIsRank = true;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItemInfo->GetType());
	SAFE_POINTER_RETVAL(pFTItem, false);

	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL(pFTItemInfo, false);

	// Teste de tipo de item
	switch (pFTItemInfo->s_nType_L)
	{
	case nItem::DigimonSkillChipATK:	//	67	// Memória das características de ataque
	case nItem::DigimonSkillChipDEF:	//	68	// Memória característica defensiva
	case nItem::DigimonSkillChipAST:	//	69	// Memória característica auxiliar
	case nItem::Digitama2:		//	92
	case nItem::Digitama3:		//	93
	case nItem::Cash_Evoluter: // [3/14/2016 hyun] 추가	150
#if COMMON_LIB_FIXED
	//case nItem::Cash_Evoluter2:
#endif
	case nItem::Cash_DigiCore:	//	161
	case nItem::EvoCapsule:		//	193
	case nItem::Spirit:			//	195
		bIsType = true;
	}

	if (bIsRank && bIsType)
		return true;

	return false;
}

bool cDataMng::IsSellItem(cItemInfo* pItemInfo)
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, false);
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pItemInfo->GetType());
	SAFE_POINTER_RETVAL(pFTItem, false);
	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL(pFTItemInfo, false);

	// Se não houver preço
	if (pFTItemInfo->s_dwSale == 0)
	{
		//cPrintMsg::PrintMsg( 11001 );
		return false;
	}

	// Verificação de tipo
	switch (pFTItemInfo->s_nType_L)
	{
	case nItem::Digitama1:		//	91
	case nItem::Digitama2:		//	92
	case nItem::Digitama3:		//	93
		//cPrintMsg::PrintMsg( 11001 );
		return true;
	}

	return false;
}
//==============================================================================================
//
//		Limite de tempo de jogo
//
//==============================================================================================
#ifdef PLAY_PANELTY

u4 cDataMng::PlayTime_Get_GameResetTime()
{
	assert_cs(net::game);

	tm time;
	nBase::GetTime2TM(net::game->GetTimeTS(), time);
	time.tm_year += 1900;
	time.tm_mon += 1;

	int nHour = 23;
	int nMin = 59;
	int nSec = 60;
	nSec -= time.tm_sec;
	nMin -= time.tm_min;
	nHour -= time.tm_hour;
	return nSec + nMin * 60 + nHour * 60 * 60;
}

void cDataMng::PlayTime_GameResetTime_PrintMsg()
{
	int nRTime = g_pDataMng->PlayTime_Get_GameResetTime();
	int nH = nRTime / 3600;
	nRTime = nRTime % 3600;
	int nM = nRTime / 60;

	TCHAR sz[64];
	_stprintf_s(sz, 64, L"%.2d : %.2d", nH, nM);
	cPrintMsg::PrintMsg(30416, sz);
}

#endif

bool cDataMng::SystemBuffCheck(int	InvenIdx, CsItem* pFTItem, CsC_AvObject* pTarget, USHORT	BuffID)
{
	CsBuff::sINFO* pFTBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff(BuffID)->GetInfo();
	if (pFTBuffInfo->s_nBuffType == 3) // Verifique se o fã do sistema
	{
		CsBuff::sINFO* pFTFindExist = NULL;
		//Verifique se há um lustre da mesma classe
		cBuffData* pBuffData = g_pCharMng->GetTamerUser()->GetBuff();
		std::list< cBuffData::sBUFF_DATA >* pList = pBuffData->GetSystemBuff();
		std::list< cBuffData::sBUFF_DATA >::iterator it = pList->begin();
		std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pList->end();
		for (; it != itEnd; ++it)
		{
			if ((*it).s_pBuffTable->GetInfo()->s_nBuffClass == pFTBuffInfo->s_nBuffClass)
			{
				if ((*it).s_pBuffTable->GetInfo()->s_nMinLv <= pFTBuffInfo->s_nMinLv)
				{
					pFTFindExist = (*it).s_pBuffTable->GetInfo();
					break;
				}
			}
		}

		if (pFTFindExist)
		{
			// Quando um buff mais alto está sendo aplicado
			cPrintMsg::PrintMsg(30453, pFTFindExist->s_szName);
			return true;
		}
		else
		{
			cPrintMsg::PrintMsg(20021, pFTItem->GetInfo()->s_szName);
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1(InvenIdx);
			pMBox->SetValue2(pTarget->GetUniqID());
			pMBox->SetValue3(0);
			return true;
		}
	}
	return false;
}
bool cDataMng::AlreadyBuffCheck(int InvenIdx, CsItem* pFTItem, CsC_AvObject* pTarget, USHORT	BuffID)
{
	CsBuff::sINFO* pFTBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff(BuffID)->GetInfo();

	if (pFTBuffInfo->s_nBuffType == 1) // É um efeito de buff?
	{
		//	유Verifique se há um buff duplicado (domador)
		cBuffData* pBuffData = g_pCharMng->GetTamerUser()->GetBuff();
		std::list< cBuffData::sBUFF_DATA >* pList = pBuffData->GetBuff();
		std::list< cBuffData::sBUFF_DATA >::iterator it = pList->begin();
		std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pList->end();
		for (; it != itEnd; ++it)
		{
			cBuffData::sBUFF_DATA pBuff = (*it);
			CsBuff* pCsBuff = pBuff.s_pBuffTable;
			SAFE_POINTER_CON(pCsBuff);
			CsBuff::sINFO* pBuffInfo = pCsBuff->GetInfo();
			SAFE_POINTER_CON(pBuffInfo);

			// Buff duplicado
			if (pBuffInfo->s_dwSkillCode == pFTBuffInfo->s_dwSkillCode ||	// O código de habilidade é o mesmo
				pBuffInfo->s_nBuffClass == pFTBuffInfo->s_nBuffClass)	// Se a classe Buff for a mesma
			{
				//Tratamento de acordo com o nível
				OverlapBuffLevelCheck((int)pBuff.s_nBuffCurLV, pFTItem, InvenIdx, pTarget->GetUniqID());
				return true;
			}
		}

		//	Teste para ver se há um buff duplicado em Digimon
		pBuffData = g_pCharMng->GetDigimonUser(0)->GetBuff();
		pList = pBuffData->GetBuff();
		it = pList->begin();
		itEnd = pList->end();
		for (; it != itEnd; ++it)
		{
			cBuffData::sBUFF_DATA pBuff = (*it);
			CsBuff* pCsBuff = pBuff.s_pBuffTable;
			SAFE_POINTER_CON(pCsBuff);
			CsBuff::sINFO* pBuffInfo = pCsBuff->GetInfo();
			SAFE_POINTER_CON(pBuffInfo);

			// Buff duplicado
			// Buff duplicado
			if (pBuffInfo->s_dwSkillCode == pFTBuffInfo->s_dwSkillCode ||	// O código de habilidade é o mesmo
				pBuffInfo->s_nBuffClass == pFTBuffInfo->s_nBuffClass)	// Se a classe Buff for a mesma
			{
				// Tratamento de acordo com o nível
				OverlapBuffLevelCheck((int)pBuff.s_nBuffCurLV, pFTItem, InvenIdx, pTarget->GetUniqID());
				return true;
			}
		}
	}
	return false;
}

void cDataMng::OverlapBuffLevelCheck(int PrevBuffLevel, CsItem* pFTItem, int Value1, int Value2)
{
	SAFE_POINTER_RET(pFTItem);

	//	PrevBuffLevel;	// Estágio de buff anterior
	int	BuffLevel = pFTItem->GetInfo()->s_nTypeValue;	// Fase de buffer atual

	// Quando o novo buff é o mesmo ou superior
	if (PrevBuffLevel <= BuffLevel)
	{
		cPrintMsg::PrintMsg(30456, pFTItem->GetInfo()->s_szName);
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1(Value1);
		pMBox->SetValue2(Value2);
		pMBox->SetValue3(0);
	}
	// O buff de nível superior é aplicado.
	else
	{
		cPrintMsg::PrintMsg(30452);
	}
}