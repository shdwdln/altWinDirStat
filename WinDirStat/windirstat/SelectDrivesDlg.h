// SelectDrivesDlg.h	- Declaration of CDriveItem, CDrivesList and CSelectDrivesDlg
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


#ifndef SELECTDRIVESDLG_H
#define SELECTDRIVESDLG_H
#else
#error ass
#endif

#pragma once

#include "stdafx.h"
//#include "ownerdrawnlistcontrol.h"
#include "layout.h"



class CDrivesList;

// CDriveItem. An item in the CDrivesList Control.
// All methods are called by the gui thread.
class CDriveItem : public COwnerDrawnListItem {
public:
	CDriveItem                ( CDrivesList* const list,             _In_z_ PCWSTR pszPath                                                                        );


	virtual INT Compare       ( _In_ const COwnerDrawnListItem* const other, _In_ _In_range_( 0, 7 ) const INT subitem ) const override final;

	virtual bool DrawSubitem  ( _In_ _In_range_( 0, 7 ) const ENUM_COL subitem,             _In_ CDC& pdc,           _In_ CRect rc,             _In_ const UINT state, _Out_opt_ _Deref_out_range_( 100, 100 ) INT* const width, _Inout_ INT* const focusLeft ) const override final;
	virtual CString GetText   ( _In_ _In_range_( 0, 7 ) const INT subitem                                                                                                     ) const override final;

	_Pre_satisfies_( this->m_querying ) void StartQuery( _In_ const HWND dialog, _In_ const UINT serial );

	void SetDriveInformation  ( _In_ const bool success,            _In_z_ const PCWSTR name, _In_ const std::uint64_t total, _In_ const std::uint64_t free                          );

	CString GetDrive( ) const {
		return m_path.Left( 2 );
		}
	//bool IsSUBSTed            ( ) const;
	
public:
	CDrivesList* m_list;	// Backpointer
	bool         m_isRemote : 1;	// Whether the drive type is DRIVE_REMOTE (network drive)
	bool         m_querying : 1;	// Information thread is running.
	bool         m_success  : 1;	// Drive is accessible. false while m_querying is true.

	CString      m_path;			// e.g. "C:\"
	CString      m_name;			// e.g. "BOOT (C:)"	
	

	//18446744073709551615 is the maximum theoretical size of an NTFS file              according to http://blogs.msdn.com/b/oldnewthing/archive/2007/12/04/6648243.aspx

	_Field_range_( 0, 18446744073709551615 ) std::uint64_t     m_totalBytes; // Capacity
	_Field_range_( 0, 18446744073709551615 ) std::uint64_t     m_freeBytes;  // Free space

	DOUBLE       m_used;			// used space / total space
	};


// CDriveInformationThread. Does the GetVolumeInformation() call, which may hang for ca. 30 sec, it a network drive is not accessible.
class CDriveInformationThread : public CWinThread {
	// Set of all running CDriveInformationThreads.
	// Used by InvalidateDialogHandle().

	// The objects register and deregister themselves in _runningThreads
	void AddRunningThread              ( );
	void RemoveRunningThread           ( );

public:
	static void InvalidateDialogHandle ( );
	static void OnAppExit( ) { }

	CDriveInformationThread            ( _In_z_ PCWSTR path,  LPARAM driveItem, HWND dialog,     UINT serial    );
	~CDriveInformationThread( ) {  DeleteCriticalSection( &m_cs ); }
	virtual BOOL InitInstance          ( ) override final;
	
	LPARAM GetDriveInformation         ( _Inout_ bool& success, _Inout_ CString& name,    _Inout_ std::uint64_t& total, _Inout_ std::uint64_t& free );

private:
	const CString    m_path;		    // Path like "C:\"
	const LPARAM     m_driveItem;	    // The list item, we belong to

	CRITICAL_SECTION m_cs;	            // for m_dialog
	_Guarded_by_( m_cs ) HWND             m_dialog;
	const UINT       m_serial;	        // serial number of m_dialog

	// "[out]"-parameters
	CString          m_name;			// Result: name like "BOOT (C:)", valid if m_success
	std::uint64_t    m_totalBytes;	    // Result: capacity of the drive, valid if m_success
	std::uint64_t    m_freeBytes;	    // Result: free space on the drive, valid if m_success
	bool             m_success :1;		// Result: false, iff drive is unaccessible.
	};

class CDrivesList : public COwnerDrawnListControl {
	DECLARE_DYNAMIC(CDrivesList)
public:
	CDrivesList( ) : COwnerDrawnListControl( _T( "drives" ), 20 ) { }
	CDriveItem* GetItem( const INT i ) const {
		return reinterpret_cast<CDriveItem * > ( GetItemData( i ) );
		}

	void SelectItem( _In_ CDriveItem* const item ) {
		auto i = FindListItem( item );
		SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		}
	
	bool IsItemSelected( const INT i ) const {
		return ( LVIS_SELECTED == GetItemState( i, LVIS_SELECTED ) );
		}

	//virtual bool HasImages( ) const;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown( const UINT nFlags, const CPoint point );
	
	afx_msg void OnLvnDeleteitem( NMHDR* pNMHDR, LRESULT* pResult ) {
		auto pNMLV = reinterpret_cast< LPNMLISTVIEW >( pNMHDR );
		delete GetItem( pNMLV->iItem );
		*pResult = 0;
		}
	
