#ifndef IUPWEBGTK_DLOPEN_H
#define IUPWEBGTK_DLOPEN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TODO: Eventually gtk itself could be dynamically discovered to handle different versions or even different non-GTK backends. */
#include <gtk/gtk.h>
#include <dlfcn.h>

typedef struct WebKitWebBackForwardList WebKitWebBackForwardList;
typedef struct WebKitWebHistoryItem WebKitWebHistoryItem;
typedef struct WebKitWebFrame WebKitWebFrame;
typedef struct WebKitWebView WebKitWebView;
typedef struct WebKitNetworkRequest WebKitNetworkRequest;
typedef struct WebKitWebNavigationAction WebKitWebNavigationAction;
typedef struct WebKitWebPolicyDecision WebKitWebPolicyDecision;

typedef enum {
    WEBKIT_LOAD_PROVISIONAL,
    WEBKIT_LOAD_COMMITTED,
    WEBKIT_LOAD_FINISHED,
    WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT,
    WEBKIT_LOAD_FAILED
} WebKitLoadStatus;


typedef WebKitWebBackForwardList* (*webkit_web_view_get_back_forward_list_PROC)(WebKitWebView *web_view);
static webkit_web_view_get_back_forward_list_PROC webkit_web_view_get_back_forward_list;

typedef WebKitWebHistoryItem* (*webkit_web_back_forward_list_get_nth_item_PROC)(WebKitWebBackForwardList *web_back_forward_list, gint index);
static webkit_web_back_forward_list_get_nth_item_PROC webkit_web_back_forward_list_get_nth_item;

typedef const gchar* (*webkit_web_history_item_get_uri_PROC)(WebKitWebHistoryItem *web_history_item);
static webkit_web_history_item_get_uri_PROC webkit_web_history_item_get_uri;

typedef gint (*webkit_web_back_forward_list_get_forward_length_PROC)(WebKitWebBackForwardList *web_back_forward_list);
static webkit_web_back_forward_list_get_forward_length_PROC webkit_web_back_forward_list_get_forward_length;

typedef gint (*webkit_web_back_forward_list_get_back_length_PROC)(WebKitWebBackForwardList *web_back_forward_list);
static webkit_web_back_forward_list_get_back_length_PROC webkit_web_back_forward_list_get_back_length;

typedef void (*webkit_web_view_load_string_PROC)(WebKitWebView        *web_view,
                                                 const gchar          *content,
                                                 const gchar          *mime_type,
                                                 const gchar          *encoding,
                                                 const gchar          *base_uri);
static webkit_web_view_load_string_PROC webkit_web_view_load_string;

typedef void (*webkit_web_view_copy_clipboard_PROC)(WebKitWebView        *web_view);
static webkit_web_view_copy_clipboard_PROC webkit_web_view_copy_clipboard;

typedef void (*webkit_web_view_select_all_PROC)(WebKitWebView        *web_view);
static webkit_web_view_select_all_PROC webkit_web_view_select_all;

typedef WebKitWebFrame* (*webkit_web_view_get_main_frame_PROC)(WebKitWebView        *web_view);
static webkit_web_view_get_main_frame_PROC webkit_web_view_get_main_frame;

typedef void (*webkit_web_frame_print_PROC)(WebKitWebFrame       *frame);
static webkit_web_frame_print_PROC webkit_web_frame_print;

typedef void (*webkit_web_view_set_zoom_level_PROC)(WebKitWebView        *web_view,
                                                 gfloat                zoom_level);
static webkit_web_view_set_zoom_level_PROC webkit_web_view_set_zoom_level;

typedef gfloat (*webkit_web_view_get_zoom_level_PROC)(WebKitWebView        *web_view);
static webkit_web_view_get_zoom_level_PROC webkit_web_view_get_zoom_level;

typedef WebKitLoadStatus (*webkit_web_view_get_load_status_PROC)(WebKitWebView        *web_view);
static webkit_web_view_get_load_status_PROC webkit_web_view_get_load_status;

