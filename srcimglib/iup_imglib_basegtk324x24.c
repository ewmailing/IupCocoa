/** \file
 * \brief IupImgLib
 *
 * See Copyright Notice in iup.h
 */

#include <stdlib.h>

#include "iup.h"
#include "iup_image.h"

#include "iup_imglib.h"

/* source code, included only here */
#include "iup_imglib_basegtk324x24.h"

void iupImglibBaseLibGtk324x24Open(void)
{
  iupImageStockSet("IUP_ActionCancel", load_image_gtk_cancel, 0);
  iupImageStockSet("IUP_ActionOk", load_image_gtk_apply, 0);
  iupImageStockSet("IUP_ArrowDown", load_image_go_down, 0);
  iupImageStockSet("IUP_ArrowLeft", load_image_go_previous_ltr, 0);
  iupImageStockSet("IUP_ArrowRight", load_image_go_next_ltr, 0);
  iupImageStockSet("IUP_ArrowUp", load_image_go_up, 0);
  iupImageStockSet("IUP_EditCopy", load_image_edit_copy, 0);
  iupImageStockSet("IUP_EditCut", load_image_edit_cut, 0);
  iupImageStockSet("IUP_EditErase", load_image_edit_delete, 0);
  iupImageStockSet("IUP_EditFind", load_image_edit_find, 0);
  iupImageStockSet("IUP_EditPaste", load_image_edit_paste, 0);
  iupImageStockSet("IUP_EditRedo", load_image_edit_redo_ltr, 0);
  iupImageStockSet("IUP_EditUndo", load_image_edit_undo_ltr, 0);
  iupImageStockSet("IUP_FileClose", load_image_folder, 0);
  iupImageStockSet("IUP_FileNew", load_image_document_new, 0);
  iupImageStockSet("IUP_FileOpen", load_image_document_open, 0);
  iupImageStockSet("IUP_FileProperties", load_image_document_properties, 0);
  iupImageStockSet("IUP_FileSave", load_image_document_save, 0);
  iupImageStockSet("IUP_MediaForward", load_image_media_seek_forward_ltr, 0);
  iupImageStockSet("IUP_MediaGoToEnd", load_image_media_skip_forward_ltr, 0);
  iupImageStockSet("IUP_MediaGoToBegin", load_image_media_skip_backward_ltr, 0);
  iupImageStockSet("IUP_MediaPause", load_image_media_playback_pause, 0);
  iupImageStockSet("IUP_MediaPlay", load_image_media_playback_start_ltr, 0);
  iupImageStockSet("IUP_MediaRecord", load_image_media_record, 0);
  iupImageStockSet("IUP_MediaReverse", load_image_media_playback_start_rtl, 0);
  iupImageStockSet("IUP_MediaRewind", load_image_media_seek_backward_ltr, 0);
  iupImageStockSet("IUP_MediaStop", load_image_media_playback_stop, 0);
  iupImageStockSet("IUP_MessageError", load_image_process_stop, 0);
  iupImageStockSet("IUP_MessageHelp", load_image_help_contents, 0);
  iupImageStockSet("IUP_MessageInfo", load_image_dialog_information, 0);
  iupImageStockSet("IUP_NavigateHome", load_image_go_home, 0);
  iupImageStockSet("IUP_NavigateRefresh", load_image_view_refresh, 0);
  iupImageStockSet("IUP_Print", load_image_document_print, 0);
  iupImageStockSet("IUP_PrintPreview", load_image_document_print_preview, 0);
  iupImageStockSet("IUP_ToolsColor", load_image_gtk_select_color, 0);
  iupImageStockSet("IUP_ToolsSettings", load_image_gtk_preferences, 0);
  iupImageStockSet("IUP_ToolsSortAscend", load_image_view_sort_ascending, 0);
  iupImageStockSet("IUP_ToolsSortDescend", load_image_view_sort_descending, 0);
  iupImageStockSet("IUP_ViewFullScreen", load_image_view_fullscreen, 0);
  iupImageStockSet("IUP_ZoomActualSize", load_image_zoom_original, 0);
  iupImageStockSet("IUP_ZoomIn", load_image_zoom_in, 0);
  iupImageStockSet("IUP_ZoomOut", load_image_zoom_out, 0);
  iupImageStockSet("IUP_ZoomSelection", load_image_zoom_fit_best, 0);

  iupImageStockSet("IUP_Webcam", load_image_webcam, 0);
}
