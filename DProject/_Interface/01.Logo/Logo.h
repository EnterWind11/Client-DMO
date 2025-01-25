
#pragma once

#include "../00.Contents/LogoContents.h"

class CLogo : public NiMemObject, public LogoContents::ObserverType
{
	enum	FAID_STATE
	{
		eFAID_NONE=-1,	// fazer nada
		eFAID_IN,		// aparecimento gradual
		eFAID_WAIT,		// espere pelo fade
		eFAID_OUT,		// desaparecer
	};
	struct sRenderImg
	{
		sRenderImg(){};
		~sRenderImg()
		{};

		void Render(int nScreenWidth, int nScreenHeight, float fCurAlpha);

		cStringList				stringList;
		std::vector<cSprite*>	imgs;
	};
public:
	CLogo();
	virtual ~CLogo();

	virtual	bool		Construct(void);

public:
	void				Init();
	void				Update();
	void				Render();
	void				ResetDevice();

private:
	void				AddLogo( char* pFile, CsPoint csSize );
	//void				AddCompanyLogo();
	//void				AddPublicLogo();
	//void				AddSGPublicLogo();
	void				AddGuideLogo();
	void				Destroy();

	void				FaidUpdate();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	FAID_STATE			m_FaidState;
	float				m_Duration;		// duração
	float				m_fCurAlpha;
	int					m_nImgIndex;
	int					m_nMaxImage;
	std::vector<sRenderImg*>	m_imgVector;
};

