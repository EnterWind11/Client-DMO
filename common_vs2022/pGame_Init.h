
#pragma once

#include "pGame.h"
#include "pSync.h"
#include "plength.h"


class cPacket;
struct NEW_DUNGEON_INFO
{
	uint nDungeonKey;
	uint nInsMapTeam;
};
/*
// cliente => servidor de mapas
struct CG_GAME_INITDATA_REQ
{
	void push(cPacket &pack);
	void pop(cPacket &pack);
};

// cliente => servidor de mapas
struct GC_GAME_INITDATA_RES
{
	void push(cPacket &pack);
	void pop(cPacket &pack);
};


// servidor de mapa => núcleo
struct SVR_GC_GAME_INITDATA_REQ
{
	void push(cPacket &pack);
	void pop(cPacket &pack);
};
*/

// servidor de mapa <= núcleo
struct SVR_CG_GAME_INITDATA_RES
{
	void push(cPacket& pack);
	void pop(cPacket& pack);

	uint nIDX;
	char szID[Language::pLength::id + 1];
	uint nClass;
	nSync::Pos nDungeonPos;

	int64 dbIDX;
	int nType;
	char szName[Language::pLength::name + 1];

	uint nExp;
	u2 m_nLevel;
	u2 nStatPoint;
};


////예시
//{
//	SVR_CG_GAME_INITDATA_REQ req = { 0, };
//	.. Preencha os dados com a estrutura
//
//	cPacket pack;
//	req.push(pack);	//Pacote
//	send(pack);
//
//
//	SVR_CG_GAME_INITDATA_RES res = { 0, };
//
//	cPacket pack;
//	res.pop(pack);	//Importar dados do pacote para a estrutura
//}