typedef void (*webkit_web_view_reload_PROC)(WebKitWebView        *web_view);
static webkit_web_view_reload_PROC webkit_web_view_reload;

typedef void (*webkit_web_view_stop_loading_PROC)(WebKitWebView        *web_view);
static webkit_web_view_stop_loading_PROC webkit_web_view_stop_loading;

typedef void (*webkit_web_view_go_back_or_forward_PROC)(WebKitWebView        *web_view,
                                                 gint                  steps);
static webkit_web_view_go_back_or_forward_PROC webkit_web_view_go_back_or_forward;


typedef void (*webkit_web_view_load_uri_PROC)(WebKitWebView        *web_view,
                                                 const gchar          *uri);
static webkit_web_view_load_uri_PROC webkit_web_view_load_uri;


typedef const gchar* (*webkit_web_frame_get_uri_PROC)(WebKitWebFrame       *frame);
static webkit_web_frame_get_uri_PROC webkit_web_frame_get_uri;

typedef const gchar* (*webkit_web_view_get_uri_PROC)(WebKitWebView        *web_view);
static webkit_web_view_get_uri_PROC webkit_web_view_get_uri;


typedef const gchar* (*webkit_network_request_get_uri_PROC)(WebKitNetworkRequest *request);
static webkit_network_request_get_uri_PROC webkit_network_request_get_uri;

typedef GtkWidget* (*webkit_web_view_new_PROC)(void);
static webkit_web_view_new_PROC webkit_web_view_new;


typedef gboolean (*webkit_web_view_can_go_back_PROC)(WebKitWebView        *web_view);
static webkit_web_view_can_go_back_PROC webkit_web_view_can_go_back;

typedef gboolean (*webkit_web_view_can_go_forward_PROC)(WebKitWebView        *web_view);
static webkit_web_view_can_go_forward_PROC webkit_web_view_can_go_forward;

typedef void (*webkit_web_view_go_back_PROC)(WebKitWebView        *web_view);
static webkit_web_view_go_back_PROC webkit_web_view_go_back;

typedef void (*webkit_web_view_go_forward_PROC)(WebKitWebView        *web_view);
static webkit_web_view_go_forward_PROC webkit_web_view_go_forward;

static void iupgtkWebBrowser_ClearDLSymbols()
{
  webkit_web_view_get_back_forward_list = NULL;
  webkit_web_back_forward_list_get_nth_item = NULL;
  webkit_web_history_item_get_uri = NULL;
  webkit_web_back_forward_list_get_forward_length = NULL;
  webkit_web_back_forward_list_get_back_length = NULL;
  webkit_web_view_load_string = NULL;
  webkit_web_view_copy_clipboard = NULL;
  webkit_web_view_select_all = NULL;
  webkit_web_view_get_main_frame = NULL;
  webkit_web_frame_print = NULL;
  webkit_web_view_set_zoom_level = NULL;
  webkit_web_view_get_zoom_level = NULL;
  webkit_web_view_get_load_status = NULL;
  webkit_web_view_reload = NULL;
  webkit_web_view_stop_loading = NULL;
  webkit_web_view_go_back_or_forward = NULL;
  webkit_web_view_load_uri = NULL;
  webkit_web_frame_get_uri = NULL;
  webkit_web_view_get_uri = NULL;
  webkit_network_request_get_uri = NULL;
  webkit_web_view_new = NULL;
  webkit_web_view_can_go_back = NULL;
  webkit_web_view_can_go_forward = NULL;
  webkit_web_view_go_back = NULL;
  webkit_web_view_go_forward = NULL;
}

