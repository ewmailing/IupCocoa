
-- #################################################################################
--                                     Constants
-- #################################################################################


IUP_ERROR     = 1           iup.ERROR     = IUP_ERROR     
IUP_NOERROR   = 0           iup.NOERROR   = IUP_NOERROR   
IUP_OPENED    = -1          iup.OPENED    = IUP_OPENED    
IUP_INVALID   = -1          iup.INVALID   = IUP_INVALID
                  
IUP_CENTER   = 65535        iup.CENTER    = IUP_CENTER 
IUP_LEFT     = 65534        iup.LEFT      = IUP_LEFT   
IUP_RIGHT    = 65533        iup.RIGHT     = IUP_RIGHT  
IUP_MOUSEPOS = 65532        iup.MOUSEPOS  = IUP_MOUSEPOS
IUP_CURRENT  = 65531        iup.CURRENT   = IUP_CURRENT 
IUP_CENTERPARENT = 65530    iup.CENTERPARENT = IUP_CENTERPARENT
IUP_TOP      = IUP_LEFT     iup.TOP       = IUP_TOP   
IUP_BOTTOM   = IUP_RIGHT    iup.BOTTOM    = IUP_BOTTOM

IUP_BUTTON1   =   49        iup.BUTTON1   = IUP_BUTTON1     -- '1'  
IUP_BUTTON2   =   50        iup.BUTTON2   = IUP_BUTTON2     -- '2'  
IUP_BUTTON3   =   51        iup.BUTTON3   = IUP_BUTTON3     -- '3'  
IUP_BUTTON4   =   52        iup.BUTTON4   = IUP_BUTTON4     -- '4'  
IUP_BUTTON5   =   53        iup.BUTTON5   = IUP_BUTTON5     -- '5'  
                  
IUP_IGNORE    = -1          iup.IGNORE    = IUP_IGNORE    
IUP_DEFAULT   = -2          iup.DEFAULT   = IUP_DEFAULT   
IUP_CLOSE     = -3          iup.CLOSE     = IUP_CLOSE     
IUP_CONTINUE  = -4          iup.CONTINUE  = IUP_CONTINUE  
                  
IUP_SBUP      = 0           iup.SBUP      = IUP_SBUP      
IUP_SBDN      = 1           iup.SBDN      = IUP_SBDN      
IUP_SBPGUP    = 2           iup.SBPGUP    = IUP_SBPGUP    
IUP_SBPGDN    = 3           iup.SBPGDN    = IUP_SBPGDN    
IUP_SBPOSV    = 4           iup.SBPOSV    = IUP_SBPOSV    
IUP_SBDRAGV   = 5           iup.SBDRAGV   = IUP_SBDRAGV   
IUP_SBLEFT    = 6           iup.SBLEFT    = IUP_SBLEFT    
IUP_SBRIGHT   = 7           iup.SBRIGHT   = IUP_SBRIGHT   
IUP_SBPGLEFT  = 8           iup.SBPGLEFT  = IUP_SBPGLEFT  
IUP_SBPGRIGHT = 9           iup.SBPGRIGHT = IUP_SBPGRIGHT 
IUP_SBPOSH    = 10          iup.SBPOSH    = IUP_SBPOSH    
IUP_SBDRAGH   = 11          iup.SBDRAGH   = IUP_SBDRAGH   
                  
IUP_SHOW      = 0           iup.SHOW      = IUP_SHOW      
IUP_RESTORE   = 1           iup.RESTORE   = IUP_RESTORE   
IUP_MINIMIZE  = 2           iup.MINIMIZE  = IUP_MINIMIZE  
IUP_MAXIMIZE  = 3           iup.MAXIMIZE  = IUP_MAXIMIZE
IUP_HIDE      = 4           iup.HIDE      = IUP_HIDE

IUP_RECBINARY = 0           iup.RECBINARY = IUP_RECBINARY
IUP_RECTEXT = 1             iup.RECTEXT = IUP_RECTEXT

