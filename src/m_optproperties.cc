/* Copyright (C) 2002-2008
   Sreerekha T.R. <rekha@uni-bremen.de>
   Claudia Emde <claudia.emde@dlr.de>
   Cory Davies <cory@met.ed.ac.uk>
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

/*!
  \file   m_optproperties.cc
  \author Sreerekha T.R. <rekha@uni-bremen.de>, 
          Claudia Emde <claudia.emde@dlr.de>
          Cory Davies <cory@met.ed.ac.uk>
  \date   Mon Jun 10 11:19:11 2002 
  \brief  This filecontains workspace methods for calculating the optical 
  properties for the radiative transfer. 

  Optical properties are the extinction matrix, absorption vector and
  scattering vector.  The optical properties for the gases can be
  calculated with or without Zeeman effect.
*/

/*===========================================================================
  === External declarations
  ===========================================================================*/

#include <cmath>
#include "arts.h"
#include "exceptions.h"
#include "array.h"
#include "matpackIII.h"
#include "matpackVII.h"
#include "logic.h"
#include "interpolation.h"
#include "messages.h"
#include "xml_io.h"
#include "optproperties.h"
#include "math_funcs.h"
#include "sorting.h"
#include "check_input.h"
#include "auto_md.h" 

extern const Numeric PI;
extern const Numeric DEG2RAD;
extern const Numeric RAD2DEG;

#define part_type scat_data_raw[i_pt].ptype
#define f_datagrid scat_data_raw[i_pt].f_grid
#define T_datagrid scat_data_raw[i_pt].T_grid
#define za_datagrid scat_data_raw[i_pt].za_grid
#define aa_datagrid scat_data_raw[i_pt].aa_grid
#define pha_mat_data_raw scat_data_raw[i_pt].pha_mat_data  //CPD: changed from pha_mat_data
#define ext_mat_data_raw scat_data_raw[i_pt].ext_mat_data  //which wouldn't let me play with
#define abs_vec_data_raw scat_data_raw[i_pt].abs_vec_data  //scat_data_mono.
#define pnd_limit 1e-12 // If particle number density is below this value, 
                        // no transformations will be performed


/* Workspace method: Doxygen documentation will be auto-generated */
void pha_mat_sptFromData( // Output:
                         Tensor5& pha_mat_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_raw,
                         const Vector& scat_za_grid,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Index& f_index,
                         const Vector& f_grid,
                         const Numeric& rte_temperature,
                         const Tensor4& pnd_field, 
                         const Index& scat_p_index,
                         const Index& scat_lat_index,
                         const Index& scat_lon_index
                         )
{
  out3 << "Calculate *pha_mat_spt* from database\n";

  const Index N_pt = scat_data_raw.nelem();
  const Index stokes_dim = pha_mat_spt.ncols();

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                        "must be 1,2,3 or 4");
  }
  
  assert( pha_mat_spt.nshelves() == N_pt );
  
  // Phase matrix in laboratory coordinate system. Dimensions:
  // [frequency, za_inc, aa_inc, stokes_dim, stokes_dim]
  Tensor5 pha_mat_data_int;
  

  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // If the particle number density at a specific point in the atmosphere for 
      // the i_pt particle type is zero, we don't need to do the transfromation!
      if (pnd_field(i_pt, scat_p_index, scat_lat_index, scat_lon_index) > pnd_limit)
        {

          // First we have to transform the data from the coordinate system 
          // used in the database (depending on the kind of particle type 
          // specified by *ptype*) to the laboratory coordinate sytem. 
      
          // Frequency interpolation:
     
          // The data is interpolated on one frequency. 
          pha_mat_data_int.resize(pha_mat_data_raw.nshelves(), pha_mat_data_raw.nbooks(),
                                  pha_mat_data_raw.npages(), pha_mat_data_raw.nrows(), 
                                  pha_mat_data_raw.ncols());

      
          // Gridpositions:
          GridPos freq_gp;
          gridpos(freq_gp, f_datagrid, f_grid[f_index]);

          GridPos t_gp;
          gridpos(t_gp, T_datagrid, rte_temperature);

          // Interpolationweights:
          Vector itw(4);
          interpweights(itw, freq_gp, t_gp);
     
          for (Index i_za_sca = 0; i_za_sca < pha_mat_data_raw.nshelves() ; i_za_sca++)
            {
              for (Index i_aa_sca = 0; i_aa_sca < pha_mat_data_raw.nbooks(); i_aa_sca++)
                {
                  for (Index i_za_inc = 0; i_za_inc < pha_mat_data_raw.npages(); 
                       i_za_inc++)
                    {
                      for (Index i_aa_inc = 0; i_aa_inc < pha_mat_data_raw.nrows(); 
                           i_aa_inc++)
                        {  
                          for (Index i = 0; i < pha_mat_data_raw.ncols(); i++)
                            {
                              pha_mat_data_int(i_za_sca, 
                                               i_aa_sca, i_za_inc, 
                                               i_aa_inc, i) =
                                interp(itw,
                                       pha_mat_data_raw(joker, joker, i_za_sca, 
                                                        i_aa_sca, i_za_inc, 
                                                        i_aa_inc, i),
                                       freq_gp, t_gp);
                            }
                        }
                    }
                }
            }
      
          // Do the transformation into the laboratory coordinate system.
          for (Index za_inc_idx = 0; za_inc_idx < scat_za_grid.nelem(); 
               za_inc_idx ++)
            {
              for (Index aa_inc_idx = 0; aa_inc_idx < scat_aa_grid.nelem(); 
                   aa_inc_idx ++) 
                {
                  pha_matTransform(pha_mat_spt
                                   (i_pt, za_inc_idx, aa_inc_idx, joker, joker),
                                   pha_mat_data_int, 
                                   za_datagrid, aa_datagrid,
                                   part_type, scat_za_index, scat_aa_index,
                                   za_inc_idx, 
                                   aa_inc_idx, scat_za_grid, scat_aa_grid);
                }
            }
        }
    }
}
  

