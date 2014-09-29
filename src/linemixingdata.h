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

/** Contains the linemixingdata class definition
 * \file   linemixingdata.h
 * 
 * \author Richard Larsson
 * \date   2012-10-31
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
      LM_NONE,          // Reserved for no line mixing
      LM_LBLRTM,        // Reserved for LBLRTM line mixing
      LM_2NDORDER       // Reserved for Makarov et al. 2011 second order line mixing
    };
  
    // Defining an object
    LineMixingData() : mtype(LM_NONE), mdata() {}
    
    // Use these to get the raw data from this class
    const LM_Type& Type() const {return mtype;}
    const ArrayOfVector& Data() const {return mdata;}
    
    // Use these to return data in the format required by the line shape calculator
    void GetLBLRTM(Numeric& Y, Numeric& G, const Numeric& Temperature, const Index& order); 
    void Get2ndOrder(Numeric& Y, Numeric& G, Numeric& DV, const Numeric& Temperature);
    
    // Use these to insert the data in the required format
    void SetLBLRTMFromTheirCatalog(const Vector& t, const Vector& y, const Vector& g) {
      mtype = LM_LBLRTM;
      mdata.resize(3);
      mdata[0] = t;
      mdata[1] = y;
      mdata[2] = g;
    }
    
    // Use these to read data from ARTS catalog
    void Vector2LBLRTMData(const Vector& input);
    void Vector2NoneData(const Vector&);
    void Vector2SecondOrderData(const Vector& input);
    void StorageTag2SetType(const String& input);
    void SetDataFromVectorWithKnownType(const Vector& input);
    
    // Use these to save data  to  ARTS catalog
    void LBLRTMData2Vector(Vector& output);
    void Type2StorageTag(String& output);
    void SecondOrderData2Vector(Vector& output);
    
private:
    // mtype identifies the type of line mixing and mdata should contain the required data
    LM_Type mtype;
    ArrayOfVector mdata;
};

#endif // linemixingdata_h
