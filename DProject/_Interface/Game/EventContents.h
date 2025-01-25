#pragma once


class AdaptTutorialQuest;

class EventContents
	: public IUIContentSubject< EventContents >
{
public:
	struct sExpEventData
	{
		sExpEventData();
		~sExpEventData();

		void	ResetData();

		u4		m_nExpRate;			// Taxa de experiência
		u4		m_nNextDayExpRate;	// A proporção de valor de experiência recebida quando conectada no dia seguinte
		u4		m_nExpTarget;		// Experiência Tipo 1: Geral 2: Booster
		u4		m_nSpecialExp;		// Se o usuário estiver sujeito a experiência especial, ele contém um valor que não é zero.Não é até 500 ou mais.
		bool	m_HideExpEventUI;	// Ui escondido
	};

	struct sDailyEventData
	{
		sDailyEventData();
		~sDailyEventData();

		void	ResetData();
		
		int		GetTimeLineCount() const;
		int		GetWeekStartIdx() const;
		int		GetCompleteCount() const;

		n4		m_nEventNo;				// Dm_event -Daily Referência -Event Número
		u4		m_nRemainedTimeTS;		// O tempo restante para receber o item do evento
		u4		m_nTotalTime;			// Tempo total para receber o item do evento
		byte	m_nWeek;				// 2017-02-16-NOVA servidor

		u4		m_nRecvServerTimeTS;		
		struct sItems
		{
			int	m_nItemCode;	// Código do item
			int m_nItemCount;	// Número de ítens
		};

		std::list<sItems>	m_ItemLists;	// Lista de itens para receber
	};

	//2017-02-16-nova Informações mensais do evento
	struct sMonthlyEventData
	{
		sMonthlyEventData();
		~sMonthlyEventData();

		void	ResetData();
		bool	SetUpdateData( byte const& nEventNo, byte const& attendCnt, bool const& bNotify );
		bool	IsHaveEvent() const;

		byte	m_nEventNo;		//Número do evento
		byte	m_nAttendCnt;	//Número de dias de presença
		bool	m_bNotify;		//Primeira notificação

		CsEventTable::sMonthlyEvent* monthInfo;
	};

	//////////////////////////////////////////////////////////////////////////
	struct sHotTimeEventData
	{
		sHotTimeEventData():nEventState(0),nCurrentTableIdx(0),nNextTableIdx(0),bIsGetItem(false),nStartTimeLeftTs(0),nEndTimeLeftTs(0),bWaitRecv(false),
			pHotTimeCur(NULL), pHotTimeNext(NULL) {};
		~sHotTimeEventData() {};

		void ResetData();
		bool IsHaveEvent() const;
		bool IsGetItemTime() const;		// Verifique se o item pode ser recebido
		bool IsGetItem() const;			// Teste se você recebeu um item
		void SetGetItem(bool bValue);	// Teste se você recebeu um item
		int  GetCurrentItemTIDX() const;
		int	 GetNextItemTIDX() const;
		
		int				nEventState;
		int				nCurrentTableIdx;
		int				nNextTableIdx;	
		bool			bIsGetItem;
		u4				nStartTimeLeftTs;
		u4				nEndTimeLeftTs;	

		wstring			szStartDate;
		wstring			szEndDate;

		bool			bWaitRecv;

		CsEventTable::sHotTimeEvent const* pHotTimeCur;
		CsEventTable::sHotTimeEvent const* pHotTimeNext;
	};

	//////////////////////////////////////////////////////////////////////////

	// Informações do item de recebimento de participação
	struct sCheckItemInfo
	{
		sCheckItemInfo() : bChecked(false) {}
		~sCheckItemInfo() {}

		void			SetCheckItem( std::map< uint, uint >& mapItems, std::wstring const& wsDate, bool const& bCheck );
		void			SetCheckState( bool const& bCheck );

		std::map< uint, uint > const&	GetCheckItems() const;
		std::wstring	GetCheckDate() const;
		bool			GetCheckState() const;

		//		ItemID, ItemCount
		std::map< uint, uint >	mapCheckItems;
		std::wstring	wsCheckDate;
		bool			bChecked;
	};

	// Informações do grupo de participação
	struct sCheckGroupInfo
	{
		sCheckGroupInfo() : nCheckGroupID(0), nCheckIndex(0), nCheckNextLeftTS(0) {}
		~sCheckGroupInfo() {}

#if COMMON_LIB_FIXED
		//void			InitCheckInfo( nsDailyCheckEvent::sDailyCheckData& cDailyInfo );
#endif

		void			SetCheckNextTimer( uint const& nLeftTS );
		bool			SetCheckNextIndex();
		void			SetCheckInfo( uint const& nGroupID, uint const& nTodayIndex, uint const& nNextLeftTS );
		void			ResetCheckInfo();
		sCheckItemInfo const*	GetCheckItemInfo( uint const& nIndex ) const;
		sCheckItemInfo const*	GetCheckableItemInfo() const;
		uint			GetCheckItemListCount() const;
		uint			GetCheckIndex() const;
		bool			IsCurrentGroupID( uint const& nGroupID ) const;
		bool			IsCheckable() const;
		bool			IsNextCheckTime();

		uint			nCheckGroupID;		// ID do Grupo de Atendimento (1~)
		uint			nCheckIndex;		// Índice de Data de Recebimento (1~)
		uint			nCheckNextLeftTS;	// Tempo restante até o próximo dia de atendimento
		CsTimeSeq		cLeftTimer;			// Próximo cronômetro Data de Atendimento
		std::map< uint, sCheckItemInfo >	mapCheckInfos;		// Índice do item de recebimento (1~), informações do item de recebimento
	};

	// Informações do evento de participação
	struct sDailyCheckEventData
	{
		sDailyCheckEventData() : bWaitRecv(false) {}
		~sDailyCheckEventData() {}

#if 0
		void			InitDailyCheckList( nsDailyCheckEvent::sDailyCheckData& cDailyInfo );
#endif

		void			SetDailyCheckList( uint const& nGroupID, uint const& nTodayIndex, uint const& nNextLeftTS );
		bool			SetNextDailyCheck( uint const& nGroupID );
		void			ResetData();
		void			ResetCheckListData( uint const& nGroupID );

		uint			GetGroupID( int const& nIndex ) const;
		uint			GetGroupCount() const;
		std::wstring	GetGroupName( int const& nIndex ) const;
		sCheckGroupInfo const*	GetCheckGroupInfo( int const& nIndex ) const;
		sCheckItemInfo const*	GetCheckableItemInfo( int const& nIndex ) const;
		bool			IsCheckable( int const& nIndex ) const;
		bool			IsHaveDailyCheckEvent() const;
		bool			IsWaitRecv() const;

		bool			bWaitRecv;
		std::list< sCheckGroupInfo >		lDailyCheck;
	};

public:
	enum eEvent
	{
		eUPDATE_EXP_EVENT_DATA,
		eDailyEvent_Update,
		eMonthlyEvent_Update,	
		eHotTimeEvent_Update,
		eHotTimeEvent_GetItem_PacketRecv,
		eDailyCheckEvent_UpdateInfo,
		eDailyCheckEvent_UpdateItem,
		eDailyCheckEvent_NotTime,
	};
public:
	static int const IsContentsIdentity(void);

public:
	EventContents(void);
	virtual ~EventContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

	static void CALLBACK ExpEventCheckTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime );

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	//////////////////////////////////////////////////////////////////////////
	// Exp Event Function
	//////////////////////////////////////////////////////////////////////////
	void		Recv_ExpEventData(void* pData);
	void		Hide_ExpEventData(void* pData);
	void		Show_ExpEventData(void* pData);
	void		Get_NextExpRate(void* pData);
	void		Recv_ExpEventDisable( void* pData );

	void		RewardUI_Toggle(void* pData);

	void		SetExpEventTimer();
	void		RemoveExpEventTimer();

