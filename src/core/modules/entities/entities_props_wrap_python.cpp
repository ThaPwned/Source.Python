/**
* =============================================================================
* Source Python
* Copyright (C) 2015 Source Python Development Team.  All rights reserved.
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

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "export_main.h"
#include "utilities/wrap_macros.h"
#include "dt_common.h"
#include "dt_send.h"
#include "server_class.h"
#include "modules/memory/memory_tools.h"
#include "utilities/sp_util.h"
#include "entities_props_wrap.h"

#include ENGINE_INCLUDE_PATH(entities_props_wrap_python.h)


//-----------------------------------------------------------------------------
// Forward declarations.
//-----------------------------------------------------------------------------
void export_send_table();
void export_send_prop();
void export_send_prop_types();
void export_send_prop_flags(scope);
void export_send_prop_variant();
void export_server_class();


//-----------------------------------------------------------------------------
// Declare the _entities._props module.
//-----------------------------------------------------------------------------
DECLARE_SP_SUBMODULE(_entities, _props)
{
	export_send_table();
	export_send_prop();
	export_send_prop_types();
	export_send_prop_flags(_props);
	export_send_prop_variant();
	export_server_class();
}


//-----------------------------------------------------------------------------
// Expose SendTable.
//-----------------------------------------------------------------------------
void export_send_table()
{
	class_<SendTable, SendTable *> SendTable_("SendTable", no_init);
	
	// Properties...
	SendTable_.def_readonly("props", &SendTable::m_pProps);
	SendTable_.def_readonly("length", &SendTable::m_nProps);
	SendTable_.add_property("name", &SendTable::GetName);
	
	// Methods...
	SendTable_.def("get_name", &SendTable::GetName);
	SendTable_.def("get_length", &SendTable::GetNumProps);
	SendTable_.def("is_initialized", &SendTable::IsInitialized);
	SendTable_.def("set_initialized", &SendTable::SetInitialized);
	
	SendTable_.def("get_prop", &SendTable::GetProp,
		reference_existing_object_policy()
	);
	
	SendTable_.def("set_write_flag", &SendTable::SetWriteFlag);
	SendTable_.def("get_write_flag", &SendTable::GetWriteFlag);
	SendTable_.def("has_props_encoded_against_tick_count", &SendTable::HasPropsEncodedAgainstTickCount);
	SendTable_.def("set_has_props_encoded_against_tick_count", &SendTable::SetHasPropsEncodedAgainstTickcount);
	
	// Special methods...
	SendTable_.def("__getitem__",
		&SendTableSharedExt::__getitem__,
		reference_existing_object_policy()
	);
	
	SendTable_.def("__len__", &SendTable::GetNumProps);
	
	// Add memory tools...
	SendTable_ ADD_MEM_TOOLS(SendTable, "SendTable");
}


//-----------------------------------------------------------------------------
// Expose SendProp.
//-----------------------------------------------------------------------------
void export_send_prop()
{
	class_<SendProp, SendProp *, boost::noncopyable> SendProp_("SendProp", no_init);
	
	// Properties...
	SendProp_.def_readonly("type", &SendProp::m_Type);
	SendProp_.def_readonly("bits", &SendProp::m_nBits);
	SendProp_.def_readonly("low_value", &SendProp::m_fLowValue);
	SendProp_.def_readonly("high_value", &SendProp::m_fHighValue);
	SendProp_.def_readonly("array_prop", &SendProp::m_pArrayProp);
	SendProp_.def_readonly("length", &SendProp::m_nElements);
	SendProp_.def_readonly("element_stride", &SendProp::m_ElementStride);
	SendProp_.def_readonly("exclude_data_table_name", &SendProp::m_pExcludeDTName);
	SendProp_.def_readonly("parent_array_prop_name", &SendProp::m_pParentArrayPropName);
	SendProp_.add_property("name", &SendProp::GetName);
	SendProp_.def_readonly("high_low_mul", &SendProp::m_fHighLowMul);
	SendProp_.add_property("flags", &SendProp::GetFlags);
	
	SendProp_.add_property("data_table",
		make_function(
			&SendProp::GetDataTable,
			reference_existing_object_policy()
		)
	);
	
	SendProp_.add_property("offset", &SendProp::GetOffset);
	
	// CS:GO specific properties...
	SendProp_.NOT_IMPLEMENTED_ATTR("priority");
	
	// Methods...
	SendProp_.def("get_offset", &SendProp::GetOffset);
	
	SendProp_.def("get_data_table",
		&SendProp::GetDataTable,
		reference_existing_object_policy()
	);

	SendProp_.def("get_exclude_data_table_name", &SendProp::GetExcludeDTName);
	SendProp_.def("get_parent_array_prop_name", &SendProp::GetParentArrayPropName);
	SendProp_.def("get_name", &SendProp::GetName);
	SendProp_.def("is_signed", &SendProp::IsSigned);
	SendProp_.def("is_exclude_prop", &SendProp::IsExcludeProp);
	SendProp_.def("is_inside_array", &SendProp::IsInsideArray);
	
	SendProp_.def("get_array_prop",
		&SendProp::GetArrayProp,
		reference_existing_object_policy()
	);

	SendProp_.def("get_length", &SendProp::GetNumElements);
	SendProp_.def("get_element_stride", &SendProp::GetElementStride);
	SendProp_.def("get_type", &SendProp::GetType);
	SendProp_.def("get_flags", &SendProp::GetFlags);

	SendProp_.def("call_proxy",
		&SendPropSharedExt::call_proxy, ("entity_index", arg("element")=0),
		manage_new_object_policy()
	);

	// CS:GO specific methods...
	SendProp_.NOT_IMPLEMENTED("get_priority");
	
	// Engine specific stuff...
	export_engine_specific_send_prop(SendProp_);
	
	// Add memory tools...
	SendProp_ ADD_MEM_TOOLS(SendProp, "SendProp");
}


//-----------------------------------------------------------------------------
// Expose SendPropType.
//-----------------------------------------------------------------------------
void export_send_prop_types()
{
	enum_<SendPropType> SendPropType_("SendPropType");
	
	// Values...
	SendPropType_.value("INT", DPT_Int);
	SendPropType_.value("FLOAT", DPT_Float);
	SendPropType_.value("VECTOR", DPT_Vector);
	SendPropType_.value("VECTORXY", DPT_VectorXY);
	SendPropType_.value("STRING", DPT_String);
	SendPropType_.value("ARRAY", DPT_Array);
	SendPropType_.value("DATATABLE", DPT_DataTable);
	
	// CS:GO specific values...
	NOT_IMPLEMENTED_VALUE(SendPropType, "INT64");
	
	// Engine specific stuff...
	export_engine_specific_send_prop_types(SendPropType_);
}


//-----------------------------------------------------------------------------
// Expose SendPropFlags.
//-----------------------------------------------------------------------------
void export_send_prop_flags(scope _props)
{
	_props.attr("SPROP_UNSIGNED") = SPROP_UNSIGNED;
	_props.attr("SPROP_COORD") = SPROP_COORD;
	_props.attr("SPROP_NOSCALE") = SPROP_NOSCALE;
	_props.attr("SPROP_ROUNDDOWN") = SPROP_ROUNDDOWN;
	_props.attr("SPROP_ROUNDUP") = SPROP_ROUNDUP;
	_props.attr("SPROP_NORMAL") = SPROP_NORMAL;
	_props.attr("SPROP_EXCLUDE") = SPROP_EXCLUDE;
	_props.attr("SPROP_XYZE") = SPROP_XYZE;
	_props.attr("SPROP_INSIDEARRAY") = SPROP_INSIDEARRAY;
	_props.attr("SPROP_PROXY_ALWAYS_YES") = SPROP_PROXY_ALWAYS_YES;
	_props.attr("SPROP_IS_A_VECTOR_ELEM") = SPROP_IS_A_VECTOR_ELEM;
	_props.attr("SPROP_COLLAPSIBLE") = SPROP_COLLAPSIBLE;
	_props.attr("SPROP_COORD_MP") = SPROP_COORD_MP;
	_props.attr("SPROP_COORD_MP_LOWPRECISION") = SPROP_COORD_MP_LOWPRECISION;
	_props.attr("SPROP_COORD_MP_INTEGRAL") = SPROP_COORD_MP_INTEGRAL;
	_props.attr("SPROP_CHANGES_OFTEN") = SPROP_CHANGES_OFTEN;
}


//-----------------------------------------------------------------------------
// Expose DVariant.
//-----------------------------------------------------------------------------
void export_send_prop_variant()
{
	class_<DVariant, DVariant *> SendPropVariant("SendPropVariant");

	// Properties...
	SendPropVariant.def_readonly("type", &DVariant::m_Type);

	// Methods...
	SendPropVariant.def("to_string", &DVariant::ToString);

	// Getter methods...
	SendPropVariant.def("get_float", &SendPropVariantExt::get_typed_value<DPT_Float, float, &DVariant::m_Float>);
	SendPropVariant.def("get_int", &SendPropVariantExt::get_typed_value<DPT_Int, long, &DVariant::m_Int>);
	SendPropVariant.def("get_string", &SendPropVariantExt::get_string);
	SendPropVariant.def("get_data", &SendPropVariantExt::get_data, manage_new_object_policy());
	SendPropVariant.def("get_vector", &SendPropVariantExt::get_vector, manage_new_object_policy());

	// Setter methods...
	SendPropVariant.def("set_float", &SendPropVariantExt::get_typed_value<DPT_Float, float, &DVariant::m_Float>);
	SendPropVariant.def("set_string", &SendPropVariantExt::set_string);
	SendPropVariant.def("set_int", &SendPropVariantExt::set_typed_value<DPT_Int, long, &DVariant::m_Int>);
	SendPropVariant.def("set_data", &SendPropVariantExt::set_data);
	SendPropVariant.def("set_vector", &SendPropVariantExt::set_vector);

	// CS:GO specific methods...
	SendPropVariant.NOT_IMPLEMENTED("get_int64");
	SendPropVariant.NOT_IMPLEMENTED("set_int64");

	// Engine specific stuff...
	export_engine_specific_send_prop_variant(SendPropVariant);

	// Add memory tools...
	SendPropVariant ADD_MEM_TOOLS(DVariant, "SendPropVariant");
}


//-----------------------------------------------------------------------------
// Expose ServerClass.
//-----------------------------------------------------------------------------
void export_server_class()
{
	class_<ServerClass, ServerClass *> ServerClass_("ServerClass", no_init);
	
	// Properties...
	ServerClass_.def_readonly("table", &ServerClass::m_pTable);
	ServerClass_.def_readonly("next", &ServerClass::m_pNext);
	ServerClass_.def_readonly("class_index", &ServerClass::m_ClassID);
	
	// Engine specific stuff...
	export_engine_specific_server_class(ServerClass_);
	
	// Add memory tools...
	ServerClass_ ADD_MEM_TOOLS(ServerClass, "ServerClass");
}