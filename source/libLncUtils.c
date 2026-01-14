// Huge shoutout to https://github.com/OSM-Made 😁

#include "libLncUtils.h"

bool initialized = false;

int sceLncUtilIsAppLaunched(const char *titleId, bool *state)
{
    initialize();

    int appId = sceLncUtilGetAppId(titleId);
    *state = (appId & ~0xFFFFFF) == 0x60000000;
    return 0;
}

char **getTitleIdList(int *count)
{
    const char *paths[] = {
        "/system_data/priv/appmeta/",
        "/system/vsh/app/"};
    const int numPaths = 2;

    char **titleIds = NULL;
    int capacity = 0;
    int size = 0;

    for (int i = 0; i < numPaths; i++)
    {
        DIR *dir = opendir(paths[i]);
        if (!dir)
            continue;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR)
            {
                const char *name = entry->d_name;
                if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0 &&
                    strcmp(name, "addcont") != 0 && strcmp(name, "external") != 0)
                {
                    if (size >= capacity)
                    {
                        capacity = capacity == 0 ? 16 : capacity * 2;
                        titleIds = realloc(titleIds, capacity * sizeof(char *));
                    }

                    titleIds[size] = malloc(strlen(name) + 1);
                    strcpy(titleIds[size], name);
                    size++;
                }
            }
        }
        closedir(dir);
    }

    *count = size;
    return titleIds;
}

void freeTitleIdList(char **titleIds, int count)
{
    if (titleIds)
    {
        for (int i = 0; i < count; i++)
            free(titleIds[i]);

        free(titleIds);
    }
}

int initialize(void)
{
    if (initialized)
        return 0;

    bool success = true;
    int result;

    result = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
    success &= (result != 0);
    result = sceKernelLoadStartModule("/system/common/lib/libSceSystemService.sprx", 0, 0, 0, 0, 0);
    success &= (result > 0);

    if (success)
    {
        OrbisUserServiceInitializeParams userServiceParams;
        userServiceParams.priority = ORBIS_KERNEL_PRIO_FIFO_HIGHEST;
        sceUserServiceInitialize(&userServiceParams);
    }

    result = sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE);
    success &= (result != 0);
    result = sceKernelLoadStartModule("/system/common/lib/libSceUserService.sprx", 0, 0, 0, 0, 0);
    success &= (result > 0);

    if (success)
        sceLncUtilInitialize();
    else
        return 1;

    initialized = true;
    return 0;
}

int terminate(void)
{
    if (!initialized)
        return 1;

    int libSystemService = sceUserServiceTerminate();
    int libUserService = sceLncUtilTerminate();

    if (libSystemService == 0 && libUserService == 0)
    {
        initialized = false;
        return 0;
    }

    return 1;
}

int getUserId(void)
{
    initialize();

    int32_t userId = -1;
    sceUserServiceGetForegroundUser(&userId);

    if (userId == -1)
        userId = 0;

    return userId;
}

int getAppIdOfApp(bool miniApp)
{
    initialize();

    if (miniApp)
        return sceLncUtilGetAppIdOfMiniApp();

    return sceLncUtilGetAppIdOfBigApp();
}

int getAppIdByPid(int pid)
{
    initialize();

    /* !! UNIMPLEMENTED !! */

    return -1;
}

int getAppIdByTitleId(const char *titleId)
{
    initialize();
    return sceLncUtilGetAppId(titleId);
}

const char *getTitleIdByAppId(int appId)
{
    static char found[256] = {0};
    int count;
    char **titleIds = getTitleIdList(&count);

    if (!titleIds)
        return NULL;

    for (int i = 0; i < count; i++)
    {
        int tidAppId = getAppIdByTitleId(titleIds[i]);
        if (tidAppId == appId)
        {
            strncpy(found, titleIds[i], sizeof(found) - 1);
            found[sizeof(found) - 1] = '\0';
            freeTitleIdList(titleIds, count);
            return found;
        }
    }

    freeTitleIdList(titleIds, count);
    return NULL;
}