public:
	unsigned int GetCurrentExpRate() const;
	unsigned int GetExpRateType() const;
	unsigned int GetNextExpRate() const;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Daily Event Function
	//////////////////////////////////////////////////////////////////////////
public:
	void			CloseDailyEventUI();	
	void			CloseRewardEventUI();				//2017-03-08-nova
	bool			AcceptButtonEvent(int sel, int selSub = 0);			//2017-03-08-nova

	unsigned int	GetDailyEventTotalTs() const;
	unsigned int	GetDailyEventCurrentTs() const;
	unsigned int	GetDailyEventRemainedTs() const;
	std::list<sDailyEventData::sItems> const& GetDailyEventItems() const;

	//////////////////////////////////////////////////////////////////////////
	byte			GetMonthlyEventNo() const;
	byte			GetMonthlyEventAttendCnt() const;
	bool			GetMonthlyEventNotify() const;
	//////////////////////////////////////////////////////////////////////////

	int				GetDailyEventNo() const;			//2017-02-16-nova	Número diário do evento
	byte			GetDailyEventWeek() const;			//2017-02-16-nova	Informações diárias do dia
	//unsigned int	GetDailyEventCurrentTime() const;	//2017-02-16-nova	Tempo de progresso do evento diário
	int				GetDailyEventPlayTimeMinute() const;
	int				GetDailyEventTimeLineCount() const;
	int				GetDailyEventCompleteCount() const;
	int				GetDailyEventWeekStartIdx() const;
	
	//////////////////////////////////////////////////////////////////////////

	sMonthlyEventData const& GetMonthlyEventData() const;

	int				GetMonthItemID(int no);
	USHORT			GetMonthItemCnt(int no);
	void			SetMonthMessage(cText* text);

	// Hot Time Event Uma função que envia pacotes para o servidor para receber um item
	bool			Send_HotTimeEvent_ItemGet();
	bool			IsHaveHotTimeEvent() const;
	sHotTimeEventData const& GetHotTimeEvent_Info() const;

	std::wstring	GetItemName( DWORD const& dwItemCode );
	NiColor			GetItemColor( DWORD const& dwItemCode );

	//////////////////////////////////////////////////////////////////////////
	// Daily Check Event Function
