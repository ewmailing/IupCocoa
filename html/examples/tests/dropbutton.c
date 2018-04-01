#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupkey.h"
#include "iupcontrols.h"



#define TEST_IMAGE_SIZE 20

static Ihandle* load_image_FileSave(void)
{
  unsigned char imgdata[] = {
    255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 183, 182, 245, 255, 183, 182, 245, 255, 179, 178, 243, 255, 174, 173, 241, 255, 168, 167, 238, 255, 162, 161, 234, 255, 155, 154, 231, 255, 148, 147, 228, 255, 143, 142, 224, 255, 136, 135, 221, 255, 129, 128, 218, 255, 123, 122, 214, 255, 117, 116, 211, 255, 112, 111, 209, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 179, 178, 243, 255, 190, 189, 255, 255, 147, 146, 248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 75, 88, 190, 255, 89, 88, 176, 255, 89, 88, 176, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 173, 172, 240, 255, 190, 189, 255, 255, 138, 137, 239, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 63, 82, 184, 255, 51, 51, 103, 255, 86, 85, 170, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 167, 166, 237, 255, 190, 189, 255, 255, 129, 128, 230, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 52, 77, 179, 255, 122, 121, 223, 255, 83, 82, 164, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 159, 158, 233, 255, 190, 189, 255, 255, 119, 118, 220, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 210, 219, 234, 255, 40, 71, 173, 255, 114, 113, 215, 255, 80, 79, 159, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 152, 151, 229, 255, 190, 189, 255, 255, 110, 109, 211, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 210, 219, 234, 255, 198, 209, 229, 255, 28, 65, 167, 255, 103, 103, 204, 255, 77, 77, 154, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 146, 145, 226, 255, 190, 189, 255, 255, 103, 102, 204, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 210, 219, 234, 255, 198, 209, 229, 255, 189, 202, 225, 255, 17, 59, 161, 255, 92, 93, 194, 255, 74, 74, 148, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 139, 138, 223, 255, 188, 187, 255, 255, 183, 182, 255, 255, 96, 99, 201, 255, 86, 94, 196, 255, 75, 88, 190, 255, 63, 82, 184, 255, 52, 77, 179, 255, 40, 71, 173, 255, 28, 65, 167, 255, 17, 59, 161, 255, 92, 93, 193, 255, 84, 86, 186, 255, 71, 71, 143, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 132, 131, 219, 255, 180, 179, 255, 255, 174, 173, 255, 255, 164, 163, 252, 255, 143, 142, 244, 255, 135, 134, 236, 255, 129, 128, 230, 255, 122, 121, 223, 255, 114, 113, 215, 255, 108, 107, 209, 255, 92, 93, 193, 255, 84, 86, 186, 255, 76, 80, 178, 255, 68, 68, 137, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 124, 123, 215, 255, 170, 169, 255, 255, 160, 159, 251, 255, 148, 147, 245, 255, 75, 91, 113, 255, 75, 91, 113, 255, 75, 91, 113, 255, 75, 91, 113, 255, 82, 98, 118, 255, 91, 106, 125, 255, 84, 86, 186, 255, 76, 79, 178, 255, 68, 73, 170, 255, 65, 65, 131, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 118, 117, 212, 255, 160, 159, 255, 255, 145, 144, 245, 255, 135, 134, 236, 255, 75, 91, 113, 255, 0, 0, 0, 255, 52, 60, 71, 255, 206, 217, 233, 255, 212, 221, 236, 255, 103, 116, 133, 255, 67, 75, 174, 255, 68, 73, 170, 255, 60, 66, 163, 255, 62, 62, 125, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 112, 111, 209, 255, 154, 153, 255, 255, 135, 134, 236, 255, 129, 128, 230, 255, 75, 91, 113, 255, 52, 60, 71, 255, 104, 120, 141, 255, 216, 224, 237, 255, 224, 231, 241, 255, 115, 127, 143, 255, 53, 65, 163, 255, 60, 66, 162, 255, 53, 61, 156, 255, 60, 59, 120, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 108, 107, 207, 255, 143, 142, 243, 255, 129, 128, 230, 255, 36, 68, 170, 255, 33, 50, 71, 255, 171, 180, 195, 255, 179, 187, 198, 255, 188, 193, 202, 255, 196, 200, 206, 255, 72, 77, 86, 255, 51, 62, 158, 255, 54, 61, 156, 255, 49, 57, 152, 255, 57, 57, 114, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 108, 107, 207, 84, 101, 100, 195, 255, 86, 85, 170, 255, 83, 82, 164, 255, 80, 79, 159, 255, 77, 77, 154, 255, 74, 74, 148, 255, 71, 71, 143, 255, 68, 68, 137, 255, 65, 65, 131, 255, 60, 59, 120, 255, 60, 59, 120, 255, 57, 57, 114, 255, 55, 54, 110, 255, 255,  0, 255, 255,
    255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255,  0, 255, 255, 255, 0, 255, 255};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static unsigned char image_data_8 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
{
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static unsigned char image_data_8_pressed [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
{
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,5,1,1,1,1,1,2,2,2,2,2,5,0,0,0,5, 
  5,0,0,0,1,5,1,1,1,1,2,2,2,2,5,2,0,0,0,5, 
  5,0,0,0,1,1,5,1,1,1,2,2,2,5,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,5,1,1,2,2,5,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,5,1,2,5,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,5,5,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,5,5,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,5,3,4,5,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,5,3,3,4,4,5,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,5,3,3,3,4,4,4,5,4,4,0,0,0,5, 
  5,0,0,0,3,5,3,3,3,3,4,4,4,4,5,4,0,0,0,5, 
  5,0,0,0,5,3,3,3,3,3,4,4,4,4,4,5,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static unsigned char image_data_8_inactive [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
{
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,5,5,1,1,1,1,1,1,1,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,1,1,1,1,1,1,1,5,5,5,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static unsigned char image_data_24 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE*3] = 
{
  000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000, 
  000,000,000,255,255,255,255,255,255,255,255,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,000,000,000,
  000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
};

static unsigned char image_data_32 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE*4] = 
{
  000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255, 
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,000,000,255,255,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,255,255,255,192,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,255,
  000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,000,000,000,255,
};

static char* get_name(Ihandle* ih)
{
  return IupGetAttribute(ih, "NAME");
}

static void show_menu(Ihandle* ih)
{
  int x, y;

  Ihandle* menu_file = IupMenu(
    IupSetAttributes(IupItem("Item with Image", "item_cb"), "IMAGE=image_tec"),
    IupSetAttributes(IupItem("Toggle using VALUE", NULL), "VALUE=ON, KEY=K_V"), 
    IupSetAttributes(IupItem("Auto &Toggle", "item_cb"), "AUTOTOGGLE=YES, VALUE=OFF, IMAGE=image_test, IMAGEPRESS=image_test_pressed"), 
    IupSeparator(), 
    IupItem("E&xit (Close)", NULL), 
    NULL);
  Ihandle* menu = IupMenu(
    IupSetAttributes(IupSubmenu("Submenu", menu_file), "KEY=K_S, IMAGE=image_tec"), 
    IupItem("Item", "item_cb"), 
    IupSetAttributes(IupItem("Item", "item_cb"), "VALUE=ON"), 
    IupSetAttributes(IupItem("Item", "item_cb"), "KEY=K_I, IMAGE=image_tec"), 
    NULL);

  x = IupGetInt(ih, "X");
  y = IupGetInt(ih, "Y");
  y += IupGetInt2(ih, "RASTERSIZE");

  IupPopup(menu, x, y);

  IupDestroy(menu);
}

static int action_cb(Ihandle *ih)
{
  static int count = 1;
  printf("FLAT_ACTION(%s) - %d\n", get_name(ih), count); count++;
  //show_menu(ih);
  return IUP_DEFAULT;
}

static int getfocus_cb(Ihandle *ih)
{
  printf("GETFOCUS_CB(%s)\n", get_name(ih));
  return IUP_DEFAULT;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB(%s)\n", get_name(ih));
  return IUP_DEFAULT;
}
     
static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB(%s)\n", get_name(ih));
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB(%s)\n", get_name(ih));
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  printf("ENTERWINDOW_CB(%s)\n", get_name(ih));
  return IUP_DEFAULT;
}

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  printf("BUTTON_CB(%s, but=%c (%d), x=%d, y=%d [%s])\n", get_name(ih),(char)but,pressed,x,y, status);
  return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  if (iup_isprint(c))
    printf("K_ANY(%s, %d = %s \'%c\')\n", get_name(ih), c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%s, %d = %s)\n", get_name(ih), c, iupKeyCodeToName(c));
  printf("  MODKEYSTATE(%s)\n", IupGetGlobal("MODKEYSTATE"));
  return IUP_DEFAULT;
}

static int active_cb(Ihandle *ih)
{
  static int active = 1;
  Ihandle* dlg = IupGetDialog(ih);
  Ihandle* box = IupGetChild(dlg, 0);
  if (active)
    IupSetAttribute(box, "ACTIVE", "NO");
  else
    IupSetAttribute(box, "ACTIVE", "YES");
  active = !active;
  IupSetAttribute(ih, "ACTIVE", "YES");
  return IUP_DEFAULT;
}

static int dropdown_cb(Ihandle *ih, int state)
{
  printf("DROPDOWN_CB(%d)\n", state);
  return IUP_DEFAULT;
}

static void set_callbacks(Ihandle* button)
{
  IupSetCallback(button, "FLAT_ACTION",       action_cb);
//  IupSetCallback(button, "FLAT_BUTTON_CB",    (Icallback)button_cb);
  IupSetCallback(button, "DROPDOWN_CB", (Icallback)dropdown_cb);

  //IupSetCallback(button, "K_ANY",        (Icallback)k_any);
  IupSetCallback(button, "HELP_CB",      (Icallback)help_cb);

  //IupSetCallback(button, "GETFOCUS_CB",  (Icallback)getfocus_cb); 
  //IupSetCallback(button, "KILLFOCUS_CB", (Icallback)killfocus_cb);
//  IupSetCallback(button, "FLAT_ENTERWINDOW_CB", (Icallback)enterwindow_cb);
//  IupSetCallback(button, "FLAT_LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
}

static int show_cb(Ihandle *ih, int state)
{
  char* state_str[5] ={
    "SHOW",
    "RESTORE", 
    "MINIMIZE",
    "MAXIMIZE",
    "HIDE"
  };
  printf("SHOW_CB(%s, %s)\n", IupGetAttribute(ih, "TESTTITLE"), state_str[state]);
  return IUP_DEFAULT;
}

static Ihandle* CreateMatrixList(void)
{
  Ihandle *mlist;

  mlist = IupMatrixList();
  IupSetInt(mlist, "COUNT", 10);
  IupSetInt(mlist, "VISIBLELINES", 9);
  IupSetAttribute(mlist, "COLUMNORDER", "LABEL:COLOR:IMAGE");
  //  IupSetAttribute(mlist, "COLUMNORDER", "LABEL:COLOR");
  //  IupSetAttribute(mlist, "COLUMNORDER", "LABEL");
  //  IupSetAttribute(mlist, "ACTIVE", "NO");
  //  IupSetAttribute(mlist, "FOCUSCOLOR", "BGCOLOR");
//  IupSetAttribute(mlist, "SHOWDELETE", "Yes");
  //  IupSetAttribute(mlist, "EXPAND", "Yes");

  IupSetAttribute(mlist, "EDITABLE", "Yes");
//  IupSetCallback(mlist, "LISTACTION_CB", (Icallback)listaction_cb);

  /* Bluish style */
  if (1)
  {
    IupSetAttribute(mlist, "TITLE", "Test");
    IupSetAttribute(mlist, "BGCOLOR", "220 230 240");
    IupSetAttribute(mlist, "FRAMECOLOR", "120 140 160");
    IupSetAttribute(mlist, "ITEMBGCOLOR0", "120 140 160");
    IupSetAttribute(mlist, "ITEMFGCOLOR0", "255 255 255");
  }

  IupSetAttribute(mlist, "1", "AAA");
  IupSetAttribute(mlist, "2", "BBB");
  IupSetAttribute(mlist, "3", "CCC");
  IupSetAttribute(mlist, "4", "DDD");
  IupSetAttribute(mlist, "5", "EEE");
  IupSetAttribute(mlist, "6", "FFF");
  IupSetAttribute(mlist, "7", "GGG");
  IupSetAttribute(mlist, "8", "HHH");
  IupSetAttribute(mlist, "9", "III");
  IupSetAttribute(mlist, "10", "JJJ");

  IupSetAttribute(mlist, "COLOR1", "255 0 0");
  IupSetAttribute(mlist, "COLOR2", "255 255 0");
  //IupSetAttribute(mlist, "COLOR3", "0 255 0");
  IupSetAttribute(mlist, "COLOR4", "0 255 255");
  IupSetAttribute(mlist, "COLOR5", "0 0 255");
  IupSetAttribute(mlist, "COLOR6", "255 0 255");
  IupSetAttribute(mlist, "COLOR7", "255 128 0");
  IupSetAttribute(mlist, "COLOR8", "255 128 128");
  IupSetAttribute(mlist, "COLOR9", "0 255 128");
  IupSetAttribute(mlist, "COLOR10", "128 255 128");

  IupSetAttribute(mlist, "ITEMACTIVE3", "NO");
  IupSetAttribute(mlist, "ITEMACTIVE7", "NO");
  IupSetAttribute(mlist, "ITEMACTIVE8", "NO");

  IupSetAttribute(mlist, "IMAGEACTIVE9", "No");

  IupSetAttribute(mlist, "IMAGEVALUE1", "ON");
  IupSetAttribute(mlist, "IMAGEVALUE2", "ON");
  IupSetAttribute(mlist, "IMAGEVALUE3", "ON");

  IupSetAttribute(mlist, "FLATSCROLLBAR", "VERTICAL");
  //  IupSetAttribute(mlist, "SHOWFLOATING", "Yes");
  //IupSetAttribute(mlist, "EXPAND", "VERTICAL");

  return mlist;
}

static int list_cb(Ihandle* list, char *text, int item, int state)
{
  if (state == 1)
  {
    Ihandle* ih = IupGetAttributeHandle(IupGetDialog(list), "DROPBUTTON");
    IupSetAttribute(ih, "SHOWDROPDOWN", "No");
    IupSetStrAttribute(ih, "TITLE", text);
  }
  return IUP_DEFAULT;
}

static Ihandle* CreateList(void)
{
  Ihandle* list = IupList(NULL);
  IupSetAttributes(list, "1=\"Banana\", 2=\"Apple\", 3=\"Orange\", 4=\"Strawberry\", 5=\"Grape\","
                   "XXX_VALUE=2, XXX_BGCOLOR=\"192 64 192\"");
  IupSetAttribute(list, "VISIBLEITEMS", "20");
  //  IupSetAttribute(list, "VISIBLECOLUMNS", "7");
  IupSetAttribute(list, "VISIBLELINES", "4");
  IupSetCallback(list, "ACTION", (Icallback)list_cb);
  return list;
}

static int text_kcr_cb(Ihandle* text)
{
  Ihandle* ih = IupGetAttributeHandle(IupGetDialog(text), "DROPBUTTON");
  IupSetAttribute(ih, "SHOWDROPDOWN", "No");
  IupSetStrAttribute(ih, "TITLE", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static Ihandle* CreateListText(void)
{
  Ihandle* list = CreateList();
  Ihandle* text = IupText(NULL);
  IupSetAttribute(text, "EXPAND", "HORIZONTAL");
  IupSetAttribute(list, "NAME", "DB-LIST");
  IupSetAttribute(text, "NAME", "DB-TEXT");
  IupSetCallback(text, "K_CR", (Icallback)text_kcr_cb);
  IupSetCallback(list, "GETFOCUS_CB", (Icallback)getfocus_cb);
  IupSetCallback(text, "GETFOCUS_CB", (Icallback)getfocus_cb);
  Ihandle* vbox = IupVbox(text, list, NULL);
  return vbox;
}

void DropButtonTest(void)
{
  Ihandle *dlg, *button, *label, *image1, *image1i, *image1p, *image2, *image3, 
          *box1, *box2;
  int toggle = 0;

  box1 = IupVbox(NULL);
  IupSetAttribute(box1, "MARGIN", "5x5");
  IupSetAttribute(box1, "GAP", "5");
//  IupSetAttribute(box1, "BGCOLOR", "75 150 170");
//  IupSetAttribute(box1, "PADDING", "15x15");

  button = IupDropButton(NULL);
  IupSetStrAttribute(button, "TITLE", "_@IUP_OK");
  IupSetCallback(button, "FLAT_ACTION", active_cb);
  IupAppend(box1, button);

  button = IupDropButton(NULL);
  if (IupGetInt(NULL, "UTF8MODE"))
    IupSetAttribute(button, "TITLE", "Test(Γ§Γ£ΓµΓ΅Γ³Γ©)");
  else
    IupSetAttribute(button, "TITLE", "Test[ηγυασι]");
  IupSetAttribute(button, "TIP", "Button & Tip");
  //IupSetAttribute(button, "PADDING", "15x15");
  //IupSetAttribute(button, "BGCOLOR", "128 128 255");
//  IupSetAttribute(button, "SIZE", "40");
//  IupSetAttribute(button, "EXPAND", "Yes");
//  IupSetAttribute(button, "FGCOLOR", "0 0 255");
//  IupSetAttribute(button, "RASTERSIZE", "200x100");
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateList());
//  IupSetAttribute(button, "ALIGNMENT", "ACENTER:ACENTER");
//  IupSetAttribute(button, "ALIGNMENT", "ALEFT:ATOP");
  IupSetAttribute(button, "NAME", "button1");
  IupSetAttribute(button, "CANFOCUS", "NO");
  IupSetAttribute(button, "SHOWBORDER", "Yes");
  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  set_callbacks(button);
  IupAppend(box1, button);

  button = IupDropButton(NULL);
  IupSetAttribute(button, "TITLE", "Text1\nSecond Line");
//  IupSetAttribute(button, "RASTERSIZE", "200x100");
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateList());
//  IupSetAttribute(button, "ALIGNMENT", "ACENTER:ACENTER");
  IupSetAttribute(button, "FONT", "Helvetica, Underline 14");
  IupSetAttribute(button, "NAME", "button2");
  IupSetAttribute(button, "DROPPOSITION", "TOPRIGHT");
  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  set_callbacks(button);
  IupAppend(box1, button);

  button = IupDropButton(NULL);
  IupSetAttribute(button, "TITLE", "Text2\nSecond Line");
//  IupSetAttribute(button, "RASTERSIZE", "200x100");
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateList());
  IupSetAttribute(button, "TEXTALIGNMENT", "ACENTER");
//  IupSetAttribute(button, "ALIGNMENT", "ARIGHT:ABOTTOM");
  IupSetAttribute(button, "NAME", "button3");
  IupSetAttribute(button, "CANFOCUS", "NO");
  IupSetAttribute(button, "DROPONARROW", "NO");
  IupSetAttribute(button, "DROPPOSITION", "TOPLEFT");
  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  set_callbacks(button);
  IupAppend(box1, button);

  button = IupDropButton(NULL);
  IupSetAttribute(button, "RASTERSIZE", "60x30");
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateListText());
  IupSetAttribute(button, "FGCOLOR", "255 128 92");
  IupSetAttribute(button, "ARROWCOLOR", "0 0 0");
  IupSetAttribute(button, "NAME", "color");
  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  //  IupSetAttribute(button, "EXPAND", "HORIZONTAL");
  set_callbacks(button);
  IupAppend(box1, button);

  box2 = IupVbox(NULL);
  IupSetAttribute(box2, "MARGIN", "5x5");
  IupSetAttribute(box2, "GAP", "5");
//  IupSetAttribute(box2, "BGCOLOR", "75 150 170");
//  IupSetAttribute(box2, "PADDING", "15x15");

  image1 = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8);
  IupSetAttribute(image1, "0", "BGCOLOR");
  IupSetAttribute(image1, "1", "255 0 0"); 
  IupSetAttribute(image1, "2", "0 255 0");
  IupSetAttribute(image1, "3", "0 0 255"); 
  IupSetAttribute(image1, "4", "255 255 255"); 
  IupSetAttribute(image1, "5", "0 0 0"); 

