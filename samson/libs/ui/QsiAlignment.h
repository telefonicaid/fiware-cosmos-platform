/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef QSI_ALIGNMENT_H
#define QSI_ALIGNMENT_H

/* ****************************************************************************
*
* FILE                     QsiAlignment.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 14 2011
*
*/



namespace Qsi
{



class Base;



/* ****************************************************************************
*
* Alignment - 
*/
typedef struct Alignment
{
	typedef enum Type
	{
		Unaligned,
		North,
		South,
		East,
		West,
		Center
#if 0
		,
		NorthEast,
		NorthWest,
		SouthEast,
		SouthWest
#endif
	} Type;

	Type    type;
	Base*   master;
	Base*   slave;
	int     margin;

	static const char* name(Type type)
	{
		switch (type)
		{
		case North:      return "North";
		case South:      return "South";
		case East:       return "East";
		case West:       return "West";
		case Center:     return "Center";
		case Unaligned:  return "Unaligned";
		}

		return "Undefined alignment";
	}

	static Type invert(Type type)
	{
		switch (type)
		{
		case North:      return South;
		case South:      return North;
		case East:       return West;
		case West:       return East;
		case Center:     return Center;
		case Unaligned:  return Unaligned;
		}

		return Unaligned;
	}
} Alignment;

}

#endif
