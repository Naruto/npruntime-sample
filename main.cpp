/*
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef __WINDOWS__
#include <windows.h>
int64_t
#endif
#include <npapi.h>
#include <npfunctions.h>
#include <npruntime.h>

#define EXPORT __attribute__((visibility("default")))
#define ARRAY_COUNT(array)      (sizeof(array) / sizeof(array[0]))

#ifdef ANDROID_PLUGINS
  #include "android_npapi.h"
  #include "ANPSurface_npapi.h"
  #include "ANPSystem_npapi.h"
  #define LOGI(...) gLogI.log(kDebug_ANPLogType, __VA_ARGS__)
  #define LOGE(...) gLogI.log(kError_ANPLogType, __VA_ARGS__)
  #define PLUGIN_DEBUG_LOG(...)                   \
    do {                                          \
      if(gLogI.inSize == sizeof(gLogI))           \
        LOGI(__VA_ARGS__);                        \
    } while(0)
  ANPAudioTrackInterfaceV0    gSoundI;
  ANPBitmapInterfaceV0        gBitmapI;
  ANPCanvasInterfaceV0        gCanvasI;
  ANPEventInterfaceV0         gEventI;
  ANPLogInterfaceV0           gLogI;
  ANPPaintInterfaceV0         gPaintI;
  ANPPathInterfaceV0          gPathI;
  ANPSurfaceInterfaceV0       gSurfaceI;
  ANPSystemInterfaceV0        gSystemI;
  ANPTypefaceInterfaceV0      gTypefaceI;
  ANPWindowInterfaceV0        gWindowI;

extern "C" {
EXPORT NPError NP_Initialize(NPNetscapeFuncs* browserFuncs,
                             NPPluginFuncs* pluginFuncs);
EXPORT NPError NP_GetValue(NPP instance, NPPVariable variable, void *value);
EXPORT const char* NP_GetMIMEDescription(void);
EXPORT void NP_Shutdown(void);
};
#elif XP_UNIX
  #define PLUGIN_DEBUG_LOG(...) printf(__VA_ARGS__)
  typedef unsigned short uint16_t;

  #ifdef __LP64__
//  typedef unsigned int uint32_t;
  #else /* __LP64__ */
//  typedef unsigned long uint32_t;
  #endif /* __LP64__ */

  typedef short int16_t;

  #ifdef __LP64__
//  typedef int int32_t;
  #else /* __LP64__ */
//  typedef long int32_t;
  #endif /* __LP64__ */

#else
  #define PLUGIN_DEBUG_LOG(...) printf(__VA_ARGS__)
  typedef unsigned short uint16_t;

  #ifdef __LP64__
//  typedef unsigned int uint32_t;
  #else /* __LP64__ */
//  typedef unsigned long uint32_t;
  #endif /* __LP64__ */

  typedef short int16_t;

  #ifdef __LP64__
//  typedef int int32_t;
  #else /* __LP64__ */
//  typedef long int32_t;
  #endif /* __LP64__ */
#endif  // ANDROID_PLUGINS


#define PLUGIN_NAME        "Test Plug-in"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " (NPAPI)"
#define PLUGIN_VERSION     "1.0.0.0"
#define PLUGIN_MIME        "application/x-test-plugin::Test Plug-in"

NPNetscapeFuncs* sBrowserFuncs;

NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc,
        char* argn[], char* argv[], NPSavedData* saved);
NPError NPP_Destroy(NPP instance, NPSavedData** save);
NPError NPP_SetWindow(NPP instance, NPWindow* window);
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream,
        NPBool seekable, uint16_t* stype);
NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32_t   NPP_WriteReady(NPP instance, NPStream* stream);
int32_t   NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len,
        void* buffer);
void    NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void    NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t   NPP_HandleEvent(NPP instance, void* event);
void    NPP_URLNotify(NPP instance, const char* URL, NPReason reason,
        void* notifyData);
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);

