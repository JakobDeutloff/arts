/* Copyright (C) 2012
   Patrick Eriksson <Patrick.Eriksson@chalmers.se>
   Stefan Buehler   <sbuehler(at)ltu.se>

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

/*!
  \file   m_transmitter.cc
  \author Patrick Eriksson <patrick.eriksson@chalmers.se>
  \date   2012-10-31

  \brief  Workspace functions related to transmitters and radiative transfer
  for transmitted signals.

  These functions are listed in the doxygen documentation as entries of the
  file auto_md.h.
*/



/*===========================================================================
  === External declarations
  ===========================================================================*/

#include <cmath>
#include <stdexcept>
#include "arts.h"
#include "auto_md.h"
#include "complex.h"
#include "geodetic.h"
#include "jacobian.h"
#include "lin_alg.h"
#include "logic.h"
#include "math_funcs.h"
#include "messages.h"
#include "rte.h"
#include "sensor.h"

extern const Numeric DEG2RAD;
extern const Numeric PI;
extern const Numeric RAD2DEG;
extern const Numeric SPEED_OF_LIGHT;



/* Workspace method: Doxygen documentation will be auto-generated */
void iyRadioLink(
         Workspace&                   ws,
         Matrix&                      iy,
         ArrayOfTensor4&              iy_aux,
         Ppath&                       ppath,
         ArrayOfTensor3&              diy_dx,
   const Index&                       stokes_dim,
   const Vector&                      f_grid,
   const Index&                       atmosphere_dim,
   const Vector&                      p_grid,
   const Vector&                      lat_grid,
   const Vector&                      lon_grid,
   const Tensor3&                     z_field,
   const Tensor3&                     t_field,
   const Tensor4&                     vmr_field,
   const ArrayOfArrayOfSpeciesTag&    abs_species,
   const Tensor3&                     wind_u_field,
   const Tensor3&                     wind_v_field,
   const Tensor3&                     wind_w_field,
   const Tensor3&                     mag_u_field,
   const Tensor3&                     mag_v_field,
   const Tensor3&                     mag_w_field,
   const Tensor3&                     edensity_field,
   const Vector&                      refellipsoid,
   const Matrix&                      z_surface,
   const Index&                       cloudbox_on,
   const ArrayOfIndex&                cloudbox_limits,
   const Tensor4&                     pnd_field,
   const Index&                       use_mean_scat_data,
   const ArrayOfSingleScatteringData& scat_data_raw,
   const Matrix&                      particle_masses,
   const ArrayOfString&               iy_aux_vars,
   const Index&                       jacobian_do,
   const Agenda&                      ppath_agenda,
   const Agenda&                      ppath_step_agenda,
   const Agenda&                      propmat_clearsky_agenda,
   const Agenda&                      iy_transmitter_agenda,
   const Index&                       iy_agenda_call1,
   const Tensor3&                     iy_transmission,
   const Vector&                      rte_pos,      
   const Vector&                      rte_pos2,      
   const Numeric&                     rte_alonglos_v,      
   const Numeric&                     ppath_lraytrace,
   const Index&                       defocus_method,
   const Numeric&                     defocus_shift,
   const Verbosity&                   verbosity )
{
  // Throw error if unsupported features are requested
  if( !iy_agenda_call1 )
    throw runtime_error( 
                  "Recursive usage not possible (iy_agenda_call1 must be 1)" );
  if( iy_transmission.ncols() )
    throw runtime_error( "*iy_transmission* must be empty" );
  if( jacobian_do )
    throw runtime_error( "This method does not provide any jacobians and "
                         "*jacobian_do* must be 0." );
  if( defocus_method < 1 || defocus_method > 2 )
    throw runtime_error( "Allowed choices for *defocus_method* is 1 and 2." );
  diy_dx.resize(0);


  //- Determine propagation path
  ppath_agendaExecute( ws, ppath, ppath_lraytrace, rte_pos, Vector(0), 
                       rte_pos2, cloudbox_on, 0, t_field, z_field, vmr_field, 
                       edensity_field, f_grid, ppath_agenda );

  //- Check ppath, and set np to zero if ground intersection
  const Index radback = ppath_what_background(ppath);
  if( !( radback == 0  ||  radback == 2  ||  radback == 9 ) )
    { throw runtime_error( "Radiative background not set to \"transmitter\", "
                           "\"surface\" or \"unvalid\" by *ppath_agenda*.\n"
                           "Is correct WSM used in the agenda?" ); }
  // np should already be 1 fon non-OK cases, but for extra safety ...
  if( radback == 0  || radback == 2 )
    { ppath.np = 1; }

  // Some basic sizes
  //
  const Index nf = f_grid.nelem();
  const Index ns = stokes_dim;
  const Index np = ppath.np;

  //=== iy_aux part ===========================================================
  Index auxPressure        = -1,
        auxTemperature     = -1,
        auxAbsSum          = -1,
        auxPartExt         = -1,
        auxImpactParam     = -1,
        auxFreeSpaceLoss   = -1,
        auxFreeSpaceAtte   = -1,
        auxAtmosphericLoss = -1,
        auxDefocusingLoss  = -1,
        auxFarRotTotal     = -1,
        auxFarRotSpeed     = -1,
        auxExtraPathDelay  = -1,
        auxBendingAngle    = -1;
  ArrayOfIndex auxAbsSpecies(0), auxAbsIsp(0);
  ArrayOfIndex auxVmrSpecies(0), auxVmrIsp(0);
  ArrayOfIndex auxPartCont(0), auxPartContI(0);
  ArrayOfIndex auxPartField(0), auxPartFieldI(0);
  //
  const Index naux = iy_aux_vars.nelem(); 
  iy_aux.resize( naux );
  //
  for( Index i=0; i<naux; i++ )
    {
      if( iy_aux_vars[i] == "Pressure" )
        { auxPressure = i;      iy_aux[i].resize( 1, 1, 1, np ); }
      else if( iy_aux_vars[i] == "Temperature" )
        { auxTemperature = i;   iy_aux[i].resize( 1, 1, 1, np ); }
      else if( iy_aux_vars[i].substr(0,13) == "VMR, species " )
        { 
          Index ispecies;
          istringstream is(iy_aux_vars[i].substr(13,2));
          is >> ispecies;
          if( ispecies < 0  ||  ispecies>=abs_species.nelem() )
            {
              ostringstream os;
              os << "You have selected VMR of species with index "
                 << ispecies << ".\nThis species does not exist!";
              throw runtime_error( os.str() );
            }
          auxVmrSpecies.push_back(i);
          auxVmrIsp.push_back(ispecies);
          iy_aux[i].resize( 1, 1, 1, np );               
        }
      else if( iy_aux_vars[i] == "Absorption, summed" )
        { auxAbsSum = i;   iy_aux[i].resize( nf, ns, ns, np ); }
      else if( iy_aux_vars[i] == "Particle extinction, summed" )
        { 
          auxPartExt = i;   
          iy_aux[i].resize( nf, ns, ns, np ); 
          iy_aux[i] = 0;
        }
      else if( iy_aux_vars[i].substr(0,20) == "Absorption, species " )
        { 
          Index ispecies;
          istringstream is(iy_aux_vars[i].substr(20,2));
          is >> ispecies;
          if( ispecies < 0  ||  ispecies>=abs_species.nelem() )
            {
              ostringstream os;
              os << "You have selected absorption species with index "
                 << ispecies << ".\nThis species does not exist!";
              throw runtime_error( os.str() );
            }
          auxAbsSpecies.push_back(i);
          auxAbsIsp.push_back(ispecies);
          iy_aux[i].resize( nf, ns, ns, np );               
        }
      else if( iy_aux_vars[i].substr(0,14) == "Mass content, " )
        { 
          Index icont;
          istringstream is(iy_aux_vars[i].substr(14,2));
          is >> icont;
          if( icont < 0  ||  icont>=particle_masses.ncols() )
            {
              ostringstream os;
              os << "You have selected particle mass content category with "
                 << "index " << icont << ".\nThis category is not defined!";
              throw runtime_error( os.str() );
            }
          auxPartCont.push_back(i);
          auxPartContI.push_back(icont);
          iy_aux[i].resize( 1, 1, 1, np );
        }
      else if( iy_aux_vars[i].substr(0,10) == "PND, type " )
        { 
          Index ip;
          istringstream is(iy_aux_vars[i].substr(10,2));
          is >> ip;
          if( ip < 0  ||  ip>=pnd_field.nbooks() )
            {
              ostringstream os;
              os << "You have selected particle number density field with "
                 << "index " << ip << ".\nThis field is not defined!";
              throw runtime_error( os.str() );
            }
          auxPartField.push_back(i);
          auxPartFieldI.push_back(ip);
          iy_aux[i].resize( 1, 1, 1, np );
        }
      else if( iy_aux_vars[i] == "Impact parameter" )
        { auxImpactParam = i;       iy_aux[i].resize( 1, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Free space loss" )
        { auxFreeSpaceLoss = i;     iy_aux[i].resize( 1, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Free space attenuation" )
        { auxFreeSpaceAtte = i;     iy_aux[i].resize( 1, 1, 1, np ); }
      else if( iy_aux_vars[i] == "Atmospheric loss" )
        { auxAtmosphericLoss = i;   iy_aux[i].resize( nf, 1, 1, 1 ); } 
      else if( iy_aux_vars[i] == "Defocusing loss" )
        { auxDefocusingLoss = i;    iy_aux[i].resize( 1, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Faraday rotation" )
        { auxFarRotTotal = i; iy_aux[i].resize( nf, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Faraday speed" )
        { auxFarRotSpeed = i; iy_aux[i].resize( nf, 1, 1, np ); }
      else if( iy_aux_vars[i] == "Extra path delay" )
        { auxExtraPathDelay = i;    iy_aux[i].resize( 1, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Bending angle" )
        { auxBendingAngle = i;      iy_aux[i].resize( 1, 1, 1, 1 ); } 
      else
        {
          ostringstream os;
          os << "In *iy_aux_vars* you have included: \"" << iy_aux_vars[i]
             << "\"\nThis choice is not recognised.";
          throw runtime_error( os.str() );
        }
    }
  //===========================================================================


  // Handle cases whn no link was establsihed
  if( radback == 0  || radback == 2 )
    {
      Numeric fillvalue = 0;
      if( radback == 0 )
        { fillvalue = NAN; }
      //
      iy.resize( nf, stokes_dim );
      iy = fillvalue;
      //
      for( Index i=0; i<naux; i++ )
        { iy_aux[i] = fillvalue; }
      //
      return;
    }


  // Transmitted signal
  //
  iy_transmitter_agendaExecute( ws, iy, f_grid, 
                                ppath.pos(np-1,Range(0,atmosphere_dim)),
                                ppath.los(np-1,joker), iy_transmitter_agenda );
  if( iy.ncols() != stokes_dim  ||  iy.nrows() != nf )
    { throw runtime_error( "The size of *iy* returned from "
                                 "*iy_transmitter_agenda* is not correct." ); }


  // Get atmospheric and attenuation quantities for each ppath point/step
  //
  Vector       ppath_p, ppath_t, ppath_ne;
  Matrix       ppath_vmr, ppath_pnd, ppath_mag, ppath_wind, ppath_f;
  Tensor5      ppath_abs;
  Tensor4      trans_partial, trans_cumulat, pnd_ext_mat;
  Vector       scalar_tau, farrot_c1;
  Numeric      farrot_c2;
  ArrayOfIndex clear2cloudbox;
  //
  if( np > 1 )
    {
      get_ppath_atmvars( ppath_p, ppath_t, ppath_vmr, ppath_wind, ppath_mag, 
                         ppath_ne, ppath, atmosphere_dim, p_grid, t_field, 
                         vmr_field, wind_u_field, wind_v_field, wind_w_field,
                         mag_u_field, mag_v_field, mag_w_field,
                         edensity_field);      
      get_ppath_f(       ppath_f, ppath, f_grid,  atmosphere_dim, 
                         rte_alonglos_v, ppath_wind );
      get_ppath_abs(     ws, ppath_abs, propmat_clearsky_agenda, ppath, 
                         ppath_p, ppath_t, ppath_vmr, ppath_f, 
                         ppath_mag, f_grid, stokes_dim );
      if( !cloudbox_on )
        { 
          get_ppath_trans(  trans_partial, trans_cumulat, scalar_tau, farrot_c1,
                            farrot_c2, ppath, ppath_abs, ppath_mag, 
                            ppath_ne, atmosphere_dim, f_grid, stokes_dim );
        }
      else
        {
          Array<ArrayOfSingleScatteringData> scat_data;
          Tensor3 pnd_abs_vec;
          //
          get_ppath_ext(    clear2cloudbox, pnd_abs_vec, pnd_ext_mat, 
                            scat_data, ppath_pnd, ppath, ppath_t, stokes_dim, 
                            ppath_f, atmosphere_dim, cloudbox_limits, pnd_field,
                            use_mean_scat_data, scat_data_raw, verbosity );
          get_ppath_trans2( trans_partial, trans_cumulat, scalar_tau, farrot_c1,
                            farrot_c2, ppath, ppath_abs, ppath_mag, ppath_ne, 
                            atmosphere_dim, f_grid, stokes_dim, clear2cloudbox,
                            pnd_ext_mat );
        }
    }

  // Ppath length variables
  //
  Numeric lbg;  // Bent geometrical length of ray path
  Numeric lba;  // Bent apparent length of ray path
  //
  lbg = ppath.end_lstep;
  lba = lbg;

  // Do RT calculations
  //
  if( np > 1 )
    {
      //=== iy_aux part =======================================================
      // iy_aux for point np-1:
      // Pressure
      if( auxPressure >= 0 ) 
        { iy_aux[auxPressure](0,0,0,np-1) = ppath_p[np-1]; }
      // Temperature
      if( auxTemperature >= 0 ) 
        { iy_aux[auxTemperature](0,0,0,np-1) = ppath_t[np-1]; }
      // VMR
      for( Index j=0; j<auxVmrSpecies.nelem(); j++ )
        { iy_aux[auxVmrSpecies[j]](0,0,0,np-1) = ppath_vmr(auxVmrIsp[j],np-1);}
      // Absorption
      if( auxAbsSum >= 0 ) 
        { for( Index iv=0; iv<nf; iv++ ) {
            for( Index is1=0; is1<ns; is1++ ){
              for( Index is2=0; is2<ns; is2++ ){
                iy_aux[auxAbsSum](iv,is1,is2,np-1) = 
                                ppath_abs(joker,iv,is1,is2,np-1).sum(); } } } } 
      for( Index j=0; j<auxAbsSpecies.nelem(); j++ )
        { for( Index iv=0; iv<nf; iv++ ) {
            for( Index is1=0; is1<stokes_dim; is1++ ){
              for( Index is2=0; is2<stokes_dim; is2++ ){
                iy_aux[auxAbsSpecies[j]](iv,is1,is2,np-1) = 
                               ppath_abs(auxAbsIsp[j],iv,is1,is2,np-1); } } } }
      // Particle properties
      if( cloudbox_on  )
        {
          // Extinction
          if( auxPartExt >= 0  && clear2cloudbox[np-1] >= 0 ) 
            { 
              const Index ic = clear2cloudbox[np-1];
              for( Index iv=0; iv<nf; iv++ ) {
                for( Index is1=0; is1<ns; is1++ ){
                  for( Index is2=0; is2<ns; is2++ ){
                    iy_aux[auxPartExt](iv,is1,is2,np-1) = 
                                              pnd_ext_mat(iv,is1,is2,ic); } } } 
            } 
          // Particle mass content
          for( Index j=0; j<auxPartCont.nelem(); j++ )
            { iy_aux[auxPartCont[j]](0,0,0,np-1) = ppath_pnd(joker,np-1) *
                                      particle_masses(joker,auxPartContI[j]); }
          // Particle field
          for( Index j=0; j<auxPartField.nelem(); j++ )
            { iy_aux[auxPartField[j]](0,0,0,np-1) = 
                                            ppath_pnd(auxPartFieldI[j],np-1); }
        }
      // Free space
      if( auxFreeSpaceAtte >= 0 )
        { iy_aux[auxFreeSpaceAtte](joker,0,0,np-1) = 2/lbg; }
      // Faraday speed
      if( auxFarRotSpeed >= 0 )
        { for( Index iv=0; iv<nf; iv++ ) {
            iy_aux[auxFarRotSpeed](iv,0,0,np-1) = RAD2DEG*farrot_c1[np-1] / 
                                                   (f_grid[iv]*f_grid[iv]); } }
      //=======================================================================

      // Loop ppath steps
      for( Index ip=np-2; ip>=0; ip-- )
        {
          // Lengths
          lbg += ppath.lstep[ip];
          lba += ppath.lstep[ip] * (ppath.ngroup[ip]+ppath.ngroup[ip+1]) / 2.0;

          // Atmospheric loss of path step + Faraday rotation
          if( stokes_dim == 1 )
            {
              for( Index iv=0; iv<nf; iv++ )  
                { iy(iv,0) = iy(iv,0) * trans_partial(iv,0,0,ip); }
            }
          else
            {
              for( Index iv=0; iv<nf; iv++ )  
                {
                  // Unpolarised:
                  if( is_diagonal( trans_partial(iv,joker,joker,ip) ) )
                    {
                      for( Index is=0; is<ns; is++ )
                        { iy(iv,is) = iy(iv,is) * trans_partial(iv,is,is,ip); }
                    }
                  // The general case:
                  else
                    {
                      Vector t1(ns);
                      mult( t1, trans_partial(iv,joker,joker,ip), iy(iv,joker));
                      iy(iv,joker) = t1;
                    }
                }
            }

          //=== iy_aux part ===================================================
          // Pressure
          if( auxPressure >= 0 ) 
            { iy_aux[auxPressure](0,0,0,ip) = ppath_p[ip]; }
          // Temperature
          if( auxTemperature >= 0 ) 
            { iy_aux[auxTemperature](0,0,0,ip) = ppath_t[ip]; }
          // VMR
          for( Index j=0; j<auxVmrSpecies.nelem(); j++ )
            { iy_aux[auxVmrSpecies[j]](0,0,0,ip) =  ppath_vmr(auxVmrIsp[j],ip);}
          // Absorption
          if( auxAbsSum >= 0 ) 
            { for( Index iv=0; iv<nf; iv++ ) {
                for( Index is1=0; is1<ns; is1++ ){
                  for( Index is2=0; is2<ns; is2++ ){
                    iy_aux[auxAbsSum](iv,is1,is2,ip) = 
                                  ppath_abs(joker,iv,is1,is2,ip).sum(); } } } } 
          for( Index j=0; j<auxAbsSpecies.nelem(); j++ )
            { for( Index iv=0; iv<nf; iv++ ) {
                for( Index is1=0; is1<stokes_dim; is1++ ){
                  for( Index is2=0; is2<stokes_dim; is2++ ){
                    iy_aux[auxAbsSpecies[j]](iv,is1,is2,ip) = 
                                 ppath_abs(auxAbsIsp[j],iv,is1,is2,ip); } } } }
          // Particle properties
          if( cloudbox_on ) 
            {
              // Extinction
              if( auxPartExt >= 0  &&  clear2cloudbox[ip] >= 0 ) 
                { 
                  const Index ic = clear2cloudbox[ip];
                  for( Index iv=0; iv<nf; iv++ ) {
                    for( Index is1=0; is1<ns; is1++ ){
                      for( Index is2=0; is2<ns; is2++ ){
                        iy_aux[auxPartExt](iv,is1,is2,ip) = 
                                              pnd_ext_mat(iv,is1,is2,ic); } } }
                }
              // Particle mass content
              for( Index j=0; j<auxPartCont.nelem(); j++ )
                { iy_aux[auxPartCont[j]](0,0,0,ip) = ppath_pnd(joker,ip) *
                                      particle_masses(joker,auxPartContI[j]); }
              // Particle field
              for( Index j=0; j<auxPartField.nelem(); j++ )
                { iy_aux[auxPartField[j]](0,0,0,ip) = 
                                              ppath_pnd(auxPartFieldI[j],ip); }
            }
          // Free space loss
          if( auxFreeSpaceAtte >= 0 )
            { iy_aux[auxFreeSpaceAtte](joker,0,0,ip) = 2/lbg; }
          // Faraday speed
          if( auxFarRotSpeed >= 0 )
            { for( Index iv=0; iv<nf; iv++ ) {
                iy_aux[auxFarRotSpeed](iv,0,0,ip) = RAD2DEG*farrot_c1[ip] / 
                                                   (f_grid[iv]*f_grid[iv]); } }
          //===================================================================
        }


      //=== iy_aux part =======================================================
      if( auxAtmosphericLoss >= 0 )
        { iy_aux[auxAtmosphericLoss](joker,0,0,0) = iy(joker,0); }      
      if( auxFarRotTotal >= 0 )
        { for( Index iv=0; iv<nf; iv++ ) {
            iy_aux[auxFarRotTotal](iv,0,0,0) = RAD2DEG*farrot_c2 / 
                                                   (f_grid[iv]*f_grid[iv]); } }
      if( auxImpactParam >= 0 )
        { 
          assert( ppath.constant >= 0 );
          iy_aux[auxImpactParam](joker,0,0,0) = ppath.constant; 
        }
      //=======================================================================


      // Remaing length of ppath
      lbg += ppath.start_lstep;
      lba += ppath.start_lstep;


      // Determine total free space loss
      Numeric fspl = 1 / ( 4 * PI * lbg*lbg ); 
      //
      if( auxFreeSpaceLoss >= 0 )
        { iy_aux[auxFreeSpaceLoss] = fspl; }


      // Determine defocusing loss
      Numeric dfl = 1;
      if( defocus_method == 1 )
        {
          defocusing_general( ws, dfl, ppath_step_agenda, atmosphere_dim, 
                              p_grid, lat_grid, lon_grid, t_field, z_field, 
                              vmr_field, edensity_field, -1, refellipsoid, 
                              z_surface, ppath, ppath_lraytrace,
                              defocus_shift, verbosity );
        }
      else if( defocus_method == 2 )
        { defocusing_sat2sat( ws, dfl, ppath_step_agenda, atmosphere_dim, 
                              p_grid, lat_grid, lon_grid, t_field, z_field, 
                              vmr_field, edensity_field, -1, refellipsoid, 
                              z_surface, ppath, ppath_lraytrace, 
                              defocus_shift, verbosity ); 
        }
      if( auxDefocusingLoss >= 0 )
        { iy_aux[auxDefocusingLoss] = dfl; }



      // Include free space and defocusing losses
      iy *= fspl*dfl;


      // Extra path delay
      if( auxExtraPathDelay >= 0 )
        {
          // Radius of rte_pos and rte_pos2
          const Numeric r1 = ppath.end_pos[0] +
                             pos2refell_r( atmosphere_dim, refellipsoid, 
                                           lat_grid, lon_grid, ppath.end_pos );
          const Numeric r2 = ppath.start_pos[0] +
                             pos2refell_r( atmosphere_dim, refellipsoid, 
                                         lat_grid, lon_grid, ppath.start_pos );

          // Geometrical distance between start and end point
          Numeric lgd ;
          if( atmosphere_dim <= 2 )
            { distance2D( lgd, r1, ppath.end_pos[1], r2, ppath.start_pos[1] ); }
          else 
            { distance3D( lgd, r1, ppath.end_pos[1],   ppath.end_pos[2],
                               r2, ppath.start_pos[1], ppath.start_pos[2] ); }
          //
          iy_aux[auxExtraPathDelay] = ( lba - lgd ) / SPEED_OF_LIGHT;
        }


      // Bending angle
      if( auxBendingAngle >= 0 )
        { 
          Numeric ba = -999;
          bending_angle1d( ba, ppath ); 
          //
          iy_aux[auxBendingAngle] = ba;
        }
    }
}





/* Workspace method: Doxygen documentation will be auto-generated */
void iyTransmissionStandard(
         Workspace&                   ws,
         Matrix&                      iy,
         ArrayOfTensor4&              iy_aux,
         Ppath&                       ppath,
         ArrayOfTensor3&              diy_dx,
   const Index&                       stokes_dim,
   const Vector&                      f_grid,
   const Index&                       atmosphere_dim,
   const Vector&                      p_grid,
   const Tensor3&                     z_field,
   const Tensor3&                     t_field,
   const Tensor4&                     vmr_field,
   const ArrayOfArrayOfSpeciesTag&    abs_species,
   const Tensor3&                     wind_u_field,
   const Tensor3&                     wind_v_field,
   const Tensor3&                     wind_w_field,
   const Tensor3&                     mag_u_field,
   const Tensor3&                     mag_v_field,
   const Tensor3&                     mag_w_field,
   const Tensor3&                     edensity_field,
   const Index&                       cloudbox_on,
   const ArrayOfIndex&                cloudbox_limits,
   const Tensor4&                     pnd_field,
   const Index&                       use_mean_scat_data,
   const ArrayOfSingleScatteringData& scat_data_raw,
   const Matrix&                      particle_masses,
   const ArrayOfString&               iy_aux_vars,
   const Index&                       jacobian_do,
   const ArrayOfRetrievalQuantity&    jacobian_quantities,
   const ArrayOfArrayOfIndex&         jacobian_indices,
   const Agenda&                      ppath_agenda,
   const Agenda&                      propmat_clearsky_agenda,
   const Agenda&                      iy_transmitter_agenda,
   const Index&                       iy_agenda_call1,
   const Tensor3&                     iy_transmission,
   const Vector&                      rte_pos,      
   const Vector&                      rte_los,      
   const Vector&                      rte_pos2,
   const Numeric&                     rte_alonglos_v,      
   const Numeric&                     ppath_lraytrace,      
   const Verbosity&                   verbosity )
{
  // Throw error if unsupported features are requested
  if( !iy_agenda_call1 )
    throw runtime_error( 
                  "Recursive usage not possible (iy_agenda_call1 must be 1)" );
  if( iy_transmission.ncols() )
    throw runtime_error( "*iy_transmission* must be empty" );


  // Determine propagation path
  //
  ppath_agendaExecute( ws, ppath, ppath_lraytrace, rte_pos, rte_los, rte_pos2, 
                       0, 0, t_field, z_field, vmr_field, edensity_field, 
                       f_grid, ppath_agenda );

  // Some basic sizes
  //
  const Index nf = f_grid.nelem();
  const Index ns = stokes_dim;
  const Index np = ppath.np;
  const Index nq = jacobian_quantities.nelem();

  // Transmitted signal
  //
  iy_transmitter_agendaExecute( ws, iy, f_grid, 
                                ppath.pos(np-1,Range(0,atmosphere_dim)),
                                ppath.los(np-1,joker), iy_transmitter_agenda );
  if( iy.ncols() != stokes_dim  ||  iy.nrows() != nf )
    {
      ostringstream os;
      os << "The size of *iy* returned from *iy_transmitter_agdna* is\n"
         << "not correct:\n"
         << "  expected size = [" << nf << "," << stokes_dim << "]\n"
         << "  size of iy    = [" << iy.nrows() << "," << iy.ncols()<< "]\n";
      throw runtime_error( os.str() );      
    }


  //=== iy_aux part ===========================================================
  Index auxPressure    = -1,
        auxTemperature = -1,
        auxAbsSum      = -1,
        auxPartExt     = -1,
        auxIy          = -1,
        auxTrans       = -1,
        auxOptDepth    = -1,
        auxFarRotTotal = -1,
        auxFarRotSpeed = -1;
  ArrayOfIndex auxAbsSpecies(0), auxAbsIsp(0);
  ArrayOfIndex auxVmrSpecies(0), auxVmrIsp(0);
  ArrayOfIndex auxPartCont(0), auxPartContI(0);
  ArrayOfIndex auxPartField(0), auxPartFieldI(0);
  //
  const Index naux = iy_aux_vars.nelem();
  iy_aux.resize( naux );
  //
  for( Index i=0; i<naux; i++ )
    {
      if( iy_aux_vars[i] == "Pressure" )
        { auxPressure = i;      iy_aux[i].resize( 1, 1, 1, np ); }
      else if( iy_aux_vars[i] == "Temperature" )
        { auxTemperature = i;   iy_aux[i].resize( 1, 1, 1, np ); }
      else if( iy_aux_vars[i].substr(0,13) == "VMR, species " )
        { 
          Index ispecies;
          istringstream is(iy_aux_vars[i].substr(13,2));
          is >> ispecies;
          if( ispecies < 0  ||  ispecies>=abs_species.nelem() )
            {
              ostringstream os;
              os << "You have selected VMR of species with index "
                 << ispecies << ".\nThis species does not exist!";
              throw runtime_error( os.str() );
            }
          auxVmrSpecies.push_back(i);
          auxVmrIsp.push_back(ispecies);
          iy_aux[i].resize( 1, 1, 1, np );               
        }
      else if( iy_aux_vars[i] == "Absorption, summed" )
        { auxAbsSum = i;   iy_aux[i].resize( nf, ns, ns, np ); }
      else if( iy_aux_vars[i] == "Particle extinction, summed" )
        { 
          auxPartExt = i;   
          iy_aux[i].resize( nf, ns, ns, np ); 
          iy_aux[i] = 0;
        }
      else if( iy_aux_vars[i].substr(0,20) == "Absorption, species " )
        { 
          Index ispecies;
          istringstream is(iy_aux_vars[i].substr(20,2));
          is >> ispecies;
          if( ispecies < 0  ||  ispecies>=abs_species.nelem() )
            {
              ostringstream os;
              os << "You have selected absorption species with index "
                 << ispecies << ".\nThis species does not exist!";
              throw runtime_error( os.str() );
            }
          auxAbsSpecies.push_back(i);
          auxAbsIsp.push_back(ispecies);
          iy_aux[i].resize( nf, ns, ns, np );               
        }
      else if( iy_aux_vars[i].substr(0,14) == "Mass content, " )
        { 
          Index icont;
          istringstream is(iy_aux_vars[i].substr(14,2));
          is >> icont;
          if( icont < 0  ||  icont>=particle_masses.ncols() )
            {
              ostringstream os;
              os << "You have selected particle mass content category with "
                 << "index " << icont << ".\nThis category is not defined!";
              throw runtime_error( os.str() );
            }
          auxPartCont.push_back(i);
          auxPartContI.push_back(icont);
          iy_aux[i].resize( 1, 1, 1, np );
        }
      else if( iy_aux_vars[i].substr(0,10) == "PND, type " )
        { 
          Index ip;
          istringstream is(iy_aux_vars[i].substr(10,2));
          is >> ip;
          if( ip < 0  ||  ip>=pnd_field.nbooks() )
            {
              ostringstream os;
              os << "You have selected particle number density field with "
                 << "index " << ip << ".\nThis field is not defined!";
              throw runtime_error( os.str() );
            }
          auxPartField.push_back(i);
          auxPartFieldI.push_back(ip);
          iy_aux[i].resize( 1, 1, 1, np );
        }
      else if( iy_aux_vars[i] == "iy"   &&  auxIy < 0 )
        { auxIy = i;           iy_aux[i].resize( nf, ns, 1, np ); }
      else if( iy_aux_vars[i] == "Transmission"   &&  auxTrans < 0 )
        { auxTrans = i;        iy_aux[i].resize( nf, ns, ns, np ); }
      else if( iy_aux_vars[i] == "Optical depth" )
        { auxOptDepth = i;     iy_aux[i].resize( nf, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Faraday rotation" )
        { auxFarRotTotal = i; iy_aux[i].resize( nf, 1, 1, 1 ); }
      else if( iy_aux_vars[i] == "Faraday speed" )
        { auxFarRotSpeed = i; iy_aux[i].resize( nf, 1, 1, np ); }
      else
        {
          ostringstream os;
          os << "In *iy_aux_vars* you have included: \"" << iy_aux_vars[i]
             << "\"\nThis choice is not recognised.";
          throw runtime_error( os.str() );
        }
    }
  //===========================================================================


  //### jacobian part #########################################################
  // Initialise analytical jacobians (diy_dx)
  //
  Index j_analytical_do = 0;
  //
  if( jacobian_do ) { FOR_ANALYTICAL_JACOBIANS_DO( j_analytical_do = 1; ) }
  //
  if( j_analytical_do )
    {
      if( cloudbox_on )
        throw runtime_error( "The combination of active ckloudbox and "
                             "analytical jacibians is not yet handled." );
      diy_dx.resize( jacobian_indices.nelem() ); 
      //
      FOR_ANALYTICAL_JACOBIANS_DO( 
        diy_dx[iq].resize( jacobian_indices[iq][1] - jacobian_indices[iq][0] + 
                           1, nf, stokes_dim ); 
        diy_dx[iq] = 0.0;
      ) 
    }
  //###########################################################################


  // Get atmospheric and RT quantities for each ppath point/step
  //
  Vector       ppath_p, ppath_t;
  Matrix       ppath_vmr, ppath_pnd, ppath_wind, ppath_mag, ppath_f;
  Tensor5      ppath_abs;
  Tensor4      trans_partial, trans_cumulat, pnd_ext_mat;
  Vector       scalar_tau, farrot_c1;
  Numeric      farrot_c2;
  ArrayOfIndex clear2cloudbox;
  //
  if( np > 1 )
    {
      Vector ppath_ne;
      get_ppath_atmvars( ppath_p, ppath_t, ppath_vmr, ppath_wind, ppath_mag, 
                         ppath_ne, ppath, atmosphere_dim, p_grid, t_field, 
                         vmr_field, wind_u_field, wind_v_field, wind_w_field,
                         mag_u_field, mag_v_field, mag_w_field,
                         edensity_field );      
      get_ppath_f(       ppath_f, ppath, f_grid,  atmosphere_dim, 
                         rte_alonglos_v, ppath_wind );
      get_ppath_abs(     ws, ppath_abs, propmat_clearsky_agenda, ppath, 
                         ppath_p, ppath_t, ppath_vmr, ppath_f, 
                         ppath_mag, f_grid, stokes_dim );
      if( !cloudbox_on )
        { 
          get_ppath_trans( trans_partial, trans_cumulat, scalar_tau, farrot_c1,
                           farrot_c2, ppath, ppath_abs, ppath_mag, 
                           ppath_ne, atmosphere_dim, f_grid, stokes_dim );
        }
      else
        {
          Array<ArrayOfSingleScatteringData> scat_data;
          Tensor3 pnd_abs_vec;
          //
          get_ppath_ext(    clear2cloudbox, pnd_abs_vec, pnd_ext_mat, scat_data,
                            ppath_pnd, ppath, ppath_t, stokes_dim, ppath_f, 
                            atmosphere_dim, cloudbox_limits, pnd_field, 
                            use_mean_scat_data, scat_data_raw, verbosity );
          get_ppath_trans2( trans_partial, trans_cumulat, scalar_tau, farrot_c1,
                            farrot_c2, ppath, ppath_abs, ppath_mag, ppath_ne, 
                            atmosphere_dim, f_grid, stokes_dim, clear2cloudbox,
                            pnd_ext_mat );
        }
    }


  //=== iy_aux part ===========================================================
  // Fill parts of iy_aux that are defined even for np=1.
  // Radiance 
  if( auxIy >= 0 ) 
    { iy_aux[auxIy](joker,joker,0,np-1) = iy; }
  if( auxOptDepth >= 0 ) 
    {
      if( np == 1 )
        { iy_aux[auxOptDepth] = 0; }
      else
        { iy_aux[auxOptDepth](joker,0,0,0) = scalar_tau; }
    } 
  if( auxTrans >= 0 ) // Complete tensor filled!
    { 
      if( np == 1 )
        { for( Index iv=0; iv<nf; iv++ ) {
            id_mat( iy_aux[auxTrans](iv,joker,joker,0) ); } }
      else
        { iy_aux[auxTrans] = trans_cumulat; }
    }
  // Faraday rotation, total
  if( auxFarRotTotal >= 0 )
    { for( Index iv=0; iv<nf; iv++ ) {
        iy_aux[auxFarRotTotal](iv,0,0,0) = RAD2DEG*farrot_c2 / 
                                                   (f_grid[iv]*f_grid[iv]); } }
  //===========================================================================


  // Do RT calculations
  //
  if( np > 1 )
    {
      //### jacobian part #####################################################
      // Create container for the derivatives with respect to changes
      // at each ppath point. And find matching abs_species-index and 
      // "temperature flag" (for analytical jacobians).
      //
      ArrayOfTensor3  diy_dpath; 
      ArrayOfIndex    abs_species_i, is_t, wind_i; 
      //
      const Numeric   dt = 0.1;
      const Numeric   dw = 5;
            Tensor5   ppath_at2, ppath_awu, ppath_awv, ppath_aww;
      //
      if( j_analytical_do )
        { 
          // So far no polarised absorption handled for jacobians
          for( Index iv=0; iv<nf; iv++ ) {
            if( !is_diagonal( trans_cumulat(iv,joker,joker,np-1) ) )
                    throw runtime_error( "The combination of polarised "
                           "absorption and jacobians is not yet handled." ); }
          //------------------------------------------------------------------
          diy_dpath.resize( nq ); 
          abs_species_i.resize( nq ); 
          is_t.resize( nq ); 
          wind_i.resize( nq ); 
          //
          FOR_ANALYTICAL_JACOBIANS_DO( 
            diy_dpath[iq].resize( np, nf, stokes_dim ); 
            diy_dpath[iq] = 0.0;
          )
            get_pointers_for_analytical_jacobians( abs_species_i, is_t, wind_i,
                                            jacobian_quantities, abs_species );
          //
          // Determine if temperature is among the analytical jac. quantities.
          // If yes, get absorption for disturbed temperature
          // And the same winds
          for( Index iq=0; iq<is_t.nelem(); iq++ )
            {
              if( is_t[iq] ) 
                { 
                  Vector t2 = ppath_t;   t2 += dt;
                  get_ppath_abs( ws, ppath_at2, propmat_clearsky_agenda, 
                                 ppath, ppath_p, t2, ppath_vmr, ppath_f,
                                 ppath_mag, f_grid, stokes_dim );
                }
              else if( wind_i[iq] )
                {
                  if( wind_i[iq] == 1 )
                    {
                      Matrix f2, w2 = ppath_wind;   w2(0,joker) += dw;
                      get_ppath_f(   f2, ppath, f_grid,  atmosphere_dim, 
                                     rte_alonglos_v, w2 );
                      get_ppath_abs( ws, ppath_awu, propmat_clearsky_agenda,
                                     ppath, ppath_p, ppath_t, ppath_vmr, 
                                     f2, ppath_mag, f_grid, stokes_dim );
                    }
                  else if( wind_i[iq] == 2 )
                    {
                      Matrix f2, w2 = ppath_wind;   w2(1,joker) += dw;
                      get_ppath_f(   f2, ppath, f_grid,  atmosphere_dim, 
                                     rte_alonglos_v, w2 );
                      get_ppath_abs( ws, ppath_awv, propmat_clearsky_agenda,
                                     ppath, ppath_p, ppath_t, ppath_vmr, 
                                     f2, ppath_mag, f_grid, stokes_dim );
                    }
                  else if( wind_i[iq] == 3 )
                    {
                      Matrix f2, w2 = ppath_wind;   w2(0,joker) += dw;
                      get_ppath_f(   f2, ppath, f_grid,  atmosphere_dim, 
                                     rte_alonglos_v, w2 );
                      get_ppath_abs( ws, ppath_aww, propmat_clearsky_agenda,
                                     ppath, ppath_p, ppath_t, ppath_vmr, 
                                     f2, ppath_mag, f_grid, stokes_dim );
                    }
                }
            }
        }
      //#######################################################################

      //=== iy_aux part =======================================================
      // iy_aux for point np-1:
      // Pressure
      if( auxPressure >= 0 ) 
        { iy_aux[auxPressure](0,0,0,np-1) = ppath_p[np-1]; }
      // Temperature
      if( auxTemperature >= 0 ) 
        { iy_aux[auxTemperature](0,0,0,np-1) = ppath_t[np-1]; }
      // VMR
      for( Index j=0; j<auxVmrSpecies.nelem(); j++ )
        { iy_aux[auxVmrSpecies[j]](0,0,0,np-1) = ppath_vmr(auxVmrIsp[j],np-1); }
      // Absorption
      if( auxAbsSum >= 0 ) 
        { for( Index iv=0; iv<nf; iv++ ) {
            for( Index is1=0; is1<ns; is1++ ){
              for( Index is2=0; is2<ns; is2++ ){
                iy_aux[auxAbsSum](iv,is1,is2,np-1) = 
                                ppath_abs(joker,iv,is1,is2,np-1).sum(); } } } } 
      for( Index j=0; j<auxAbsSpecies.nelem(); j++ )
        { for( Index iv=0; iv<nf; iv++ ) {
            for( Index is1=0; is1<stokes_dim; is1++ ){
              for( Index is2=0; is2<stokes_dim; is2++ ){
                iy_aux[auxAbsSpecies[j]](iv,is1,is2,np-1) = 
                               ppath_abs(auxAbsIsp[j],iv,is1,is2,np-1); } } } }
      // Particle properties
      if( cloudbox_on  )
        {
          // Extinction
          if( auxPartExt >= 0  && clear2cloudbox[np-1] >= 0 ) 
            { 
              const Index ic = clear2cloudbox[np-1];
              for( Index iv=0; iv<nf; iv++ ) {
                for( Index is1=0; is1<ns; is1++ ){
                  for( Index is2=0; is2<ns; is2++ ){
                    iy_aux[auxPartExt](iv,is1,is2,np-1) = 
                                              pnd_ext_mat(iv,is1,is2,ic); } } } 
            } 
          // Particle mass content
          for( Index j=0; j<auxPartCont.nelem(); j++ )
            { iy_aux[auxPartCont[j]](0,0,0,np-1) = ppath_pnd(joker,np-1) *
                                      particle_masses(joker,auxPartContI[j]); }
          // Particle field
          for( Index j=0; j<auxPartField.nelem(); j++ )
            { iy_aux[auxPartField[j]](0,0,0,np-1) = 
                                            ppath_pnd(auxPartFieldI[j],np-1); }
        }
      // Radiance for this point is handled above
      // Faraday speed
      if( auxFarRotSpeed >= 0 )
        { for( Index iv=0; iv<nf; iv++ ) {
            iy_aux[auxFarRotSpeed](iv,0,0,np-1) = RAD2DEG*farrot_c1[np-1] / 
                                                   (f_grid[iv]*f_grid[iv]); } }
      //=======================================================================

      
      // Loop ppath steps
      for( Index ip=np-2; ip>=0; ip-- )
        {
          //### jacobian part #################################################
          if( j_analytical_do )
            {
              // Common terms introduced for efficiency and clarity
              Vector X(nf);   // See AUG
              //
              for( Index iv=0; iv<nf; iv++ )
                { X[iv] = 0.5 * ppath.lstep[ip] * trans_cumulat(iv,0,0,ip+1); }

              // Loop quantities
              for( Index iq=0; iq<nq; iq++ ) 
                {
                  if( jacobian_quantities[iq].Analytical() )
                    {
                      // Absorbing species
                      const Index isp = abs_species_i[iq]; 
                      if( isp >= 0 )
                        {
                          // Scaling factors to handle retrieval unit
                          // (gives the cross-section to apply)
                          Numeric unitscf1, unitscf2;
                          vmrunitscf( unitscf1, 
                                      jacobian_quantities[iq].Mode(), 
                                      ppath_vmr(isp,ip), ppath_p[ip], 
                                      ppath_t[ip] );
                          vmrunitscf( unitscf2, 
                                      jacobian_quantities[iq].Mode(), 
                                      ppath_vmr(isp,ip+1), ppath_p[ip+1], 
                                      ppath_t[ip+1] );
                          //
                          for( Index iv=0; iv<nf; iv++ )
                            {
                              // All Stokes components equal
                              for( Index is=0; is<stokes_dim; is++ )
                                { 
                                  const Numeric Z = -X[iv] * iy(iv,is);
                                  diy_dpath[iq](ip  ,iv,is) += Z * unitscf1 *
                                                    ppath_abs(isp,iv,0,0,ip);
                                  diy_dpath[iq](ip+1,iv,is) += Z * unitscf2 *
                                                    ppath_abs(isp,iv,0,0,ip+1);
                                }
                            }
                        }

                      // Temperature
                      else if( is_t[iq] )
                        {
                          for( Index iv=0; iv<nf; iv++ )
                            {
                              // The terms associated with Dtau/Dk:
                              const Numeric k1 = 
                                             ppath_abs(joker,iv,0,0,ip  ).sum();
                              const Numeric k2 = 
                                             ppath_abs(joker,iv,0,0,ip+1).sum();
                              const Numeric dkdt1 =
                               ( ppath_at2(joker,iv,0,0,ip  ).sum() - k1 ) / dt;
                              const Numeric dkdt2 =
                               ( ppath_at2(joker,iv,0,0,ip+1).sum() - k2 ) / dt;
                              for( Index is=0; is<ns; is++ )
                                { 
                                  const Numeric Z = -X[iv] * iy(iv,is);
                                  diy_dpath[iq](ip  , iv, is) += Z * dkdt1;
                                  diy_dpath[iq](ip+1, iv, is) += Z * dkdt2;
                                }
                              //
                              // The terms associated with Delta-s:
                              if( jacobian_quantities[iq].Subtag() == "HSE on" )
                                {
                                  const Numeric kbar = 0.5 * ( k1 + k2 );
                                  for( Index is=0; is<ns; is++ )
                                    { 
                                      const Numeric Z = -X[iv] * iy(iv,is);
                                      diy_dpath[iq](ip  ,iv,is) += Z * kbar /
                                                                  ppath_t[ip];
                                      diy_dpath[iq](ip+1,iv,is) += Z * kbar /
                                                                 ppath_t[ip+1];
                                    }
                                } //hse
                            }  // frequency
                        }  // if is_t

                      // Winds
                      else if( wind_i[iq] )
                        {
                          Numeric dkdx1, dkdx2; 
                          for( Index iv=0; iv<nf; iv++ )
                            {
                              const Numeric k1 = 
                                             ppath_abs(joker,iv,0,0,ip  ).sum();
                              const Numeric k2 = 
                                             ppath_abs(joker,iv,0,0,ip+1).sum();
                              // u
                              if( wind_i[iq] == 1 )
                                {
                                  dkdx1 = 
                                    (ppath_awu(joker,iv,0,0,ip  ).sum()-k1)/dw;
                                  dkdx2 =
                                    (ppath_awu(joker,iv,0,0,ip+1).sum()-k2)/dw;
                                }
                              // v
                              else if( wind_i[iq] == 2 )
                                {
                                  dkdx1 = 
                                    (ppath_awv(joker,iv,0,0,ip  ).sum()-k1)/dw;
                                  dkdx2 =
                                    (ppath_awv(joker,iv,0,0,ip+1).sum()-k2)/dw;
                                }
                              // w
                              else if( wind_i[iq] == 3 )
                                {
                                  dkdx1 = 
                                    (ppath_aww(joker,iv,0,0,ip  ).sum()-k1)/dw;
                                  dkdx2 =
                                    (ppath_aww(joker,iv,0,0,ip+1).sum()-k2)/dw;
                                }
                              else
                                { assert(0); }

                              // All Stokes components equal
                              for( Index is=0; is<stokes_dim; is++ )
                                { 
                                  const Numeric Z = -X[iv] * iy(iv,is);
                                  diy_dpath[iq](ip  ,iv,is) += Z * dkdx1;
                                  diy_dpath[iq](ip+1,iv,is) += Z * dkdx2;
                                }
                            }
                        }
                    } // if analytical
                } // for iq
            }
          //###################################################################

          // Spectrum at end of ppath step 
          if( stokes_dim == 1 )
            {
              for( Index iv=0; iv<nf; iv++ )  
                { iy(iv,0) = iy(iv,0) * trans_partial(iv,0,0,ip); }
            }
          else
            {
              for( Index iv=0; iv<nf; iv++ )  
                {
                  // Unpolarised:
                  if( is_diagonal( trans_partial(iv,joker,joker,ip) ) )
                    {
                      for( Index is=0; is<ns; is++ )
                        { iy(iv,is) = iy(iv,is) * trans_partial(iv,is,is,ip); }
                    }
                  // The general case:
                  else
                    {
                      Vector t1(ns);
                      mult( t1, trans_partial(iv,joker,joker,ip), iy(iv,joker));
                      iy(iv,joker) = t1;
                    }
                }
            }

          //=== iy_aux part ===================================================
          // Pressure
          if( auxPressure >= 0 ) 
            { iy_aux[auxPressure](0,0,0,ip) = ppath_p[ip]; }
          // Temperature
          if( auxTemperature >= 0 ) 
            { iy_aux[auxTemperature](0,0,0,ip) = ppath_t[ip]; }
          // VMR
          for( Index j=0; j<auxVmrSpecies.nelem(); j++ )
            { iy_aux[auxVmrSpecies[j]](0,0,0,ip) =  ppath_vmr(auxVmrIsp[j],ip);}
          // Absorption
          if( auxAbsSum >= 0 ) 
            { for( Index iv=0; iv<nf; iv++ ) {
                for( Index is1=0; is1<ns; is1++ ){
                  for( Index is2=0; is2<ns; is2++ ){
                    iy_aux[auxAbsSum](iv,is1,is2,ip) = 
                                  ppath_abs(joker,iv,is1,is2,ip).sum(); } } } } 
          for( Index j=0; j<auxAbsSpecies.nelem(); j++ )
            { for( Index iv=0; iv<nf; iv++ ) {
                for( Index is1=0; is1<stokes_dim; is1++ ){
                  for( Index is2=0; is2<stokes_dim; is2++ ){
                    iy_aux[auxAbsSpecies[j]](iv,is1,is2,ip) = 
                                 ppath_abs(auxAbsIsp[j],iv,is1,is2,ip); } } } }
          // Particle properties
          if( cloudbox_on ) 
            {
              // Extinction
              if( auxPartExt >= 0  &&  clear2cloudbox[ip] >= 0 ) 
                { 
                  const Index ic = clear2cloudbox[ip];
                  for( Index iv=0; iv<nf; iv++ ) {
                    for( Index is1=0; is1<ns; is1++ ){
                      for( Index is2=0; is2<ns; is2++ ){
                        iy_aux[auxPartExt](iv,is1,is2,ip) = 
                                              pnd_ext_mat(iv,is1,is2,ic); } } }
                }
              // Particle mass content
              for( Index j=0; j<auxPartCont.nelem(); j++ )
                { iy_aux[auxPartCont[j]](0,0,0,ip) = ppath_pnd(joker,ip) *
                                      particle_masses(joker,auxPartContI[j]); }
              // Particle field
              for( Index j=0; j<auxPartField.nelem(); j++ )
                { iy_aux[auxPartField[j]](0,0,0,ip) = 
                                              ppath_pnd(auxPartFieldI[j],ip); }
            }
          // Radiance 
          if( auxIy >= 0 ) 
            { iy_aux[auxIy](joker,joker,0,ip) = iy; }
          // Faraday speed
          if( auxFarRotSpeed >= 0 )
            { for( Index iv=0; iv<nf; iv++ ) {
                iy_aux[auxFarRotSpeed](iv,0,0,ip) = RAD2DEG*farrot_c1[ip] / 
                                                   (f_grid[iv]*f_grid[iv]); } }
          //===================================================================
        } 

      //### jacobian part #####################################################
      // Map jacobians from ppath to retrieval grids
      // (this operation corresponds to the term Dx_i/Dx)
      if( j_analytical_do )
        { 
          FOR_ANALYTICAL_JACOBIANS_DO( 
            diy_from_path_to_rgrids( diy_dx[iq], jacobian_quantities[iq], 
                               diy_dpath[iq], atmosphere_dim, ppath, ppath_p );
          )
        }
      //#######################################################################
    } // if np>1
}





/* Workspace method: Doxygen documentation will be auto-generated */
void iy_transmitterMultiplePol(
        Matrix&        iy,
  const Index&         stokes_dim,
  const Vector&        f_grid,
  const ArrayOfIndex&  sensor_pol,
  const Verbosity&  )
{
  const Index nf = f_grid.nelem();
  
  if( sensor_pol.nelem() != nf )
    throw runtime_error( "The length of *f_grid* and the number of elements "
                         "in *sensor_pol* must be equal." );

  iy.resize( nf, stokes_dim );
  iy = 0;

  ArrayOfVector   s2p;
  stokes2pol( s2p, 1 );

  for( Index i=0; i<nf; i++ )
    {
      for( Index j=0; j<s2p[sensor_pol[i]-1].nelem(); j++ )
        {
          iy(i,j) = s2p[sensor_pol[i]-1][j];
        }
    }
}



/* Workspace method: Doxygen documentation will be auto-generated */
void iy_transmitterSinglePol(
        Matrix&        iy,
  const Index&         stokes_dim,
  const Vector&        f_grid,
  const ArrayOfIndex&  sensor_pol,
  const Verbosity&  )
{
  const Index nf = f_grid.nelem();
  
  if( sensor_pol.nelem() != 1 )
    throw runtime_error( "The number of elements in *sensor_pol* must be 1." );

  iy.resize( nf, stokes_dim );
  iy = 0;

  ArrayOfVector   s2p;
  stokes2pol( s2p, 1 );

  for( Index j=0; j<s2p[sensor_pol[0]-1].nelem(); j++ )
    {
      iy(0,j) = s2p[sensor_pol[0]-1][j];
      for( Index i=1; i<nf; i++ )
        {
          iy(i,j) = iy(0,j);
        }
    }
}



