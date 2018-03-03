#ifndef IUPWEBGTK_DLOPEN_H
#define IUPWEBGTK_DLOPEN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TODO: Eventually gtk itself could be dynamically discovered to handle different versions or even different non-GTK backends. */
#include <gtk/gtk.h>

typedef struct WebKitWebBackForwardList WebKitWebBackForwardList;
typedef struct WebKitWebHistoryItem WebKitWebHistoryItem;
typedef struct WebKitWebFrame WebKitWebFrame;
typedef struct WebKitWebView WebKitWebView;

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

typedef gint (*webkit_web_back_forward_list_get_forward_length_PROC)(WebKitWebBackForwardList *web_back_forward_list);
static webkit_web_back_forward_list_get_forward_length_PROC webkit_web_back_forward_list_get_forward_length;

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


typedef WebKitLoadStatus (*webkit_web_view_get_load_status_PROC)(WebKitWebView        *web_view);
static webkit_web_view_get_load_status_PROC webkit_web_frame_print;

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


typedef const gchar* (*webkit_network_request_get_uri_PROC)(WebKitNetworkRequest *request);
static webkit_network_request_get_uri_PROC webkit_network_request_get_uri;

typedef GtkWidget* (*webkit_web_view_new_PROC)(void);
static webkit_web_view_new_PROC webkit_web_view_new;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* IUPWEBGTK_DLOPEN_H */




