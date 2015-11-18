#pragma once

#define TestExample(File) \
	TEST(Example##File) { testExample(L"examples/" #File ".principia"); }

#define TestExamples \
	TestExample(Ackermann) \
	TestExample(Church) \
	TestExample(Euclid) \
	TestExample(EvenOdd) \
	TestExample(Factorial) \
	TestExample(Fibonacci) \
	TestExample(Proof) \
	TestExample(Scott) \
	TestExample(Test)
