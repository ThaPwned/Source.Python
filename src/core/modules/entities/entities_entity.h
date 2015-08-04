/**
* =============================================================================
* Source Python
* Copyright (C) 2012-2015 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/

#ifndef _ENTITIES_ENTITY_H
#define _ENTITIES_ENTITY_H

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "utilities/baseentity.h"
#include "utilities/sp_util.h"
#include "utilities/conversions.h"
#include "utilities/wrap_macros.h"
#include "toolframework/itoolentity.h"


//-----------------------------------------------------------------------------
// Definitions.
//-----------------------------------------------------------------------------
#define MAX_KEY_VALUE_LENGTH 1024


//-----------------------------------------------------------------------------
// External variables.
//-----------------------------------------------------------------------------
extern IServerTools* servertools;


//-----------------------------------------------------------------------------
// CBaseEntity extension class.
//-----------------------------------------------------------------------------
class CBaseEntityWrapper: public IServerEntity
{
private:
	// Make sure that nobody can call the constructor/destructor
	CBaseEntityWrapper() {}
	~CBaseEntityWrapper() {}

public:
	// We need to keep the order of these methods up-to-date and maybe we need
	// to add new methods for other games.
	// TODO: Do we want to do this? Or do we want to dynamically call the methods from Python?
	virtual ServerClass* GetServerClass() = 0;
	virtual int YouForgotToImplementOrDeclareServerClass() = 0;
	virtual datamap_t* GetDataDescMap() = 0;

public:
	static boost::shared_ptr<CBaseEntityWrapper> __init__(unsigned int uiEntityIndex)
	{
		return CBaseEntityWrapper::wrap(BaseEntityFromIndex(uiEntityIndex, true));
	}

	static boost::shared_ptr<CBaseEntityWrapper> wrap(CBaseEntity* pEntity)
	{
		return boost::shared_ptr<CBaseEntityWrapper>(
			(CBaseEntityWrapper *) pEntity,
			&NeverDeleteDeleter<CBaseEntityWrapper *>
		);
	}

	static str GetKeyValueString(CBaseEntity* pBaseEntity, const char* szName)
	{
		char szResult[MAX_KEY_VALUE_LENGTH];
		if (!servertools->GetKeyValue(pBaseEntity, szName, szResult, MAX_KEY_VALUE_LENGTH))
			BOOST_RAISE_EXCEPTION(PyExc_NameError, "\"%s\" is not a valid KeyValue for entity class \"%s\".",
				szName, ((CBaseEntityWrapper *)pBaseEntity)->GetDataDescMap()->dataClassName);

		// Fix for field name "model". I think a string_t object is copied to szResult.
		if (strcmp(szName, "model") == 0)
			return *(char **) szResult;

		return str(szResult);
	}

	static int GetKeyValueInt(CBaseEntity* pBaseEntity, const char* szName)
	{
		return extract<int>(eval("lambda x: int(x)")(GetKeyValueString(pBaseEntity, szName)));
	}

	static float GetKeyValueFloat(CBaseEntity* pBaseEntity, const char* szName)
	{
		return extract<float>(eval("lambda x: float(x)")(GetKeyValueString(pBaseEntity, szName)));
	}

	static Vector GetKeyValueVector(CBaseEntity* pBaseEntity, const char* szName)
	{
		char szResult[MAX_KEY_VALUE_LENGTH];
		if (!servertools->GetKeyValue(pBaseEntity, szName, szResult, MAX_KEY_VALUE_LENGTH))
			BOOST_RAISE_EXCEPTION(PyExc_NameError, "\"%s\" is not a valid KeyValue for entity class \"%s\".",
				szName, ((CBaseEntityWrapper *)pBaseEntity)->GetDataDescMap()->dataClassName);

		float fResult[3];
		if (!sputils::UTIL_StringToFloatArray(fResult, 3, szResult))
			BOOST_RAISE_EXCEPTION(PyExc_ValueError, "KeyValue does not seem to be a vector.");

		return Vector(fResult[0], fResult[1], fResult[2]);
	}

	static bool GetKeyValueBool(CBaseEntity* pBaseEntity, const char* szName)
	{
		return strcmp(extract<const char *>(GetKeyValueString(pBaseEntity, szName)), "1") == 0;
	}

	static Color GetKeyValueColor(CBaseEntity* pBaseEntity, const char* szName)
	{
		object color = eval("lambda x: tuple(map(int, x.split(' ')))")(GetKeyValueString(pBaseEntity, szName));
		return Color(extract<int>(color[0]), extract<int>(color[1]), extract<int>(color[2]), extract<int>(color[3]));
	}

	static void SetKeyValueColor(CBaseEntity* pBaseEntity, const char* szName, Color color)
	{
		char string[16];
		Q_snprintf(string, sizeof(string), "%i %i %i %i", color.r(), color.g(), color.b(), color.a());
		SetKeyValue(pBaseEntity, szName, string);
	}

	template<class T>
	static void SetKeyValue(CBaseEntity* pBaseEntity, const char* szName, T value)
	{
		if (!servertools->SetKeyValue(pBaseEntity, szName, value))
			BOOST_RAISE_EXCEPTION(PyExc_NameError, "\"%s\" is not a valid KeyValue for entity class \"%s\".",
				szName, ((CBaseEntityWrapper *)pBaseEntity)->GetDataDescMap()->dataClassName);
	}

	static edict_t* GetEdict(CBaseEntity* pBaseEntity)
	{
		return EdictFromBaseEntity(pBaseEntity);
	}
	
	static int GetIndex(CBaseEntity* pBaseEntity)
	{
		return IndexFromBaseEntity(pBaseEntity);
	}
	
	static CPointer* GetPointer(CBaseEntity* pBaseEntity)
	{
		return PointerFromBaseEntity(pBaseEntity);
	}
	
	static CBaseHandle GetBaseHandle(CBaseEntity* pBaseEntity)
	{
		return BaseHandleFromBaseEntity(pBaseEntity);
	}
	
	static int GetIntHandle(CBaseEntity* pBaseEntity)
	{
		return IntHandleFromBaseEntity(pBaseEntity);
	}
};


#endif // _ENTITIES_ENTITY_H
