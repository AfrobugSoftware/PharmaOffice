#pragma once
#include <wx/dataview.h>
#include <utility>
#include "datatree.h"

namespace pof {
	template<typename T, typename... args>
	class DataTreeModel : public wxDataViewTreeStore
	{

	public:	
		struct idx_t{
			T m_ident;
			wxDataViewItem m_item;

			using std::rel_ops::operator>;
			using std::rel_ops::operator<=;
			using std::rel_ops::operator>=;
			using std::rel_ops::operator!=;

			bool operator==(const idx_t& i) {
				return (m_ident == i.m_ident && m_item == i.m_item);
			}

			bool operator<(const idx_t& i) {
				return (m_ident < i.m_ident&& m_item == i.m_item);
			}
		};


		DataTreeModel() = default;
	protected:
		std::shared_ptr<pof::base::node<idx_t>> m_root;

	};

};