#if 0
  image1i = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8_inactive);
  IupSetAttribute(image1i, "0", "BGCOLOR");
  IupSetAttribute(image1i, "1", "255 0 0"); 
  IupSetAttribute(image1i, "2", "0 255 0");
  IupSetAttribute(image1i, "3", "0 0 255"); 
  IupSetAttribute(image1i, "4", "255 255 255"); 
  IupSetAttribute(image1i, "5", "0 0 0"); 
  IupSetHandle("image1i", image1i); /* so it will be destroyed even when not used */
#endif

#if 0
  image1p = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8_pressed);
  IupSetAttribute(image1p, "0", "BGCOLOR");
  IupSetAttribute(image1p, "1", "255 0 0"); 
  IupSetAttribute(image1p, "2", "0 255 0");
  IupSetAttribute(image1p, "3", "0 0 255"); 
  IupSetAttribute(image1p, "4", "255 255 255"); 
  IupSetAttribute(image1p, "5", "0 0 0"); 
#endif

  image2 = IupImageRGB(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_24);

  image3 = IupImageRGBA(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_32);

  button = IupDropButton(NULL);
  IupSetAttribute(button, "TITLE", "Images");
  IupSetAttributeHandle(button, "IMAGE", load_image_FileSave());
  
  //IupSetAttribute(button, "IMAGE", "TECGRAF_BITMAP");
  IupAppend(box2, button);

  button = IupDropButton(NULL);
