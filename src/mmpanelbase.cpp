/*******************************************************
Copyright (C) 2006 Madhan Kanagavel
Copyright (C) 2014 Nikolay

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

#include "mmpanelbase.h"
#include "constants.h"
#include "util.h"
#include <algorithm>
#include "model/Model_Setting.h"

static int COL_MAX = 10;
static const wxString translate_me[] =
{
    wxTRANSLATE("Initial Value"),
    wxTRANSLATE("Current Value")
};

static const char *ASSETS_LIST_JSON = R"({
"0": {"name":" ", "format":2, "width":24},
"1": {"name":"ID", "format":1, "width":0},
"2": {"name":"Name", "format":0, "width":150},
"3": {"name":"Date", "format":1, "width":100},
"4": {"name":"Type", "format":0, "width":100},
"5": {"name":"Initial Value", "format":1, "width":-2},
"6": {"name":"Current Value", "format":1, "width":-2},
"7": {"name":"Notes", "format":0, "width":450},
"key": "ASSET_COL_SETTINGS"
})";

class wxSQLite3Database;
class wxListItemAttr;

wxBEGIN_EVENT_TABLE(mmListCtrl, wxListCtrl)
    EVT_LIST_COL_END_DRAG(wxID_ANY, mmListCtrl::OnItemResize)
wxEND_EVENT_TABLE()

mmListCtrl::mmListCtrl(wxWindow *parent, wxWindowID winid)
    : wxListCtrl(parent, winid
        , wxDefaultPosition, wxDefaultSize
        , wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL
    )
    , attr1_(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont))
    , attr2_(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont))
    , m_selected_row(-1)
    , m_selected_col(0)
    , m_asc(true)
    , m_json(wxEmptyString)
{
    mmCreateColumns(winid);
}

mmListCtrl::~mmListCtrl()
{
    if (attr1_) delete attr1_;
    if (attr2_) delete attr2_;
    //TODO: save columns order
    for (int i = 0; i < GetColumnCount(); i++) {
        wxLogDebug("Col#%i is %i", i, GetColumnOrder(i));
    }
    wxLogDebug("Buy mmListCtrl -----");
}

void mmListCtrl::OnItemResize(wxListEvent& event)
{
    int i = event.GetColumn();
    int width = event.GetItem().GetWidth();
    json::Object o = str2json_obj(m_json);
    json::Object d = o[std::to_wstring(i)];
    d[L"width"] = json::Number(width);
    o[std::to_wstring(i)] = d;
    m_json = json_obj2str(o);
    const wxString& key = wxString(json::String(o[L"key"]));
    Model_Setting::instance().Set(key, m_json);
}

void mmListCtrl::mmCreateColumns(int id)
{
    switch (id)
    {
    case mmID_ASSETS_LIST:
        m_json = ASSETS_LIST_JSON;
        break;
    default:
        m_json = wxEmptyString;
    }

    if (m_json.empty()) return;

    json::Object o = str2json_obj(m_json);
    const wxString& key = wxString(json::String(o[L"key"]));
    m_json = Model_Setting::instance().GetStringSetting(key, m_json);
    o = str2json_obj(m_json);

    for (int i = 0; i < COL_MAX; i++)
    {
        json::Object d = json::Object(o[std::to_wstring(i)]);
        if (d.Empty()) break;
        const wxString& name = wxGetTranslation(wxString(json::String(d[L"name"])));
        int format = json::Number(d[L"format"]);
        int width = json::Number(d[L"width"]);
        this->InsertColumn(i, name, format, width);
    }

}

wxListItemAttr* mmListCtrl::OnGetItemAttr(long row) const
{
    return (row % 2) ? attr2_ : attr1_;
}

wxString mmListCtrl::BuildPage(const wxString &title) const
{
    wxString text;
    text << "<html>" << wxTextFile::GetEOL();
    text << "<head>" << wxTextFile::GetEOL();
    text << "<title>" << title << "</title>" << wxTextFile::GetEOL();
    text << "</head>" << wxTextFile::GetEOL();
    text << "<body>" << wxTextFile::GetEOL();
    text << "<table ";
    if ((GetWindowStyle() & wxLC_HRULES) ||
        (GetWindowStyle() & wxLC_VRULES))
        text << "border=1";
    else
        text << "border=0";
    text << " cellpadding=4 cellspacing=0 >" << wxTextFile::GetEOL();

    text << "<tr>" << wxTextFile::GetEOL();
    for (int c = 0; c < GetColumnCount(); c++)
    {
        wxListItem col;
        col.SetMask(wxLIST_MASK_TEXT);
        GetColumn(c, col);
        text << "<th><i>" << col.GetText() << "</i></th>" << wxTextFile::GetEOL();
    }
    text << "</tr>" << wxTextFile::GetEOL();

    for (int i = 0; i < GetItemCount(); i++)
    {
        text << "<tr>" << wxTextFile::GetEOL();
        for (int col = 0; col < GetColumnCount(); col++)
        {
            text << "<td>" << wxListCtrl::GetItemText(i, col) << "</td>" << wxTextFile::GetEOL();
        }
        text << "</tr>" << wxTextFile::GetEOL();
    }
    text << "</table>" << wxTextFile::GetEOL();
    text << "</body>" << wxTextFile::GetEOL();
    text << "</html>" << wxTextFile::GetEOL();

    return text;
}

//*--------------------------------------------------------------------*//
void mmPanelBase::PrintPage()
{
    wxWebView * htmlWindow = wxWebView::New(this, wxID_ANY);
    htmlWindow->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
    htmlWindow->SetPage(BuildPage(), "");
    htmlWindow->Print();
}

