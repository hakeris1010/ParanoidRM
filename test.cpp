/****************************************************************************
 *  Tests the common cases of the recycle-bin.cpp process.                  *
 *  - Input: the command-line params                                        *
 *  - Output: the formatted files-to-delete string, which can be passed     *
 *            to the RemoveFiles() function.                                *
 *                                                                          *
 ***************************************************************************/ 

#include <iostream>
#include <cstring>
#include <vector>
#include <string>

using namespace std::string_literals;

const int VERBOSITY = 0;

struct TestCase{
    std::vector< std::wstring > params;
    std::wstring delString;

    TestCase( std::initializer_list< std::wstring >&& _pars, std::wstring&& _delstr )
        : params( std::move( _pars ) ), delString( std::move( _delstr ) )
    { }
};

static const std::vector< TestCase > testCases = {
    TestCase( { L"dooo" }, L"dooo\0\0"s ),
    TestCase( { L"foo", L"bar", L"--", L"-baz" }, L"foo\0bar\0-baz\0\0"s ),
    TestCase( { L"-rf", L"bar", L"A:" }, L"bar\0\0"s ),
    TestCase( { L"B:\\", L"A:\\", L"A:/", L"O:", L"/", L"/t/", L"/t//", L"O:\\\\\\/" }, L"\0\0"s ),
    TestCase( { L"/r/", L"r:", L"r:\\", L"/r", L"foo", L"/r/usersa/", L"/r/file", 
                L"/r/users", L"/r/users/nyan/haha", L"/e/kaka/" },
                L"foo\0/r/usersa/\0/r/file\0/r/users/nyan/haha\0\0"s )
};

static size_t currentTest = 0;

extern int process_recycle( int argc, const wchar_t** argv);
extern void printDeletionString( const wchar_t* start, const wchar_t* end );

// Mock the dependency.
int RemoveFiles( const wchar_t* from ){
    if( VERBOSITY > 0 )
        std::cout <<"[RemoveFiles] test # "<< currentTest <<"\n";

    auto&& currResult = testCases[ currentTest ].delString;
    if( wcsncmp( from, currResult.c_str(), currResult.size() ) ){
        if( VERBOSITY > 1 ){
            std::cout << "Expected: ";
            printDeletionString( currResult.c_str(), currResult.c_str() + currResult.size() );
            std::cout << "Got: ";
            printDeletionString( from, from + currResult.size() );
            std::cout << "\n";
        }

        throw std::runtime_error( "Test #"+std::to_string( currentTest ) );
    }
    return 0;
}

int test_main(){
    for( size_t i = 0; i < testCases.size(); i++ ){
        if( VERBOSITY > 1 )
            std::cout <<"Preparing test # "<< i <<"\n";
         
        currentTest = i;

        std::vector< const wchar_t* > params;
        params.push_back( nullptr ); // ProgName;
        for( auto&& p : testCases[i].params ){
            params.push_back( &p[0] );
        }

        process_recycle( (int)params.size(), (const wchar_t**)( &params[0] ) );
    }
}



