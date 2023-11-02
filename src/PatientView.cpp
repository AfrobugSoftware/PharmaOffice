#include "PofPch.h"
#include "PatientView.h"
BEGIN_EVENT_TABLE(pof::PatientView, wxPanel)
END_EVENT_TABLE()

pof::PatientView::PatientView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel(parent, id, pos, size, style){
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE);


}

pof::PatientView::~PatientView()
{
}

void pof::PatientView::CreateToolBars()
{
}

void pof::PatientView::CreateViews()
{
}

void pof::PatientView::OnPatientSelected(wxDataViewEvent& evt)
{
}