/* Workspace method: Doxygen documentation will be auto-generated */
void pha_mat_sptFromDataDOITOpt( // Output:
                         Tensor5& pha_mat_spt,
                         // Input:
                         const ArrayOfTensor7& pha_mat_sptDOITOpt,
                         const ArrayOfSingleScatteringData& scat_data_mono,
                         const Index& doit_za_grid_size,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Numeric& rte_temperature,
                         const Tensor4&  pnd_field, 
                         const Index& scat_p_index,
                         const Index&  scat_lat_index,
                         const Index& scat_lon_index
                         )
{
  // atmosphere_dim = 3
  if (pnd_field.ncols() > 1)
    {
      assert(pha_mat_sptDOITOpt.nelem() == scat_data_mono.nelem());
      // I assume that if the size is o.k. for one particle type is will 
      // also be o.k. for more particle types. 
      assert(pha_mat_sptDOITOpt[0].nlibraries() == scat_data_mono[0].T_grid.nelem());
      assert(pha_mat_sptDOITOpt[0].nvitrines() == doit_za_grid_size);
      assert(pha_mat_sptDOITOpt[0].nshelves() == scat_aa_grid.nelem() );
      assert(pha_mat_sptDOITOpt[0].nbooks() == doit_za_grid_size);
      assert(pha_mat_sptDOITOpt[0].npages() == scat_aa_grid.nelem()); 
    }
  
  // atmosphere_dim = 1, only zenith angle required for scattered directions. 
  else if ( pnd_field.ncols() == 1 )
    {
      //assert(is_size(scat_theta, doit_za_grid_size, 1,
      //                doit_za_grid_size, scat_aa_grid.nelem()));
      
      assert(pha_mat_sptDOITOpt.nelem() == scat_data_mono.nelem());
      // I assume that if the size is o.k. for one particle type is will 
      // also be o.k. for more particle types. 
      assert(pha_mat_sptDOITOpt[0].nlibraries() == scat_data_mono[0].T_grid.nelem());
      assert(pha_mat_sptDOITOpt[0].nvitrines() == doit_za_grid_size);
      assert(pha_mat_sptDOITOpt[0].nshelves() == 1);
      assert(pha_mat_sptDOITOpt[0].nbooks() == doit_za_grid_size);
      assert(pha_mat_sptDOITOpt[0].npages() == scat_aa_grid.nelem()); 
    }
  
  assert(doit_za_grid_size > 0);
  
  // Create equidistant zenith angle grid
  Vector za_grid;
  nlinspace(za_grid, 0, 180, doit_za_grid_size);  
  
  const Index N_pt = scat_data_mono.nelem();
  const Index stokes_dim = pha_mat_spt.ncols();
  
  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                        "must be 1,2,3 or 4");
  }
  
  assert( pha_mat_spt.nshelves() == N_pt );

  GridPos T_gp;
  Vector itw(2);
  
  
  // Initialisation
  pha_mat_spt = 0.;

  // Do the transformation into the laboratory coordinate system.
  for (Index i_pt = 0; i_pt < N_pt; i_pt ++)
    {
      // If the particle number density at a specific point in the atmosphere  
      // for the i_pt particle type is zero, we don't need to do the 
      // transfromation!
      if (pnd_field(i_pt, scat_p_index, scat_lat_index, scat_lon_index) > pnd_limit) //TRS
        {
          if( scat_data_mono[i_pt].T_grid.nelem() > 1)
            {
              //     chk_if_in_range("T_grid", rte_temperature, 
              //                scat_data_mono[i_pt].T_grid[0],
              //                scat_data_mono[i_pt].T_grid
              //                [scat_data_mono[i_pt].T_grid.nelem()-1]);
              
              // Gridpositions:
              gridpos(T_gp, scat_data_mono[i_pt].T_grid, rte_temperature); 
              // Interpolationweights:
              interpweights(itw, T_gp);
            }
          
          
          
          for (Index za_inc_idx = 0; za_inc_idx < doit_za_grid_size;
               za_inc_idx ++)
            {
              for (Index aa_inc_idx = 0; aa_inc_idx < scat_aa_grid.nelem();
                   aa_inc_idx ++) 
                {
                  if( scat_data_mono[i_pt].T_grid.nelem() == 1)
                    {
                      pha_mat_spt(i_pt, za_inc_idx, aa_inc_idx, joker, joker) =
                        pha_mat_sptDOITOpt[i_pt](0, scat_za_index,
                                                 scat_aa_index, za_inc_idx, 
                                                 aa_inc_idx, joker, joker);
                    }
                  
                  // Temperature interpolation
                  else
                    {
                      for (Index i = 0; i< stokes_dim; i++)
                        {
                          for (Index j = 0; j< stokes_dim; j++)
                            {
                              pha_mat_spt(i_pt, za_inc_idx, aa_inc_idx, i, j)=
                                interp(itw,pha_mat_sptDOITOpt[i_pt]
                                       (joker, scat_za_index,
                                        scat_aa_index, za_inc_idx, 
                                        aa_inc_idx, i, j) , T_gp);
                            }
                        }
                    }
                }
            }
        }// TRS
    }
}


