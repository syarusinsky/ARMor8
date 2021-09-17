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
}

void ScrollableMenuModel::addEntry (const char* entry, bool tickable)
{
	if ( m_TotalEntries < MAX_ENTRIES )
	{
		char* entryAlloc = new char[strlen(entry) + 1]; // plus one for null character
		strcpy( entryAlloc, entry );

		m_Entries[m_TotalEntries] = entryAlloc;

		if ( tickable )
		{
			m_TickableIndices[m_TotalEntries] = true;
		}

		m_TotalEntries++;
	}
}

char** ScrollableMenuModel::getEntries()
{
	return &m_Entries[m_TopVisibleIndex];
}

void ScrollableMenuModel::advanceCursor()
{
	if ( m_CursorIndex < m_NumVisibleEntries - 1 )
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
