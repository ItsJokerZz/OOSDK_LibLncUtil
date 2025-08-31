# PS4 System Service Library

You can either load it and use dlsym or link it directly as a static library.

## Available Functions

- ```int initialize();```  
  Initializes the library and required system modules.

- `int getUserId();`  
  Retrieves the current user ID.

- `int getAppId(const char *titleId);`  
  Returns the application ID for a given title.

- `int launchApp(const char *titleId);`  
  Launches the specified application.

- `bool appRunning(const char *titleId);`  
  Checks whether the specified application is currently running.
