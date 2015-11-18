#include <Utilities/inttypes.h>
#include <Utilities/ansi.h>
#include <Utilities/pretty_print.h>
#include <Unicode/string.h>
#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporter.h>
#include <iostream>
#include <vector>

SUITE(TestUnitTest) {

TEST(SanityTest)
{
	CHECK_EQUAL(1,1);
}

TEST(TestExceptions)
{
	CHECK_THROW(throw std::runtime_error("Catch this"), std::runtime_error);
}

}

class MyTestReporter: public UnitTest::TestReporter
{
private:
	virtual void ReportTestStart(UnitTest::TestDetails const& test);
	virtual void ReportFailure(UnitTest::TestDetails const& test, char const* failure);
	virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed);
	virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
	void ReportStartSuite(const char* suite);
	void ReportEndSuite(const char* suite);
	
private:
	const char* _currentSuite = nullptr;
	bool _currentFailed = false;
};

void MyTestReporter::ReportStartSuite(const char* suite)
{
	using namespace std;
	wcerr << Ansi::bold << "Test suite " << suite << L":" << Ansi::normal << endl;
}

void MyTestReporter::ReportEndSuite(const char* suite)
{
}

void MyTestReporter::ReportTestStart(UnitTest::TestDetails const& test)
{
	using namespace std;
	if(_currentSuite != test.suiteName) {
		if(_currentSuite != nullptr)
			ReportEndSuite(_currentSuite);
		_currentSuite = test.suiteName;
		ReportStartSuite(_currentSuite);
	}
	_currentFailed = false;
	wcerr << L"Test " << test.testName << L"…" << flush;
}

void MyTestReporter::ReportFailure(UnitTest::TestDetails const& details, char const* failure)
{
	using namespace std;
	using namespace Ansi;
	if(_currentFailed == false) {
		wcerr << L"\b ";
		wcerr << boldRed << "FAILED" << normal << ":" << endl;
		_currentFailed  = true;
	}
	wcerr << " * " << details.filename << ":" << details.lineNumber << ": ";
	wcerr << "error: Failure in " << details.testName << ": "  << failure << endl;
}

void MyTestReporter::ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed)
{
	using namespace std;
	using namespace Ansi;
	if(_currentFailed == false) {
		wcerr << "\b ";
		wcerr << boldGreen << "succeeded" << normal;
		wcerr << " in " << (secondsElapsed * 1.0e3) << " ms." << endl;
	}
}

void MyTestReporter::ReportSummary(
	int const totalTestCount, int const failedTestCount,
	int const failureCount, float const secondsElapsed)
{
	using namespace std;
	using namespace Ansi;
	if(_currentSuite != nullptr)
		ReportEndSuite(_currentSuite);
	wcerr << endl;
	wcerr << "Total test time: " << (secondsElapsed * 1.0e3) << " ms." << endl;
	if (failureCount > 0) {
		wcerr << boldRed << "FAILURE" << normal << ": ";
		wcerr << failedTestCount << " out of " << totalTestCount;
		wcerr << " tests failed (" << failureCount << " failures)." << endl;
	} else {
		wcerr << boldGreen << "Success" << normal << ": ";
		wcerr << totalTestCount << " tests passed." << endl;
	}
}

sint32 Main(const std::vector<string>& args)
{
	using namespace std;
	using namespace UnitTest;
	wcerr << "Unittest++ " << PROGRAM << " version " << VERSION << endl;
	MyTestReporter myReporter;
	TestRunner runner(myReporter);
	return runner.RunTestsIf(Test::GetTestList(), NULL, True(), 0);
}
