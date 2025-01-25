
#pragma once


namespace nNpc
{
	enum eAttribute
	{
		Map,
		X,
		Y,
		Type,

		ItemCount,	// shop npc인 경우 

		MoveType,	// 고정, 선형이동, 영역이동등 처리

		MaxAttribute
	};

	enum eType
	{	// bit-field 로 변경 필요
		TypeNormal			= 0,	// 일반 - 퀘스트를 주거나 받기
		TypeShop			= 1,	// 상인
		TypeShopDT			= 2,	// 디지타마 상인
		TypePortal			= 3,	// 이동
		TypeIncubator		= 4,	// 용병 부화 담당
		TypeSocket			= 5,	// 아이템 분석 및 소켓 처리
		TypeWarehouse		= 6,	// 창고 담당
		TypeCareSlot		= 7,	// 보관소 담당
		TypeGuild			= 8,	// 상인, 길드 생성, 길드 랩업
		TypeDigiCore		= 9,	// 상인, 디지코어 관련 아이템 판매	==> 디지코어 갯수만 확인, 이외의 상인은 구입표준가격으로 처리
		TypeCapsuleMachine	= 10,	// 캡슐 머신
		TypeSkill			= 11,	// 스킬 상급자(스킬 가르키는 넘)
		TypeShopEvent		= 12,	// 상인, 이벤트 스토어
		// 13번은 고동혁인듯
		TypeShopBarter		= 14,	// 물물 교환 상점
		TypeRareMachine		= 15,	// 레어머신
		TypeDonate			= 16,	//16번은 기부하는애(마스터즈매칭시스템)
		TypeMysteryMachine	= 17,	// 미스테리 머신

		TypeSpiritChange	= 18,	// 스피릿 교환 NPC

		TypePinochiCard		= 19,	// 피노키몬 카드게임
		TypeMake			= 20,	// 제작 상점
		TypeExtraEvo		= 24,	// 특수진화 NPC

		TypeChangeTName	= 29,
		TypeTimeLimited	= 30,		// 시간 제한으로 아이템이 사라지는 경우
	};

	enum eMoveType
	{
		None	= 0,
		Move	= 1,
	};
};