	afx_msg void MeasureItem( PMEASUREITEMSTRUCT pMeasureItemStruct ) {
		pMeasureItemStruct->itemHeight = m_rowHeight;
		}
	
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	};

//
// CSelectDrivesDlg. The initial dialog, where the user can select 
// one or more drives or a folder for scanning.
class CSelectDrivesDlg : public CDialog {
	DECLARE_DYNAMIC( CSelectDrivesDlg )
	enum {
		IDD = IDD_SELECTDRIVES
		};

public:
	CSelectDrivesDlg( CWnd* pParent = NULL );
	virtual ~CSelectDrivesDlg();

	// Dialog Data
	INT m_radio;// out.
	_When_( ( this->m_radio != RADIO_AFOLDER ), _At_( this->m_folderName, _Notvalid_ ) ) CString      m_folderName;	    // out. Valid if m_radio = RADIO_AFOLDER
	_When_( ( this->m_radio == RADIO_AFOLDER ), _At_( this->m_drives,     _Notvalid_ ) ) CStringArray m_drives;	        // out. Valid if m_radio != RADIO_AFOLDER

protected:
	_Pre_defensive_ virtual void DoDataExchange ( CDataExchange* pDX ) override final;
	virtual BOOL OnInitDialog   (                    ) override final;
	_Pre_defensive_ virtual void OnOK           (                    ) override final;


	void buildSelectList( );
	void initWindow( );
	void addControls( );
	void insertColumns( );
	void setListOptions( );

	_Pre_defensive_ void UpdateButtons          (                    );

	static UINT  _serial;	// Each Instance of this dialog gets a serial number
	CDrivesList  m_list;
	CButton      m_okButton;
	CStringArray m_selectedDrives;
	CLayout      m_layout;

	// Callback function for the dialog shown by SHBrowseForFolder()
	// MUST be static!
	static INT CALLBACK BrowseCallbackProc( _In_ HWND hWnd, _In_ UINT uMsg, LPARAM lParam, _In_ LPARAM pData);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBrowsefolder();
	afx_msg void OnLbnSelchangeDrives();
	afx_msg void OnBnClickedAlllocaldrives( ) {
		UpdateButtons( );
		}
	afx_msg void OnBnClickedAfolder( ) {
		UpdateButtons( );
		}
	afx_msg void OnBnClickedSomedrives( ) {
		m_list.SetFocus( );
		UpdateButtons( );
		}
	afx_msg void OnEnChangeFoldername( ) {
		UpdateButtons( );
		}
	afx_msg void OnMeasureItem( const INT nIDCtl, PMEASUREITEMSTRUCT pMeasureItemStruct ) {
		if ( nIDCtl == IDC_DRIVES ) {
			pMeasureItemStruct->itemHeight = 20;
			}
		else {
			CDialog::OnMeasureItem( nIDCtl, pMeasureItemStruct );
			}

		}
	
	afx_msg void OnLvnItemchangedDrives( NMHDR* pNMHDR, LRESULT* pResult ) {
		UNREFERENCED_PARAMETER( pNMHDR );
		m_radio = RADIO_SOMEDRIVES;
		UpdateData( false );
		UpdateButtons( );
		*pResult = 0;
		}

	afx_msg void OnSize( UINT nType, INT cx, INT cy ) {
		CDialog::OnSize( nType, cx, cy );
		m_layout.OnSize( );
		}
	
	afx_msg void OnGetMinMaxInfo( _Out_ MINMAXINFO* lpMMI ) {
		m_layout.OnGetMinMaxInfo( lpMMI );
		CDialog::OnGetMinMaxInfo( lpMMI );
		}

	afx_msg void OnDestroy( ) {
		CDriveInformationThread::InvalidateDialogHandle( );
		m_layout.OnDestroy( );
		CDialog::OnDestroy( );
		}
	
	afx_msg LRESULT OnWmuOk( const WPARAM, const LPARAM ) {
		OnOK( );
		return 0;
		}

	afx_msg _Function_class_( "GUI_THREAD" ) LRESULT OnWmuThreadFinished( const WPARAM, const LPARAM lparam );
	afx_msg void OnSysColorChange( ) {
		CDialog::OnSysColorChange();
		m_list.SysColorChanged();
		}
	};

// $Log$
// Revision 1.11  2004/11/14 21:50:44  assarbad
// - Pre-select the last used folder
//
// Revision 1.10  2004/11/13 08:17:07  bseifert
// Remove blanks in Unicode Configuration names.
//
// Revision 1.9  2004/11/12 22:14:16  bseifert
// Eliminated CLR_NONE. Minor corrections.
//
// Revision 1.8  2004/11/12 00:47:42  assarbad
// - Fixed the code for coloring of compressed/encrypted items. Now the coloring spans the full row!
//
// Revision 1.7  2004/11/08 00:46:26  assarbad
// - Added feature to distinguish compressed and encrypted files/folders by color as in the Windows 2000/XP explorer.
//   Same rules apply. (Green = encrypted / Blue = compressed)
//
// Revision 1.6  2004/11/05 16:53:06  assarbad
// Added Date and History tag where appropriate.
//
