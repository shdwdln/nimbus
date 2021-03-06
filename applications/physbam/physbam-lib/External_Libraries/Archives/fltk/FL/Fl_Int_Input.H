//
// "$Id: Fl_Int_Input.H,v 1.1 2004/02/15 00:59:19 aselle Exp $"
//
// Integer input header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2003 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#ifndef Fl_Int_Input_H
#define Fl_Int_Input_H

#include "Fl_Input.H"

/** The Fl_Int_Input class is a subclass of Fl_Input that only allows the 
 * user to type decimal digits (or hex numbers of the form \c 0xaef). */
class Fl_Int_Input : public Fl_Input {
public:
      /** Creates a new Fl_Int_Input widget using the given position, size,
       * and label string. The default boxtype is \c FL_DOWN_BOX. */
    Fl_Int_Input(int X,int Y,int W,int H,const char *l = 0)
	: Fl_Input(X,Y,W,H,l) {type(FL_INT_INPUT);}
};

#endif

//
// End of "$Id: Fl_Int_Input.H,v 1.1 2004/02/15 00:59:19 aselle Exp $".
//
