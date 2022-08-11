/* Copyright (C) 2021
   Jon Petersen <jon.petersen@studium.uni-hamburg.de>
   Manfred Brath  <manfred.brath@uni-hamburg.de>

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

/*===========================================================================
  ===  File description
  ===========================================================================*/

#include "messages.h"
#include "physics_funcs.h"
#include "arts.h"
#include "auto_md.h"
#include "star.h"


/*!
  \file   m_star.cc
  \author Jon Petersen  <jon.petersen@studium.uni-hamburg.de>
          Manfred Brath  <manfred.brath@.uni-hamburg.de>
  \date   2021-02-08

  \brief  Workspace functions related to simulation of radiation fluxes.

  These functions are listed in the doxygen documentation as entries of the
  file auto_md.h.
*/


using Constant::pi;

/*===========================================================================
  === The functions
  ===========================================================================*/

/* Workspace method: Doxygen documentation will be auto-generated */
void starsAddSingleBlackbody(ArrayOfStar &stars,
                         Index &stars_do,
                         // Inputs:
                         const Vector &f_grid,
                         const Index &stokes_dim,
                         const Numeric &radius,
                         const Numeric &distance,
                         const Numeric &temperature,
                         const Numeric &latitude,
                         const Numeric &longitude,
                         const Verbosity &) {

  // some sanity checks
  ARTS_USER_ERROR_IF (distance<radius,
                      "The distance to the center of the star (",distance," m) \n"
                     " is smaller than the radius of the star (", radius," m )")

  Star& new_star = stars.emplace_back();

  // spectrum
  new_star.spectrum=Matrix(f_grid.nelem(), stokes_dim,0. );

  planck(new_star.spectrum(joker,0), f_grid, temperature);
  new_star.spectrum *= pi ; // outgoing flux at the surface of the star.


  new_star.description = "Blackbody star" ;
  new_star.radius = radius;
  new_star.distance = distance;
  new_star.latitude = latitude;
  new_star.longitude = longitude;

  // set flag
  stars_do = 1;
}

/* Workspace method: Doxygen documentation will be auto-generated */
void starsAddSingleFromGrid(ArrayOfStar &stars,
                         Index &stars_do,
                         // Inputs:
                         const Vector &f_grid,
                         const Index &stokes_dim,
                         const GriddedField2& star_spectrum_raw,
                         const Numeric &radius,
                         const Numeric &distance,
                         const Numeric &temperature,
                         const Numeric &latitude,
                         const Numeric &longitude,
                         const String &description,
                         const Verbosity &verbosity) {

  // some sanity checks
  ARTS_USER_ERROR_IF (distance<radius,
                      "The distance to the center of the star (",distance," m) \n"
                     " is smaller than the radius of the star (", radius," m )")

  // interpolate field
  Matrix int_data = regrid_star_spectrum(star_spectrum_raw, f_grid, stokes_dim, temperature, verbosity);

  // create star
  Star& new_star = stars.emplace_back();

  new_star.spectrum = int_data; // set spectrum
  new_star.spectrum *= pi; // outgoing flux at the surface of the star.

  new_star.description = description;
  new_star.radius = radius;
  new_star.distance = distance;
  new_star.latitude = latitude;
  new_star.longitude = longitude;

  // set flag
  stars_do = 1;

}

void starsOff(Index &stars_do,
             ArrayOfStar &stars,
             const Verbosity &){

  // set flag to False (default)
  stars_do = 0;

  // create empty Array of Matrix for the star_spectrum
  stars.resize(0);

}