/* Workspace method: Doxygen documentation will be auto-generated */
void opt_prop_sptFromData( // Output and Input:
                         Tensor3& ext_mat_spt,
                         Matrix& abs_vec_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_raw,
                         const Vector& scat_za_grid,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Index& f_index,
                         const Vector& f_grid,
                         const Numeric& rte_temperature, 
                         const Tensor4& pnd_field, 
                         const Index& scat_p_index,
                         const Index& scat_lat_index,
                         const Index& scat_lon_index
                         )
{
  
  const Index N_pt = scat_data_raw.nelem();
  const Index stokes_dim = ext_mat_spt.ncols();
  const Numeric za_sca = scat_za_grid[scat_za_index];
  const Numeric aa_sca = scat_aa_grid[scat_aa_index];

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                        "must be 1,2,3 or 4");
  }
  
  assert( ext_mat_spt.npages() == N_pt );
  assert( abs_vec_spt.nrows() == N_pt );

  // Phase matrix in laboratory coordinate system. Dimensions:
  // [frequency, za_inc, aa_inc, stokes_dim, stokes_dim]
  Tensor3 ext_mat_data_int;
  Tensor3 abs_vec_data_int;
  
  // Initialisation
  ext_mat_spt = 0.;
  abs_vec_spt = 0.;


  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // If the particle number density at a specific point in the atmosphere for 
      // the i_pt particle type is zero, we don't need to do the transfromation

      if (pnd_field(i_pt, scat_p_index, scat_lat_index, scat_lon_index) > pnd_limit)
        {
      
      
          // First we have to transform the data from the coordinate system 
          // used in the database (depending on the kind of particle type 
          // specified by *ptype*) to the laboratory coordinate sytem. 
      
          // Frequency interpolation:
     
          // The data is interpolated on one frequency. 
          //
          // Resize the variables for the interpolated data:
          //
          ext_mat_data_int.resize(ext_mat_data_raw.npages(),
                                  ext_mat_data_raw.nrows(), 
                                  ext_mat_data_raw.ncols());
          //
          abs_vec_data_int.resize(abs_vec_data_raw.npages(),
                                  abs_vec_data_raw.nrows(), 
                                  abs_vec_data_raw.ncols());
      
      
          // Gridpositions:
          GridPos freq_gp;
          gridpos(freq_gp, f_datagrid, f_grid[f_index]); 
          GridPos t_gp;
          Vector itw;
      
          if ( T_datagrid.nelem() > 1)
            {
              gridpos(t_gp, T_datagrid, rte_temperature);
          
              // Interpolationweights:
              itw.resize(4);
              interpweights(itw, freq_gp, t_gp);
          
              for (Index i_za_sca = 0; i_za_sca < ext_mat_data_raw.npages();
                   i_za_sca++)
                {
                  for(Index i_aa_sca = 0; i_aa_sca < ext_mat_data_raw.nrows(); 
                      i_aa_sca++)
                    {
                      //
                      // Interpolation of extinction matrix:
                      //
                      for (Index i = 0; i < ext_mat_data_raw.ncols(); i++)
                        {
                          ext_mat_data_int(i_za_sca, i_aa_sca, i) =
                            interp(itw, ext_mat_data_raw(joker, joker, 
                                                         i_za_sca, i_aa_sca, i),
                                   freq_gp, t_gp);
                        }
                    }
                }

              for (Index i_za_sca = 0; i_za_sca < abs_vec_data_raw.npages();
                   i_za_sca++)
                {
                  for(Index i_aa_sca = 0; i_aa_sca < abs_vec_data_raw.nrows(); 
                      i_aa_sca++)
                    {
                      //
                      // Interpolation of absorption vector:
                      //
                      for (Index i = 0; i < abs_vec_data_raw.ncols(); i++)
                        {
                          abs_vec_data_int(i_za_sca, i_aa_sca, i) =
                            interp(itw, abs_vec_data_raw(joker, joker, i_za_sca, 
                                                         i_aa_sca, i),
                                   freq_gp, t_gp);
                        }
                    }
                }
            }
          else 
            {
              // Interpolationweights:
              itw.resize(2);
              interpweights(itw, freq_gp);
          
              for (Index i_za_sca = 0; i_za_sca < ext_mat_data_raw.npages();
                   i_za_sca++)
                {
                  for(Index i_aa_sca = 0; i_aa_sca < ext_mat_data_raw.nrows(); 
                      i_aa_sca++)
                    {
                      //
                      // Interpolation of extinction matrix:
                      //
                      for (Index i = 0; i < ext_mat_data_raw.ncols(); i++)
                        {
                          ext_mat_data_int(i_za_sca, i_aa_sca, i) =
                            interp(itw, ext_mat_data_raw(joker, 0, 
                                                         i_za_sca, i_aa_sca, i),
                                   freq_gp);
                        }
                    }
                }

              for (Index i_za_sca = 0; i_za_sca < abs_vec_data_raw.npages();
                   i_za_sca++)
                {
                  for(Index i_aa_sca = 0; i_aa_sca < abs_vec_data_raw.nrows(); 
                      i_aa_sca++)
                    {
                      //
                      // Interpolation of absorption vector:
                      //
                      for (Index i = 0; i < abs_vec_data_raw.ncols(); i++)
                        {
                          abs_vec_data_int(i_za_sca, i_aa_sca, i) =
                            interp(itw, abs_vec_data_raw(joker, 0, i_za_sca, 
                                                         i_aa_sca, i),
                                   freq_gp);
                        }
                    }
                }
            } 
      

          //
          // Do the transformation into the laboratory coordinate system.
          //
          // Extinction matrix:
          //
     
  
          ext_matTransform(ext_mat_spt(i_pt, joker, joker),
                           ext_mat_data_int,
                           za_datagrid, aa_datagrid, part_type,
                           za_sca, aa_sca);
          // 
          // Absorption vector:
          //
          abs_vecTransform(abs_vec_spt(i_pt, joker),
                           abs_vec_data_int,
                           za_datagrid, aa_datagrid, part_type,
                           za_sca, aa_sca);                
        }

    }
}
                          

/* Workspace method: Doxygen documentation will be auto-generated */
void ext_matAddPart(
                    Tensor3& ext_mat,
                    const Tensor3& ext_mat_spt,
                    const Tensor4& pnd_field,
                    const Index& atmosphere_dim,
                    const Index& scat_p_index,
                    const Index& scat_lat_index,
                    const Index& scat_lon_index) 
                     
{
  Index N_pt = ext_mat_spt.npages();
  Index stokes_dim = ext_mat_spt.nrows();
  
  Matrix ext_mat_part(stokes_dim, stokes_dim, 0.0);

  
  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error(
                        "The dimension of stokes vector can be "
                        "only 1,2,3, or 4");
  }
  if ( ext_mat_spt.ncols() != stokes_dim){
    
    throw runtime_error(" The columns of ext_mat_spt should "
                        "agree to stokes_dim");
  }

  if (atmosphere_dim == 1)
    {
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt; l++)
        { 
          
          // now the last two loops over the stokes dimension.
          for (Index m = 0; m < stokes_dim; m++)
            {
              for (Index n = 0; n < stokes_dim; n++)
                //summation of the product of pnd_field and 
                //ext_mat_spt.
                ext_mat_part(m, n) += 
                  (ext_mat_spt(l, m, n) * pnd_field(l, scat_p_index, 0, 0));
            }
        }

      //Add particle extinction matrix to *ext_mat*.
      ext_mat(0, Range(joker), Range(joker)) += ext_mat_part;
    }
 
  if (atmosphere_dim == 3)
    {
      
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt; l++)
        { 
          
          // now the last two loops over the stokes dimension.
          for (Index m = 0; m < stokes_dim; m++)
            {
              for (Index n = 0; n < stokes_dim; n++)
                //summation of the product of pnd_field and 
                //ext_mat_spt.
                ext_mat_part(m, n) +=  (ext_mat_spt(l, m, n) * 
                                        pnd_field(l, scat_p_index, 
                                                  scat_lat_index, 
                                                  scat_lon_index));
              
            } 
        }

      //Add particle extinction matrix to *ext_mat*.
      ext_mat(0, Range(joker), Range(joker)) += ext_mat_part;

    }

} 


