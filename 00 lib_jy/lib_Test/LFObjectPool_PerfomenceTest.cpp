#include <iostream>
#include <Windows.h>
#include <thread>
#include "../CrashDump.h"
#include "../LFObjectPoolTLS.h"
#pragma comment(lib, "winmm.lib")

#define TEST_TYPE 0

#define OBJECT_SIZE 1024
#define LOOP_COUNT 3
#define ALLOC_COUNT 1000000
#define THREAD_COUNT 6

struct Object {
	char v[OBJECT_SIZE];
};

void HeapPerformanceTest() {
	Object ** objectArr = (Object**)malloc(sizeof(Object*) * ALLOC_COUNT);

	auto startTime = timeGetTime();
	for(int i=0;i< LOOP_COUNT;i++){
		for (int j = 0; j < ALLOC_COUNT; j++) {
			objectArr[j] = new Object;
		}
		for (int j = 0; j < ALLOC_COUNT; j++) {
			delete objectArr[j];
		}
	}
	auto profileTime = timeGetTime() - startTime;
	free(objectArr);
	printf("profile Time : %d \n", profileTime);
}

LFObjectPoolTLS<Object> TLSPool;
void TLSPoolPerformanceTest() {
	Object** objectArr = (Object**)malloc(sizeof(Object*) * ALLOC_COUNT);

	auto startTime = timeGetTime();
	for (int i = 0; i < LOOP_COUNT; i++) {
		for (int j = 0; j < ALLOC_COUNT; j++) {
			objectArr[j] = TLSPool.Alloc();
		}
		for (int j = 0; j < ALLOC_COUNT; j++) {
			TLSPool.Free(objectArr[j]);
		}
	}
	auto profileTime = timeGetTime() - startTime;
	free(objectArr);
	printf("profile Time : %d \n", profileTime);
}

LFObjectPool<Object> Pool;
void PoolPerformanceTest() {
	Object** objectArr = (Object**)malloc(sizeof(Object*) * ALLOC_COUNT);

	auto startTime = timeGetTime();
	for (int i = 0; i < LOOP_COUNT; i++) {
		for (int j = 0; j < ALLOC_COUNT; j++) {
			objectArr[j] = Pool.Alloc();
		}
		for (int j = 0; j < ALLOC_COUNT; j++) {
			Pool.Free(objectArr[j]);
		}
	}
	auto profileTime = timeGetTime() - startTime;
	free(objectArr);
	printf("profile Time : %d \n", profileTime);
}


void ObjectPoolTest() {
#if TEST_TYPE == 0
	printf("New/Delete \n");
#elif TEST_TYPE == 1
	printf("Pool \n");
#else 
	printf("TLSPool \n");
#endif

	std::thread testThread[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; i++) {
		#if TEST_TYPE == 0
		testThread[i] = std::thread(HeapPerformanceTest);
		#elif TEST_TYPE == 1
		testThread[i] = std::thread(PoolPerformanceTest);
		#else 
		testThread[i] = std::thread(TLSPoolPerformanceTest);
		#endif
	}
	// join
	for (int i = 0; i < THREAD_COUNT; i++) {
		if (testThread[i].joinable())
			testThread[i].join();
	}
}