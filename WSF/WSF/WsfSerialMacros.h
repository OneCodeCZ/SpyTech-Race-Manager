///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Macros with ABSTRACT as a part of the name must be used when the class is
// abstract. The macro then doesn't define the CreateObject() method.

// MFC 8.0:  add an extra NULL parameter in _IMPLEMENT_RUNTIMECLASS

#define WSL_IMPLEMENT_SERIAL_ABSTRACT(class_name, base_class_name, wSchema) \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
		NULL) \
	AFX_CLASSINIT _init_##class_name(RUNTIME_CLASS(class_name)); \
	CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \
		{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
			return ar; } \

