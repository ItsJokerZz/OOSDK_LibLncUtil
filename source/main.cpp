#include <orbis/libkernel.h>
#include <orbis/SystemService.h>
#include <orbis/UserService.h>

#include <string.h>

static bool initialized = false;

extern "C"
{
    int32_t sceLncUtilInitialize(void);
    int32_t sceLncUtilGetAppId(const char *title_id);
    int32_t sceLncUtilLaunchApp(const char *title_id,
                                const char *argv[],
                                LncAppParam *param);

    int initialize()
    {
        if (initialized)
            return 0;

        int libSystemService = sceKernelLoadStartModule("/system/common/lib/libSceSystemService.sprx", 0, 0, 0, 0, 0);
        int libUserService = sceKernelLoadStartModule("/system/common/lib/libSceUserService.sprx", 0, 0, 0, 0, 0);

        if (libSystemService >= 0)
        {
            OrbisUserServiceInitializeParams userServiceParams{.priority = 256};
            sceUserServiceInitialize(&userServiceParams);
        }

        if (libUserService >= 0)
            sceLncUtilInitialize();

        if (libSystemService >= 0 && libUserService >= 0)
            initialized = true;

        return 0;
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

    int getAppId(const char *titleId)
    {
        initialize();
        return sceLncUtilGetAppId(titleId);
    }

    int launchApp(const char *titleId)
    {
        initialize();

        LncAppParam params;
        memset(&params, 0, sizeof(params));
        params.size = sizeof(LncAppParam);
        params.user_id = getUserId();
        params.app_opt = 0;
        params.crash_report = 0;
        params.LaunchAppCheck_flag = LaunchApp_SkipSystemUpdate;

        return sceLncUtilLaunchApp(titleId, nullptr, &params);
    }

    bool appRunning(const char *titleId)
    {
        initialize();

        return (getAppId(titleId) &
                ~0xFFFFFF) == 0x60000000;
    }
}
