#pragma once

// #include "bandicap.h"
// class MovieCapture
// {
// 
// protected:
// 	LPDIRECT3DDEVICE9		m_lpDevice9;
// 	CBandiCaptureLibrary	m_BandiLibrary;
// 
// 	bool	m_bInit;		// Se a gravação for possível, é verdade quando é criada.
// 	bool	m_bRecording;
// 
// 
// 
// public:
// 	HRESULT Initialize(LPDIRECT3DDEVICE9 pD3d);
// 	void RecordRender(LPDIRECT3DDEVICE9 pD3d);
// 
// 	void Release();
// 
// 
// public:
// 	void StartRecord();		// Comece a gravar vídeo.
// 	void RecordLoop();		// Determine a gravação dentro.
// 
// 
// public:
// 	static MovieCapture& GetInstance()
// 	{
// 		static MovieCapture _kInstance;
// 		return _kInstance;
// 	}
// 
// private:
// 	MovieCapture();
// public:
// 	~MovieCapture();
// };
// 
// 
