#pragma once

#include "ThreadPool.h"
#include "Storage.h"
#include "Provider.h"
#include "Assembler.h"
#include "Seller.h"
#include "Shovel.h"
#include "ShovelBlade.h"
#include "ShovelHandle.h"
#include "ControllerShovelStorage.h"

#define storageBladeCount 70
#define storageHandleCount 45
#define storageShovelCount 10

#define providerBladeSpeed 1350
#define providerHandleSpeed 1000
#define assemblerSpeed 1500
#define sellerSpeed 3000
#define controllerSpeed 1000

#define providerBladeCount 2
#define providerHandleCount 2
#define assemblerCount 2
#define sellerCount 1
#define ZaVOdWorkerCount 4

int main()
{
	Storage<ShovelBlade> storageBlade(storageBladeCount);
	Storage<ShovelHandle> storageHandle(storageHandleCount);
	Storage<Shovel> storageShovel(storageShovelCount);
	mutex assemblerMutex;

	for (size_t i = 0; i < providerBladeCount; i++)
	{
		Provider<ShovelBlade> providerShovelBlade(storageBlade, providerBladeSpeed);
		providerShovelBlade.StartWork();
	}
	for (size_t i = 0; i < providerHandleCount; i++)
	{
		Provider<ShovelHandle> providerShovelHandle(storageHandle, providerHandleSpeed);
		providerShovelHandle.StartWork();
	}
	/*for (size_t i = 0; i < assemblerCount; i++)
	{
		Assembler assembler(storageShovel, storageBlade, storageHandle, assemblerSpeed);
		assembler.StartWork();
	}*/
	for (size_t i = 0; i < sellerCount; i++)
	{
		Seller seller(storageShovel, sellerSpeed);
		seller.StartWork();
	}
	ThreadPool<RequestShovelTask> ZaVOd(ZaVOdWorkerCount);

	ControllerShovelStorage controller(ZaVOd, storageShovel, storageBlade, storageHandle, controllerSpeed);
	controller.StartWork();

	while (true)
	{
		cout << "Blade storage: " << storageBlade.Count() << endl;
		cout << "Handle storage: " << storageHandle.Count() << endl;
		cout << "Shovel storage: " << storageShovel.Count() << endl;

		cout << "Completed task count: " << ZaVOd.TotalCompletedTasks() << endl;

		this_thread::sleep_for(chrono::milliseconds(500));
	}

}
