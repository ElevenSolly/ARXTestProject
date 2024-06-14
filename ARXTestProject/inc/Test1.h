#pragma once

void Test1();
void Test2();
void Test3();
void Test4();
void Test5();
void Test6();
void Test7();
void Test8();
void Test9();
void Test10();

#define ADD_COMMAND_1 \
	ADD_COMMAND(Test1);	\
	ADD_COMMAND(Test2);	\
	ADD_COMMAND(Test3);	\
	ADD_COMMAND(Test4);	\
	ADD_COMMAND(Test5);	\
	ADD_COMMAND(Test6);	\
	ADD_COMMAND(Test7);	\
	ADD_COMMAND(Test8);	\
	ADD_COMMAND(Test9);	\
	ADD_COMMAND(Test10);