NPError
NP_Initialize(NPNetscapeFuncs* browserFuncs, NPPluginFuncs* pluginFuncs)
{
    // Make sure we have a function table equal or larger than we are built against.
    if (browserFuncs->size < sizeof(NPNetscapeFuncs)) {
        return NPERR_GENERIC_ERROR;
    }

    // Copy the function table (structure)
    sBrowserFuncs = browserFuncs;

    // Build the plugin function table
    pluginFuncs->version = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
    pluginFuncs->size = sizeof(NPPluginFuncs);
    pluginFuncs->newp = NPP_New;
    pluginFuncs->destroy = NPP_Destroy;
    pluginFuncs->setwindow = NPP_SetWindow;
    pluginFuncs->newstream = NPP_NewStream;
    pluginFuncs->destroystream = NPP_DestroyStream;
    pluginFuncs->asfile = NPP_StreamAsFile;
    pluginFuncs->writeready = NPP_WriteReady;
    pluginFuncs->write = (NPP_WriteProcPtr)NPP_Write;
    pluginFuncs->print = NPP_Print;
    pluginFuncs->event = NPP_HandleEvent;
    pluginFuncs->urlnotify = NPP_URLNotify;
    pluginFuncs->getvalue = NPP_GetValue;
    pluginFuncs->setvalue = NPP_SetValue;

#ifdef ANDROID_PLUGINS
    static const struct {
        NPNVariable     v;
        uint32_t        size;
        ANPInterface*   i;
    } gPairs[] = {
        { kAudioTrackInterfaceV0_ANPGetValue,   sizeof(gSoundI),    &gSoundI },
        { kBitmapInterfaceV0_ANPGetValue,       sizeof(gBitmapI),   &gBitmapI },
        { kCanvasInterfaceV0_ANPGetValue,       sizeof(gCanvasI),   &gCanvasI },
        { kEventInterfaceV0_ANPGetValue,        sizeof(gEventI),    &gEventI },
        { kLogInterfaceV0_ANPGetValue,          sizeof(gLogI),      &gLogI },
        { kPaintInterfaceV0_ANPGetValue,        sizeof(gPaintI),    &gPaintI },
        { kPathInterfaceV0_ANPGetValue,         sizeof(gPathI),     &gPathI },
        { kSurfaceInterfaceV0_ANPGetValue,      sizeof(gSurfaceI),  &gSurfaceI },
        { kSystemInterfaceV0_ANPGetValue,       sizeof(gSystemI),   &gSystemI },
        { kTypefaceInterfaceV0_ANPGetValue,     sizeof(gTypefaceI), &gTypefaceI },
        { kWindowInterfaceV0_ANPGetValue,       sizeof(gWindowI),   &gWindowI },
    };
    for (size_t i = 0; i < ARRAY_COUNT(gPairs); i++) {
        gPairs[i].i->inSize = gPairs[i].size;
        NPError err = sBrowserFuncs->getvalue(NULL, gPairs[i].v, gPairs[i].i);
        if (err) {
            return err;
        }
    }
#endif // ANDROID_PLUGINS

    return NPERR_NO_ERROR;
}

#ifdef ANDROID_PLUGINS

#else
char*
NP_GetPluginVersion()
{
  PLUGIN_DEBUG_LOG("Func: %s\n", __func__);
  return (char*)PLUGIN_VERSION;
}
#endif  // ANDROID_PLUGINS


#ifdef ANDROID_PLUGINS
void NP_Shutdown(void)
{
  PLUGIN_DEBUG_LOG("Func: %s\n", __func__);
  sBrowserFuncs = NULL;
  return;
}
#else
NPError NP_Shutdown(void)
{
  PLUGIN_DEBUG_LOG("Func: %s\n", __func__);
  sBrowserFuncs = NULL;
  return NPERR_NO_ERROR;
}
#endif

const char *NP_GetMIMEDescription(void)
{
  return PLUGIN_MIME;
}

#ifdef ANDROID_PLUGINS
EXPORT NPError NP_GetValue(NPP instance, NPPVariable variable, void *value)
#else
EXPORT NPError NP_GetValue(void* future, NPPVariable variable, void* value)
#endif  // ANDROID_PLUGINS
{
    if (variable == NPPVpluginNameString) {
       *((char**)value) = (char *)PLUGIN_NAME;
        return NPERR_NO_ERROR;
    }

    if (variable == NPPVpluginDescriptionString) {
        *((char**)value) = (char *)PLUGIN_DESCRIPTION;
        return NPERR_NO_ERROR;
    }

    return NPERR_GENERIC_ERROR;
}

typedef bool (*js_element_method_cb)(NPObject *obj, NPIdentifier methodName,
                                     const NPVariant *args, uint32_t argCount,
                                     NPVariant *result);
struct js_element_table_t {
  char  *method_name;
  js_element_method_cb func;
};
typedef struct js_element_table_t js_element_table_t;


