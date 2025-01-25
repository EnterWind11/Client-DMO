

#pragma once

#define MAX_ANAL_LEN		512

// 스트링 분석기
class cStringAnalysis : public NiMemObject
{
public:
	static void		GlobalInit();
	static void		GlobalShotDown();
	static bool		CutLimitWigth( std::list<dm_string_t>& outPut, cText::sTEXTINFO const& pTextInfo, int const& nWidthLimit );
	static bool		CutLimitWigth( std::list<dm_string_t>& outPut, dm_string_t const& surText, 
								   CFont const* pFont, CFont::eFACE_SIZE fontSize, int const& nWidthLimit );

public:
	void			Init();
	void			Delete();


	//============================================================================================
	//		파싱
	//============================================================================================
protected:
	enum ePARCING_TYPE{
		PT_NONE,
		PT_TEXT_COLOR,
		PT_TEXT_BOLD,
	};
	struct sPARCING : public NiMemObject
	{
		CSGBMEMPOOL_H( cStringAnalysis::sPARCING );

		ePARCING_TYPE	s_eType;
		DWORD			s_Value;
	};

	NiColor					m_OrgColor;
	int						m_nOrgBoldLevel;
	cString*				m_pString;
	cStringList*			m_pStringList;
	bool					m_bNewLine;

	std::list< sPARCING* >	m_listParcing;

	cText::sTEXTINFO*		m_pTextInfo;

	TCHAR					m_szToken[ MAX_ANAL_LEN ];
	int						m_nTokenIndex;
	int						m_nTotalWidth;
	sSTR_INFO*				m_pStrInfo;
	int						m_nLineLimit;

protected:
	void			_Parcing( TCHAR const* str, int nStrLen, int& nIndex );
	int				_ParcingTypeStart( TCHAR const* str, ePARCING_TYPE& pt );
	bool			_ParcingValueEnableCheck( int nStrLen, int nIndex, ePARCING_TYPE& pt );
	int				_ParcingValue( TCHAR const* str, ePARCING_TYPE& pt );
	int				_ParcingTypeEnd( TCHAR const* str );
	//void			_ParcingWord( TCHAR const* str, int nStrLen, int& nIndex, int nWidth, cButton::eIMAGE_TYPE Type );

	void		_RemoveParcing( ePARCING_TYPE pt );

	//============================================================================================
	//		문자열 자르기
	//============================================================================================
protected:
	bool	_MakeToken_Parcing_ForCardMaster( CsPoint pos );
	bool	_MakeToken_Parcing( bool bNewString );
	bool	_MakeToken( bool bNewString );	
	bool	_MakeToken_OnlyNewLine( bool bNewString );
	void	_AddText( CsPoint ptDeltaPos );

public:	
	void	Cut_Parcing_ForCardMaster( cString* pString, int width, CsPoint	Pos, TCHAR const* str, cText::sTEXTINFO* pTextInfo );	// 색깔 구분 해서 문자열 자르기

	void	Cut_LinkParcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, cButton::eIMAGE_TYPE Type );	// 색깔 구분, 단어링크, 문자열 자르기
	void	Cut_Parcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo );	// 색깔 구분 해서 문자열 자르기
	void	Cut( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1 );			// 색깔 구분 해서 문자열 자르기
	void	Cut_NoMultyLine( cStringList* pStringList, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1 );			// 색깔 구분 해서 문자열 자르기
	void	AddCut( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo );
	void	Cut_MailParcing( cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo );			// 색깔 구분 해서 문자열 자르기
	void	Cut_LimitWigth( cStringList* pStringList, int width, TCHAR * str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1 );			// 색깔 구분 해서 문자열 자르기

	TCHAR*	StringTransfer( TCHAR* szOut, int nOutSize, TCHAR const* szIn );										// 문자열 대체

	void	MaxLine_Cut(TCHAR const* word, cStringList* pStringList, int width, TCHAR const* str, cText::sTEXTINFO* pTextInfo, int nLineLimit = -1);	// 문자열을 길이를 넘어서면 word문자열 입력후 자름.

	TCHAR*		Quest_Parcing( TCHAR * szDest, const int nDestSize, TCHAR const* szSrc, CsQuestCompRequireGroup* pApplyGroup );

	void		ItemComment_Parcing( cStringList* pList, cItemInfo* pEquipItem );
	void		ItemComment_Parcing( cStringList* pList, int nEquipType );

	void		Equip_EItemComment_Parcing( cStringList* pList, cString* pBeginString, cItemInfo* pEquip, int nSocketIndex, bool bCashEndItem = false );	
	
// [4/14/2016 hyun] 추가
	void		SetParcingColor(const NiColor color);
	NiColor		GetParcingColor() const;
	void		CompleteParcingColor();

	NiColor		m_ParcingColor;
	bool		m_bChangeColor;
// @@ 여기까지

	std::wstring				m_strTempText;
	std::vector<std::wstring> 	m_vecStringTotalText;


	//TCHAR*		FindToken( TCHAR* szMsg );
	//static int StartSpaceDiv( std::list< CString* >* pList, CFont* pFont, NiColor color, int x, int y, int nFontSize, int width, TCHAR* str );			// 색깔 구분 해서 문자열 자르기,공백 단위로 자름
	
	//static int StartNoColorSpaceDiv( std::list< CString* >* pList, CFont* pFont, NiColor color, int x, int y, int nFontSize, int width, TCHAR* str );	// 색깔 구분 안하고 문자열 자르기,공백 단위로 자름
	//static NiColor GetColor(TCHAR *);

	//static int GetWordLen(CFont* pFont, int nFontSize, TCHAR* str);			// 다음 단어 길이 가져오기
	//static void CutWord(CFont* pFont, int nFontSize, TCHAR* str,TCHAR* cut);	// 다음 단어 길이 가져오기
	//static void StrFilter(TCHAR* str);
};

extern cStringAnalysis* g_pStringAnalysis;
