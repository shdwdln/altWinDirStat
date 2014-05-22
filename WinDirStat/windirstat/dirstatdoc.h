// dirstatdoc.h		- Declaration of the CDirstatDoc class
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2004 Bernhard Seifert
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: bseifert@users.sourceforge.net, bseifert@daccord.net
//
// Last modified: $Date$

#pragma once

#include "selectdrivesdlg.h"
#include <vector>

class CItem;

//
// The treemap colors as calculated in CDirstatDoc::SetExtensionColors()
// all have the "brightness" BASE_BRIGHTNESS.
// I define brightness as a number from 0 to 3.0: (r+g+b)/255.
// RGB(127, 255, 0), for example, has a brightness of 2.5.
// 
#define BASE_BRIGHTNESS 1.8

//
// Data stored for each extension.
//
struct SExtensionRecord
{
	/*
	  4,294,967,295  (4294967295 ) is the maximum number of files in an NTFS filesystem according to http://technet.microsoft.com/en-us/library/cc781134(v=ws.10).aspx
	  18446744073709551615 is the maximum theoretical size of an NTFS file according to http://blogs.msdn.com/b/oldnewthing/archive/2007/12/04/6648243.aspx
	*/
	_Field_range_(0, 4294967295 ) LONGLONG files;
	_Field_range_(0, 18446744073709551615) LONGLONG bytes;
	COLORREF color;
};

//
// Maps an extension (".bmp") to an SExtensionRecord.
//
typedef CMap<CString, LPCTSTR, SExtensionRecord, SExtensionRecord&> CExtensionData;



//
// Hints for UpdateAllViews()
//
enum
{
	HINT_NULL,				        // General update
	HINT_NEWROOT,			        // Root item has changed - clear everything.
	HINT_SELECTIONCHANGED,	        // The selection has changed, EnsureVisible.
	HINT_SHOWNEWSELECTION,	        // The selection has changed, Show Path
	HINT_SELECTIONSTYLECHANGED,	    // Only update selection in Graphview
	HINT_EXTENSIONSELECTIONCHANGED,	// Type list selected a new extension
	HINT_ZOOMCHANGED,		        // Only zoom item has changed.
	HINT_REDRAWWINDOW,		        // Only graphically redraw views.
	HINT_SOMEWORKDONE,		        // Directory list shall process mouse messages first, then re-sort.
	HINT_LISTSTYLECHANGED,	        // Options: List style (grid/stripes) or treelist colors changed
	HINT_TREEMAPSTYLECHANGED	    // Options: Treemap style (grid, colors etc.) changed
};

// CDirstatDoc. The "Document" class. 
// Owner of the root item and various other data (see data members).
class CDirstatDoc : public CDocument
{
protected:
	CDirstatDoc();	// Created by MFC only
	DECLARE_DYNCREATE(CDirstatDoc)

public:
	virtual ~CDirstatDoc();

	static void    DecodeSelection      ( _In_ const CString s,   _Inout_ CString& folder,  _Inout_ CStringArray& drives      );
	virtual void   DeleteContents       (                                                                                );
	static CString EncodeSelection      ( _In_ const RADIO radio,            _In_ const CString folder, _In_ const CStringArray& drives );
	static TCHAR   GetEncodingSeparator (                                                                                );
	virtual BOOL   OnNewDocument        (                                                                                );
	virtual BOOL   OnOpenDocument       ( _In_ const LPCTSTR   lpszPathName                                                   );
	virtual void   SetPathName          ( _In_ const LPCTSTR   lpszPathName, BOOL bAddToMRU                                   );
	virtual void   Serialize            ( _In_ const CArchive& ar                                                             );
	CExtensionData* GetExtensionDataPtr (                                                                                );
	std::map<CString, SExtensionRecord>* GetstdExtensionDataPtr( );

	COLORREF        GetCushionColor     ( _In_ LPCTSTR ext );
	COLORREF        GetZoomColor        (             ) const;