RED    = IupRGB(1, 0, 0)    iup.RED    = RED    
GREEN  = IupRGB(0, 1, 0)    iup.GREEN  = GREEN  
BLUE   = IupRGB(0, 0, 1)    iup.BLUE   = BLUE   
BLACK  = IupRGB(0, 0, 0)    iup.BLACK  = BLACK  
WHITE  = IupRGB(1, 1, 1)    iup.WHITE  = WHITE  
YELLOW = IupRGB(1, 1, 0)    iup.YELLOW = YELLOW 

IUP_ON =        "ON"           iup.ON =        IUP_ON
IUP_OFF =       "OFF"          iup.OFF =       IUP_OFF
IUP_YES =       "YES"          iup.YES =       IUP_YES
IUP_NO =        "NO"           iup.NO =        IUP_NO
IUP_APPEND =    "APPEND"       iup.APPEND =    IUP_APPEND
IUP_VERTICAL =  "VERTICAL"     iup.VERTICAL =  IUP_VERTICAL
IUP_HORIZONTAL ="HORIZONTAL"   iup.HORIZONTAL =IUP_HORIZONTAL
                    
IUP_ACENTER =   "ACENTER"      iup.ACENTER =   IUP_ACENTER
IUP_ALEFT =     "ALEFT"        iup.ALEFT =     IUP_ALEFT
IUP_ARIGHT =    "ARIGHT"       iup.ARIGHT =    IUP_ARIGHT
IUP_ATOP =      "ATOP"         iup.ATOP =      IUP_ATOP
IUP_ABOTTOM =   "ABOTTOM"      iup.ABOTTOM =   IUP_ABOTTOM
                    
IUP_NORTH =     "NORTH"        iup.NORTH =     IUP_NORTH
IUP_SOUTH =     "SOUTH"        iup.SOUTH =     IUP_SOUTH
IUP_WEST =      "WEST"         iup.WEST =      IUP_WEST
IUP_EAST =      "EAST"         iup.EAST =      IUP_EAST
IUP_NE =        "NE"           iup.NE =        IUP_NE
IUP_SE =        "SE"           iup.SE =        IUP_SE
IUP_NW =        "NW"           iup.NW =        IUP_NW
IUP_SW =        "SW"           iup.SW =        IUP_SW
                    
IUP_FULL =      "FULL"         iup.FULL =      IUP_FULL
IUP_HALF =      "HALF"         iup.HALF =      IUP_HALF
IUP_THIRD =     "THIRD"        iup.THIRD =     IUP_THIRD
IUP_QUARTER =   "QUARTER"      iup.QUARTER =   IUP_QUARTER
IUP_EIGHTH =    "EIGHTH"       iup.EIGHTH =    IUP_EIGHTH
                    
IUP_ARROW =     "ARROW"        iup.ARROW =     IUP_ARROW
IUP_BUSY =      "BUSY"         iup.BUSY =      IUP_BUSY
IUP_RESIZE_N =  "RESIZE_N"     iup.RESIZE_N =  IUP_RESIZE_N
IUP_RESIZE_S =  "RESIZE_S"     iup.RESIZE_S =  IUP_RESIZE_S
IUP_RESIZE_E =  "RESIZE_E"     iup.RESIZE_E =  IUP_RESIZE_E
IUP_RESIZE_W =  "RESIZE_W"     iup.RESIZE_W =  IUP_RESIZE_W
IUP_RESIZE_NE = "RESIZE_NE"    iup.RESIZE_NE = IUP_RESIZE_NE
IUP_RESIZE_NW = "RESIZE_NW"    iup.RESIZE_NW = IUP_RESIZE_NW
IUP_RESIZE_SE = "RESIZE_SE"    iup.RESIZE_SE = IUP_RESIZE_SE
IUP_RESIZE_SW = "RESIZE_SW"    iup.RESIZE_SW = IUP_RESIZE_SW
IUP_MOVE =      "MOVE"         iup.MOVE =      IUP_MOVE
IUP_HAND =      "HAND"         iup.HAND =      IUP_HAND
IUP_NONE =      "NONE"         iup.NONE =      IUP_NONE
IUP_IUP =       "IUP"          iup.IUP =       IUP_IUP
IUP_CROSS =     "CROSS"        iup.CROSS =     IUP_CROSS
IUP_PEN =       "PEN"          iup.PEN =       IUP_PEN
IUP_TEXT =      "TEXT"         iup.TEXT =      IUP_TEXT
IUP_RESIZE_C =  "RESIZE_C"     iup.RESIZE_C =  IUP_RESIZE_C
IUP_OPENHAND =  "OPENHAND"     iup.OPENHAND =  IUP_OPENHAND

