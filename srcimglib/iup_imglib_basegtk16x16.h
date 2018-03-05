#ifdef IUP_IMGLIB_OLD
static Ihandle* load_image_iupgtk_tile(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 185, 195, 205, 255, 103, 124, 146, 255, 103, 124, 146, 255, 102, 123, 145, 255, 95, 115, 135, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 185, 195, 205, 255, 103, 124, 146, 255, 103, 124, 146, 255, 102, 123, 145, 255, 95, 115, 135, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 178, 189, 200, 255, 98, 119, 141, 255, 98, 119, 141, 255, 95, 116, 138, 255, 89, 109, 130, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 178, 189, 200, 255, 98, 119, 141, 255, 98, 119, 141, 255, 95, 116, 138, 255, 89, 109, 130, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 248, 248, 248, 255, 224, 224, 224, 255, 220, 220, 220, 255, 209, 209, 209, 255, 169, 169, 169, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 248, 248, 248, 255, 224, 224, 224, 255, 220, 220, 220, 255, 209, 209, 209, 255, 169, 169, 169, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 207, 207, 207, 255, 181, 181, 181, 255, 178, 178, 178, 255, 169, 169, 169, 255, 158, 158, 158, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 207, 207, 207, 255, 181, 181, 181, 255, 178, 178, 178, 255, 169, 169, 169, 255, 158, 158, 158, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 185, 195, 205, 255, 103, 124, 146, 255, 103, 124, 146, 255, 102, 123, 145, 255, 95, 115, 135, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 185, 195, 205, 255, 103, 124, 146, 255, 103, 124, 146, 255, 102, 123, 145, 255, 95, 115, 135, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 178, 189, 200, 255, 98, 119, 141, 255, 98, 119, 141, 255, 95, 116, 138, 255, 89, 109, 130, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 178, 189, 200, 255, 98, 119, 141, 255, 98, 119, 141, 255, 95, 116, 138, 255, 89, 109, 130, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 248, 248, 248, 255, 224, 224, 224, 255, 220, 220, 220, 255, 209, 209, 209, 255, 169, 169, 169, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 248, 248, 248, 255, 224, 224, 224, 255, 220, 220, 220, 255, 209, 209, 209, 255, 169, 169, 169, 255, 0, 0, 0, 255,
    0, 0, 0, 3, 0, 0, 0, 255, 207, 207, 207, 255, 181, 181, 181, 255, 178, 178, 178, 255, 169, 169, 169, 255, 158, 158, 158, 255, 0, 0, 0, 255, 0, 0, 0, 6, 0, 0, 0, 255, 207, 207, 207, 255, 181, 181, 181, 255, 178, 178, 178, 255, 169, 169, 169, 255, 158, 158, 158, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static Ihandle* load_image_iupgtk_save_all(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 85, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 89, 0, 0, 0, 13,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 148, 179, 204, 255, 219, 164, 159, 255, 219, 164, 158, 255, 214, 157, 151, 255, 213, 154, 148, 255, 209, 151, 148, 255, 115, 125, 138, 255, 0, 0, 0, 255, 0, 0, 0, 29,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 146, 176, 201, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 120, 131, 140, 255, 0, 0, 0, 255, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 85, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 89, 239, 239, 239, 255, 122, 132, 141, 255, 0, 0, 0, 255, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 148, 179, 204, 255, 219, 164, 159, 255, 219, 164, 158, 255, 214, 157, 151, 255, 213, 154, 148, 255, 209, 151, 148, 255, 115, 125, 138, 255, 0, 0, 0, 255, 251, 251, 251, 255, 120, 131, 140, 255, 0, 0, 0, 255, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 85, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 15, 15, 15, 255, 153, 153, 153, 255, 120, 131, 140, 255, 0, 0, 0, 255, 63, 79, 91, 255, 130, 158, 183, 255, 0, 0, 0, 255, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 255, 162, 129, 127, 255, 229, 153, 137, 255, 225, 146, 133, 255, 220, 148, 135, 255, 215, 139, 127, 255, 207, 131, 121, 255, 100, 125, 145, 255, 0, 0, 0, 255, 122, 132, 141, 255, 0, 0, 0, 255, 73, 94, 111, 255, 115, 144, 164, 255, 0, 0, 0, 255, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 255, 181, 181, 181, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 71, 94, 111, 255, 0, 0, 0, 255, 120, 131, 140, 255, 0, 0, 0, 255, 70, 89, 103, 255, 106, 134, 154, 255, 0, 0, 0, 255, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 255, 167, 167, 167, 255, 236, 236, 236, 255, 236, 236, 236, 255, 236, 236, 236, 255, 236, 236, 236, 255, 233, 233, 233, 255, 63, 85, 102, 255, 0, 0, 0, 255, 130, 158, 183, 255, 0, 0, 0, 255, 7, 9, 10, 255, 24, 30, 36, 255, 0, 0, 0, 110, 0, 0, 0, 30,
    0, 0, 0, 0, 0, 0, 0, 255, 179, 167, 167, 255, 252, 252, 252, 255, 252, 252, 252, 255, 252, 252, 252, 255, 252, 252, 252, 255, 251, 251, 251, 255, 72, 82, 90, 255, 0, 0, 0, 255, 115, 144, 164, 255, 0, 0, 0, 255, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 126, 148, 165, 255, 125, 142, 156, 255, 123, 140, 155, 255, 119, 138, 153, 255, 126, 143, 157, 255, 125, 141, 155, 255, 49, 68, 81, 255, 0, 0, 0, 255, 106, 134, 154, 255, 0, 0, 0, 255, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 87, 107, 123, 255, 216, 214, 209, 255, 94, 91, 84, 255, 211, 209, 205, 255, 200, 199, 193, 255, 73, 94, 111, 255, 44, 56, 65, 255, 0, 0, 0, 255, 24, 30, 36, 255, 0, 0, 0, 110, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 87, 104, 119, 255, 227, 225, 222, 255, 80, 77, 69, 255, 197, 194, 188, 255, 226, 224, 222, 255, 76, 96, 111, 255, 47, 56, 62, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 85, 8, 10, 11, 255, 17, 17, 16, 255, 21, 21, 20, 255, 23, 23, 22, 255, 19, 19, 18, 255, 7, 9, 10, 255, 5, 6, 6, 240, 0, 0, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 8, 0, 0, 0, 11, 0, 0, 0, 11, 0, 0, 0, 11, 0, 0, 0, 11, 0, 0, 0, 11, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static Ihandle* load_image_iupgtk_cascade(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 185, 195, 205, 255, 102, 123, 145, 255, 101, 122, 143, 255, 101, 123, 143, 255, 93, 113, 133, 255, 0, 0, 0, 255, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 177, 188, 199, 255, 98, 119, 141, 255, 97, 118, 140, 255, 96, 117, 139, 255, 89, 109, 130, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 247, 247, 247, 255, 228, 228, 228, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 245, 245, 245, 255, 224, 224, 224, 255, 0, 0, 0, 255, 185, 195, 205, 255, 103, 124, 146, 255, 102, 123, 145, 255, 102, 124, 145, 255, 102, 123, 145, 255, 95, 115, 135, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 204, 204, 204, 255, 181, 181, 181, 255, 0, 0, 0, 255, 178, 189, 200, 255, 98, 119, 141, 255, 97, 118, 140, 255, 96, 117, 139, 255, 95, 116, 138, 255, 89, 109, 130, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 249, 249, 249, 255, 224, 224, 224, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 255, 248, 248, 248, 255, 220, 220, 220, 255, 0, 0, 0, 255, 185, 195, 205, 255, 103, 124, 146, 255, 103, 124, 146, 255, 102, 123, 145, 255, 102, 123, 145, 255, 95, 115, 135, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 255, 207, 207, 207, 255, 178, 178, 178, 255, 0, 0, 0, 255, 178, 189, 200, 255, 98, 119, 141, 255, 98, 119, 141, 255, 97, 118, 140, 255, 95, 116, 138, 255, 89, 109, 130, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 250, 250, 250, 255, 231, 231, 231, 255, 228, 228, 228, 255, 224, 224, 224, 255, 217, 217, 217, 255, 175, 175, 175, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 248, 248, 248, 255, 224, 224, 224, 255, 220, 220, 220, 255, 217, 217, 217, 255, 209, 209, 209, 255, 169, 169, 169, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 207, 207, 207, 255, 181, 181, 181, 255, 178, 178, 178, 255, 175, 175, 175, 255, 169, 169, 169, 255, 158, 158, 158, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static Ihandle* load_image_iupgtk_text(void)
{
  unsigned char imgdata[] = {
    255, 255, 255, 0, 140, 141, 137, 97, 140, 142, 137, 241, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 139, 141, 136, 243, 139, 141, 136, 100, 255, 255, 255, 0,
    255, 255, 255, 0, 140, 142, 137, 240, 242, 242, 241, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 242, 242, 241, 255, 140, 142, 137, 241, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 215, 215, 214, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 220, 221, 220, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 213, 213, 212, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 213, 213, 212, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 215, 215, 214, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 220, 221, 220, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 243, 244, 242, 255, 242, 243, 241, 255, 242, 243, 241, 255, 242, 243, 241, 255, 242, 243, 241, 255, 242, 243, 241, 255, 242, 243, 241, 255, 243, 244, 242, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 215, 216, 215, 255, 188, 188, 188, 255, 188, 188, 188, 255, 188, 188, 188, 255, 188, 188, 188, 255, 188, 188, 188, 255, 188, 188, 188, 255, 215, 216, 215, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 215, 215, 214, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 186, 186, 186, 255, 220, 221, 220, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 136, 138, 133, 255, 255, 255, 255, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 244, 245, 243, 255, 255, 255, 255, 255, 136, 138, 133, 255, 255, 255, 255, 0,
    255, 255, 255, 0, 140, 142, 137, 240, 242, 242, 241, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 242, 242, 241, 255, 140, 142, 137, 241, 255, 255, 255, 0,
    255, 255, 255, 0, 140, 141, 137, 97, 140, 142, 137, 241, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 136, 138, 133, 255, 139, 141, 136, 243, 139, 141, 136, 100, 255, 255, 255, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static Ihandle* load_image_iupgtk_zoom(void)
{
  unsigned char imgdata[] = {
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 46, 52, 54, 42, 46, 52, 54, 158, 46, 52, 54, 208, 46, 52, 54, 247, 46, 52, 54, 214, 46, 52, 54, 165, 46, 52, 54, 59, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    255, 255, 255, 0, 46, 52, 54, 3, 46, 52, 54, 124, 47, 53, 55, 236, 124, 127, 128, 198, 192, 193, 192, 227, 220, 221, 218, 254, 201, 204, 200, 239, 166, 169, 166, 220, 58, 64, 66, 234, 46, 52, 54, 145, 46, 52, 54, 9, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    255, 255, 255, 0, 46, 52, 54, 124, 60, 66, 68, 212, 215, 216, 214, 236, 237, 237, 235, 255, 233, 234, 230, 255, 229, 230, 226, 255, 225, 227, 222, 255, 221, 223, 217, 255, 215, 218, 212, 253, 111, 116, 114, 217, 46, 52, 54, 150, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 42, 47, 53, 55, 236, 215, 216, 214, 236, 238, 238, 236, 255, 237, 237, 235, 255, 237, 237, 235, 255, 237, 238, 235, 255, 235, 235, 233, 255, 225, 227, 222, 255, 215, 219, 212, 255, 212, 216, 209, 255, 63, 69, 70, 232, 46, 52, 54, 67, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 158, 125, 128, 129, 199, 236, 237, 234, 255, 225, 225, 224, 255, 237, 238, 235, 255, 237, 238, 235, 255, 238, 238, 236, 255, 238, 238, 236, 255, 226, 226, 224, 255, 237, 238, 235, 255, 215, 219, 212, 255, 186, 189, 185, 234, 46, 52, 54, 184, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 208, 192, 194, 193, 227, 232, 233, 230, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 220, 222, 216, 255, 215, 217, 212, 253, 46, 52, 54, 237, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 247, 220, 221, 218, 254, 228, 230, 226, 255, 238, 238, 235, 255, 237, 238, 235, 255, 238, 238, 236, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 237, 238, 235, 255, 224, 226, 221, 255, 225, 227, 222, 255, 48, 54, 56, 245, 46, 52, 54, 23, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 214, 201, 204, 200, 239, 224, 226, 221, 255, 235, 235, 233, 255, 237, 238, 235, 255, 238, 238, 236, 255, 237, 238, 235, 255, 238, 238, 236, 255, 237, 238, 235, 255, 237, 238, 235, 255, 228, 230, 225, 255, 220, 221, 218, 254, 47, 53, 55, 242, 46, 52, 54, 1, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 165, 165, 169, 166, 220, 221, 223, 217, 255, 225, 227, 222, 255, 237, 238, 235, 255, 238, 238, 236, 255, 238, 238, 236, 255, 238, 238, 236, 255, 238, 238, 236, 255, 237, 238, 235, 255, 232, 233, 230, 255, 180, 182, 180, 228, 46, 52, 54, 191, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    46, 52, 54, 59, 59, 65, 66, 234, 215, 218, 211, 253, 215, 219, 212, 255, 225, 227, 222, 255, 236, 236, 234, 255, 238, 238, 236, 255, 237, 237, 235, 255, 235, 235, 232, 255, 236, 236, 234, 255, 232, 233, 231, 255, 58, 63, 64, 224, 46, 52, 54, 87, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    255, 255, 255, 0, 46, 52, 54, 145, 117, 121, 120, 219, 212, 216, 208, 255, 215, 219, 212, 255, 220, 222, 216, 255, 225, 227, 222, 255, 228, 230, 225, 255, 232, 233, 230, 255, 232, 233, 231, 255, 97, 101, 102, 206, 48, 55, 56, 183, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0,
    255, 255, 255, 0, 46, 52, 54, 9, 46, 52, 54, 150, 70, 76, 77, 233, 191, 194, 189, 237, 214, 217, 211, 253, 224, 226, 221, 255, 220, 221, 217, 254, 180, 183, 181, 229, 58, 63, 65, 224, 48, 55, 56, 183, 59, 63, 63, 244, 50, 55, 57, 222, 46, 52, 54, 45, 255, 255, 255, 0, 255, 255, 255, 0,
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 46, 52, 54, 67, 46, 52, 54, 184, 46, 52, 54, 237, 54, 60, 62, 245, 47, 53, 55, 242, 46, 52, 54, 191, 46, 52, 54, 87, 255, 255, 255, 0, 50, 55, 57, 222, 56, 61, 62, 255, 48, 53, 55, 240, 46, 52, 54, 45, 255, 255, 255, 0,
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 46, 52, 54, 23, 46, 52, 54, 1, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 46, 52, 54, 45, 48, 53, 55, 240, 59, 64, 64, 254, 48, 53, 55, 240, 46, 52, 54, 45,
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 46, 52, 54, 45, 48, 53, 55, 240, 56, 61, 62, 254, 48, 54, 56, 214,
    255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 46, 52, 54, 45, 48, 54, 56, 214, 48, 54, 56, 167};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static Ihandle* load_image_iupgtk_close_all(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 228, 0, 0, 0, 247, 0, 0, 0, 255, 5, 5, 4, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 35, 36, 29, 169, 198, 202, 166, 255, 196, 201, 165, 255, 201, 206, 169, 255, 85, 88, 71, 255, 25, 25, 25, 4, 31, 31, 31, 3, 14, 14, 14, 3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 33, 34, 27, 246, 116, 117, 108, 255, 116, 117, 108, 255, 116, 117, 108, 255, 116, 117, 108, 255, 116, 117, 108, 255, 115, 117, 108, 255, 114, 115, 106, 246, 123, 124, 118, 227, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 6, 6, 4, 255, 206, 211, 174, 255, 204, 209, 172, 255, 21, 22, 18, 255, 6, 6, 5, 255, 0, 0, 0, 255, 160, 164, 134, 255, 187, 191, 158, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 205, 210, 173, 255, 174, 178, 147, 255, 217, 221, 192, 255, 217, 221, 192, 255, 186, 190, 163, 255, 48, 48, 40, 255, 183, 188, 155, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 194, 199, 164, 255, 38, 39, 31, 255, 116, 117, 108, 255, 116, 117, 108, 255, 116, 117, 108, 255, 116, 117, 108, 255, 116, 117, 108, 255, 115, 117, 108, 255, 114, 115, 106, 246, 123, 124, 118, 227, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 255, 176, 180, 149, 255, 12, 12, 9, 255, 207, 212, 175, 255, 207, 212, 175, 255, 206, 211, 174, 255, 205, 210, 173, 255, 205, 210, 173, 255, 203, 208, 171, 255, 190, 195, 161, 255, 0, 0, 0, 255, 0, 0, 0, 0, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 26, 26, 20, 87, 10, 10, 8, 161, 0, 0, 0, 255, 205, 210, 173, 255, 174, 178, 147, 255, 6, 6, 5, 255, 6, 6, 5, 255, 6, 6, 5, 255, 6, 6, 5, 255, 183, 188, 155, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 51, 51, 30, 9, 48, 48, 36, 25, 0, 0, 0, 255, 202, 207, 170, 255, 91, 93, 76, 255, 198, 202, 166, 255, 196, 201, 165, 255, 201, 206, 169, 255, 85, 88, 71, 255, 176, 180, 149, 255, 0, 0, 0, 255, 14, 14, 14, 3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 176, 180, 149, 255, 12, 12, 9, 255, 207, 212, 175, 255, 207, 212, 175, 255, 207, 212, 175, 255, 207, 212, 175, 255, 115, 117, 108, 255, 115, 117, 108, 255, 115, 117, 108, 255, 122, 123, 116, 255, 31, 31, 31, 6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 143, 147, 121, 255, 6, 6, 4, 255, 206, 211, 174, 255, 206, 211, 174, 255, 206, 211, 174, 255, 206, 211, 174, 255, 205, 210, 173, 255, 202, 207, 170, 255, 187, 191, 158, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 51, 30, 9, 12, 12, 9, 170, 0, 0, 0, 255, 202, 207, 170, 255, 202, 207, 170, 255, 201, 206, 170, 255, 199, 204, 168, 255, 198, 203, 168, 255, 194, 199, 164, 255, 179, 183, 151, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 194, 199, 164, 255, 194, 198, 164, 255, 192, 197, 162, 255, 189, 194, 160, 255, 185, 189, 156, 255, 182, 186, 154, 255, 166, 170, 140, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 143, 147, 121, 255, 138, 142, 116, 255, 134, 138, 113, 255, 133, 137, 112, 255, 125, 128, 105, 255, 117, 120, 99, 255, 109, 112, 92, 255, 0, 0, 0, 255, 51, 51, 39, 33, 37, 37, 27, 5,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 26, 20, 87, 10, 10, 8, 161, 10, 10, 8, 162, 10, 10, 8, 162, 10, 10, 8, 162, 10, 10, 8, 162, 10, 10, 8, 162, 10, 10, 8, 161, 9, 10, 7, 159, 47, 47, 37, 9, 21, 21, 21, 1};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}
#endif

static Ihandle* load_image_iupgtk_webcam(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 139, 139, 139, 46, 82, 82, 82, 129, 80, 80, 80, 166, 65, 65, 65, 140, 107, 107, 107, 102, 165, 165, 165, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 244, 244, 244, 4, 170, 171, 170, 81, 76, 76, 76, 242, 105, 105, 105, 255, 114, 114, 114, 255, 112, 112, 112, 255, 91, 91, 91, 255, 102, 103, 104, 181, 208, 208, 207, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 229, 229, 229, 2, 174, 175, 173, 91, 167, 168, 166, 218, 198, 199, 198, 255, 144, 144, 144, 255, 115, 115, 115, 255, 113, 113, 113, 255, 113, 113, 113, 255, 120, 120, 120, 255, 187, 187, 187, 255, 185, 186, 184, 251, 159, 160, 158, 164, 207, 207, 206, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 228, 228, 228, 11, 164, 165, 163, 180, 191, 191, 190, 255, 244, 244, 244, 255, 251, 251, 251, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 255, 251, 251, 251, 255, 229, 229, 229, 255, 163, 164, 162, 234, 181, 182, 180, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 185, 186, 186, 1, 155, 156, 154, 184, 207, 208, 204, 255, 244, 244, 243, 255, 251, 250, 248, 255, 254, 254, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 255, 248, 248, 248, 255, 239, 239, 239, 255, 167, 168, 166, 251, 177, 178, 176, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 155, 156, 154, 123, 192, 193, 191, 255, 238, 238, 238, 255, 246, 246, 246, 255, 254, 254, 254, 255, 254, 254, 248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 251, 251, 251, 255, 242, 242, 241, 255, 235, 235, 235, 255, 153, 155, 153, 235, 207, 207, 206, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 200, 200, 200, 22, 156, 157, 155, 240, 231, 231, 231, 255, 237, 237, 237, 255, 247, 247, 247, 255, 255, 255, 255, 255, 255, 254, 251, 255, 216, 216, 216, 255, 116, 117, 117, 255, 72, 74, 74, 255, 85, 87, 87, 255, 156, 157, 157, 255, 247, 248, 247, 255, 255, 255, 255, 255, 253, 253, 253, 255, 243, 243, 242, 255, 233, 233, 233, 255, 210, 210, 209, 255, 145, 146, 144, 146, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 147, 148, 147, 120, 198, 198, 198, 255, 224, 224, 224, 255, 235, 235, 235, 255, 246, 246, 246, 255, 254, 254, 254, 255, 163, 164, 163, 255, 26, 29, 31, 255, 29, 39, 52, 255, 42, 56, 73, 255, 36, 48, 64, 255, 25, 32, 40, 255, 61, 63, 63, 255, 233, 233, 233, 255, 252, 252, 252, 255, 242, 242, 242, 255, 232, 232, 232, 255, 227, 227, 227, 255, 155, 155, 154, 235, 174, 175, 174, 8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 133, 134, 132, 188, 222, 222, 221, 255, 221, 221, 221, 255, 231, 231, 231, 255, 242, 242, 242, 255, 212, 212, 212, 255, 32, 35, 37, 255, 33, 48, 68, 255, 165, 172, 182, 255, 249, 250, 250, 255, 233, 234, 237, 255, 80, 94, 113, 255, 34, 43, 53, 255, 93, 94, 94, 255, 248, 248, 248, 255, 239, 239, 239, 255, 229, 229, 229, 255, 220, 220, 220, 255, 176, 177, 176, 255, 151, 153, 151, 63, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 151, 152, 150, 242, 217, 217, 217, 255, 217, 217, 217, 255, 227, 227, 227, 255, 236, 236, 236, 255, 115, 116, 116, 255, 39, 50, 61, 255, 29, 45, 66, 255, 158, 160, 162, 255, 198, 198, 198, 255, 197, 197, 197, 255, 84, 90, 98, 255, 30, 46, 67, 255, 34, 38, 40, 255, 214, 215, 214, 255, 234, 234, 234, 255, 225, 225, 225, 255, 215, 215, 215, 255, 196, 196, 196, 255, 145, 146, 144, 127, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 146, 148, 145, 245, 212, 212, 212, 255, 212, 212, 212, 255, 221, 221, 221, 255, 229, 229, 229, 255, 76, 78, 78, 255, 42, 56, 74, 255, 28, 40, 55, 255, 22, 25, 27, 255, 20, 22, 22, 255, 19, 22, 22, 255, 36, 40, 44, 255, 28, 43, 63, 255, 42, 48, 53, 255, 170, 171, 171, 255, 227, 227, 227, 255, 219, 219, 219, 255, 210, 210, 210, 255, 198, 198, 197, 255, 127, 129, 126, 130, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 148, 149, 147, 245, 209, 209, 209, 255, 206, 206, 206, 255, 214, 214, 214, 255, 222, 222, 222, 255, 84, 86, 86, 255, 49, 61, 76, 255, 28, 40, 56, 255, 36, 41, 44, 255, 19, 22, 22, 255, 43, 46, 46, 255, 55, 60, 63, 255, 29, 45, 65, 255, 44, 49, 52, 255, 176, 177, 177, 255, 220, 220, 220, 255, 212, 212, 212, 255, 204, 204, 204, 255, 189, 189, 188, 255, 135, 136, 134, 117, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 122, 123, 121, 182, 209, 209, 209, 255, 199, 199, 199, 255, 206, 206, 206, 255, 213, 213, 213, 255, 142, 143, 143, 255, 55, 61, 66, 255, 31, 48, 70, 255, 55, 61, 59, 255, 86, 88, 81, 255, 98, 100, 86, 255, 62, 70, 73, 255, 47, 61, 78, 255, 56, 58, 58, 255, 212, 212, 212, 255, 212, 212, 212, 255, 204, 204, 204, 255, 199, 199, 199, 255, 164, 165, 163, 255, 163, 164, 163, 67, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 133, 135, 132, 106, 183, 183, 182, 255, 197, 197, 197, 255, 198, 198, 198, 255, 204, 204, 204, 255, 210, 210, 210, 255, 73, 76, 76, 255, 67, 74, 82, 255, 56, 67, 72, 255, 113, 115, 50, 255, 101, 105, 58, 255, 64, 74, 84, 255, 57, 60, 62, 255, 151, 152, 152, 255, 209, 209, 209, 255, 203, 203, 203, 255, 197, 197, 197, 255, 208, 208, 208, 255, 142, 143, 142, 237, 197, 197, 196, 8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 219, 219, 218, 26, 138, 139, 137, 239, 214, 214, 214, 255, 198, 198, 198, 255, 196, 196, 196, 255, 200, 200, 200, 255, 207, 207, 207, 255, 104, 105, 105, 255, 59, 62, 63, 255, 64, 69, 72, 255, 64, 67, 69, 255, 65, 68, 69, 255, 160, 161, 161, 255, 209, 209, 209, 255, 198, 198, 198, 255, 196, 196, 196, 255, 201, 201, 201, 255, 190, 191, 190, 255, 116, 118, 116, 123, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 142, 143, 141, 110, 167, 168, 167, 255, 214, 214, 214, 255, 202, 202, 202, 255, 197, 197, 197, 255, 196, 196, 196, 255, 208, 208, 208, 255, 207, 207, 207, 255, 186, 186, 186, 255, 192, 192, 192, 255, 215, 215, 215, 255, 200, 200, 200, 255, 196, 196, 196, 255, 202, 202, 202, 255, 206, 206, 206, 255, 216, 216, 215, 255, 125, 127, 124, 222, 193, 194, 193, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 241, 241, 241, 3, 122, 124, 122, 154, 179, 180, 178, 255, 220, 220, 220, 255, 209, 209, 209, 255, 205, 205, 205, 255, 202, 202, 202, 255, 200, 200, 200, 255, 192, 196, 190, 255, 197, 198, 196, 255, 200, 200, 200, 255, 203, 203, 203, 255, 207, 207, 207, 255, 213, 213, 213, 255, 219, 219, 219, 255, 132, 133, 131, 241, 160, 161, 160, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 196, 196, 196, 1, 133, 134, 133, 151, 153, 154, 153, 252, 223, 223, 223, 255, 221, 221, 221, 255, 216, 216, 216, 255, 211, 213, 211, 255, 99, 158, 74, 255, 153, 184, 139, 255, 215, 215, 215, 255, 217, 217, 217, 255, 225, 225, 225, 255, 199, 199, 199, 255, 129, 130, 129, 232, 157, 158, 157, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 251, 251, 251, 6, 107, 110, 111, 206, 86, 88, 86, 255, 151, 152, 150, 255, 201, 201, 201, 255, 228, 228, 228, 255, 233, 233, 233, 255, 233, 233, 233, 255, 218, 218, 218, 255, 181, 181, 180, 255, 121, 122, 120, 255, 70, 73, 72, 255, 167, 169, 168, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 181, 182, 182, 120, 66, 70, 70, 255, 83, 85, 82, 255, 87, 89, 86, 255, 85, 86, 83, 255, 85, 86, 83, 255, 92, 94, 91, 255, 89, 91, 88, 255, 87, 89, 86, 255, 89, 90, 87, 255, 87, 89, 86, 255, 73, 75, 73, 255, 83, 87, 88, 200, 85, 87, 83, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 130, 130, 202, 67, 70, 68, 255, 86, 88, 85, 255, 101, 102, 99, 255, 114, 116, 112, 255, 127, 129, 126, 255, 141, 142, 140, 255, 137, 139, 136, 255, 123, 124, 121, 255, 107, 109, 106, 255, 91, 93, 90, 255, 79, 81, 78, 255, 71, 76, 75, 255, 141, 143, 140, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 134, 134, 172, 64, 68, 67, 255, 89, 91, 87, 255, 103, 104, 101, 255, 116, 118, 115, 255, 131, 133, 130, 255, 146, 148, 145, 255, 142, 144, 141, 255, 126, 127, 124, 255, 110, 112, 109, 255, 102, 103, 101, 255, 106, 107, 105, 255, 75, 79, 80, 232, 152, 154, 153, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 182, 182, 182, 23, 93, 97, 98, 156, 83, 87, 87, 247, 88, 91, 91, 255, 101, 104, 104, 255, 114, 117, 116, 255, 124, 127, 127, 255, 124, 127, 126, 255, 117, 120, 119, 255, 107, 110, 110, 255, 97, 101, 101, 255, 82, 86, 87, 210, 105, 109, 109, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 199, 199, 199, 49, 185, 185, 185, 131, 162, 163, 163, 140, 147, 149, 148, 140, 142, 144, 143, 140, 143, 145, 144, 140, 151, 153, 153, 140, 162, 163, 163, 110, 180, 181, 181, 73, 192, 192, 192, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image = IupImageRGBA(24, 24, imgdata);
  return image;
}
