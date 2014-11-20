/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#pragma once

#include "util.h"
#include <wx/webview.h>
#include <wx/webviewfshandler.h>
//----------------------------------------------------------------------------

class mmListCtrl: public wxListCtrl
{
    wxDECLARE_EVENT_TABLE();
public:
    mmListCtrl(){}
    mmListCtrl(wxWindow *parent, wxWindowID winid);
    virtual ~mmListCtrl();

public:
    wxListItemAttr *attr1_, *attr2_; // style1
    long m_selected_row;
    int m_selected_col;
    bool m_asc;

public:
    virtual wxListItemAttr* OnGetItemAttr(long row) const;
    wxString BuildPage(const wxString &title) const;
private:
    void OnItemResize(wxListEvent& event);
    void mmCreateColumns(int id);
    wxString m_json;
};

//----------------------------------------------------------------------------
class mmPanelBase : public wxPanel
{
public:
    mmPanelBase() {}
    virtual ~mmPanelBase() {}

public:
    virtual wxString BuildPage() const { return "TBD"; }
    virtual void PrintPage();
    void windowsFreezeThaw()
    {
#ifdef __WXGTK__
        return;
#endif

        if (this->IsFrozen())
            this->Thaw();
        else
            this->Freeze();
    }
public:
    virtual void sortTable() = 0;
};