bool appLaunchedByTitleId(const char *titleId)
{
    initialize();

    bool state = false;
    sceLncUtilIsAppLaunched(titleId, &state);

    return state;
}

bool appLaunchedByAppId(int appId)
{
    const char *titleId = getTitleIdByAppId(appId);
    if (!titleId)
        return false;

    bool state = false;
    sceLncUtilIsAppLaunched(titleId, &state);

    return state;
}

bool appSuspendedByTitleId(const char *titleId)
{
    initialize();

    bool state = false;
    int appId = getAppIdByTitleId(titleId);
    sceLncUtilIsAppSuspended(appId, &state);
    return state;
}

bool appSuspendedByAppId(int appId)
{
    initialize();

    bool state = false;
    sceLncUtilIsAppSuspended(appId, &state);
    return state;
}

int blockSuspendingByTitleId(const char *titleId)
{
    initialize();
    return sceLncUtilBlockAppSuspend(titleId);
}

int blockSuspendingByAppId(int appId)
{
    initialize();

    const char *titleId = getTitleIdByAppId(appId);
    return sceLncUtilBlockAppSuspend(titleId);
}

int suspendAppByTitleId(const char *titleId)
{
    initialize();

    int appId = getAppIdByTitleId(titleId);
    return sceLncUtilSuspendApp(appId, 0);
}

int suspendAppByAppId(int appId)
{
    initialize();
    return sceLncUtilSuspendApp(appId, 0);
}

int resumeAppByTitleId(const char *titleId)
{
    initialize();

    int appId = getAppIdByTitleId(titleId);
    return sceLncUtilResumeApp(appId, 0);
}

int resumeAppByAppId(int appId)
{
    initialize();
    return sceLncUtilResumeApp(appId, 0);
}

int setAppFocusByTitleId(const char *titleId)
{
    initialize();

    int appId = getAppIdByTitleId(titleId);
    return sceLncUtilSetAppFocus(appId, 0);
}

int setAppFocusByAppId(int appId)
{
    initialize();
    return sceLncUtilSetAppFocus(appId, 0);
}

int launchAppByTitleId(const char *titleId, const char *argv[], LncAppParam *param)
{
    initialize();

    if (appLaunchedByTitleId(titleId))
        return 0;

    static const char *defaultArgv[] = {NULL};
    if (!argv)
        argv = defaultArgv;

    LncAppParam defaultParam;
    if (!param)
    {
        memset(&defaultParam, 0, sizeof(defaultParam));
        defaultParam.size = sizeof(LncAppParam);
        defaultParam.user_id = getUserId();
        defaultParam.app_opt = 0;
        defaultParam.crash_report = 0;
        defaultParam.LaunchAppCheck_flag = LaunchApp_None;
        param = &defaultParam;
    }

    return sceLncUtilLaunchApp(titleId, argv, param);
}

int launchAppByAppId(int appId, const char *argv[], LncAppParam *param)
{
    initialize();

    const char *titleId = getTitleIdByAppId(appId);

    if (appLaunchedByTitleId(titleId))
        return 0;

    static const char *defaultArgv[] = {NULL};
    if (!argv)
        argv = defaultArgv;

    LncAppParam defaultParam;
    if (!param)
    {
        memset(&defaultParam, 0, sizeof(defaultParam));
        defaultParam.size = sizeof(LncAppParam);
        defaultParam.user_id = getUserId();
        defaultParam.app_opt = 0;
        defaultParam.crash_report = 0;
        defaultParam.LaunchAppCheck_flag = LaunchApp_None;
        param = &defaultParam;
    }

    return sceLncUtilLaunchApp(titleId, argv, param);
}

int killAppByTitleId(const char *titleId)
{
    initialize();

    int appId = getAppIdByTitleId(titleId);
    if (appLaunchedByAppId(appId))
    {
        sceLncUtilKillApp(appId);
        return 0;
    }

    return 1;
}

int killAppByAppId(int appId)
{
    initialize();

    if (appLaunchedByAppId(appId))
    {
        sceLncUtilKillApp(appId);
        return 0;
    }

    return 1;
}
