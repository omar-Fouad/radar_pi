/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 *           Sean D'Epagnier
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#ifndef _CONTROLSDIALOG_H_
#define _CONTROLSDIALOG_H_

#include "radar_pi.h"

PLUGIN_BEGIN_NAMESPACE

#define OFFSCREEN_CONTROL_X (-10000)
#define OFFSCREEN_CONTROL_Y (-10000)

#define AUTO_RANGE (-20000)  // Auto values are -20000 - auto_index

const static wxPoint OFFSCREEN_CONTROL = wxPoint(OFFSCREEN_CONTROL_X, OFFSCREEN_CONTROL_Y);

class RadarControlButton;
class RadarButton;

extern wxString guard_zone_names[2];

extern string ControlTypeNames[CT_MAX];

//----------------------------------------------------------------------------------------------------------
//    Radar Control Dialog Specification
//----------------------------------------------------------------------------------------------------------
class ControlsDialog : public wxDialog {
 public:
  ControlsDialog();

  ~ControlsDialog();
  void Init();

  bool Create(wxWindow *parent, radar_pi *pi, RadarInfo *ri, wxWindowID id = wxID_ANY, const wxString &caption = _("Radar"),
              const wxPoint &pos = OFFSCREEN_CONTROL);

  void AdjustRange(int adjustment);
  wxString &GetRangeText();
  void SetTimedIdleIndex(int index);
  void UpdateGuardZoneState();
  virtual void UpdateDialogShown() = 0;
  virtual void UpdateControlValues(bool force) = 0;
  void SetErrorMessage(wxString &msg);
  void ShowBogeys(wxString text, bool confirmed);
  void HideBogeys();

  void HideTemporarily();  // When a second dialog (which is not a child class) takes over -- aka GuardZone
  void UnHideTemporarily();
  void ShowDialog();
  void HideDialog();
  void ShowCursorPane() { SwitchTo(m_cursor_sizer, wxT("cursor")); }

  radar_pi *m_pi;
  RadarInfo *m_ri;
  wxString m_log_name;
  wxBoxSizer *m_top_sizer;
  wxBoxSizer *m_control_sizer;
  wxPoint m_panel_position;
  bool m_manually_positioned;

 protected:
  bool m_hide;
  bool m_hide_temporarily;
  time_t m_auto_hide_timeout;  // At what time do we hide the dialog

  wxWindow *m_parent;
  wxBoxSizer *m_advanced_sizer;
  wxBoxSizer *m_view_sizer;
  wxBoxSizer *m_edit_sizer;
  wxBoxSizer *m_guard_sizer;
  wxBoxSizer *m_adjust_sizer;
  wxBoxSizer *m_cursor_sizer;
  wxBoxSizer *m_power_sizer;
  wxBoxSizer *m_transmit_sizer;  // Controls disabled if not transmitting
  wxBoxSizer *m_from_sizer;      // If on edit control, this is where the button is from

  // Edit Controls
  RadarControlButton *m_from_control;  // Only set when in edit mode

  // The 'edit' control has these buttons:
  wxButton *m_plus_ten_button;
  wxButton *m_plus_button;
  wxStaticText *m_value_text;
  wxStaticText *m_comment_text;
  wxButton *m_minus_button;
  wxButton *m_minus_ten_button;
  wxButton *m_auto_button;

  // Main control
  wxButton *m_guard_1_button;
  wxButton *m_guard_2_button;
  wxButton *m_power_button;

  // Guard Zone Edit

  GuardZone *m_guard_zone;
  wxStaticText *m_guard_zone_text;
  wxRadioBox *m_guard_zone_type;
  wxTextCtrl *m_outer_range;
  wxTextCtrl *m_inner_range;
  wxTextCtrl *m_start_bearing;
  wxTextCtrl *m_end_bearing;
  wxCheckBox *m_arpa_box;
  wxCheckBox *m_alarm;

  // Cursor controls
  wxButton *m_bearing_buttons[BEARING_LINES];
  wxButton *m_clear_cursor;
  wxButton *m_acquire_target;
  wxButton *m_delete_target;
  wxButton *m_delete_all;

  // View controls
  RadarControlButton *m_target_trails_button;
  wxButton *m_targets_button;
  wxButton *m_trails_motion_button;
  wxButton *m_clear_trails_button;
  wxButton *m_orientation_button;

  // Power controls
  wxStaticText *m_power_text;
  wxButton *m_transmit_button;
  wxButton *m_standby_button;
  RadarControlButton *m_timed_idle_button;
  RadarControlButton *m_timed_run_button;

