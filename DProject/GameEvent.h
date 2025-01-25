#pragma once 

class CGameEventMng : public CEventCall_Base
{
public:
	CGameEventMng(void);
	~CGameEventMng(void);

public:
	virtual void	EventStart();
	virtual void	EventEnd();

	virtual void	PlaySound( char* szSound ) { g_pSoundMgr->PlaySound( szSound ); }
	virtual void	PlayMusic( char* szMusic ) { g_pSoundMgr->PlayMusic( szMusic ); }

	virtual void	EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType );
	virtual void    EventSave( int nSceneNum );

public:
	void		Delete();
	void		Init();
	
	void		Update( float fDeltaTime );
	void		Render();
	void		RenderText();

	void		ResetDevice();
	void		EventSkip();

protected:
	// Variáveis
	NiPoint3	m_vDelta;
	float		m_fCameraRot;
	float		m_fCameraPitch;
	bool		m_bSkip;	
	float		m_fAccumTime;
	bool		m_bIsSound;

	//////////////////////////////////////////////////////////////////////////
	//
	// A espada usada quando a cena do evento
	//
	//////////////////////////////////////////////////////////////////////////
protected:
	// Uso variável quando é um evento
	cSprite*		m_pSprite1;
	cSprite*		m_pSprite2;		
	cButton*		m_pSkip;

protected:
	enum eALPA_TYPE { ALPA_STOP = 0, ALPA_INCREASE, ALPA_DECREASE };
	void			_SetAlpha( eALPA_TYPE eAlpha, float fAlpha );
	void			_SetText( uint nTargetUID, TCHAR* szText );

protected:
	CsTimeSeq		m_LifeTimeSeq;
	cStringList		m_StringList; // Embaixador do evento da câmera
	eALPA_TYPE		m_eAlpa;
	float			m_fAlpaValue;


	////////////////////////////////////////////////////////////////////////////////////
	//
	//	Salvar cenas de eventos que usam apenas áreas
	//
	////////////////////////////////////////////////////////////////////////////////////
public:
	struct sAreaEventScene : public NiMemObject
	{
	public:
		void Delete() { m_vpAreaScene.Destroy(); }
		void AddAreaScene( CEventSceneData* nSceneData ) { m_vpAreaScene.PushBack( nSceneData ); }
		void DeleteAreaScene( CEventSceneData* nSceneData ) { m_vpAreaScene.DeleteElement_FromData( nSceneData ); }

		CsVectorPB< CEventSceneData* > m_vpAreaScene;
	};

public:
	void	LoadAreaScene();
	void	ReLoadScene( DWORD dwMapID );
	sAreaEventScene* GetAreaScene( UINT MapID );

protected:
	std::map< UINT, sAreaEventScene* > m_mapAreaSceneData; /**< Todos os eventos que usam apenas a área */
	sAreaEventScene* m_CurMapAreaData;

};

extern CGameEventMng* g_pGameEventMng;