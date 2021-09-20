#include "ScrollableMenuModel.hpp"

#include <string.h>

ScrollableMenuModel::ScrollableMenuModel (unsigned int numVisibleEntries) :
	m_Entries{ nullptr },
	m_TotalEntries( 0 ),
	m_NumVisibleEntries( numVisibleEntries ),
	m_TopVisibleIndex( 0 ),
	m_CursorIndex( 0 ),
	m_TickableIndices{ false }
{
}

ScrollableMenuModel::~ScrollableMenuModel()
{
	// clean up allocs
	for ( unsigned int entry = 0; entry < m_TotalEntries; entry++ )
	{
		delete[] m_Entries[entry];
	}
}

unsigned int ScrollableMenuModel::addEntry (const char* entry, bool tickable)
{
	unsigned int retVal = static_cast<unsigned int>( -1 );
	if ( m_TotalEntries < MAX_ENTRIES )
	{
		char* entryAlloc = new char[strlen(entry) + 1]; // plus one for null character
		strcpy( entryAlloc, entry );

		m_Entries[m_TotalEntries] = entryAlloc;

		if ( tickable )
		{
			m_TickableIndices[m_TotalEntries] = true;
		}

		retVal = m_TotalEntries;
		m_TotalEntries++;
	}

	return retVal;
}

char** ScrollableMenuModel::getEntries()
{
	return &m_Entries[m_TopVisibleIndex];
}

void ScrollableMenuModel::advanceCursor()
{
	if ( m_CursorIndex < m_NumVisibleEntries - 1 && m_CursorIndex < m_TotalEntries - 1 )
	{
		m_CursorIndex++;
	}
	else if ( m_TopVisibleIndex + m_NumVisibleEntries < m_TotalEntries )
	{
		m_TopVisibleIndex++;
	}
}

void ScrollableMenuModel::reverseCursor()
{
	if ( m_CursorIndex > 0 )
	{
		m_CursorIndex--;
	}
	else if ( m_TopVisibleIndex > 0 )
	{
		m_TopVisibleIndex--;
	}
}

void ScrollableMenuModel::returnToTop()
{
	m_CursorIndex = 0;
	m_TopVisibleIndex = 0;
}

void ScrollableMenuModel::advanceToBottom()
{
	m_CursorIndex = m_NumVisibleEntries - 1;
	m_TopVisibleIndex = m_TotalEntries - m_NumVisibleEntries;
}

unsigned int ScrollableMenuModel::getEntryIndex()
{
	return this->getCursorIndex() + this->getFirstVisibleIndex();
}

unsigned int ScrollableMenuModel::getFirstVisibleIndex()
{
	return m_TopVisibleIndex;
}

unsigned int ScrollableMenuModel::getCursorIndex()
{
	return m_CursorIndex;
}

bool ScrollableMenuModel::indexIsTickable (unsigned int index)
{
	if ( index < m_TotalEntries && m_TickableIndices[index] )
	{
		return true;
	}

	return false;
}

unsigned int ScrollableMenuModel::getTotalNumEntries()
{
	return m_TotalEntries;
}
