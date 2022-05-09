#include "../../common/header/common.h"

int TestSys()
{
	char buf[1024];

	char* home = Sys_GetHomeDir();
	printf("0 Sys_GetHomeDir: '%s'\n", home);

	Sys_GetWorkDir(buf, 1024);

	printf("0 Sys_GetWorkDir: '%s'\n", buf);

	Sys_SetWorkDir("/dev_hdd0/game");

	Sys_GetWorkDir(buf, 1024);

	printf("1 Sys_GetWorkDir: '%s'\n", buf);

	qboolean ret = Sys_Realpath("./QUAKE2PORT", &buf[0], 1024);

	printf("Sys_Realpath(.) == %d : '%s'\n", ret, buf);
	

	Sys_Rename("/dev_hdd0/game/test/file2.test", "/dev_hdd0/game/test/file.test");

	printf("0 Sys_IsDir(/dev_hdd0/game/create_folder): %d\n", Sys_IsDir("/dev_hdd0/game/create_folder"));

	Sys_Mkdir("/dev_hdd0/game/create_folder");

	printf("1 Sys_IsDir(/dev_hdd0/game/create_folder): %d\n", Sys_IsDir("/dev_hdd0/game/create_folder"));

	printf("2 Sys_IsDir(/dev_hdd0/game/test/file.test): %d\n", Sys_IsDir("/dev_hdd0/game/test/file.test"));

	printf("0 Sys_IsFile(/dev_hdd0/game/create_folder): %d\n", Sys_IsFile("/dev_hdd0/game/create_folder"));

	printf("1 Sys_IsFile(/dev_hdd0/game/test/file.test): %d\n", Sys_IsFile("/dev_hdd0/game/test/file.test"));

	//Sys_RemoveDir("/dev_hdd0/game/create_folder");
	//printf("3 Sys_IsDir(/dev_hdd0/game/create_folder): %d\n", Sys_IsDir("/dev_hdd0/game/create_folder"));

	Sys_Rename("/dev_hdd0/game/test/file.test", "/dev_hdd0/game/test/file2.test");

	// Sys_Remove("/dev_hdd0/game/test/delme.file");

	printf("TestSys done\n");
	return 0;
}