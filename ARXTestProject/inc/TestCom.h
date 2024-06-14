#pragma once

void TestCom1();
void TestCom2();
void TestCom3();
void TestCom4();
void TestCom5();
void TestCom6();
void TestCom7();
void TestCom8();
void TestCom9();
void TestCom10();

#define ADD_COMMAND_COM \
	ADD_COMMAND(TestCom1);	\
	ADD_COMMAND(TestCom2);	\
	ADD_COMMAND(TestCom3);	\
	ADD_COMMAND(TestCom4);	\
	ADD_COMMAND(TestCom5);	\
	ADD_COMMAND(TestCom6);	\
	ADD_COMMAND(TestCom7);	\
	ADD_COMMAND(TestCom8);	\
	ADD_COMMAND(TestCom9);	\
	ADD_COMMAND(TestCom10);

