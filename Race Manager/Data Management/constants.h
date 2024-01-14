///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define FILTER_TEMPLATE						"Race Files (*.rce)|*.rce|All files (*.*)|*.*||"
#define RACE_EXT									"rce"
#define DRIVERS_FILTER_TEMPLATE		"Drivers Files (*.drvs)|*.drvs|All files (*.*)|*.*||"
#define DRIVERS_EXT								"drvs"

#define	MAX_LANES							8
#define	MAX_GROUPS						26				// max. is num of letters!!!!!
#define	MAX_PAUSING						2
#define	MAX_ENTITIES					10000			// only for dialog controls

#define UV_GROUPS_HAS_CHANGED	100				// CView::OnUpdate hint