
#pragma once 

class CNormalChar : public CsC_AvObject
{
	THREAD_MEMPOOL_H( CNormalChar )

protected:
	virtual void		_LoadedFile( int nCallRTTI );
	

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//====================================================================================
	//	스탯	- 항상 동적으로 구하겠다 - 공식을 변경하고 싶으면 이부분을 바꾸라
	//====================================================================================
public:
	virtual float		GetAttackRange();
	virtual float		GetAttackCharSize();
	virtual float		GetAttackSpeed();
	virtual int			GetAttackRangeType();

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual void*		PlaySound( char* cSoundPath );


public:
	virtual bool		IsUser(){ return true; }
};