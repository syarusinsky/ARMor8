#ifndef SCROLLABLEMENUMODEL_HPP
#define SCROLLABLEMENUMODEL_HPP

/*************************************************************************
 * The ScrollableMenuModel is a simple class that represents the entries
 * in a scrollable menu as well as the cursor position and what entries
 * should currently be displayed. The entries that should currently
 * be displayed is based on the cursor position and the m_NumVisibleEntries,
 * which is the number that the display can view at once. This whole class
 * is pretty much just a simple model whose logic a UI manager can use to
 * draw simple scrollable menus.
*************************************************************************/

#include <set>

#define MAX_ENTRIES 20

class ScrollableMenuModel
{
	public:
		ScrollableMenuModel (unsigned int numVisibleEntries);
		~ScrollableMenuModel();

		unsigned int addEntry (const char* entry, bool tickable = false); // returns the index of given entry, or max uint if invalid

		char** getEntries(); // returns a pointer to the first visible entry, with the next ones following until null terminated

		void advanceCursor();
		void reverseCursor();
		void returnToTop();
		void advanceToBottom();

		unsigned int getEntryIndex(); // returns the currently selected entry's index
		unsigned int getFirstVisibleIndex();
		unsigned int getCursorIndex(); // only returns the position of the cursor (0 to number of visible entries)

		bool indexIsTickable (unsigned int index);

		unsigned int getTotalNumEntries();

	private:
		char* 			m_Entries[MAX_ENTRIES + 1]; // plus 1 so that we always null terminate the list
		unsigned int 		m_TotalEntries;

		unsigned int 		m_NumVisibleEntries;

		unsigned int 		m_TopVisibleIndex;
		unsigned int 		m_CursorIndex;

		bool 			m_TickableIndices[MAX_ENTRIES];
};

#endif // SCROLLABLEMENUMODEL_HPP