//  IupSetAttribute(button, "ALIGNMENT", "ALEFT:ATOP");
//  IupSetAttribute(button, "RASTERSIZE", "200x100");
//  IupSetAttribute(button, "IMAGEPOSITION", "BOTTOM");
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateMatrixList());
  IupSetAttribute(button, "PADDING", "5x5");
  IupSetAttribute(button, "TITLE", "Text");
//  IupSetAttribute(button, "FONT", "Helvetica, 14");
  IupSetAttribute(button, "IMAGE", "IUP_Tecgraf");
//  IupSetAttributeHandle(button, "IMAGE", image1);
//  IupSetAttributeHandle(button, "IMAGEINACTIVE", image1i);
//  IupSetAttributeHandle(button, "IMAGEPRESS", image1p);
  IupSetAttribute(button, "TIP", "Image Label");
  IupSetAttribute(button, "NAME", "button4");
  IupSetAttribute(button, "PADDING", "5x5");
  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  set_callbacks(button);
  IupAppend(box2, button);

  button = IupDropButton(NULL);
  IupSetAttributeHandle(button, "IMAGE", image2);
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateMatrixList());
  IupSetAttribute(button, "DROPONARROW", "NO");
  IupSetAttribute(button, "IMAGEPOSITION", "TOP");
  IupSetAttribute(button, "PADDING", "15x15");
  IupSetAttribute(button, "TOGGLE", "Yes");
  IupSetAttribute(button, "VALUE", "OFF");
  IupSetAttribute(button, "TITLE", "Text2");

