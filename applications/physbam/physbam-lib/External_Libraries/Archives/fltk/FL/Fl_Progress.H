//
// "$Id: Fl_Progress.H,v 1.1 2004/02/15 00:59:20 aselle Exp $"
//
// Progress bar widget definitions.
//
// Copyright 2000-2003 by Michael Sweet.
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

#ifndef _Fl_Progress_H_
#  define _Fl_Progress_H_

//
// Include necessary headers.
//

#include "Fl_Widget.H"


//
// Progress class...
//

/** The Fl_Progress widget displays a progress bar for the user.
 */
class FL_EXPORT Fl_Progress : public Fl_Widget
{
  float	value_,
	minimum_,
	maximum_;

protected:

  virtual void draw();

public:

    /** The constructor creates the progress bar using the position,
     * size, and label. */
  Fl_Progress(int x, int y, int w, int h, const char *l = 0);

    /** Sets the maximum value in the progress widget. */
  void	maximum(float v) { maximum_ = v; redraw(); }
    /** Gets the maximum value in the progress widget. */
  float	maximum() const { return (maximum_); }

    /** Sets the minimum value in the progress widget. */
  void	minimum(float v) { minimum_ = v; redraw(); }
    /** Gets the minimum value in the progress widget. */
  float	minimum() const { return (minimum_); }

    /** Sets the current value in the progress widget. */
  void	value(float v) { value_ = v; redraw(); }
    /** Gets the current value in the progress widget. */
  float	value() const { return (value_); }
};

#endif // !_Fl_Progress_H_

//
// End of "$Id: Fl_Progress.H,v 1.1 2004/02/15 00:59:20 aselle Exp $".
//
