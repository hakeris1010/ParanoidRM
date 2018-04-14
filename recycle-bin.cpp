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

#include <cstdio>
#include <windows.h>
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

// Protected filename regex.
/*static std::wregex Protected_Regex = std::wregex( 
    L"(?:C:)"
);*/

// Check whether the file name passed belongs to the "protected files",
// such as the "C:\", C:\Users", and specific user dirs.
// @return 1 if protected, 0 if deletable.
int is_file_protected( const wchar_t* fn )
{
    // Linux root 
    if( !wcscmp( st, L"/" ) || !wcsncmp( st, L"/*", 2 ) ) 
        return 1;

    // Windows Disk absolute path. We need to be extra careful when using absolute paths.
    if( !wcsncmp( st + 1, L":/", 2 ) || !wcsncmp( st + 1, L":\\", 2 ) )
    {
        // If whole disk is being deleted:
        if( !wcscmp( st + 1, L":" ) || !wcscmp( st + 1, L":" ) ||
            !wcscmp( st + 1, L":/" ) || !wcscmp( st + 1, L":\\" ) )
            return 1;

        // If specific un-protected dirs are getting deletz0red.
        // TODO: Use regex. 
    }

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

int wmain(int argc, wchar_t **argv) 
{
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
        
        // Current param is file.
        len += wcslen( argv[i] );

        fileIndexesIncluded.push_back( i );
    }

    // Add every indexed file to the buffer.
    for( auto it = fileIndexesIncluded.begin(); it != fileIndexesIncluded.end(); ++it ){
        if( // GO go go go
    }

    /*
    wchar_t *from = malloc(len * sizeof(wchar_t));

    int pos = 0;

    for (int i = ignore_until; i < argc; i++) {
        wcscpy(&from[pos], argv[i]);
        pos += wcslen(argv[i]) + 1;
    }

    from[pos] = '\0';

    assert(++pos == len && "position/length mismatch");

    SHFILEOPSTRUCTW op;

    op.hwnd = NULL;
    op.wFunc = FO_DELETE;
    op.pFrom = from;
    op.pTo = NULL;
    op.fFlags = FOF_ALLOWUNDO | FOF_NO_UI;

    int ret = SHFileOperationW(&op);

    free(from);
    */

    return ret;
}



