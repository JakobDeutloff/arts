/* Copyright (C) 2012 
Richard Larsson <ric.larsson@gmail.com>

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

/** Contains the line mixing data class definition
 * \file   linemixingdata.h
 * 
 * \author Richard Larsson
 * \date   2014-10-31
 **/

#ifndef linemixingdata_h
#define linemixingdata_h

#include <stdexcept>
#include <cmath>
#include "interpolation.h"
#include "interpolation_poly.h"
#include "matpackI.h"
#include "array.h"
#include "mystring.h"

class LineMixingData
{
public:
    
    enum LM_Type {
      LM_NONE,                          // Reserved for no line mixing
      LM_LBLRTM,                        // Reserved for LBLRTM line mixing
      LM_LBLRTM_O2NonResonant,          // Reserved for the non-resonant O2 line in LBLRTM
      LM_2NDORDER                       // Reserved for Makarov et al. 2011 second order line mixing
    };
  
    // Defining an object
    LineMixingData() : mtype(LM_NONE), mdata() {}
    
    // Use these to get the raw data from this class
    const LM_Type& Type() const {return mtype;}
    const ArrayOfVector& Data() const {return mdata;}
    
    // Use these to return data in the format required by the line shape calculator
    void GetLBLRTM(Numeric& Y, Numeric& G, const Numeric& Temperature, const Numeric& Pressure, const Numeric& Pressure_Limit, const Index& order) const; 
    void Get2ndOrder(Numeric& Y, Numeric& G, Numeric& DV, const Numeric& Temperature, const Numeric& Pressure, const Numeric& Pressure_Limit) const;
    void GetLBLRTM_O2NonResonant(Numeric& Gamma1, Numeric& Gamma2, const Numeric& Temperature, const Numeric& Pressure, const Numeric& Pressure_Limit, const Index& order) const;
    
    // Use these to insert the data in the required format from catalog readings
    void SetLBLRTMFromTheirCatalog(const Vector& t, const Vector& y, const Vector& g) 
    {
      mtype = LM_LBLRTM;
      mdata.resize(3);
      mdata[0] = t;
      mdata[1] = y;
      mdata[2] = g;
    }
    
    void SetLBLRTM_O2NonResonantFromTheirCatalog(const Vector& t, const Vector& gamma1, const Vector& gamma2) 
    {
      mtype = LM_LBLRTM_O2NonResonant;
      mdata.resize(3);
      mdata[0] = t;
      mdata[1] = gamma1;
      mdata[2] = gamma2;
    }
    
    // Use these to read data from XML-formats
    void StorageTag2SetType(const String& input);
    Index ExpectedVectorLengthFromType();
    void SetDataFromVectorWithKnownType(const Vector& input);
    
    // Use these to read data from ARTS catalog
    void Vector2LBLRTMData(const Vector& input);
    void Vector2LBLRTM_O2NonResonantData(const Vector& input);
    void Vector2NoneData(const Vector&);
    void Vector2SecondOrderData(const Vector& input);
    
    // Use these to save output vector in ARTS catalog
    void GetVectorFromData(Vector& output) const;
    void LBLRTMData2Vector(Vector& output) const;
    void LBLRTM_O2NonResonantData2Vector(Vector& output) const;
    String Type2StorageTag() const;
    void SecondOrderData2Vector(Vector& output) const;
    
private:
    // mtype identifies the type of line mixing and mdata should contain the required data
    LM_Type mtype;
    ArrayOfVector mdata;
};

#endif // linemixingdata_h