/* Workspace method: Doxygen documentation will be auto-generated */
void abs_vecAddPart(
                      Matrix& abs_vec,
                      const Matrix& abs_vec_spt,
                      const Tensor4& pnd_field,
                      const Index& atmosphere_dim,
                      const Index& scat_p_index,
                      const Index& scat_lat_index,
                      const Index& scat_lon_index) 
                    
{
  Index N_pt = abs_vec_spt.nrows();
  Index stokes_dim = abs_vec_spt.ncols();

  Vector abs_vec_part(stokes_dim, 0.0);

  if ((stokes_dim > 4) || (stokes_dim <1)){
    throw runtime_error("The dimension of stokes vector "
                        "can be only 1,2,3, or 4");
  } 
 
  if (atmosphere_dim == 1)
    {
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt ; ++l)
        {
          // now the loop over the stokes dimension.
          //(CE:) in the middle was l instead of m
          for (Index m = 0; m < stokes_dim; ++m)
             //summation of the product of pnd_field and 
            //abs_vec_spt.
            abs_vec_part[m] += 
              (abs_vec_spt(l, m) * pnd_field(l, scat_p_index, 0, 0));
          
        }
      //Add the particle absorption
      abs_vec(0, Range(joker)) += abs_vec_part;
    }
  
  if (atmosphere_dim == 3)
    {
      // this is a loop over the different particle types
      for (Index l = 0; l < N_pt ; ++l)
        {
          
          // now the loop over the stokes dimension.
          for (Index m = 0; m < stokes_dim; ++m)
             //summation of the product of pnd_field and 
            //abs_vec_spt.
            abs_vec_part[m] += (abs_vec_spt(l, m) *
                                pnd_field(l, scat_p_index,
                                          scat_lat_index, 
                                          scat_lon_index));
          
        }
      //Add the particle absorption
      abs_vec(0,Range(joker)) += abs_vec_part;
    }
} 


/* Workspace method: Doxygen documentation will be auto-generated */
void ext_matInit( Tensor3&      ext_mat,
                  const Vector& f_grid,
                  const Index&  stokes_dim,
                  const Index&   f_index)
{
  Index freq_dim;

  if( f_index < 0 )
    freq_dim = f_grid.nelem();
  else
    freq_dim = 1;
 
  ext_mat.resize( freq_dim,
                  stokes_dim,
                  stokes_dim );
  ext_mat = 0;                  // Initialize to zero!

  out2 << "Set dimensions of ext_mat as ["
       << freq_dim << ","
       << stokes_dim << ","
       << stokes_dim << "] and initialized to 0.\n";
}


/* Workspace method: Doxygen documentation will be auto-generated */
void ext_matAddGas( Tensor3&      ext_mat,
                    const Matrix& abs_scalar_gas )
{
  // Number of Stokes parameters:
  const Index stokes_dim = ext_mat.ncols();

  // The second dimension of ext_mat must also match the number of
  // Stokes parameters:
  if ( stokes_dim != ext_mat.nrows() )
    throw runtime_error("Row dimension of ext_mat inconsistent with "
                        "column dimension."); 

  // Number of frequencies:
  const Index f_dim = ext_mat.npages();

  // This must be consistent with the first dimension of
  // abs_scalar_gas. Check this:
  if ( f_dim != abs_scalar_gas.nrows() )
    throw runtime_error("Frequency dimension of ext_mat and abs_scalar_gas\n"
                        "are inconsistent in ext_matAddGas.");

  // Sum up absorption over all species.
  // This gives us an absorption vector for all frequencies. Of course
  // this includes the special case that there is only one frequency.
  Vector abs_total(f_dim);
  for ( Index i=0; i<f_dim; ++i )
    abs_total[i] = abs_scalar_gas(i,joker).sum();

  for ( Index i=0; i<stokes_dim; ++i )
    {
      // Add the absorption value to all the diagonal elements:
      ext_mat(joker,i,i) += abs_total[joker];
      
      // We don't have to do anything about the off-diagonal
      // elements! 
    }
}


/* Workspace method: Doxygen documentation will be auto-generated */
void abs_vecInit( Matrix&       abs_vec,
                  const Vector& f_grid,
                  const Index&  stokes_dim,
                  const Index&  f_index)
{
  Index freq_dim;

  if( f_index < 0 )
    freq_dim = f_grid.nelem();
  else
    freq_dim = 1;
 
  abs_vec.resize( freq_dim,
                  stokes_dim );
  abs_vec = 0;                  // Initialize to zero!

  out2 << "Set dimensions of abs_vec as ["
       << freq_dim << ","
       << stokes_dim << "] and initialized to 0.\n";
}


/* Workspace method: Doxygen documentation will be auto-generated */
void abs_vecAddGas( Matrix&       abs_vec,
                    const Matrix& abs_scalar_gas )
{
  // Number of frequencies:
  const Index f_dim = abs_vec.nrows();

  // This must be consistent with the first dimension of
  // abs_scalar_gas. Check this:
  if ( f_dim != abs_scalar_gas.nrows() )
    throw runtime_error("Frequency dimension of abs_vec and abs_scalar_gas\n"
                        "are inconsistent in abs_vecAddGas.");

  // Loop all frequencies. Of course this includes the special case
  // that there is only one frequency.
  for ( Index i=0; i<f_dim; ++i )
    {
      // Sum up the columns of abs_scalar_gas and add to the first
      // element of abs_vec.
      abs_vec(i,0) += abs_scalar_gas(i,joker).sum();
    }

  // We don't have to do anything about higher elements of abs_vec,
  // since scalar gas absorption only influences the first element.
}