IUP_HELVETICA_NORMAL_8 =   "HELVETICA_NORMAL_8"       iup.HELVETICA_NORMAL_8 =   IUP_HELVETICA_NORMAL_8
IUP_HELVETICA_ITALIC_8 =   "HELVETICA_ITALIC_8"       iup.HELVETICA_ITALIC_8 =   IUP_HELVETICA_ITALIC_8
IUP_HELVETICA_BOLD_8 =     "HELVETICA_BOLD_8"         iup.HELVETICA_BOLD_8 =     IUP_HELVETICA_BOLD_8
IUP_HELVETICA_NORMAL_10 =  "HELVETICA_NORMAL_10"      iup.HELVETICA_NORMAL_10 =  IUP_HELVETICA_NORMAL_10
IUP_HELVETICA_ITALIC_10 =  "HELVETICA_ITALIC_10"      iup.HELVETICA_ITALIC_10 =  IUP_HELVETICA_ITALIC_10
IUP_HELVETICA_BOLD_10 =    "HELVETICA_BOLD_10"        iup.HELVETICA_BOLD_10 =    IUP_HELVETICA_BOLD_10
IUP_HELVETICA_NORMAL_12 =  "HELVETICA_NORMAL_12"      iup.HELVETICA_NORMAL_12 =  IUP_HELVETICA_NORMAL_12
IUP_HELVETICA_ITALIC_12 =  "HELVETICA_ITALIC_12"      iup.HELVETICA_ITALIC_12 =  IUP_HELVETICA_ITALIC_12
IUP_HELVETICA_BOLD_12 =    "HELVETICA_BOLD_12"        iup.HELVETICA_BOLD_12 =    IUP_HELVETICA_BOLD_12
IUP_HELVETICA_NORMAL_14 =  "HELVETICA_NORMAL_14"      iup.HELVETICA_NORMAL_14 =  IUP_HELVETICA_NORMAL_14
IUP_HELVETICA_ITALIC_14 =  "HELVETICA_ITALIC_14"      iup.HELVETICA_ITALIC_14 =  IUP_HELVETICA_ITALIC_14
IUP_HELVETICA_BOLD_14 =    "HELVETICA_BOLD_14"        iup.HELVETICA_BOLD_14 =    IUP_HELVETICA_BOLD_14
IUP_COURIER_NORMAL_8 =     "COURIER_NORMAL_8"         iup.COURIER_NORMAL_8 =     IUP_COURIER_NORMAL_8
IUP_COURIER_ITALIC_8 =     "COURIER_ITALIC_8"         iup.COURIER_ITALIC_8 =     IUP_COURIER_ITALIC_8
IUP_COURIER_BOLD_8 =       "COURIER_BOLD_8"           iup.COURIER_BOLD_8 =       IUP_COURIER_BOLD_8
IUP_COURIER_NORMAL_10 =    "COURIER_NORMAL_10"        iup.COURIER_NORMAL_10 =    IUP_COURIER_NORMAL_10
IUP_COURIER_ITALIC_10 =    "COURIER_ITALIC_10"        iup.COURIER_ITALIC_10 =    IUP_COURIER_ITALIC_10
IUP_COURIER_BOLD_10 =      "COURIER_BOLD_10"          iup.COURIER_BOLD_10 =      IUP_COURIER_BOLD_10
IUP_COURIER_NORMAL_12 =    "COURIER_NORMAL_12"        iup.COURIER_NORMAL_12 =    IUP_COURIER_NORMAL_12
IUP_COURIER_ITALIC_12 =    "COURIER_ITALIC_12"        iup.COURIER_ITALIC_12 =    IUP_COURIER_ITALIC_12
IUP_COURIER_BOLD_12 =      "COURIER_BOLD_12"          iup.COURIER_BOLD_12 =      IUP_COURIER_BOLD_12
IUP_COURIER_NORMAL_14 =    "COURIER_NORMAL_14"        iup.COURIER_NORMAL_14 =    IUP_COURIER_NORMAL_14
IUP_COURIER_ITALIC_14 =    "COURIER_ITALIC_14"        iup.COURIER_ITALIC_14 =    IUP_COURIER_ITALIC_14
IUP_COURIER_BOLD_14 =      "COURIER_BOLD_14"          iup.COURIER_BOLD_14 =      IUP_COURIER_BOLD_14
IUP_TIMES_NORMAL_8 =       "TIMES_NORMAL_8"           iup.TIMES_NORMAL_8 =       IUP_TIMES_NORMAL_8
IUP_TIMES_ITALIC_8 =       "TIMES_ITALIC_8"           iup.TIMES_ITALIC_8 =       IUP_TIMES_ITALIC_8
IUP_TIMES_BOLD_8 =         "TIMES_BOLD_8"             iup.TIMES_BOLD_8 =         IUP_TIMES_BOLD_8
IUP_TIMES_NORMAL_10 =      "TIMES_NORMAL_10"          iup.TIMES_NORMAL_10 =      IUP_TIMES_NORMAL_10
IUP_TIMES_ITALIC_10 =      "TIMES_ITALIC_10"          iup.TIMES_ITALIC_10 =      IUP_TIMES_ITALIC_10
IUP_TIMES_BOLD_10 =        "TIMES_BOLD_10"            iup.TIMES_BOLD_10 =        IUP_TIMES_BOLD_10
IUP_TIMES_NORMAL_12 =      "TIMES_NORMAL_12"          iup.TIMES_NORMAL_12 =      IUP_TIMES_NORMAL_12
IUP_TIMES_ITALIC_12 =      "TIMES_ITALIC_12"          iup.TIMES_ITALIC_12 =      IUP_TIMES_ITALIC_12
IUP_TIMES_BOLD_12 =        "TIMES_BOLD_12"            iup.TIMES_BOLD_12 =        IUP_TIMES_BOLD_12
IUP_TIMES_NORMAL_14 =      "TIMES_NORMAL_14"          iup.TIMES_NORMAL_14 =      IUP_TIMES_NORMAL_14
IUP_TIMES_ITALIC_14 =      "TIMES_ITALIC_14"          iup.TIMES_ITALIC_14 =      IUP_TIMES_ITALIC_14
IUP_TIMES_BOLD_14 =        "TIMES_BOLD_14"            iup.TIMES_BOLD_14 =        IUP_TIMES_BOLD_14

iup.MASK_FLOAT   = "[+/-]?(/d+/.?/d*|/./d+)"
iup.MASK_UFLOAT  = "(/d+/.?/d*|/./d+)"
iup.MASK_EFLOAT  = "[+/-]?(/d+/.?/d*|/./d+)([eE][+/-]?/d+)?"
iup.MASK_INT     = "[+/-]?/d+"
iup.MASK_UINT    = "/d+"

iup.RECBINARY = 0
iup.RECTEXT = 1

iup.GETPARAM_OK     = -1
iup.GETPARAM_INIT   = -2
iup.GETPARAM_CANCEL = -3
iup.GETPARAM_HELP   = -4
