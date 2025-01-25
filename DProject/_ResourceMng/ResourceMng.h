
#pragma once
#include "TextureMng.h"

class ResourceMng : public CSingleton<ResourceMng>
{
public:
	ResourceMng(void);
	~ResourceMng(void);

	void					Init();
	void					End();
	void					CleanUpResource();	// Limpar recursos não utilizados
	size_t					GetTextureResourceSize() const;

	GUI::TexturePtr			GetTexture( const char *strFilename );
	GUI::TexturePtr			GetTexture( const wchar_t *strFilename );	
	void					RemoveTexture( GUI::TexturePtr& pRemove );	// excluir recurso
	void					RemoveAllResource();	// Mod limpar o recurso.

private:
	TextureMng				m_TextureMng;
};

#define RESOURCEMGR_ST		ResourceMng::GetSingleton()
#define RESOURCEMGR_STPTR	ResourceMng::GetSingletonPtr()