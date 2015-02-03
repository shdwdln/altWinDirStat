// globalhelpers.h	- Declaration of global helper functions
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

#include "stdafx.h"

#ifndef WDS_GLOBALHELPERS_H
#define WDS_GLOBALHELPERS_H




//should really just be a void* and a size_t?
template<typename type_struct_to_memset>
inline void memset_zero_struct( type_struct_to_memset& the_struct ) {
	static_assert( std::is_pod<type_struct_to_memset>::value, "can't memset a non-pod struct!" );
	static_assert( !std::is_polymorphic<type_struct_to_memset>::value, "can't memset a polymorphic type!" );
	static_assert( std::is_standard_layout<type_struct_to_memset>::value, "can't memset a non-standard layout struct!" );
	memset( &the_struct, 0, sizeof( the_struct ) );
	}

template<typename type_struct_to_init>
inline type_struct_to_init zero_init_struct( ) {
	static_assert( std::is_pod<type_struct_to_init>::value, "can't memset a non-pod struct!" );
	static_assert( !std::is_polymorphic<type_struct_to_init>::value, "can't memset a polymorphic type!" );
	static_assert( std::is_standard_layout<type_struct_to_init>::value, "can't memset a non-standard layout struct!" );
	static_assert( std::is_trivially_default_constructible<type_struct_to_init>::value, "can't memset a struct that isn't trivially default constructable!" );
	static_assert( std::is_trivially_copyable<type_struct_to_init>::value, "might have trouble returning a non-trivially-copyable item by value. You've been warned!" );
	type_struct_to_init the_struct;
	//memset( &the_struct, 0, sizeof( the_struct ) );
	memset_zero_struct( the_struct );
	return the_struct;
	}

void normalize_RECT_left_right( _Inout_ RECT& rect );

void normalize_RECT_top_bottom( _Inout_ RECT& rect );






//On returning E_FAIL, call GetLastError for details. That's not my idea!
_Success_( SUCCEEDED( return ) ) HRESULT CStyle_GetLastErrorAsFormattedMessage( WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_formatted_error, _In_range_( 128, 32767 ) const rsize_t strSize, _Out_ rsize_t& chars_written, const DWORD error = GetLastError( ) );

_Success_( SUCCEEDED( return ) ) HRESULT GetFullPathName_WriteToStackBuffer( _In_z_ PCWSTR relativePath, WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_full_path, _In_range_( 128, 512 ) const DWORD strSize, _Out_ rsize_t& chars_written );

_Success_( return ) bool MyQueryDosDevice           ( _In_z_ const PCWSTR             drive, _Out_ _Post_z_ wchar_t ( &info )[ 512u ] );

std::wstring dynamic_GetFullPathName( _In_z_ PCWSTR relativePath );





//WinDirStat string-formatting functions
namespace wds_fmt {
	void write_bad_fmt_msg( _Out_writes_z_( 41 ) _Pre_writable_size_( 42 ) _Post_readable_size_( chars_written ) PWSTR psz_fmt_msg, _Out_ rsize_t& chars_written );

	void FormatVolumeName( _In_ const std::wstring& rootPath, _In_z_ PCWSTR volumeName, _Out_ _Post_z_ _Pre_writable_size_( MAX_PATH + 1u ) PWSTR formatted_volume_name );

	_Success_( SUCCEEDED( return ) ) HRESULT CStyle_FormatLongLongHuman ( _In_ std::uint64_t n,       WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_formatted_LONGLONG_HUMAN, _In_range_( 8, 64 ) const rsize_t strSize, _Out_ rsize_t& chars_written );

	//maximum representable integral component of a double SEEMS to be 15 characters long, so we need at least 17
	_Success_( SUCCEEDED( return ) ) HRESULT CStyle_FormatDouble        ( _In_ const DOUBLE d,        WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_formatted_double, _In_range_( 17, 64 ) const rsize_t strSize, _Out_ rsize_t& chars_written );


	_Success_( SUCCEEDED( return ) ) HRESULT FormatBytes                ( _In_ const std::uint64_t n, WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_formatted_bytes, _In_range_( 38, 64 ) const rsize_t strSize, _Out_ rsize_t& chars_written );

	std::wstring FormatBytes           ( _In_ const std::uint64_t        n,                bool             humanFormat                      );


	_Success_( SUCCEEDED( return ) ) HRESULT CStyle_FormatFileTime  ( _In_ const FILETIME t,    _Out_writes_z_( strSize ) _Pre_writable_size_( strSize ) PWSTR psz_formatted_datetime, _In_range_( 128, 2048 ) const rsize_t strSize, _Out_ rsize_t& chars_written );

	_Success_( SUCCEEDED( return ) ) HRESULT CStyle_FormatAttributes( _In_ const attribs& attr, _Out_writes_z_( strSize ) _Pre_writable_size_( strSize ) PWSTR psz_formatted_attributes, _In_range_( 6, 18 ) const rsize_t strSize, _Out_ rsize_t& chars_written  );

	_Success_( SUCCEEDED( return ) ) HRESULT CStyle_GetNumberFormatted( const std::int64_t number, _Pre_writable_size_( strSize ) PWSTR psz_formatted_number, _In_range_( 21, 64 ) const rsize_t strSize, _Out_ rsize_t& chars_written );