	//const CExtensionData *GetExtensionData(); TODO: investigate failure
	CExtensionData *GetExtensionData    (             );
	std::map<CString, SExtensionRecord>* GetstdExtensionData    (             );
	LONGLONG        GetRootSize         (             ) const;


	bool IsDrive                        ( _In_ CString spec                                     );
	bool IsRootDone                     (                                                  )      const;
	bool IsZoomed                       (                                                  )      const;
	bool OptionShowFreeSpace            (                                                  )      const;
	bool OptionShowUnknown              (                                                  )      const;
	bool UserDefinedCleanupWorksForItem ( const USERDEFINEDCLEANUP *udc, const CItem *item );
	bool Work                           ( _In_ DWORD ticks                                      ); // return: true if done.


	void ForgetItemTree                 (                                                                );
	void OpenItem                       ( _In_ const CItem *item                                              );
	void RefreshJunctionItems           (                                                                );
	void RefreshMountPointItems         (                                                                );
	void SetHighlightExtension          ( _In_ const LPCTSTR ext                                              );
	void SetSelection                   ( _In_ const CItem *item,  _In_ const bool keepReselectChildStack = false );
	void SetTitlePrefix                 ( const CString prefix                                           );
	void UnlinkRoot                     (                                                                );

	CItem  *GetRootItem                 ( ) const;
	CItem  *GetSelection                ( ) const;
	CItem  *GetZoomItem                 ( ) const;

	CString GetHighlightExtension       ( ) const;

	LONGLONG GetWorkingItemReadJobs     ( ) const;


protected:

	static CExtensionData *_pqsortExtensionData;
	static int __cdecl _compareExtensions     ( _In_ const void *ext1, _In_ const void *ext2 );
	bool stdCompareExtensions                 ( _In_ const CString *stringOne, _In_ const CString *stringTwo );

	CString BuildUserDefinedCleanupCommandLine( const LPCTSTR format, const LPCTSTR rootPath, const LPCTSTR currentPath );
		
	bool DeletePhysicalItem                   ( _In_ CItem *item, _In_ const bool toTrashBin );
	bool DirectoryListHasFocus                (                                    ) const;
	bool IsReselectChildAvailable             (                                    ) const;

	CItem *PopReselectChild                   (                                    );	
	
	void ClearReselectChildStack              (                                                                                                                                                   );
	void GetDriveItems                        ( _Inout_ CArray<CItem *, CItem *>& drives                                                                                                                  );
	void PushReselectChild                    ( CItem *item                                                                                                                                       );
	void RecurseRefreshMountPointItems        ( _In_ CItem *item                                                                                                                                       );
	void RecurseRefreshJunctionItems          ( _In_ CItem *item                                                                                                                                       );
	void RefreshItem                          ( _In_ CItem *item                                                                                                                                       );
	//void RefreshRecyclers                     (                                                                                                                                                   );
	void RebuildExtensionData                 (                                                                                                                                                   );
	std::map<LONGLONG, CString>  stdSortExtData       ( _In_ std::map<CString, SExtensionRecord> & sortedExtensions                                                                                         );
	void SortExtensionData                    ( _Inout_ CStringArray& sortedExtensions                                                                                                                    );
	void SetExtensionColors                   ( _In_ const CStringArray& sortedExtensions                                                                                                              );
	void stdSetExtensionColors( _Inout_ std::map<LONGLONG, CString>& extensionsToSet, _Inout_ std::map<CString, SExtensionRecord>& theExtensions );
	void SetWorkingItemAncestor               ( _In_ CItem *item                                                                                                                                       );
	void SetWorkingItem                       ( _In_ CItem *item                                                                                                                                       );
	void SetWorkingItem                       ( _In_ CItem *item, _In_ bool hideTiming                                                                                                                                       );
	void SetZoomItem                          ( _In_ CItem *item                                                                                                                                       );