/* Workspace method: Doxygen documentation will be auto-generated */
/*
void ext_matAddGasZeeman( Tensor3&      ext_mat,
                          const Tensor3&  ext_mat_zee )
{
  // Number of Stokes parameters:
  const Index stokes_dim = ext_mat.ncols();

  // The second dimension of ext_mat must also match the number of
  // Stokes parameters:
  if ( stokes_dim != ext_mat.nrows() )
    throw runtime_error("Row dimension of ext_mat inconsistent with "
                        "column dimension."); 

  for ( Index i=0; i<stokes_dim; ++i )
    {
      for ( Index j=0; j<stokes_dim; ++j )
        {
          // Add the zeeman extinction to extinction matrix.
          ext_mat(joker,i,j) += ext_mat_zee(joker, i, j);
        }
      
    }
}
*/


/* Workspace method: Doxygen documentation will be auto-generated */
/*
void abs_vecAddGasZeeman( Matrix&      abs_vec,
                          const Matrix& abs_vec_zee )
{
  // Number of Stokes parameters:
  const Index stokes_dim = abs_vec_zee.ncols();
  // that there is only one frequency.
  for ( Index j=0; j<stokes_dim; ++j )
    {
      abs_vec(joker,j) += abs_vec_zee(joker,j);
    }
}
*/


/* Workspace method: Doxygen documentation will be auto-generated */
void pha_matCalc(
                 Tensor4& pha_mat,
                 const Tensor5& pha_mat_spt,
                 const Tensor4& pnd_field,
                 const Index& atmosphere_dim,
                 const Index& scat_p_index,
                 const Index& scat_lat_index,
                 const Index& scat_lon_index) 
                      
{

  Index N_pt = pha_mat_spt.nshelves();
  Index Nza = pha_mat_spt.nbooks();
  Index Naa = pha_mat_spt.npages();
  Index stokes_dim = pha_mat_spt.nrows();
 
  pha_mat.resize(Nza, Naa, stokes_dim, stokes_dim);

  // Initialisation
  pha_mat = 0.0;
          
  if (atmosphere_dim == 1)
    {
      // this is a loop over the different particle types
      for (Index pt_index = 0; pt_index < N_pt; ++ pt_index)
        {
          // these are loops over zenith angle and azimuth angle
          for (Index za_index = 0; za_index < Nza; ++ za_index)
            {
              for (Index aa_index = 0; aa_index < Naa; ++ aa_index)
                {
                  
                  // now the last two loops over the stokes dimension.
                  for (Index stokes_index_1 = 0; stokes_index_1 < stokes_dim; 
                       ++  stokes_index_1)
                    {
                      for (Index stokes_index_2 = 0; stokes_index_2 < stokes_dim;
                           ++ stokes_index_2)
                         //summation of the product of pnd_field and 
                          //pha_mat_spt.
                        pha_mat(za_index, aa_index,  
                                     stokes_index_1, stokes_index_2) += 
                          
                          (pha_mat_spt(pt_index, za_index, aa_index,  
                                       stokes_index_1, stokes_index_2) * 
                           pnd_field(pt_index,scat_p_index, 0, 0));
                    }
                }
            }
        }
    }
          
  if (atmosphere_dim == 3)
    {
      // this is a loop over the different particle types
      for (Index pt_index = 0; pt_index < N_pt; ++ pt_index)
        {
          
          // these are loops over zenith angle and azimuth angle
          for (Index za_index = 0; za_index < Nza; ++ za_index)
            {
              for (Index aa_index = 0; aa_index < Naa; ++ aa_index)
                {
                  
                  // now the last two loops over the stokes dimension.
                  for (Index i = 0;  i < stokes_dim; ++  i)
                    {
                      for (Index j = 0; j < stokes_dim; ++ j)
                        {
                          //summation of the product of pnd_field and 
                          //pha_mat_spt.
                          pha_mat(za_index, aa_index, i,j ) += 
                            (pha_mat_spt(pt_index, za_index, aa_index, i, j) * 
                             pnd_field(pt_index, scat_p_index,  
                                       scat_lat_index, scat_lon_index));
                          
                          
                        } 
                    }	
                }
            }		
        }	
    }		
}


/* Workspace method: Doxygen documentation will be auto-generated */
void scat_data_rawCheck(//Input:
                         const ArrayOfSingleScatteringData& scat_data_raw
                         )
{

  xml_write_to_file("SingleScatteringData", scat_data_raw);
  
  const Index N_pt = scat_data_raw.nelem();
  
  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      Numeric Csca = AngIntegrate_trapezoid
        (pha_mat_data_raw(0, 0, joker, 0, 0, 0, 0), za_datagrid);

      Numeric Cext = ext_mat_data_raw(0,0,0,0,0);

      Numeric Cabs = Cext - Csca;

      Numeric Cabs_data = abs_vec_data_raw(0,0,0,0,0);

      Numeric Csca_data = Cext - Cabs_data;

      out1 << " Coefficients in database: \n"
           << "Cext: " << Cext << " Cabs: " << Cabs_data << " Csca: " << Csca_data  
           << " \n Calculated absorption cooefficient: \n"
           << "Cabs calculated: " << Cabs   
           << " Csca: " << Csca << "\n";
      
    }
}


