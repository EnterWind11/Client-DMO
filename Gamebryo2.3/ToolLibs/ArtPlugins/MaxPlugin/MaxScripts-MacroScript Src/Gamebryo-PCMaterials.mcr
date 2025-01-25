-- EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
--
-- This software is supplied under the terms of a license agreement or
-- nondisclosure agreement with Emergent Game Technologies and may not
-- be copied or disclosed except in accordance with the terms of that
-- agreement.
--
--      Copyright (c) 1996-2007 Emergent Game Technologies.
--      All Rights Reserved.
--
-- Emergent Game Technologies, Chapel Hill, North Carolina 27517
-- http://www.emergent.net
-----------------------------------------------------------------------------

macroscript PCMaterials category:"Gamebryo Tools" 
	buttonText:"PC Materials" toolTip:"PC Materials" Icon:#("Gamebryo", 15)
(
	for matl in sceneMaterials do
	(
		if ( isKindOf matl Gamebryo_XPlatform_Material) then
		(
			matl.viewportMtlIndex = 0
		)
		else if (isKindOf matl MultiMaterial) then
		(
			for subMtl in matl.materialList do
			(
				if (isKindOf subMtl Gamebryo_XPlatform_Material) then
				(
					subMtl.viewportMtlIndex = 0
				)
			)
		)
	)
)