private:
	void			Send_DailyCheckEventInfo_Request();
	void			Recv_DailyCheckEventInfo_Request( void* pData );
	void			Send_DailyCheckEvent_ItemGet( int const& nGroupIndex );
	void			Recv_DailyCheckEvent_ItemGet( void* pData );
	void			Update_DailyCheckEvent_Timer( void* pData );

	bool			CheckInventory(EventContents::sCheckItemInfo const* pCheckItem);

#if COMMON_LIB_FIXED
	//void			InsertItemToInventory(std::list< nsDailyCheckEvent::sDailyCheckedItem >& lCheckedItems);
#endif

public:
	bool			IsHaveDailyCheckEvent() const;
	uint			GetDailyCheckEventCount() const;
	void			GetDailyCheckEventInfo( int const& nGroupIndex, std::wstring& wsTitle, std::wstring& wsDesc,
											std::wstring& wsStart, std::wstring& wsEnd );
	sCheckGroupInfo const*	GetDailyCheckGroupInfo( int const& nGroupIndex ) const;

	//////////////////////////////////////////////////////////////////////////

	std::wstring	GetSubTitleName( int const& nMainIndex, int const& nSubIndex ) const;

private:
	void			Recv_DailyEventData(void* pData);
	void			Recv_MonthlyEventData(void* pData);	

	void			Recv_NTF_HotTimeEventInfo(void* pData);
	void			Recv_HotTimeEventItemGet(void* pData);

private:
	sExpEventData		m_ExpEventData;
	sDailyEventData		m_DailyEventData;
	sMonthlyEventData	m_MonthlyEventData;	
	sHotTimeEventData	m_HotTimeEventData;
	sDailyCheckEventData	m_DailyCheckEventData;

	TOwnershipGuard< AdaptTutorialQuest >		m_pAdaptTutorial;
};
