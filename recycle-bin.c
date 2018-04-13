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
//#include <pcre.h>

#define TRASHBIN_VERSION "v0.3"

#define TRASHBIN_FILE_PARAM          0
#define TRASHBIN_NEXT_IS_NOT_SWITCH  11

#define TRASHBIN_SWITCH_SKIP         222
#define TRASHBIN_SWITCH_INTERACTIVE  3333
#define TRASHBIN_SWITCH_HELP         44444
#define TRASHBIN_SWITCH_VERSION      555555

const wchar_t TRASHBIN_protected_file_regex = "

// Prepares protected directory regex.
void prepare_protected_regex

// Check whether the file name passed belongs to the "protected files",
// such as the "C:\", C:\Users", and specific user dirs.
// @return 1 if protected, 0 if deletable.
int isFileProtected( const wchar_t* fn )
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

int paramType( const wchar_t* st ){
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
    size_t ignore_until = 1;

    /*printf("argc: %d\n\n", argc);
    for( size_t i = 0; i < argc; i++ ){
        printf("%d : %d\n", i, wcslen(argv[i]));
    }

    return 0;*/

    if (argc == 1) {
		puts("Specify at least one path");
		return 1;
	}

	if (argc >= 2) {
		if (wcscmp(argv[1], L"--version") == 0) {
			puts("1.0.1");
			return 0;
		}

		if (wcscmp(argv[1], L"--help") == 0) {
			puts("\n  Move files and folders to the recycle bin\n\n  Usage: recycle-bin <path> [...]\n\n  Created by Sindre Sorhus");
			return 0;
		}

		if ( !wcscmp(argv[1], L"-r") || !wcscmp(argv[1], L"-f") ||
             !wcscmp(argv[1], L"-fr") || !wcscmp(argv[1], L"-rf") ) 
        { 
            ignore_until = 2;
        }
	}

    if( argc >= 3 && ignore_until > 1 ){
        if ( !wcscmp(argv[2], L"-r") || !wcscmp(argv[2], L"-f") ||
             !wcscmp(argv[2], L"-fr") || !wcscmp(argv[2], L"-rf") ) 
        { 
            ignore_until = 3;
        }
    }

	size_t len = argc - (ignore_until-1);

	for (int i = ignore_until; i < argc; i++) {
		len += wcslen(argv[i]);
	}

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

	return ret;
}
