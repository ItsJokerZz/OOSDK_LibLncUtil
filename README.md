# PS4 System Service Library

You can either load it and use dlsym or link it directly as a static library.

## Available Functions

- `int initialize();`  
- `int terminate();`
- `int getUserId();`  
  
<br>

- `int getAppIdOfApp(bool miniApp);`  
- `int getAppIdByTitleId(const char *titleId);`  
- `const char *getTitleIdByAppId(int appId);`  
  
<br>

- `int sceLncUtilIsAppLaunched(const char *titleId, bool *state);`  
- `bool appLaunchedByTitleId(const char *titleId);`  
- `bool appLaunchedByAppId(int appId);`  
  
<br>

- `bool appSuspendedByTitleId(const char *titleId);`  
- `bool appSuspendedByAppId(int appId);`  
  
<br>

- `int blockSuspendingByTitleId(const char *titleId);`  
- `int blockSuspendingByAppId(int appId);`  
  
<br>

- `int suspendAppByTitleId(const char *titleId);`  
- `int suspendAppByAppId(int appId);`  
  
<br>

- `int resumeAppByTitleId(const char *titleId);`  
- `int resumeAppByAppId(int appId);`  
  
<br>

- `int setAppFocusByTitleId(const char *titleId);`  
- `int setAppFocusByAppId(int appId);`  
  
<br>

- `int launchAppByTitleId(const char *titleId, const char *argv[] = nullptr, LncAppParam *param = nullptr);`  
- `int launchAppByAppId(int appId, const char *argv[] = nullptr, LncAppParam *param = nullptr);`  
  
<br>

- `int killAppByTitleId(const char *titleId);`  
- `int killAppByAppId(int appId);`  
