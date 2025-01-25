#pragma once
#pragma once
#pragma once

#include "common_vs2022/pSkill.h"
#include "common_vs2022/Protocol/ItemScan_Define.h"



class GS2C_RECV_DIGIMON_SKILL_LEVEL_UP
{
public:
	GS2C_RECV_DIGIMON_SKILL_LEVEL_UP() : nProtocol(pSkill::LevelUp), nResult(0), nDigimonSourceIndex(0), nEvolutionSlotIndex(0), nSkillIndex(0), nSkillLevel(0)
	{
	}
	WORD GetProtocol() { return nProtocol; }
public:
	WORD nProtocol;
	u4 nResult;
	u4 nDigimonSourceIndex;
	u1 nEvolutionSlotIndex;
	u1 nSkillIndex;
	u4 nSkillPoint;
	u1 nSkillLevel;

};