  // Methods common to any radar
  void EnsureWindowNearOpenCPNWindow();
  void ShowGuardZone(int zone);
  void SetGuardZoneVisibility();
  void SetMenuAutoHideTimeout();
  void SwitchTo(wxBoxSizer *to, const wxChar *name);
  void UpdateTrailsState();

  // Methods that we know that every radar must or may implement its own way
  virtual void CreateControls() = 0;
  virtual void UpdateRadarSpecificState(){};
};

class RadarButton : public wxButton {
 public:
  RadarButton(){

  };

  RadarButton(ControlsDialog *parent, wxWindowID id, wxSize buttonSize, const wxString &label) {
    Create(parent, id, label, wxDefaultPosition, buttonSize, 0, wxDefaultValidator, label);

    m_parent = parent;
    m_pi = m_parent->m_pi;
    SetFont(m_parent->m_pi->m_font);
    SetLabel(label);  // Use the \n on Mac to enforce double height button
  }

  ControlsDialog *m_parent;
  radar_pi *m_pi;

  void SetLabel(const wxString &label) {
    wxString newLabel;

#ifdef __WXOSX__
    newLabel << wxT("\n");
#endif
    newLabel << label;
#ifdef __WXOSX__
    newLabel << wxT("\n");
#endif
    wxButton::SetLabel(newLabel);
  }
};

class DynamicStaticText : public wxStaticText {
 public:
  DynamicStaticText() {}

  DynamicStaticText(wxWindow *parent, wxWindowID id, const wxString &label, const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize, long style = 0, const wxString &name = wxStaticTextNameStr) {
    Create(parent, id, label, pos, size, style, name);
  }

  void SetLabel(const wxString &label) {
    wxStaticText::SetLabel(label);
    SetSize(GetTextExtent(label));
  }
};

class RadarControlButton : public wxButton {
 public:
  RadarControlButton(){

  };

  RadarControlButton(ControlsDialog *parent, wxWindowID id, wxSize buttonSize, const wxString &label, ControlType ct,
                     bool newHasAuto, int newValue, const wxString &newUnit = wxT(""), const wxString &newComment = wxT("")) {
    Create(parent, id, label + wxT("\n"), wxDefaultPosition, buttonSize, 0, wxDefaultValidator, label);

    m_parent = parent;
    m_pi = m_parent->m_pi;
    minValue = 0;
    maxValue = 100;
    value = 0;
    if (ct == CT_GAIN) {
      value = 50;
    }
    autoValue = 0;
    autoValues = newHasAuto ? 1 : 0;
    autoNames = 0;
    firstLine = label;
    unit = newUnit;
    comment = newComment;
    names = 0;
    controlType = ct;
    if (autoValues > 0) {
      SetLocalAuto(AUTO_RANGE - 1);  // Not sent to radar, radar will update state
    } else {
      SetLocalValue(newValue);
    }

    this->SetFont(m_parent->m_pi->m_font);
  }

  virtual void AdjustValue(int adjustment);
  virtual void SetAuto(int newValue);
  virtual void SetLocalValue(int newValue);
  virtual void SetLocalAuto(int newValue);
  const wxString *names;
  const wxString *autoNames;
  wxString unit;
  wxString comment;

  wxString firstLine;

  ControlsDialog *m_parent;
  radar_pi *m_pi;

  int value;
  int autoValue;   // 0 = not auto mode, 1 = normal auto value, 2... etc special, auto_names is set
  int autoValues;  // 0 = none, 1 = normal auto value, 2.. etc special, auto_names is set

  int minValue;
  int maxValue;
  ControlType controlType;
};

class RadarRangeControlButton : public RadarControlButton {
 public:
  RadarRangeControlButton(ControlsDialog *parent, RadarInfo *ri, wxWindowID id, wxSize buttonSize, const wxString &label) {
    Create(parent, id, label + wxT("\n"), wxDefaultPosition, buttonSize, 0, wxDefaultValidator, label);

    m_parent = parent;
    m_pi = m_parent->m_pi;
    m_ri = ri;
    minValue = 0;
    maxValue = 0;
    value = -1;  // means: never set
    autoValue = 0;
    autoValues = 1;
    autoNames = 0;
    unit = wxT("");
    firstLine = label;
    names = 0;
    controlType = CT_RANGE;

    this->SetFont(m_parent->m_pi->m_font);
  }

  virtual void AdjustValue(int adjustment);
  virtual void SetAuto(int newValue);
  void SetRangeLabel();

 private:
  RadarInfo *m_ri;
};

PLUGIN_END_NAMESPACE

#endif