	void write_BAD_FMT     ( _Out_writes_z_( 8 )  _Pre_writable_size_( 8 ) _Post_readable_size_( 8 ) PWSTR pszFMT, _Out_ rsize_t& chars_written );

	void write_MEM_INFO_ERR( _Out_writes_z_( 13 ) _Pre_writable_size_( 13 ) PWSTR psz_formatted_usage );

	void write_RAM_USAGE( _Out_writes_z_( 12 ) _Pre_writable_size_( 13 ) PWSTR psz_ram_usage );
	}


_Success_( SUCCEEDED( return ) ) const HRESULT allocate_and_copy_name_str( _Pre_invalid_ _Post_z_ _Post_readable_size_( new_name_length ) wchar_t*& new_name_ptr, _In_ _In_range_( 0, UINT16_MAX ) const rsize_t& new_name_length, const std::wstring& name );


                             bool DriveExists       ( _In_z_ _In_reads_( path_len ) const PCWSTR path, _In_ _In_range_( 0, 4 ) const rsize_t path_len );

_Success_( return != false ) bool GetVolumeName     ( _In_z_ const PCWSTR            rootPath,    _Out_ _Post_z_ wchar_t ( &volumeName )[ MAX_PATH + 1u ]                        );
_Success_( return != false ) bool GetVolumeName     ( _In_z_ const PCWSTR            rootPath );

//void check8Dot3NameCreationAndNotifyUser( );

void displayWindowsMsgBoxWithError  ( const DWORD error = GetLastError( ) );

void displayWindowsMsgBoxWithMessage( std::wstring message );

void displayWindowsMsgBoxWithMessage( PCWSTR message );

void MyGetDiskFreeSpace             ( _In_z_ const PCWSTR            pszRootPath, _Out_ _Out_range_( 0, 18446744073709551615 ) std::uint64_t& total, _Out_ _Out_range_( 0, 18446744073709551615 ) std::uint64_t& unused   );



const LARGE_INTEGER help_QueryPerformanceCounter( );
const LARGE_INTEGER help_QueryPerformanceFrequency( );

std::wstring EncodeSelection( _In_ const RADIO radio, _In_ const std::wstring folder, _In_ const std::vector<std::wstring>& drives );

RECT BuildRECT( const SRECT& in );

_Pre_satisfies_( min_val < max_val )
_Post_satisfies_( min_val <= val )
_Post_satisfies_( val <= max_val )
void CheckMinMax( _Inout_ LONG& val, _In_ const LONG min_val, _In_ const LONG max_val );

_Pre_satisfies_( min_val < max_val )
_Post_satisfies_( min_val <= val )
_Post_satisfies_( val <= max_val )
void CheckMinMax( _Inout_ LONG& val, _In_ const INT min_val, _In_ const INT max_val );

_Pre_satisfies_( min_val < max_val )
_Post_satisfies_( min_val <= val )
_Post_satisfies_( val <= max_val )
void CheckMinMax( _Inout_ INT& val,  _In_ const INT min_val, _In_ const INT max_val );


bool Compare_FILETIME_cast ( const FILETIME& t1,  const FILETIME& t2  ) ;
INT  Compare_FILETIME      ( const FILETIME& lhs, const FILETIME& rhs ) ;
bool Compare_FILETIME_eq   ( const FILETIME& lhs, const FILETIME& rhs ) ;

void DistributeFirst( _Inout_ _Out_range_( 0, 255 ) INT& first, _Inout_ _Out_range_( 0, 255 ) INT& second, _Inout_ _Out_range_( 0, 255 ) INT& third ) ;
void NormalizeColor( _Inout_ _Out_range_( 0, 255 ) INT& red, _Inout_ _Out_range_( 0, 255 ) INT& green, _Inout_ _Out_range_( 0, 255 ) INT& blue ) ;

namespace CColorSpace {

	// Gives a color a defined brightness.
	//static COLORREF MakeBrightColor( _In_ const COLORREF color, _In_ _In_range_(0, 1) const DOUBLE brightness );
	COLORREF MakeBrightColor( _In_ const COLORREF color, _In_ _In_range_(0, 1) const DOUBLE brightness );
	}





// $Log$
// Revision 1.15  2004/11/28 14:40:06  assarbad
// - Extended CFileFindWDS to replace a global function
// - Now packing/unpacking the file attributes. This even spares a call to find encrypted/compressed files.
//
// Revision 1.14  2004/11/25 21:13:38  assarbad
// - Implemented "attributes" column in the treelist
// - Adopted width in German dialog
// - Provided German, Russian and English version of IDS_TREECOL_ATTRIBUTES
//
// Revision 1.13  2004/11/25 11:58:52  assarbad
// - Minor fixes (odd behavior of coloring in ANSI version, caching of the GetCompressedFileSize API)
//   for details see the changelog.txt
//
// Revision 1.12  2004/11/12 13:19:44  assarbad
// - Minor changes and additions (in preparation for the solution of the "Browse for Folder" problem)
//
// Revision 1.11  2004/11/07 20:14:30  assarbad
// - Added wrapper for GetCompressedFileSize() so that by default the compressed file size will be shown.
//
// Revision 1.10  2004/11/05 16:53:07  assarbad
// Added Date and History tag where appropriate.
//
#endif