static void iupgtkWebBrowser_SetDLSymbols(void* webkit_library)
{
  if(NULL != webkit_library)
  {
    webkit_web_view_get_back_forward_list = (webkit_web_view_get_back_forward_list_PROC)dlsym(webkit_library, "webkit_web_view_get_back_forward_list");
    webkit_web_back_forward_list_get_nth_item = (webkit_web_back_forward_list_get_nth_item_PROC)dlsym(webkit_library, "webkit_web_back_forward_list_get_nth_item");
    webkit_web_history_item_get_uri = (webkit_web_history_item_get_uri_PROC)dlsym(webkit_library, "webkit_web_history_item_get_uri");
    webkit_web_back_forward_list_get_forward_length = (webkit_web_back_forward_list_get_forward_length_PROC)dlsym(webkit_library, "webkit_web_back_forward_list_get_forward_length");
    webkit_web_back_forward_list_get_back_length = (webkit_web_back_forward_list_get_back_length_PROC)dlsym(webkit_library, "webkit_web_back_forward_list_get_back_length");
    webkit_web_view_load_string = (webkit_web_view_load_string_PROC)dlsym(webkit_library, "webkit_web_view_load_string");
    webkit_web_view_copy_clipboard = (webkit_web_view_copy_clipboard_PROC)dlsym(webkit_library, "webkit_web_view_copy_clipboard");
    webkit_web_view_select_all = (webkit_web_view_select_all_PROC)dlsym(webkit_library, "webkit_web_view_select_all");
    webkit_web_view_get_main_frame = (webkit_web_view_get_main_frame_PROC)dlsym(webkit_library, "webkit_web_view_get_main_frame");
    webkit_web_frame_print = (webkit_web_frame_print_PROC)dlsym(webkit_library, "webkit_web_frame_print");
    webkit_web_view_set_zoom_level = (webkit_web_view_set_zoom_level_PROC)dlsym(webkit_library, "webkit_web_view_set_zoom_level");
    webkit_web_view_get_zoom_level = (webkit_web_view_get_zoom_level_PROC)dlsym(webkit_library, "webkit_web_view_get_zoom_level");
    webkit_web_view_get_load_status = (webkit_web_view_get_load_status_PROC)dlsym(webkit_library, "webkit_web_view_get_load_status");
    webkit_web_view_reload = (webkit_web_view_reload_PROC)dlsym(webkit_library, "webkit_web_view_reload");
    webkit_web_view_stop_loading = (webkit_web_view_stop_loading_PROC)dlsym(webkit_library, "webkit_web_view_stop_loading");
    webkit_web_view_go_back_or_forward = (webkit_web_view_go_back_or_forward_PROC)dlsym(webkit_library, "webkit_web_view_go_back_or_forward");
    webkit_web_view_load_uri = (webkit_web_view_load_uri_PROC)dlsym(webkit_library, "webkit_web_view_load_uri");
    webkit_web_frame_get_uri = (webkit_web_frame_get_uri_PROC)dlsym(webkit_library, "webkit_web_frame_get_uri");
    webkit_web_view_get_uri = (webkit_web_view_get_uri_PROC)dlsym(webkit_library, "webkit_web_view_get_uri");
    webkit_network_request_get_uri = (webkit_network_request_get_uri_PROC)dlsym(webkit_library, "webkit_network_request_get_uri");
    webkit_web_view_new = (webkit_web_view_new_PROC)dlsym(webkit_library, "webkit_web_view_new");
    webkit_web_view_can_go_back = (webkit_web_view_can_go_back_PROC)dlsym(webkit_library, "webkit_web_view_can_go_back");
    webkit_web_view_can_go_forward = (webkit_web_view_can_go_forward_PROC)dlsym(webkit_library, "webkit_web_view_can_go_forward");
    webkit_web_view_go_back = (webkit_web_view_go_back_PROC)dlsym(webkit_library, "webkit_web_view_go_back");
    webkit_web_view_go_forward = (webkit_web_view_go_forward_PROC)dlsym(webkit_library, "webkit_web_view_go_forward");
  }
}




#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* IUPWEBGTK_DLOPEN_H */




