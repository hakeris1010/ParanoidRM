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

struct TestCase{
    std::vector< std::wstring > params;
    std::wstring delString;

    TestCase( std::initializer_list< std::wstring >&& _pars, std::wstring&& _delstr )
        : params( std::move( _pars ) ), delString( std::move( _delstr ) )
    { }
};

static const std::vector< TestCase > testCases = {
    TestCase( { L"foo", L"bar", L"--", L"-baz" }, L"foo\0bar\0-baz\0\0"s ),
    TestCase( { L"-rf", L"bar", L"A:" }, L"bar\0\0"s ),
    TestCase( { L"B:\\", L"A:\\", L"A:/", L"O:", L"/" }, L"\0\0"s ),
};

int test_main(){

}



