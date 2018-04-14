/****************************************************************************
 *  GrylTools ParanoiD RM (WinDose Only)                                    *
 *  (Put files to Recycle-Bin)                                              *
 *  Version v0.3                                                            *
 *                                                                          *
 *  - Implements a full RM-compliant user interface: all RM switch params   *
 *    are accepted, however most of them are just ignored, because of       *
 *    several reasons:                                                      *
 *                                                                          *
 *  - WinAPI file deletion function SHFileOperationW() doesn't treat        *
 *    directories and files separately, so no -R switch is needed, or       *
 *    else, we would need to check every argument for being a directory.    *
 *                                                                          *
 *  - Good thing is that WinAPI function accepts '*' wildcards, expanding   *
 *    them internally. That saves a whole lot of wildcard development.      *
 *                                                                          *
 *  - Also, the '--' switch becomes obsolete, because the WinAPI function   *
 *    accepts any characters in incoming filenames.                         *  
 *                                                                          *
 ***************************************************************************/ 

#include <iostream>
#include <cstring>
#include <cassert>
#include <vector>
#include <regex>

#define TRASHBIN_VERSION "v0.3"

#define TRASHBIN_FILE_PARAM          0
#define TRASHBIN_NEXT_IS_NOT_SWITCH  11

#define TRASHBIN_SWITCH_SKIP         222
#define TRASHBIN_SWITCH_INTERACTIVE  3333
#define TRASHBIN_SWITCH_HELP         44444
#define TRASHBIN_SWITCH_VERSION      555555

#ifndef VERBOSITY
#define VERBOSITY 0
#endif

#define REGEX_HDD_START  L"(?:(?:(?:/[a-z])|(?:[a-z]:))[/\\\\]*)"
#define REGEX_HDD_SPECIFIC( let ) L"(?:(?:(?:/" let ")|(?:" let ":))[/\\\\]*)"

// Protected filename regex.
static const std::wregex Protected_Regex_Match = std::wregex( 
    L"^(?:"
    REGEX_HDD_START L"|"
    L"(?:" REGEX_HDD_START "?Users(?:[/\\\\]+[\\w\\-]*[/\\\\]*)?)|"
    L"(?:" REGEX_HDD_SPECIFIC( "e" ) "[/\\\\]+[\\w\\-\\.]+[/\\\\]*)"
    L")$"
    , std::regex_constants::icase
);

static const std::wregex Protected_Regex_Search = std::wregex();

void printVersion(){
    std::cout << "Paranoid-RM "<< TRASHBIN_VERSION <<" by Gryllotronics.\n";
}

void printHelp(){
    std::cout << "rm [OPTIONS...] [FILES...]\n\nPuts Files to TrashBin instead of"
              << " perma-deleting them. \nOptions:\n"
              << " -i, --interactive        Ask before deleting file.\n"
              << " --version                Print version.\n"
              << " --help                   Print this help message.\n\n"
              << "If file starts with a dash \'-\', use \'--\' before.\n\n";
}

// Check whether the file name passed belongs to the "protected files",
// such as the "C:\", C:\Users", and specific user dirs.
// @return 1 if protected, 0 if deletable.
int is_file_protected( const wchar_t* st )
{
    // Linux root 
    if( !wcscmp( st, L"/" ) || !wcsncmp( st, L"/*", 2 ) ) 
        return 1;

    // If specific un-protected dirs are getting deletz0red.
    if( std::regex_match( st, Protected_Regex_Match ) || 
        std::regex_search( st, Protected_Regex_Search ) )
        return 1; 

    // Windows Disk absolute path. We need to be extra careful when using absolute paths.
    /*if( !wcsncmp( st + 1, L":/", 2 ) || !wcsncmp( st + 1, L":\\", 2 ) )
    {
        // If whole disk is being deleted:
        if( !wcscmp( st + 1, L":" ) || !wcscmp( st + 1, L":" ) ||
            !wcscmp( st + 1, L":/" ) || !wcscmp( st + 1, L":\\" ) )
            return 1;
    }*/

    return 0; // File's deletable.
}

int get_param_type( const wchar_t* st )
{
    if( !wcsncmp( st, L"-", 1 ) ) // Argument starts!
    {
        // Check for usable arguments.
        // Help, or version.
        if( !wcscmp( st, L"--help" ) )
            return TRASHBIN_SWITCH_HELP;

        if( !wcscmp( st, L"--version" ) )
            return TRASHBIN_SWITCH_VERSION; 

        // Interactive mode 
        if( !wcscmp( st, L"-i" ) || !wcscmp( st, L"-I" ) || 
            !wcsncmp( st, L"--interactive", 13 ) )
            return TRASHBIN_SWITCH_INTERACTIVE;

        // -- - Ignore dash coming next.
        if( !wcscmp( st, L"--" ) ) 
            return TRASHBIN_NEXT_IS_NOT_SWITCH;

        // Anything else is considered a skippable switch.
        return TRASHBIN_SWITCH_SKIP;
    }
    // Not argument - file.
    return TRASHBIN_FILE_PARAM;
}