/* Workspace method: Doxygen documentation will be auto-generated */
void DoitScatteringDataPrepare( //Output:
                               ArrayOfTensor7& pha_mat_sptDOITOpt,
                               ArrayOfSingleScatteringData& scat_data_mono,
                               //Input:
                               const Index& doit_za_grid_size,
                               const Vector& scat_aa_grid,
                               const ArrayOfSingleScatteringData&
                               scat_data_raw,
                               const Vector& f_grid,
                               const Index& f_index,
                               const Index& atmosphere_dim,
                               const Index& stokes_dim
                                  )
{
  
  // Check, whether single scattering data contains the right frequencies:
  for (Index i = 0; i<scat_data_raw.nelem(); i++)
    {
      if (scat_data_raw[i].f_grid[0] > f_grid[f_index] || 
          scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1] 
          < f_grid[f_index])
        {
          ostringstream os;
          os << "Frequency of the scattering calculation " << f_grid[f_index] 
             << " GHz is not contained \n in the frequency grid of the " << i+1 
             << "the single scattering data file \n(*ParticleTypeAdd*). "
             << "Range:"  << scat_data_raw[i].f_grid[0]/1e9 <<" - "
             << scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1]/1e9
             <<" GHz \n";
          throw runtime_error( os.str() );
        }
    }
  
  // Interpolate all the data in frequency
  scat_data_monoCalc(scat_data_mono, scat_data_raw, f_grid, f_index);
  
  // For 1D calculation the scat_aa dimension is not required:
  Index N_aa_sca;
  if(atmosphere_dim == 1)
    N_aa_sca = 1;
  else
    N_aa_sca = scat_aa_grid.nelem();
  
  Vector za_grid;
  nlinspace(za_grid, 0, 180, doit_za_grid_size);

  assert( scat_data_raw.nelem() == scat_data_mono.nelem() );
  
  Index N_pt = scat_data_raw.nelem();  
  
  pha_mat_sptDOITOpt.resize(N_pt);

  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      Index N_T = scat_data_mono[i_pt].T_grid.nelem();
      pha_mat_sptDOITOpt[i_pt].resize(N_T, doit_za_grid_size, N_aa_sca, 
                                      doit_za_grid_size, scat_aa_grid.nelem(), 
                                      stokes_dim, stokes_dim);
      
      //    Initialize:
      pha_mat_sptDOITOpt[i_pt]= 0.;
   
      // Calculate all scattering angles for all combinations of incoming 
      // and scattered directions and interpolation.
      for (Index t_idx = 0; t_idx < N_T; t_idx ++)
        {
          // These are the scattered directions as called in *scat_field_calc*
          for (Index za_sca_idx = 0; za_sca_idx < doit_za_grid_size; za_sca_idx ++)
            {
              for (Index aa_sca_idx = 0; aa_sca_idx < N_aa_sca; aa_sca_idx ++)
                {
                  // Integration is performed over all incoming directions
                  for (Index za_inc_idx = 0; za_inc_idx < doit_za_grid_size;
                       za_inc_idx ++)
                    {
                      for (Index aa_inc_idx = 0; aa_inc_idx <
                             scat_aa_grid.nelem();
                           aa_inc_idx ++)
                        {
                          pha_matTransform(pha_mat_sptDOITOpt[i_pt]
                                           (t_idx, za_sca_idx,            
                                            aa_sca_idx, za_inc_idx, aa_inc_idx,
                                            joker, joker),
                                           scat_data_mono[i_pt].
                                           pha_mat_data
                                           (0,t_idx,joker,joker,joker,
                                            joker,joker),
                                           scat_data_mono[i_pt].za_grid,
                                           scat_data_mono[i_pt].aa_grid,
                                           scat_data_mono[i_pt].ptype,
                                           za_sca_idx,
                                           aa_sca_idx,
                                           za_inc_idx,
                                           aa_inc_idx,
                                           za_grid,
                                           scat_aa_grid);
                        }
                    }
                }
            }
        }
    }
 } 


/* Workspace method: Doxygen documentation will be auto-generated */
void scat_data_monoCalc(
                        ArrayOfSingleScatteringData& scat_data_mono,
                        const ArrayOfSingleScatteringData& scat_data_raw,
                        const Vector& f_grid,
                        const Index& f_index
                       )
{
  // Check, whether single scattering data contains the right frequencies:
  for (Index i = 0; i<scat_data_raw.nelem(); i++)
    {
      if (scat_data_raw[i].f_grid[0] > f_grid[f_index] || 
          scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1] 
          < f_grid[f_index])
        {
          ostringstream os;
          os << "Frequency of the scattering calculation " << f_grid[f_index] 
             << " GHz is not contained \nin the frequency grid of the " << i+1 
             << "the single scattering data file \n(*ParticleTypeAdd*). "
             << "Range:"  << scat_data_raw[i].f_grid[0]/1e9 <<" - "
             << scat_data_raw[i].f_grid[scat_data_raw[i].f_grid.nelem()-1]/1e9
             <<" GHz \n";
          throw runtime_error( os.str() );
        }
    }

  const Index N_pt = scat_data_raw.nelem();
  
  //Initialise scat_data_mono
  scat_data_mono.resize(N_pt);
  
  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // Gridpositions:
      GridPos freq_gp;
      gridpos(freq_gp, f_datagrid, f_grid[f_index]); 
      
      // Interpolationweights:
      Vector itw(2);
      interpweights(itw, freq_gp);

      //Stuff that doesn't need interpolating
      scat_data_mono[i_pt].ptype=part_type;
      scat_data_mono[i_pt].f_grid.resize(1);
      scat_data_mono[i_pt].f_grid=f_grid[f_index];
      scat_data_mono[i_pt].T_grid=scat_data_raw[i_pt].T_grid;
      scat_data_mono[i_pt].za_grid=za_datagrid;
      scat_data_mono[i_pt].aa_grid=aa_datagrid;
          
      //Phase matrix data
      scat_data_mono[i_pt].pha_mat_data.resize(1,
                                               pha_mat_data_raw.nvitrines(),
                                               pha_mat_data_raw.nshelves(),
                                               pha_mat_data_raw.nbooks(),
                                               pha_mat_data_raw.npages(),
                                               pha_mat_data_raw.nrows(),
                                               pha_mat_data_raw.ncols());
      
      for (Index t_index = 0; t_index < pha_mat_data_raw.nvitrines(); t_index ++)
        {
          for (Index i_za_sca = 0; i_za_sca < pha_mat_data_raw.nshelves();
               i_za_sca++)
            {
              for (Index i_aa_sca = 0; i_aa_sca < pha_mat_data_raw.nbooks();
                   i_aa_sca++)
                {
                  for (Index i_za_inc = 0; i_za_inc < 
                         pha_mat_data_raw.npages(); 
                       i_za_inc++)
                    {
                      for (Index i_aa_inc = 0; 
                           i_aa_inc < pha_mat_data_raw.nrows(); 
                           i_aa_inc++)
                        {  
                          for (Index i = 0; i < pha_mat_data_raw.ncols(); i++)
                            {
                              scat_data_mono[i_pt].pha_mat_data(0, t_index, 
                                                                i_za_sca, 
                                                                i_aa_sca,
                                                                i_za_inc, 
                                                                i_aa_inc, i) =
                                interp(itw,
                                       pha_mat_data_raw(joker, t_index,
                                                        i_za_sca, 
                                                        i_aa_sca, i_za_inc, 
                                                        i_aa_inc, i),
                                       freq_gp);
                            }
                        }
                    }
                }
            }
          //Extinction matrix data
          scat_data_mono[i_pt].ext_mat_data.resize(1, T_datagrid.nelem(), 
                                                   ext_mat_data_raw.npages(),
                                                   ext_mat_data_raw.nrows(),
                                                   ext_mat_data_raw.ncols());
          for (Index i_za_sca = 0; i_za_sca < ext_mat_data_raw.npages();
               i_za_sca++)
            {
              for(Index i_aa_sca = 0; i_aa_sca < ext_mat_data_raw.nrows();
                  i_aa_sca++)
                {
                  //
                  // Interpolation of extinction matrix:
                  //
                  for (Index i = 0; i < ext_mat_data_raw.ncols(); i++)
                    {
                      scat_data_mono[i_pt].ext_mat_data(0, t_index, 
                                                        i_za_sca, i_aa_sca, i)
                        = interp(itw, ext_mat_data_raw(joker, t_index, i_za_sca,
                                                       i_aa_sca, i),
                                 freq_gp);
                    }
                }
            }
          //Absorption vector data
          scat_data_mono[i_pt].abs_vec_data.resize(1, T_datagrid.nelem(),
                                                   abs_vec_data_raw.npages(),
                                                   abs_vec_data_raw.nrows(),
                                                   abs_vec_data_raw.ncols());
          for (Index i_za_sca = 0; i_za_sca < abs_vec_data_raw.npages() ;
               i_za_sca++)
            {
              for(Index i_aa_sca = 0; i_aa_sca < abs_vec_data_raw.nrows();
                  i_aa_sca++)
                {
                  //
                  // Interpolation of absorption vector:
                  //
                  for (Index i = 0; i < abs_vec_data_raw.ncols(); i++)
                    {
                      scat_data_mono[i_pt].abs_vec_data(0, t_index, i_za_sca,
                                                        i_aa_sca, i) =
                        interp(itw, abs_vec_data_raw(joker, t_index, i_za_sca,
                                                     i_aa_sca, i),
                               freq_gp);
                    }
                }
            }
        }
    }
}


