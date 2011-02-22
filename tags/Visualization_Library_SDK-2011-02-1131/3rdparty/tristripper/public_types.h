//
// Copyright (C) 2004 Tanguy Fautr�.
// For conditions of distribution and use,
// see copyright notice in tri_stripper.h
//
//////////////////////////////////////////////////////////////////////
// SVN: $Id: public_types.h 86 2005-06-08 17:47:27Z gpsnoopy $
//////////////////////////////////////////////////////////////////////

#ifndef TRI_STRIPPER_HEADER_GUARD_PUBLIC_TYPES_H
#define TRI_STRIPPER_HEADER_GUARD_PUBLIC_TYPES_H

#include <vector>




namespace triangle_stripper
{

	typedef unsigned int index;
	typedef std::vector<index> indices;

	enum primitive_type
	{
		TRIANGLES		= 0x0004,	// = GL_TRIANGLES
		TRIANGLE_STRIP	= 0x0005	// = GL_TRIANGLE_STRIP
	};

	struct primitive_group
	{
		indices			Indices;
		primitive_type	Type;
	};

	typedef std::vector<primitive_group> primitive_vector;

}




#endif // TRI_STRIPPER_HEADER_GUARD_PUBLIC_TYPES_H
