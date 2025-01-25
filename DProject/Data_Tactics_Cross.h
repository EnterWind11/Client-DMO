
#pragma once 
#ifdef CROSSWARS_SYSTEM
#define IF_CROSSLOADERSLOT_COUNT		16

class cData_CrossTactics : public NiMemObject
{
public:
	struct sCross:public cData_PostLoad::sDATA
	{
		u1		s_nEnable;

		sCross()  { s_nEnable = 0;}

		virtual void		Delete(){  s_nEnable = 0; cData_PostLoad::sDATA::Delete(); }
		virtual void		ToObject( CsC_AvObject* pAvObject, bool bInit );
	};
	
public:
	cData_CrossTactics();

public:
	void	Delete();
	void	Reset();

	void	Init();
	void	InitNetOff();

	// mercenário
protected:

	sCross						m_TempTactics;

	sCross*						m_pTacticsArray;



	int				m_nCrossCnt;
	int				m_nCrossIdx[ IF_CROSSLOADERSLOT_COUNT ];	// Índices para o corpo do carvão.Depende do valor máximo da classificação Digimon definida na tabela.
	int				m_nCrossID;
	bool			m_bToggleEnable;		// Variável de alternância combinada



public:	
	
	void						DeleteTempTactics()	{ m_TempTactics.Delete(); }
	sCross*						GetTempTactics(){ return &m_TempTactics; }
	sCross*						GetTacticsByBaseDigimonID(DWORD dwBaseDigimonID);

	void						AddTactics( sCross* pSrcTactics, int nTacticsIndex );
	void						DeleteTactics( int nTacticsIndex );

	void						ChangeTactics( CsC_AvObject* pSrc, int nDestTacticsIndex );
	void						ChangeTactics2( CsC_AvObject* pSrc, int nDestTacticsIndex );
	sCross*						GetTactics( int nTacticsIndex ){ return &m_pTacticsArray[ nTacticsIndex ]; }

	bool						IsTactics( int nTacticsIndex ){ assert_cs( nTacticsIndex < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics ); return ( m_pTacticsArray[ nTacticsIndex ].s_Type.m_nType != 0 ); }


	void		SetCrossCnt( int n){ m_nCrossCnt = n;}
	void			SetCrossID( int n ){ m_nCrossID = n;}
	int			GetCrossCnt() {return m_nCrossCnt;}
	int*			GetCrossIdx() { return m_nCrossIdx;}
	int			GetCrossID(){return m_nCrossID;}
	void		ResetCrossIdx()
	{ 	
		m_bToggleEnable = false;
		m_nCrossID = m_nCrossCnt = 0;
		memset(m_nCrossIdx, -1, sizeof(int)*IF_CROSSLOADERSLOT_COUNT); 
	}

	void		SetCrossEnable(bool b){ m_bToggleEnable = b;}
	bool		GetCrossEnable(){return m_bToggleEnable;}


	bool CheckCodeCrown();

};
#endif