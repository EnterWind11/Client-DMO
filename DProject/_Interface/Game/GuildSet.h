#pragma once

class cGuildSet : public cBaseWindow
{
public:
	cGuildSet(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_GUILDSET; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();		

	virtual bool			OnEscapeKey();

private:
	void					CloseWindow();

protected:
	cButton*				m_pCancelButton;
	cButton*				m_pChangeButton;
	cButton*				m_pExplainButton;

	cComboBox*				m_pCombo;

	int						m_nValue;

};