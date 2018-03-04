#ifndef __IUP_IMGLIB_H 
#define __IUP_IMGLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
void iupImglibBaseLibWin16x16Open(void);     /* Used only by the Win32 driver */
void iupImglibBaseLibWin32x32Open(void);
#endif
#ifndef WIN32
void iupImglibBaseLibMot16x16Open(void);    /* Used only by the Motif driver */
#endif

void iupImglibBaseLibGtk24x24Open(void);    /* Used only by the GTK driver */
void iupImglibBaseLibGtk324x24Open(void);    /* Used only by the GTK driver */

#ifndef WIN32
void iupImglibLogosMot32x32Open(void);      /* Used only by the Motif driver */
void iupImglibLogosMot48x48Open(void);      /* Used only by the Motif driver */
#endif

void iupImglibLogos32x32Open(void);
void iupImglibLogos48x48Open(void);

void iupImglibCircleProgress(void);

#ifdef WIN32
void iupImglibIconsWin48x48Open(void);
#elif defined(MOTIF)
#elif defined(GTK3)
void iupImglibIconsGtk348x48Open(void);
#else
void iupImglibIconsGtk48x48Open(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