/* Workspace method: Doxygen documentation will be auto-generated */
void opt_prop_sptFromMonoData( // Output and Input:
                         Tensor3& ext_mat_spt,
                         Matrix& abs_vec_spt,
                         // Input:
                         const ArrayOfSingleScatteringData& scat_data_mono,
                         const Vector& scat_za_grid,
                         const Vector& scat_aa_grid,
                         const Index& scat_za_index, // propagation directions
                         const Index& scat_aa_index,
                         const Numeric& rte_temperature,
                         const Tensor4& pnd_field, 
                         const Index& scat_p_index,
                         const Index& scat_lat_index,
                         const Index& scat_lon_index
                         )
{
  const Index N_pt = scat_data_mono.nelem();
  const Index stokes_dim = ext_mat_spt.ncols();
  const Numeric za_sca = scat_za_grid[scat_za_index];
  const Numeric aa_sca = scat_aa_grid[scat_aa_index];
  
  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                         "must be 1,2,3 or 4");
  }
  
  assert( ext_mat_spt.npages() == N_pt );
  assert( abs_vec_spt.nrows() == N_pt );

  // Initialisation
  ext_mat_spt = 0.;
  abs_vec_spt = 0.;

  GridPos t_gp;
  
  Vector itw(2);
  
  // Loop over the included particle_types
  for (Index i_pt = 0; i_pt < N_pt; i_pt++)
    {
      // If the particle number density at a specific point in the atmosphere for 
      // the i_pt particle type is zero, we don't need to do the transfromation!
      if (pnd_field(i_pt, scat_p_index, scat_lat_index, scat_lon_index) > pnd_limit)
        {
 
          // First we have to transform the data from the coordinate system 
          // used in the database (depending on the kind of particle type 
          // specified by *ptype*) to the laboratory coordinate sytem. 
          
          //
          // Do the transformation into the laboratory coordinate system.
          //
          // Extinction matrix:
          //
          Index ext_npages = scat_data_mono[i_pt].ext_mat_data.npages();  
          Index ext_nrows = scat_data_mono[i_pt].ext_mat_data.nrows();  
          Index ext_ncols = scat_data_mono[i_pt].ext_mat_data.ncols();  
          Index abs_npages = scat_data_mono[i_pt].abs_vec_data.npages();  
          Index abs_nrows = scat_data_mono[i_pt].abs_vec_data.nrows();  
          Index abs_ncols = scat_data_mono[i_pt].abs_vec_data.ncols();  
          Tensor3 ext_mat_data1temp(ext_npages,ext_nrows,ext_ncols,0.0);
          Tensor3 abs_vec_data1temp(abs_npages,abs_nrows,abs_ncols,0.0);

          //Check that scattering data temperature range covers required temperature
          ConstVectorView t_grid = scat_data_mono[i_pt].T_grid;
      
          if (t_grid.nelem() > 1)
            {
              //   if ((rte_temperature<t_grid[0])||(rte_temperature>t_grid[t_grid.nelem()-1]))
              //             {
              //               throw runtime_error("rte_temperature outside scattering data temperature range");
              //             }
          
              //interpolate over temperature
              gridpos(t_gp, scat_data_mono[i_pt].T_grid, rte_temperature);
              interpweights(itw, t_gp);
              for (Index i_p = 0; i_p < ext_npages ; i_p++)
                {
                  for (Index i_r = 0; i_r < ext_nrows ; i_r++)
                    {
                      for (Index i_c = 0; i_c < ext_ncols ; i_c++)
                        {
                          ext_mat_data1temp(i_p,i_r,i_c)=interp(itw,
                                                                scat_data_mono[i_pt].ext_mat_data(0,joker,i_p,i_r,i_c),t_gp);
                        }
                    }
                }
            }
          else 
            {
              ext_mat_data1temp = 
                scat_data_mono[i_pt].ext_mat_data(0,0,joker,joker,joker);
            }
      
          ext_matTransform(ext_mat_spt(i_pt, joker, joker),
                           ext_mat_data1temp,
                           scat_data_mono[i_pt].za_grid, 
                           scat_data_mono[i_pt].aa_grid, 
                           scat_data_mono[i_pt].ptype,
                           za_sca, aa_sca);
          // 
          // Absorption vector:
          //
     
          if (t_grid.nelem() > 1)
            {
              //interpolate over temperature
              for (Index i_p = 0; i_p < abs_npages ; i_p++)
                {
                  for (Index i_r = 0; i_r < abs_nrows ; i_r++)
                    {
                      for (Index i_c = 0; i_c < abs_ncols ; i_c++)
                        {
                          abs_vec_data1temp(i_p,i_r,i_c)=interp(itw,
                                                                scat_data_mono[i_pt].abs_vec_data(0,joker,i_p,i_r,i_c),t_gp);
                        }
                    }
                }
            }
          else
            {
              abs_vec_data1temp = 
                scat_data_mono[i_pt].abs_vec_data(0,0,joker,joker,joker);
            }
      
          abs_vecTransform(abs_vec_spt(i_pt, joker),
                           abs_vec_data1temp,
                           scat_data_mono[i_pt].za_grid, 
                           scat_data_mono[i_pt].aa_grid, 
                           scat_data_mono[i_pt].ptype,
                           za_sca, aa_sca);                
        }

    }
}
 

