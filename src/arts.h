/* Copyright (C) 2000, 2001, 2002, 2003
   Stefan Buehler <sbuehler@uni-bremen.de>
   Patrick Eriksson <Patrick.Eriksson@rss.chalmers.se>
   Oliver Lemke <olemke@uni-bremen.de>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA. */

/**
  \file  arts.h

  The global header file for ARTS. This file is included directly or
  indirectly by each and every ARTS source file. It must therefor not
  contain stuff that should not always be present.

  Note that you do not have to include this file explicitly in many
  cases, since it is included directly or indirectly by most ARTS
  header files.

  \author Stefan Buehler
  \date 16.05.1999 */

/** \mainpage

    <center><img src="arts-splash.gif" alt="ARTS"></center>

    <h2>What is ARTS?</h2>

    ARTS-1-1-x: 3D version for polarized radiative transfer calculations
    including particle scattering.

    The new version ARTS-1-1-x is currently being developed. It includes
    almost all features of the first version and also a number of additional
    functions.

    One of the main new features is the implementation of particle
    scattering as for many applications, scattering of microwave radiation
    by ice particles in the atmosphere emerges as an important issue. The
    new model should be able to simulate realistic cloud cases for microwave
    measurements in limb sounding geometry. Modeling radiative transfer
    through clouds is a complicated topic for various reasons.

    The cloud coverage is vertically and horizontally strongly inhomogeneous
    which implies that a 3D model is unavoidable for simulating realistic
    cases. Especially for simulating limb measurements, a 3D geometry is
    required as the observed region in the atmosphere has a horizontally
    large extent. Clouds consist of a variety of particle types. There
    are liquid water clouds but also cirrus clouds which consist of ice
    particles of different sizes and shapes. Particle scattering leads to
    polarization effects, therefore modeling only the first component of
    the Stokes vector, the scalar intensity, is not sufficient. At least
    the first two components are required, in some cases, depending on the
    formation of the cloud, even all four components.

    The VRTE (Vector Radiative Transfer Equation) is an inhomogeneous vector
    differential equation for the Stokes vector. This equation can be
    solved numerically using an iterative method. So far gaseous absorption
    is pre-calculated using the first version of ARTS and stored in a
    lookup table. The particle properties, i.e. extinction, absorption and
    scattering, are calculated using the T-matrix method and stored in a
    data base.

    The Zeeman effect is also currently being implemented.

    As the program is modular the user can adjust the control file according
    to his/her requirements. The atmospheric dimensionality can be chosen to
    be 1D, 2D or 3D. If clearsky calculations are performed without Zeeman
    effect it does not make sense to calculate all 4 Stokes components. And
    for special symmetries in the scattering region, which can be switched
    on or off, the 3rd and 4th component of the Stokes vector are negligible
    small. Thus the user also can also decide how many Stokes components
    shall be simulated.

    <h2>Documentation</h2>

    You can use the HTML version to browse the source text. Just point and
    click, and eventually you will see the real implementation of functions
    and classes.

    If you are looking for a more comprehensive text, check out the
    Arts User Guide that is also distributed along with the
    program. Section `Documentation' in Chapter `The art of developing
    ARTS' there also tells you how you should add documentation
    headers to your code if you are an ARTS developer.
 */

#ifndef arts_h
#define arts_h

#include <cstddef>

//----------< First of all, include the configuration header >----------
// This header is generated by the configure script.
#if HAVE_CONFIG_H
#include <config.h>
#else
#error "Please run ./configure in the top arts directory before compiling."
#endif          

// C Assert macro:
#include <cassert>


#ifdef HAVE_NAMESPACES
  // We need those to support ansi-compliant compilers (gcc-3x)
  using namespace std;
#endif

//--------------------< Set floating point type >--------------------
/** The type to use for all floating point numbers. You should never
    use float or double explicitly, unless you have a very good
    reason. Always use this type instead.  */
typedef NUMERIC Numeric;

//--------------------< Set integer type >--------------------
/** The type to use for all integer numbers and indices. You should never
    use int, long, or size_t explicitly, unless you have a very good
    reason. Always use this type instead.  */
typedef INDEX Index;

//--------------------< Set string type >--------------------
/** The type to use for all strings. This is just to have consistent
    notation for all the atomic ARTS types. */ 
//typedef string String;


//-----------< define a quick output for vector<> >----------
/* A quick hack output for vector<>. This is only for
    debugging purposes.
    \return As for all output operator functions, the output stream is 
            returned.
    \param  os Output stream
    \param  v  Vector to print                      
    \author SAB  */  
// template<class T>
// ostream& operator<<(ostream& os, const vector<T>& v)
// {
//   for (vector<T>::const_iterator i=v.begin(); i<v.end()-1; ++i)
//     os << *i << ", ";
//   os << *(v.end()-1);
//   return os;
// }



//---------------< Global variable declarations >---------------
// (Definitions of these are in FIXME: where?)


//---------------< Global function declarations: >---------------
// Documentations are with function definitions.
void define_wsv_group_names();  
void define_species_data();
void define_lineshape_data();
void define_lineshape_norm_data();

void arts_exit (int status = 1);

//
// Physical constants are now in constants.cc
//



#endif // arts_h



