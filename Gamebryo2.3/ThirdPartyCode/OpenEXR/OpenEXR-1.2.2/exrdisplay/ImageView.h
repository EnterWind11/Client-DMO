///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////


#ifndef INCLUDED_IMAGE_VIEW_H
#define INCLUDED_IMAGE_VIEW_H

//----------------------------------------------------------------------------
//
//	class ImageView -- draws an Imf::Rgba image in an OpenGl window
//
//----------------------------------------------------------------------------

#include <FL/Fl_Gl_Window.H>
#include <ImfRgba.h>
#include <ImfArray.h>


class ImageView: public Fl_Gl_Window
{
  public:

    ImageView (int x, int y,
	       int w, int h,            // display window width and height
	       const char label[],
	       const Imf::Rgba pixels[/* w*h */],
	       int dw, int dh,		// data window width and height
	       int dx, int dy,		// data window offset
	       float exposure,
	       float defog,
	       float kneeLow,
	       float kneeHigh);

    virtual void        setExposure (float exposure);
    virtual void	setDefog (float defog);
    virtual void	setKneeLow (float low);
    virtual void	setKneeHigh (float high);
    
    virtual void	draw();

 protected:

    virtual void        updateScreenPixels ();
    void		computeFogColor ();

    float		_exposure;
    float		_defog;
    float		_kneeLow;
    float		_kneeHigh;
    const Imf::Rgba *	_rawPixels;
    float		_fogR;
    float		_fogG;
    float		_fogB;
    int			_dw;
    int			_dh;
    int			_dx;
    int			_dy;

 private:

    Imf::Array<unsigned char>	_screenPixels;
};


#endif
