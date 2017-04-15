
/* Custom File Format Definitions */

/* IMPORTANT: Don't use the same definitions for Preview and Thumbnail handlers */

#if !defined(DEFINE_PREVIEWHANDLER) && !defined(DEFINE_THUMBNAILHANDLER)

/* Define to implement a preview handler */
//#define DEFINE_PREVIEWHANDLER

/* Define to implement a thumbnail handler */
#define DEFINE_THUMBNAILHANDLER

#endif

#if defined(DEFINE_PREVIEWHANDLER)

/* Build using Microsoft GUIDGEN Application (option 3) */
const CLSID CLSID_ShelExtensionHandler = 
{ 0x1bd58f32, 0xb48d, 0x4c1d, { 0xa4, 0x42, 0x28, 0xa7, 0xb, 0x49, 0xa5, 0x9b } };

const GUID APPID_ShelExtensionHandler = 
{ 0x8b403b0, 0xbce3, 0x4e8e, { 0x9e, 0x60, 0xc9, 0x29, 0xeb, 0x69, 0xa4, 0x5f } };

#define FORMAT_EXTENSION L".tga"
#define HANDLER_DESCRIPTION L"TecgrafPreviewTGA"
#define APPICATION_DESCRIPTION L"TecgrafPreviewHandlerTGA"

#elif defined(DEFINE_THUMBNAILHANDLER)

// {2222E1A8-048B-4911-B64A-4A882BE64799}
const GUID CLSID_ShelExtensionHandler =
{ 0x2222e1a8, 0x48b, 0x4911, { 0xb6, 0x4a, 0x4a, 0x88, 0x2b, 0xe6, 0x47, 0x99 } };

// {A1AC1818-C5A9-40EF-9244-111C06B1A499}
const GUID APPID_ShelExtensionHandler =
{ 0xa1ac1818, 0xc5a9, 0x40ef, { 0x92, 0x44, 0x11, 0x1c, 0x6, 0xb1, 0xa4, 0x99 } };

#define FORMAT_EXTENSION L".tga"
#define HANDLER_DESCRIPTION L"TecgrafThumbnailTGA"
#define APPICATION_DESCRIPTION L"TecgrafThumbnailHandlerTGA"

#endif

#if 0
// regsvr32 ShellExtensionHandler.dll

//#define FORMAT_EXTENSION L".cgm"
//#define HANDLER_DESCRIPTION L"CgmShelExtensionHandler"
//#define FORMAT_EXTENSION L".emf"
//#define HANDLER_DESCRIPTION L"EmfShelExtensionHandler"
#endif
