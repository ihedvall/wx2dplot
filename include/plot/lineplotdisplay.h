/*
 * Copyright 2026 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/window.h>
#include <mathplot/mathplot.h>

namespace plot {

class LinePlotDisplay : public mpWindow {
public:
  LinePlotDisplay(wxWindow* parent, wxWindowID id = wxID_ANY);
};

} // namespace plot


