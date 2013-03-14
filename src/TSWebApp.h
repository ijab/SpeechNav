#ifdef WIN32

#if defined TSWEBAPP_EXPORTS || defined TSWebApp_EXPORTS
#define TSWEBAPP_EXPORTS __declspec(dllexport)
#else
#define TSWEBAPP_EXPORTS __declspec(dllimport)
#endif

#else
#define TSWEBAPP_EXPORTS
#endif               //WIN32