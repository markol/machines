#include <iostream>
#include "base/prepost.hpp"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void BaseAssertion::preconditionFail(const char* exprStr, const char* file, const char* line)
{
    std::cerr << "BaseAssertion::preconditionFail disabled for unit testing." << std::endl;
    std::cerr << "\tPRE( " << exprStr << " ) in " << file << ":" << line << std::endl;
	return;
}

void BaseAssertion::postconditionFail(const char* exprStr, const char* file, const char* line)
{
    std::cerr << "BaseAssertion::postconditionFail disabled for unit testing." << std::endl;
    return;
}

void BaseAssertion::assertFileExists(const char* fileName, const char* file, const char* line)
{
    std::cerr << "BaseAssertion::assertFileExists disabled for unit testing." << std::endl;
    return;
}

void BaseAssertion::assertFail(const char *msg, const xmsg& xms)
{
    std::cerr << "BaseAssertion::assertFail disabled for unit testing." << std::endl;
    auto what = std::string{ xms.what() };
    std::cerr << "\tASSERT_FAIL( " << what << " ) - Message: " << msg << std::endl;

    return;
}

void BaseAssertion::invariantFail(const char* exprStr, const char* file, const char* line, const char* calledFromFile, const char* calledFromLine)
{
    std::cerr << "BaseAssertion::invariantFail disabled for unit testing." << std::endl;
    return;
}

#pragma GCC diagnostic warning "-Wunused-parameter"