// Prints a string formatted, with Zeroes intact.
void printDeletionString( const wchar_t* start, const wchar_t* end )
{
    for( const wchar_t* it = start; it <= end; ++it ){
        if( *it == (wchar_t)0x0000 )
            std::wcout << L"0";
        std::wcout << *it;
    }
    std::wcout << L"\n";

    //size_t totalLen = (size_t)(end - start);
    //std::cout.write( (const char*)start, totalLen * sizeof(wchar_t) );
    //std::wcout << L"\n";
}

#ifndef TRASHBIN_TEST

// Windows function declarations.
#include <windows.h>

// Removes a string of files Win-Style.
int RemoveFiles( const wchar_t* from )
{
    SHFILEOPSTRUCTW op;

    op.hwnd = NULL;         // No parent window
    op.wFunc = FO_DELETE;   // File Operation: Delete
    op.pFrom = from;        // The Formatted Wide-String containing file names.
    op.pTo = NULL;          // No result string.
    op.fFlags = FOF_ALLOWUNDO | FOF_NO_UI;  // Allow Undo: Put file to Recycle-Bin
                                            // No-UI: Don't throw message boxes on errors.

    // Call the file operation function. Return value is 0 if good, ERRCODE if bad.
    return SHFileOperationW( &op ); 
}

/*int RemoveFiles( const wchar_t* from ){
    return 0;
}*/

#else

extern int test_main();
extern int RemoveFiles( const wchar_t* from );

#endif // TRASHBIN_TEST


int process_recycle( int argc, const wchar_t** argv) 
{
    const bool printIndividualFiles = ( VERBOSITY >= 2 );
    const bool printFormatString    = ( VERBOSITY >= 1 );

    if( argc == 1 ){
        printHelp();
        return 0;
    }

    bool interactiveMode = false;
    std::vector<size_t> fileIndexesIncluded;

    // Space for all extra Null-terminators.
    size_t len = argc;  
    bool currentIsFile = false;

    for (int i = 1; i < argc; i++ ) 
    {
        if( !currentIsFile ){
            // Check the type of param.
            switch( get_param_type( argv[ i ] ) ){
            case TRASHBIN_SWITCH_VERSION:
                printVersion();
                return 0;

            case TRASHBIN_SWITCH_HELP:
                printHelp();
                return 0;

            case TRASHBIN_NEXT_IS_NOT_SWITCH:
                currentIsFile = true;
                continue;

            case TRASHBIN_SWITCH_SKIP:
                continue;

            case TRASHBIN_SWITCH_INTERACTIVE:
                interactiveMode = true;
                continue;

            case TRASHBIN_FILE_PARAM:
            default: 
                break;
            }
        }
        else 
            currentIsFile = false;
        
        // Current param is file. Check if it's not protected.
        if( !is_file_protected( argv[ i ] ) ){
            len += wcslen( argv[i] );
            fileIndexesIncluded.push_back( i );
        }
    }

    // Allocate a buffer, and initialize with Zeros.
    std::vector< wchar_t > buffer( len + argc + 1, 0 );
    wchar_t* currPtr = &( buffer[0] );

    // Add every indexed file to the buffer.
    for( size_t i = 0; i < fileIndexesIncluded.size(); i++ ){
        const wchar_t* currFile = argv[ fileIndexesIncluded[ i ] ];

        if( interactiveMode ){
            std::wcout << L"Remove file \""<< currFile << L"\" ?  ";
            std::string st;
            std::cin >> st;
            if( !( st == "y" || st == "Y" || st == "yes" ) ){ // If not "yes", skip this file.
                continue;
            }
        }

        if( printIndividualFiles ){
            std::wcout << L"\""<< currFile << L"\" ";
            if( interactiveMode )
                std::wcout << L"\n";
        }

        // Current file must be deleted - add to buffer.
        wcscpy( currPtr, currFile );
        currPtr += wcslen( currFile ) + 1;
    }

    // Debugging verbosity
    if( printFormatString ){    
        printDeletionString( &buffer[0], currPtr );
    }

    return RemoveFiles( &buffer[0] );
}

// Main function. Calls test if specified.
int wmain( int argc, wchar_t** argv ) 
{
    #ifdef TRASHBIN_TEST
        return test_main();
    #else
        return process_recycle( argc, (const wchar_t**)argv );
    #endif 
}