	bool    m_showFreeSpace;		// Whether to show the <Free Space> item
	bool    m_showUnknown;			// Whether to show the <Unknown> item
	bool    m_showMyComputer;		// True, if the user selected more than one drive for scanning. In this case, we need a root pseudo item ("My Computer").
	bool    m_extensionDataValid;   // If this is false, m_extensionData must be rebuilt
	bool    m_timeTextWritten;

	CItem  *m_rootItem;			    // The very root item
	std::shared_ptr<CItem> m_smartRootItem;
	CItem  *m_selectedItem;		    // Currently selected item, or NULL
	CString m_highlightExtension;	// Currently highlighted extension
	CItem  *m_zoomItem;			    // Current "zoom root"
	CItem  *m_workingItem;		   // Current item we are working on. For progress indication


	//typedef CMap<CString, LPCTSTR, SExtensionRecord, SExtensionRecord&> CExtensionData;
	CExtensionData m_extensionData;		// Base for the extension view and cushion colors
	std::map<CString, SExtensionRecord> stdExtensionData;

	CList<CItem *, CItem *> m_reselectChildStack; // Stack for the "Re-select Child"-Feature

#ifdef DEBUG
	void traceOut_ColorExtensionSetDebugLog( );
	bool isColorInVector( DWORD aColor, std::vector<DWORD>& colorVector );
	struct debuggingLogger {
		bool iLessThan_Colors_GetSize;
		int iterator;
		DWORD color;
		CString extension;
		};
	std::vector<debuggingLogger> ColorExtensionSetDebugLog;
	std::vector<DWORD> workDone;
#endif
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateRefreshselected(CCmdUI *pCmdUI);
	afx_msg void OnRefreshselected();
	afx_msg void OnUpdateRefreshall(CCmdUI *pCmdUI);
	afx_msg void OnRefreshall();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnCleanupEmptyrecyclebin();
	afx_msg void OnUpdateViewShowfreespace(CCmdUI *pCmdUI);
	afx_msg void OnViewShowfreespace();
	afx_msg void OnUpdateViewShowunknown(CCmdUI *pCmdUI);
	afx_msg void OnViewShowunknown();
	afx_msg void OnUpdateTreemapZoomin(CCmdUI *pCmdUI);
	afx_msg void OnTreemapZoomin();
	afx_msg void OnUpdateTreemapZoomout(CCmdUI *pCmdUI);
	afx_msg void OnTreemapZoomout();
	afx_msg void OnUpdateExplorerHere(CCmdUI *pCmdUI);
	afx_msg void OnExplorerHere();
	afx_msg void OnUpdateCommandPromptHere(CCmdUI *pCmdUI);
	afx_msg void OnCommandPromptHere();
	afx_msg void OnUpdateCleanupDeletetotrashbin(CCmdUI *pCmdUI);
	afx_msg void OnCleanupDeletetotrashbin();
	afx_msg void OnUpdateCleanupDelete(CCmdUI *pCmdUI);
	afx_msg void OnCleanupDelete();
	afx_msg void OnUpdateTreemapSelectparent(CCmdUI *pCmdUI);
	afx_msg void OnTreemapSelectparent();
	afx_msg void OnUpdateTreemapReselectchild(CCmdUI *pCmdUI);
	afx_msg void OnTreemapReselectchild();
	afx_msg void OnUpdateCleanupOpen(CCmdUI *pCmdUI);
	afx_msg void OnCleanupOpen();
	afx_msg void OnUpdateCleanupProperties(CCmdUI *pCmdUI);
	afx_msg void OnCleanupProperties();

public:
	double m_searchTime;
	LARGE_INTEGER m_searchStartTime;
	LARGE_INTEGER m_timerFrequency;
	
	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif
};

//
// The document is needed in many places.
//
extern CDirstatDoc *GetDocument();



// $Log$
// Revision 1.9  2004/11/05 16:53:07  assarbad
// Added Date and History tag where appropriate.
//
