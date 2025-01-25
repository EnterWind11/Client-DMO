

#include "pTamer.h"



bool nTamer::IsValidType(uint nType)
{
	switch(nType)
	{
	case nTamer::Masaru		: return true;	// 최건우	// 마사루
	case nTamer::Tohma		: return true;	// 토오마
	case nTamer::Yoshino	: return true;	// 유진		// 요시노
	case nTamer::Ikuto		: return true;	// 한지호	// 이쿠토
	case nTamer::Tai		: return true;	// 신태일
	case nTamer::Mina		: return true;	// 이미나
	case nTamer::Matt		: return true;	// 매튜		// 매트
	case nTamer::Takeru		: return true;	// 리키		// 타케루
	case nTamer::Hikari		: return true;	// 나리 	// Hikari
	case nTamer::Sora		: return true;	// 소라		// Sora
	case nTamer::Izumi		: return true;
	case nTamer::Joe		: return true;
	}
	return false;
}








