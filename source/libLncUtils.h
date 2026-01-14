#pragma once

#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/SystemService.h>
#include <orbis/UserService.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int sceLncUtilInitialize(void);
int sceLncUtilTerminate(void);
int sceLncUtilGetAppIdOfBigApp(void);
int sceLncUtilGetAppIdOfMiniApp(void);
int sceLncUtilGetAppId(const char *);
int sceLncUtilIsAppLaunched(const char *, bool *);
int sceLncUtilIsAppSuspended(int, bool *);
int sceLncUtilBlockAppSuspend(const char *);
int sceLncUtilUnblockAppSuspend(const char *);
int sceLncUtilSuspendApp(int, int);
int sceLncUtilResumeApp(int, int);
int sceLncUtilSetAppFocus(int, int);
int sceLncUtilLaunchApp(const char *, const char *[], LncAppParam *);
int sceLncUtilKillApp(int);

int initialize(void);
int terminate(void);

int getUserId(void);

int getAppIdOfApp(bool miniApp);

int getAppIdByTitleId(const char *titleId);
const char *getTitleIdByAppId(int appId);

bool appLaunchedByTitleId(const char *titleId);
bool appLaunchedByAppId(int appId);

bool appSuspendedByTitleId(const char *titleId);
bool appSuspendedByAppId(int appId);

int blockSuspendingByTitleId(const char *titleId);
int blockSuspendingByAppId(int appId);

int suspendAppByTitleId(const char *titleId);
int suspendAppByAppId(int appId);

int resumeAppByTitleId(const char *titleId);
int resumeAppByAppId(int appId);

int setAppFocusByTitleId(const char *titleId);
int setAppFocusByAppId(int appId);

int launchAppByTitleId(const char *titleId, const char *argv[], LncAppParam *param);
int launchAppByAppId(int appId, const char *argv[], LncAppParam *param);

int killAppByTitleId(const char *titleId);
int killAppByAppId(int appId);
