#pragma once

class CFileUIChatting : public COptionUIChat
{
public:
	CFileUIChatting(void);
	virtual ~CFileUIChatting(void);
	
	virtual	void Write(TiXmlElement* root);		// Escreva
	virtual	bool Read(TiXmlElement* root);		// ler
	virtual	void InitDefault();					// Configuração inicial.
};