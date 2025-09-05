// Huge shoutout to https://github.com/OSM-Made 😁

#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/SystemService.h>
#include <orbis/UserService.h>

#include <vector>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace hidden /* Function Helper Methods */
{
    static bool initialized = false;

    static bool appRunning(int appId)
    {
        return (appId & ~0xFFFFFF) == 0x60000000;
    }

    std::vector<std::string> getTitleIdList()
    {
        std::vector<std::string> titleIds;
        std::vector<std::string> paths = {
            "/system_data/priv/appmeta/",
            "/system/vsh/app/"};

        for (const auto &basePath : paths)
        {
            DIR *dir = opendir(basePath.c_str());
            if (!dir)
                continue;

            struct dirent *entry;
            while ((entry = readdir(dir)) != nullptr)
            {
                if (entry->d_type == DT_DIR)
                {
                    std::string name = entry->d_name;
                    if (name != "." && name != ".." && name != "addcont" && name != "external")
                        titleIds.push_back(name);
                }
            }

            closedir(dir);
        }

        return titleIds;
    }
}

extern "C" /* SCE Function Definitions */
{
    int sceLncUtilInitialize();
    int sceLncUtilTerminate();

    int sceLncUtilGetAppIdOfBigApp();
    int sceLncUtilGetAppIdOfMiniApp();

    int sceLncUtilGetAppId(const char *);

    /* not implemented until after 5.05 */
    int sceLncUtilIsAppLaunched(const char *, bool *);
    int sceLncUtilIsAppSuspended(int, bool *);

    int sceLncUtilBlockAppSuspend(const char *);
    int sceLncUtilUnblockAppSuspend(const char *);

    int sceLncUtilSuspendApp(int, int);
    int sceLncUtilResumeApp(int, int);

    int sceLncUtilSetAppFocus(int, int);

    int sceLncUtilLaunchApp(const char *,
                            const char *[],
                            LncAppParam *);

    int sceLncUtilKillApp(int);
}

extern "C" /* Custom Wrapper Definitions */
{
    int initialize()
    {
        if (hidden::initialized)
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

        hidden::initialized = true;
        return 0;
    }

    int terminate()
    {
        if (!hidden::initialized)
            return 1;

        int libSystemService = sceUserServiceTerminate();
        int libUserService = sceLncUtilTerminate();

        if (libSystemService == 0 && libUserService == 0)
        {
            hidden::initialized = false;
            return 0;
        }

        return 1;
    }

    int getUserId()
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

    int getAppIdByTitleId(const char *titleId)
    {
        initialize();

        return sceLncUtilGetAppId(titleId);
    }

    const char *getTitleIdByAppId(int appId)
    {
        static std::string found;

        for (const auto &tid : hidden::getTitleIdList())
        {
            int tidAppId = getAppIdByTitleId(tid.c_str());
            if (tidAppId == appId)
            {
                found = tid;
                return found.c_str();
            }
        }

        return nullptr;
    }

    int sceLncUtilIsAppLaunched(const char *titleId, bool *state)
    {
        initialize();

        int appId = sceLncUtilGetAppId(titleId);
        *state = (appId & ~0xFFFFFF) == 0x60000000;
        return 0;
    }

    bool appLaunchedByTitleId(const char *titleId)
    {
        initialize();

        int appId = getAppIdByTitleId(titleId);
        return hidden::appRunning(appId);
    }

    bool appLaunchedByAppId(int appId)
    {
        initialize();

        return hidden::appRunning(appId);
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
        return sceLncUtilSuspendApp(appId, /*flag?*/ 0);
    }

    int suspendAppByAppId(int appId)
    {
        initialize();

        return sceLncUtilSuspendApp(appId, /*flag?*/ 0);
    }

    int resumeAppByTitleId(const char *titleId)
    {
        initialize();

        int appId = getAppIdByTitleId(titleId);
        return sceLncUtilResumeApp(appId, /*flag?*/ 0);
    }

    int resumeAppByAppId(int appId)
    {
        initialize();

        return sceLncUtilResumeApp(appId, /*flag?*/ 0);
    }

    int setAppFocusByTitleId(const char *titleId)
    {
        initialize();

        int appId = getAppIdByTitleId(titleId);
        return sceLncUtilSetAppFocus(appId, /*flag?*/ 0);
    }

    int setAppFocusByAppId(int appId)
    {
        initialize();

        return sceLncUtilSetAppFocus(appId, /*flag?*/ 0);
    }

    int launchAppByTitleId(const char *titleId, const char *argv[] = nullptr, LncAppParam *param = nullptr)
    {
        initialize();

        if (appLaunchedByTitleId(titleId))
            return 0;

        static const char *defaultArgv[] = {nullptr};
        if (!argv)
            argv = defaultArgv;

        LncAppParam defaultParam{};
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

    int launchAppByAppId(int appId, const char *argv[] = nullptr, LncAppParam *param = nullptr)
    {
        initialize();

        const char *titleId = getTitleIdByAppId(appId);

        if (appLaunchedByTitleId(titleId))
            return 0;

        static const char *defaultArgv[] = {nullptr};
        if (!argv)
            argv = defaultArgv;

        LncAppParam defaultParam{};
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
}

extern "C" /* Module Load/Unload Wrappers */
{
    int32_t __wrap__init(size_t, void *)
    {
        return initialize();
    }

    int32_t __wrap__fini(size_t, void *)
    {
        return terminate();
    }
}