/* Workspace method: Doxygen documentation will be auto-generated */
void pha_mat_sptFromMonoData( // Output:
                             Tensor5& pha_mat_spt,
                             // Input:
                             const ArrayOfSingleScatteringData& scat_data_mono,
                             const Index& doit_za_grid_size,
                             const Vector& scat_aa_grid,
                             const Index& scat_za_index, // propagation directions
                             const Index& scat_aa_index,
                             const Numeric& rte_temperature,
                             const Tensor4& pnd_field, 
                             const Index& scat_p_index,
                             const Index& scat_lat_index,
                             const Index& scat_lon_index
                             )
{
  out3 << "Calculate *pha_mat_spt* from scat_data_mono. \n";
  
  Vector za_grid;
  nlinspace(za_grid, 0, 180, doit_za_grid_size); 

  const Index N_pt = scat_data_mono.nelem();
  const Index stokes_dim = pha_mat_spt.ncols();

 

  if (stokes_dim > 4 || stokes_dim < 1){
    throw runtime_error("The dimension of the stokes vector \n"
                        "must be 1,2,3 or 4");
  }
  
  assert( pha_mat_spt.nshelves() == N_pt );
  
  GridPos T_gp;
  Vector itw(2);

  // Initialisation
  pha_mat_spt = 0.;
  
  for (Index i_pt = 0; i_pt < N_pt; i_pt ++)
    { 
      // If the particle number density at a specific point in the atmosphere 
      // for the i_pt particle type is zero, we don't need to do the 
      // transfromation!
      if (pnd_field(i_pt, scat_p_index, scat_lat_index, scat_lon_index) >
          pnd_limit)
        { 
         
          // Temporary phase matrix wich icludes the all temperatures.
          Tensor3 pha_mat_spt_tmp(scat_data_mono[i_pt].T_grid.nelem(), 
                                  pha_mat_spt.nrows(), pha_mat_spt.ncols());
  
          pha_mat_spt_tmp = 0.; 
      
          if( scat_data_mono[i_pt].T_grid.nelem() > 1)
            {
              //         chk_if_in_range("T_grid", rte_temperature, 
              //                           scat_data_mono[i_pt].T_grid[0],
              //                           scat_data_mono[i_pt].T_grid
              //                           [scat_data_mono[i_pt].T_grid.nelem()-1]);
          
              // Gridpositions:
              gridpos(T_gp, scat_data_mono[i_pt].T_grid, rte_temperature); 
              // Interpolationweights:
              interpweights(itw, T_gp);
            }
      
          // Do the transformation into the laboratory coordinate system.
          for (Index za_inc_idx = 0; za_inc_idx < doit_za_grid_size;
               za_inc_idx ++)
            {
              for (Index aa_inc_idx = 0; aa_inc_idx < scat_aa_grid.nelem();
                   aa_inc_idx ++) 
                {
                  for (Index t_idx = 0; t_idx < 
                         scat_data_mono[i_pt].T_grid.nelem();
                       t_idx ++)
                    {
                      pha_matTransform( pha_mat_spt_tmp(t_idx, joker, joker),
                                        scat_data_mono[i_pt].
                                        pha_mat_data
                                        (0,0,joker,joker,joker,
                                         joker,joker),
                                        scat_data_mono[i_pt].za_grid, 
                                        scat_data_mono[i_pt].aa_grid,
                                        scat_data_mono[i_pt].ptype,
                                        scat_za_index, scat_aa_index, 
                                        za_inc_idx, 
                                        aa_inc_idx, za_grid, scat_aa_grid);
                    }
                  // Temperature interpolation
                  if( scat_data_mono[i_pt].T_grid.nelem() > 1)
                    {
                      for (Index i = 0; i< stokes_dim; i++)
                        {
                          for (Index j = 0; j< stokes_dim; j++)
                            {
                              pha_mat_spt(i_pt, za_inc_idx, aa_inc_idx, i, j)=
                                interp(itw, pha_mat_spt_tmp(joker, i, j), T_gp);
                            }
                        }
                    }
                  else // no temperatue interpolation required
                    {
                      pha_mat_spt(i_pt, za_inc_idx, aa_inc_idx, joker, joker) =
                        pha_mat_spt_tmp(0, joker, joker);
                    }
                }
            }
        }
    }
}