//  IupSetAttribute(button, "SPACING", "30");
//  IupSetAttribute(button, "ALIGNMENT", "ALEFT");
//  IupSetAttribute(button, "RASTERSIZE", "200x100");
//  IupSetAttributeHandle(button, "IMAGEPRESS", image2);
  //IupSetAttribute(button, "CANFOCUS", "NO");
//  IupSetAttribute(button, "RASTERSIZE", "15x15");
  IupSetAttribute(button, "NAME", "button5");
//  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  set_callbacks(button);
  IupAppend(box2, button);

  button = IupDropButton(NULL);
  IupSetAttributeHandle(button, "IMAGE", image3);
  IupSetAttribute(button, "DROPCHILD_HANDLE", (char*)CreateMatrixList());
  IupSetAttribute(button, "DROPPOSITION", "BOTTOMRIGHT");
  IupSetAttribute(button, "TITLE", "Text3");
//  IupSetAttribute(button, "RASTERSIZE", "200x100");
//  IupSetAttribute(button, "ALIGNMENT", "ARIGHT");
  IupSetAttribute(button, "NAME", "button6");
  if (toggle) IupSetAttribute(button, "TOGGLE", "Yes");
  set_callbacks(button);
  IupAppend(box2, button);

  label = IupLabel(NULL);
  IupSetAttribute(label, "SEPARATOR", "VERTICAL");
  IupSetHandle("seplabel", label);

  //dlg = IupDialog(IupHbox(IupRadio(box1), label, IupRadio(box2), NULL));
  dlg = IupDialog(IupHbox(box1, label, box2, NULL));
  IupSetAttribute(dlg, "TITLE", "IupDropButton Test");
//  IupSetAttribute(box1, "BGCOLOR", "128 0 0");
//  IupSetAttribute(dlg, "BGCOLOR", "0 128 0");
//  IupSetAttribute(dlg, "BACKGROUND", "255 128 128");
//  IupSetAttributeHandle(dlg, "BACKGROUND", image2);
//  IupSetAttribute(dlg, "BGCOLOR", "173 177 194");  // Motif BGCOLOR for documentation

  IupSetAttributeHandle(dlg, "STARTFOCUS", button);

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupImageLibOpen();
  IupControlsOpen();

  DropButtonTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