// display Linux version
bool version(NPObject *obj, NPIdentifier methodName,
             const NPVariant *args, uint32_t argCount,
             NPVariant *result) {
  bool ret = false;

  if(argCount == 0) {
    char buf[256];
    FILE *fp = fopen("/proc/version", "r");
    if(!fp) goto end;
    memset(buf, 0, sizeof(buf));
    fread(buf, 256, 1, fp);
    fclose(fp);
    STRINGZ_TO_NPVARIANT(strdup(buf), *result);
    ret = true;
  }
 end:
  return ret;
}


// javascript method tables
static js_element_table_t js_element_table[] = {
  {(char*)"version", version},
};

static bool plugin_has_method(NPObject *obj, NPIdentifier methodName) {
  NPUTF8 *name = sBrowserFuncs->utf8fromidentifier(methodName);
  bool ret = false;
  PLUGIN_DEBUG_LOG("Func: %s name:%s\n", __func__, name);

  for(size_t i=0; i<ARRAY_COUNT(js_element_table); i++) {
    // PLUGIN_DEBUG_LOG("name:%s table_name:%s\n",
    //                  name, js_element_table[i].method_name);
    if(strcmp(name, js_element_table[i].method_name) == 0) {
      ret = true;
      break;
    }
  }

  sBrowserFuncs->memfree(name);
  return ret;
}

static bool plugin_invoke(NPObject *obj, NPIdentifier methodName,
                          const NPVariant *args, uint32_t argCount,
                          NPVariant *result) {
  // NPIdentifier でメソッドとプロパティを定義する。
  NPUTF8 *name = sBrowserFuncs->utf8fromidentifier(methodName);
  bool ret = false;
  PLUGIN_DEBUG_LOG("Func: %s name:%s\n", __func__, name);

  for(size_t i=0; i<ARRAY_COUNT(js_element_table); i++) {
    if(strcmp(name, js_element_table[i].method_name) == 0) {
      ret = js_element_table[i].func(obj, methodName,
                                     args, argCount,
                                     result);
      break;
    }
  }

  sBrowserFuncs->memfree(name);
  return ret;
}

static struct NPClass plugin_class = {
  NP_CLASS_STRUCT_VERSION,
  NULL,
  NULL,
  NULL,
  plugin_has_method,
  plugin_invoke,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc,
                char* argn[], char* argv[], NPSavedData* saved)
{
  NPError ret = NPERR_NO_ERROR;
  NPObject *plugin_instance = NULL;
  PLUGIN_DEBUG_LOG("Func: %s argc:%d\n", __func__, argc);

  plugin_instance = sBrowserFuncs->createobject(instance, &plugin_class);
  if(!plugin_instance) {
    // err
    PLUGIN_DEBUG_LOG("Func:%s line:%d err", __func__, __LINE__);
  }
  sBrowserFuncs->retainobject(plugin_instance);
  instance->pdata = plugin_instance;

  return ret;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
  NPObject *plugin_instance;
  PLUGIN_DEBUG_LOG("Func: %s\n", __func__);

  if(!instance)
    return NPERR_GENERIC_ERROR;
  plugin_instance = (NPObject *)instance->pdata;
  if(plugin_instance)
    sBrowserFuncs->releaseobject(plugin_instance);

  return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
    return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady(NPP instance, NPStream* stream)
{
    return 0;
}

int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
}

void NPP_Print(NPP instance, NPPrint* platformPrint)
{
}

int16_t NPP_HandleEvent(NPP instance, void* event)
{
  PLUGIN_DEBUG_LOG("Func: %s\n", __func__);
  return NPERR_NO_ERROR;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{

}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void* value)
{
  NPError ret = NPERR_NO_ERROR;
  NPObject *plugin_instance = NULL;
  PLUGIN_DEBUG_LOG("Func: %s variable:%d \n", __func__, variable);

  plugin_instance = (NPObject *)instance->pdata;
  switch(variable) {
  case NPPVpluginNeedsXEmbed:
    // *((bool*)value) =  false;
    *((bool*)value) =  true;    // for chrome
    break;
  case NPPVpluginScriptableNPObject: 
    {
      *(NPObject **)value = plugin_instance;
    }
    break;
  default:
    ret = NPERR_GENERIC_ERROR;
    ;
  }
  return ret;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    return NPERR_GENERIC_ERROR;
}

