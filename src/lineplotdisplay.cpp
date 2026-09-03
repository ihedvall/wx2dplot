//
// Created by ihedv on 2026-09-02.
//

#include "plot/lineplotdisplay.h"

namespace plot {
LinePlotDisplay::LinePlotDisplay(wxWindow *parent, wxWindowID id)
  : mpWindow(parent, id) {
  wxColour bg_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
  wxColour draw_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
  wxColour axis_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
  SetColourTheme(bg_colour, draw_colour, axis_colour);
/*
  SetMarginTop(0);
  SetMarginBottom(0);
  SetMarginLeft(0);
  SetMarginRight(0);
  */
  SetDrawBox(false);
}

} // plot