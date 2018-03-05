#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <iupcontrols.h>

static Ihandle* load_image_Tecgraf(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 88, 95, 108, 1, 90, 100, 117, 99, 123, 138, 166, 126, 137, 152, 181, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 95, 105, 123, 147, 122, 137, 165, 255, 136, 152, 183, 255, 132, 149, 179, 250, 133, 149, 178, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 92, 100, 115, 43, 111, 125, 150, 253, 140, 158, 190, 255, 135, 151, 182, 255, 132, 149, 179, 255, 131, 147, 177, 217, 153, 164, 188, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 113, 134, 148, 134, 151, 182, 255, 137, 154, 185, 255, 115, 129, 154, 252, 114, 128, 155, 255, 130, 146, 175, 255, 132, 147, 175, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 158, 159, 162, 3, 108, 121, 145, 230, 144, 162, 195, 255, 137, 154, 185, 197, 74, 79, 86, 45, 41, 46, 55, 246, 120, 134, 162, 255, 129, 145, 174, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 95, 101, 113, 48, 124, 139, 167, 255, 144, 161, 194, 255, 138, 155, 186, 67, 0, 0, 0, 0, 49, 54, 62, 150, 87, 98, 118, 255, 128, 144, 173, 223, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 113, 132, 105, 137, 154, 185, 255, 139, 156, 188, 231, 143, 159, 187, 3, 0, 0, 0, 0, 64, 68, 76, 61, 70, 79, 95, 255, 127, 143, 172, 254, 134, 149, 175, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 109, 121, 142, 153, 141, 159, 191, 255, 139, 156, 188, 164, 0, 0, 0, 0, 0, 0, 0, 0, 79, 82, 87, 3, 69, 77, 92, 241, 122, 137, 165, 255, 127, 142, 170, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 122, 146, 191, 145, 163, 196, 255, 139, 156, 188, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 77, 86, 101, 190, 115, 129, 156, 255, 126, 141, 170, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 125, 149, 227, 150, 168, 201, 255, 141, 157, 188, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 82, 91, 107, 144, 113, 127, 153, 255, 125, 140, 169, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 165, 167, 2, 112, 125, 150, 252, 155, 173, 203, 255, 143, 159, 189, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 85, 94, 110, 109, 114, 128, 155, 255, 125, 140, 168, 175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 167, 181, 1, 120, 130, 149, 33, 48, 53, 59, 69, 43, 46, 52, 100, 50, 54, 59, 137, 116, 130, 156, 255, 155, 171, 201, 255, 105, 118, 142, 155, 104, 117, 141, 151, 105, 118, 141, 151, 105, 118, 142, 151, 101, 113, 136, 185, 111, 124, 150, 255, 116, 130, 156, 220, 112, 125, 148, 95, 115, 127, 150, 67, 123, 134, 156, 33, 168, 176, 190, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 119, 129, 147, 5, 109, 121, 142, 71, 106, 118, 140, 140, 105, 117, 140, 197, 107, 120, 144, 242, 120, 135, 162, 255, 123, 137, 163, 255, 44, 49, 58, 255, 28, 32, 39, 255, 125, 139, 164, 255, 150, 167, 197, 255, 138, 155, 186, 255, 131, 148, 178, 255, 125, 141, 170, 255, 119, 134, 162, 255, 114, 128, 154, 255, 108, 122, 147, 255, 104, 117, 141, 255, 102, 115, 138, 255, 103, 116, 139, 255, 107, 120, 145, 255, 111, 124, 149, 245, 113, 126, 151, 200, 113, 127, 152, 140, 116, 129, 154, 71, 122, 135, 158, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 118, 128, 145, 14, 106, 118, 140, 130, 104, 116, 139, 234, 105, 118, 142, 255, 128, 144, 173, 255, 147, 165, 199, 255, 157, 177, 213, 255, 150, 168, 202, 255, 140, 156, 187, 229, 41, 45, 52, 196, 39, 43, 51, 183, 130, 143, 168, 255, 144, 161, 192, 233, 109, 122, 145, 109, 105, 116, 138, 109, 99, 110, 130, 109, 92, 103, 123, 109, 91, 100, 117, 145, 97, 109, 131, 255, 95, 106, 128, 248, 74, 83, 97, 193, 64, 72, 85, 227, 56, 63, 75, 255, 55, 62, 75, 255, 65, 73, 88, 255, 90, 101, 121, 255, 111, 125, 150, 255, 114, 128, 154, 236, 116, 129, 155, 130, 127, 140, 165, 16, 0, 0, 0, 0,
    95, 101, 113, 22, 103, 115, 137, 220, 103, 116, 140, 255, 110, 123, 148, 255, 146, 165, 198, 255, 147, 165, 197, 232, 142, 158, 188, 147, 131, 144, 169, 78, 115, 123, 139, 20, 0, 0, 0, 0, 0, 0, 0, 0, 91, 97, 108, 68, 128, 142, 167, 255, 144, 162, 193, 212, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 95, 100, 107, 31, 120, 135, 163, 255, 133, 150, 180, 231, 0, 0, 0, 0, 0, 0, 0, 0, 86, 89, 93, 20, 50, 54, 61, 73, 37, 40, 46, 141, 33, 36, 42, 230, 46, 52, 63, 255, 107, 120, 144, 255, 116, 130, 157, 255, 118, 133, 159, 223, 132, 147, 174, 24,
    76, 83, 95, 114, 104, 117, 140, 255, 105, 117, 141, 255, 118, 133, 160, 253, 139, 155, 184, 116, 134, 143, 161, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 100, 110, 74, 122, 137, 163, 255, 143, 160, 191, 200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 114, 120, 31, 123, 138, 166, 255, 136, 153, 183, 228, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 90, 93, 97, 5, 42, 45, 51, 111, 86, 97, 117, 253, 118, 133, 160, 255, 119, 133, 161, 255, 133, 149, 180, 116,
    46, 50, 56, 109, 67, 76, 91, 255, 105, 118, 142, 255, 107, 120, 145, 254, 112, 125, 149, 131, 127, 139, 161, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 98, 109, 74, 116, 130, 156, 255, 142, 159, 190, 200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 115, 122, 31, 128, 143, 172, 255, 141, 157, 185, 228, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124, 137, 163, 7, 122, 136, 162, 122, 120, 135, 162, 254, 121, 136, 164, 255, 136, 152, 184, 255, 126, 141, 168, 116,
    71, 74, 79, 17, 31, 35, 41, 206, 42, 47, 57, 255, 77, 87, 105, 255, 103, 116, 140, 255, 110, 124, 149, 239, 112, 125, 150, 157, 115, 128, 153, 89, 122, 134, 158, 30, 147, 158, 177, 2, 0, 0, 0, 0, 81, 87, 96, 65, 109, 123, 148, 255, 141, 158, 190, 212, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 103, 112, 31, 135, 151, 180, 255, 141, 156, 183, 231, 0, 0, 0, 0, 153, 164, 183, 1, 134, 147, 171, 30, 124, 138, 165, 84, 123, 138, 165, 154, 122, 137, 164, 235, 127, 143, 172, 255, 140, 157, 189, 255, 144, 162, 195, 255, 129, 144, 172, 218, 126, 138, 161, 22,
    0, 0, 0, 0, 64, 68, 73, 7, 39, 43, 49, 118, 32, 36, 42, 225, 30, 35, 42, 255, 50, 57, 68, 255, 72, 81, 97, 255, 91, 102, 123, 255, 105, 118, 142, 255, 113, 127, 152, 240, 115, 129, 155, 204, 111, 124, 149, 196, 111, 125, 150, 255, 126, 141, 170, 234, 119, 133, 159, 120, 120, 134, 160, 116, 121, 135, 161, 117, 121, 135, 162, 119, 116, 130, 155, 152, 127, 142, 170, 255, 125, 140, 168, 248, 123, 138, 166, 199, 130, 145, 173, 235, 140, 155, 183, 255, 143, 160, 190, 255, 143, 161, 193, 255, 147, 165, 199, 255, 145, 164, 197, 255, 132, 148, 177, 230, 127, 140, 166, 126, 124, 134, 151, 12, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 138, 141, 144, 3, 55, 58, 63, 58, 37, 40, 46, 126, 34, 38, 44, 184, 34, 38, 44, 235, 35, 39, 47, 254, 49, 55, 66, 255, 64, 72, 87, 255, 77, 87, 104, 255, 88, 98, 118, 255, 96, 108, 130, 255, 103, 116, 139, 255, 108, 122, 147, 255, 113, 127, 153, 255, 118, 133, 160, 255, 124, 140, 168, 255, 133, 148, 176, 255, 141, 156, 183, 255, 146, 161, 187, 255, 144, 159, 186, 255, 131, 146, 174, 254, 127, 141, 168, 237, 126, 141, 168, 188, 123, 137, 162, 131, 112, 123, 143, 61, 128, 132, 140, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91, 94, 98, 19, 64, 68, 73, 56, 53, 57, 65, 82, 62, 67, 76, 116, 66, 74, 89, 255, 95, 107, 129, 255, 80, 88, 103, 155, 81, 90, 105, 151, 86, 95, 112, 151, 95, 104, 122, 151, 98, 109, 128, 180, 124, 139, 166, 255, 109, 122, 146, 218, 100, 110, 128, 84, 96, 104, 118, 56, 105, 109, 117, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 173, 174, 176, 2, 74, 83, 98, 252, 131, 147, 178, 255, 140, 155, 184, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 122, 134, 157, 114, 151, 169, 203, 255, 123, 138, 165, 174, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 76, 90, 224, 122, 137, 165, 255, 136, 152, 182, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 125, 139, 165, 147, 146, 164, 198, 255, 122, 137, 165, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 69, 81, 191, 110, 124, 149, 255, 134, 151, 181, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 143, 170, 193, 142, 160, 192, 255, 122, 137, 164, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58, 63, 74, 150, 94, 105, 127, 255, 133, 149, 179, 166, 0, 0, 0, 0, 0, 0, 0, 0, 115, 119, 128, 5, 130, 145, 174, 242, 137, 154, 186, 255, 125, 139, 166, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 58, 66, 102, 72, 81, 97, 255, 132, 148, 178, 236, 148, 161, 187, 5, 0, 0, 0, 0, 110, 121, 140, 64, 140, 157, 189, 255, 127, 142, 171, 254, 131, 144, 169, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 65, 71, 43, 47, 52, 63, 255, 127, 143, 172, 255, 132, 148, 177, 75, 0, 0, 0, 0, 121, 134, 158, 160, 139, 156, 188, 255, 123, 138, 165, 223, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 156, 158, 1, 36, 39, 46, 227, 106, 119, 143, 255, 130, 145, 175, 203, 114, 125, 147, 51, 123, 138, 166, 247, 131, 147, 177, 255, 123, 138, 165, 151, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 41, 47, 143, 68, 77, 93, 255, 128, 144, 174, 255, 126, 141, 170, 252, 129, 145, 174, 255, 123, 138, 166, 255, 127, 141, 167, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 68, 73, 40, 34, 38, 46, 250, 117, 131, 158, 255, 126, 142, 171, 255, 124, 140, 168, 255, 125, 139, 166, 214, 140, 152, 172, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 44, 50, 134, 58, 66, 79, 255, 123, 138, 166, 255, 123, 138, 166, 250, 127, 140, 165, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 156, 158, 1, 46, 50, 55, 83, 82, 89, 102, 123, 106, 116, 136, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image = IupImageRGBA(32, 32, imgdata);
  return image;
}

static Ihandle* load_image_LogoTecgraf(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 120, 143, 125, 132, 148, 178, 173, 133, 149, 178, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 110, 130, 48, 130, 147, 177, 254, 124, 139, 167, 254, 131, 147, 176, 137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 128, 153, 134, 142, 159, 191, 194, 47, 52, 61, 110, 114, 128, 154, 222, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 143, 172, 192, 140, 156, 188, 99, 65, 69, 76, 16, 97, 109, 131, 251, 129, 144, 172, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 147, 175, 232, 140, 157, 188, 43, 0, 0, 0, 0, 100, 112, 134, 211, 126, 141, 169, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 78, 88, 26, 48, 52, 57, 60, 135, 150, 178, 254, 108, 121, 145, 83, 105, 118, 142, 76, 106, 119, 143, 201, 118, 133, 159, 122, 117, 129, 152, 25, 168, 176, 190, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    118, 128, 145, 3, 104, 117, 140, 92, 114, 127, 152, 180, 131, 147, 177, 237, 133, 149, 178, 249, 38, 42, 50, 222, 137, 152, 180, 249, 126, 142, 170, 182, 114, 128, 154, 182, 104, 117, 140, 227, 95, 107, 128, 238, 83, 93, 112, 248, 84, 95, 113, 239, 104, 117, 141, 180, 115, 129, 155, 93, 127, 140, 165, 4,
    98, 109, 130, 153, 109, 123, 147, 254, 145, 163, 195, 153, 138, 154, 182, 56, 115, 123, 138, 5, 92, 99, 109, 35, 134, 149, 177, 230, 0, 0, 0, 0, 0, 0, 0, 0, 120, 133, 159, 143, 135, 151, 181, 115, 86, 89, 93, 5, 41, 45, 51, 54, 40, 45, 53, 150, 107, 120, 144, 254, 122, 137, 164, 154,
    51, 57, 66, 147, 83, 93, 112, 255, 108, 121, 145, 159, 113, 126, 151, 62, 123, 136, 159, 8, 87, 93, 103, 35, 125, 141, 169, 230, 0, 0, 0, 0, 0, 0, 0, 0, 129, 143, 169, 143, 140, 156, 184, 115, 134, 147, 172, 8, 124, 138, 165, 60, 124, 139, 167, 155, 131, 147, 177, 255, 131, 147, 176, 153,
    64, 68, 73, 2, 36, 39, 45, 86, 41, 46, 54, 173, 60, 67, 80, 232, 75, 84, 101, 251, 89, 100, 120, 228, 105, 118, 142, 250, 110, 123, 148, 187, 118, 132, 158, 187, 126, 141, 169, 229, 134, 149, 177, 239, 136, 152, 179, 250, 136, 152, 181, 234, 139, 156, 186, 175, 130, 145, 173, 90, 124, 134, 151, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 74, 79, 19, 60, 64, 73, 50, 92, 103, 124, 254, 86, 95, 111, 84, 90, 100, 117, 76, 126, 141, 168, 201, 113, 126, 150, 119, 99, 105, 117, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 105, 125, 231, 135, 151, 181, 46, 0, 0, 0, 0, 137, 154, 184, 212, 123, 137, 164, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 74, 83, 98, 191, 133, 149, 179, 102, 111, 121, 139, 17, 134, 150, 180, 252, 126, 140, 166, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 48, 57, 132, 121, 136, 164, 197, 121, 135, 161, 115, 130, 146, 175, 221, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 47, 52, 46, 87, 98, 118, 254, 126, 142, 170, 254, 124, 139, 166, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 57, 67, 118, 115, 128, 152, 170, 127, 140, 164, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

static unsigned char img_bits1[] = 
{
 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,0,2,0,2,0,2,2,0,2,2,2,0,0,0,2,2,2,0,0,2,0,2,2,0,0,0,2,2,2
,2,2,2,0,2,0,0,2,0,0,2,0,2,0,2,2,2,0,2,0,2,2,0,0,2,0,2,2,2,0,2,2
,2,2,2,0,2,0,2,2,0,2,2,0,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,0,2,2
,2,2,2,0,2,0,2,2,0,2,2,0,2,2,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,2
,2,2,2,0,2,0,2,2,0,2,2,0,2,0,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2
,2,2,2,0,2,0,2,2,0,2,2,0,2,0,2,2,2,0,2,0,2,2,0,0,2,0,2,2,2,0,2,2
,2,2,2,0,2,0,2,2,0,2,2,0,2,2,0,0,0,0,2,2,0,0,2,0,2,2,0,0,0,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,0,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

static unsigned char img_bits2[] = 
{
 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,2,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,2,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2
,2,2,2,2,2,2,2,2,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,3,3,3,0,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,3,3,3,0,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,3,3,3,0,3,0,3,0,3,3,0,3,3,3,1,1,0,3,3,3,0,0,3,0,3,3,0,0,0,3,3,3
,3,3,3,0,3,0,0,3,0,0,3,0,3,0,1,1,3,0,3,0,3,3,0,0,3,0,3,3,3,0,3,3
,3,3,3,0,3,0,3,3,0,3,3,0,3,3,1,1,3,0,3,0,3,3,3,0,3,0,3,3,3,0,3,3
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
,3,3,3,0,3,0,3,3,0,3,3,0,3,0,1,1,3,0,3,0,3,3,0,0,3,0,3,3,3,0,3,3
,3,3,3,0,3,0,3,3,0,3,3,0,3,3,1,1,0,0,3,3,0,0,3,0,3,3,0,0,0,3,3,3
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,3
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,0,3,3,3,0,3,3,3,3,3,3,3,3
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,0,0,0,3,3,3,3,3,3,3,3,3
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
,2,2,2,2,2,2,2,3,3,3,3,3,3,3,1,1,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,2,3,3,3,3,3,3,3,3,1,1,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
};

static int valuechanged_cb(Ihandle *ih)
{
  printf("VALUECHANGED_CB(%s)=%s\n", IupGetClassName(ih), IupGetAttribute(ih, "VALUE"));
  return IUP_DEFAULT;
}

static int getfocus_cb(Ihandle *ih)
{
  printf("GETFOCUS_CB(%s)\n", IupGetClassName(ih));
  return IUP_DEFAULT;
}

static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB(%s)\n", IupGetClassName(ih));
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB(%s)\n", IupGetClassName(ih));
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  printf("ENTERWINDOW_CB(%s)\n", IupGetClassName(ih));
  return IUP_DEFAULT;
}

/* Internal SDK function */
char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  Ihandle* sbox = IupGetDialogChild(ih, "SCROLLBOXTEST");
  Ihandle* tree = IupGetDialogChild(ih, "TREE");
  if (iup_isprint(c))
    printf("K_ANY(%s, %d = %s \'%c\')\n", IupGetClassName(ih), c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%s, %d = %s)\n", IupGetClassName(ih), c, iupKeyCodeToName(c));
  if (c==K_r) { IupRecordInput("inputtest.iup", IUP_RECTEXT); return IUP_IGNORE; }  //IUP_RECBINARY, IUP_RECTEXT 
  if (c==K_s) { IupRecordInput(NULL, 0); IupPlayInput(NULL); return IUP_IGNORE; }
  if (c == K_p) { IupPlayInput("inputtest.iup"); return IUP_IGNORE; }
  if (c == K_u) { IupSetAttribute(sbox, "SCROLLTO", "180,50"); } // aprox. the first label, on second frame of labels
  if (c == K_t) { IupSetAttribute(sbox, "SCROLLTO", "TOP"); }
  if (c == K_b) { IupSetAttribute(sbox, "SCROLLTO", "BOTTOM"); }
  if (c == K_l) { IupSetAttribute(sbox, "SCROLLTOCHILD", "PBAR"); }
  if (c == K_f) { IupSetAttribute(sbox, "SCROLLTOCHILD_HANDLE", (char *) tree); }
  return IUP_CONTINUE;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB(%s)\n", IupGetClassName(ih));
  return IUP_DEFAULT;
}

static void show_menu(Ihandle* ih)
{
  int x, y;

  Ihandle* menu_file = IupMenu(
    IupSetAttributes(IupItem("Item with Image", "item_cb"), "IMAGE=image_tec"),
    IupSetAttributes(IupItem("Toggle using VALUE", NULL), "VALUE=ON, KEY=K_V"), 
    IupSetAttributes(IupItem("Auto &Toggle", "item_cb"), "AUTOTOGGLE=YES, VALUE=OFF, IMAGE=image_test, IMPRESS=image_test_pressed"), 
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
  y = IupGetInt(ih, "Y") + IupGetInt2(ih, "RASTERSIZE");

  IupPopup(menu, x, y);
  IupDestroy(menu);
}

static int action1_cb(Ihandle* ih)
{
  Ihandle* sbox = IupGetDialogChild(ih, "SCROLLBOXTEST");
  //IupSetAttribute(sbox, "SCROLLTO", "0,200");
  //IupSetAttribute(sbox, "SCROLLTO", "TOP");
  IupSetAttribute(sbox, "SCROLLTO", "BOTTOM");
  //  IupSetAttribute(sbox, "SCROLLTOCHILD", "CANVASSBTEST");
  //  IupSetAttribute(IupGetDialog(ih), "BACKGROUND", "255 128 128");
//  show_menu(ih);
  return IUP_DEFAULT;
}

static int action2_cb(Ihandle* ih)
{
  IupSetAttribute(IupGetDialog(ih), "BGCOLOR", "0 128 0");
  return IUP_DEFAULT;
}

static int action3_cb(Ihandle* ih)
{
  printf("ACTION3\n");
//  IupSetAttribute(IupGetChild(IupGetDialog(ih), 0), "BGCOLOR", "128 0 0");
  return IUP_DEFAULT;
}

static Ihandle* set_callbacks(Ihandle* ih)
{
  IupSetCallback(ih, "VALUECHANGED_CB", (Icallback)valuechanged_cb);

//  IupSetCallback(ih, "GETFOCUS_CB", (Icallback)getfocus_cb);
//  IupSetCallback(ih, "KILLFOCUS_CB", (Icallback)killfocus_cb);

//  IupSetCallback(ih, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
//  IupSetCallback(ih, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);

  IupSetCallback(ih, "K_ANY", (Icallback)k_any);
//  IupSetCallback(ih, "HELP_CB", (Icallback)help_cb);

  return ih;
}

static int resize_cb(Ihandle *ih, int w, int h)
{
  printf("RESIZE_CB(%d, %d) RASTERSIZE=%s CLIENTSIZE=%s\n", w, h, IupGetAttribute(ih, "RASTERSIZE"), IupGetAttribute(ih, "CLIENTSIZE"));
  return IUP_DEFAULT;
}

static Ihandle* create_matrix(void)
{
  Ihandle* mat = IupMatrix(NULL); 
  
//  IupSetAttribute(mat, "NUMCOL", "15"); 
  IupSetAttribute(mat, "NUMLIN", "20"); 
  IupSetAttribute(mat, "NUMCOL", "8"); 
//  IupSetAttribute(mat, "NUMCOL", "2"); 
//  IupSetAttribute(mat, "NUMLIN", "3"); 
  
  IupSetAttribute(mat, "0:0", "Inflation");
  IupSetAttribute(mat, "1:0", "Medicine\nPharma");
  IupSetAttribute(mat, "2:0", "Food"); 
  IupSetAttribute(mat, "3:0", "Energy"); 
  IupSetAttribute(mat, "0:1", "January 2000"); 
  IupSetAttribute(mat, "0:2", "February 2000"); 
  IupSetAttribute(mat, "1:1", "5.6\n3.33");
  IupSetAttribute(mat, "2:1", "2.2");
  IupSetAttribute(mat, "3:2", "Very Very Very Very Very Large Text");
  IupSetAttribute(mat, "1:2", "4.5");
  IupSetAttribute(mat, "2:2", "8.1");
  IupSetAttribute(mat, "3:1", "3.4");
  IupSetAttribute(mat, "3:3", "Font Test");
//  IupSetAttribute(mat, "HEIGHT2", "30");
//  IupSetAttribute(mat, "WIDTH2", "190");
//  IupSetAttributeId(mat, "WIDTH", 2, "190");
  IupSetAttribute(mat,"SORTSIGN2","DOWN");
//  IupSetAttribute(mat, "WIDTHDEF", "34");
//  IupSetAttribute(mat,"MULTILINE", "YES");
//  IupSetAttribute(mat,"RESIZEMATRIX", "YES");
//  IupSetAttribute(mat,"HIDDENTEXTMARKS", "YES");
//  IupSetAttribute(mat,"USETITLESIZE", "YES");
  //IupSetAttribute(mat,"SCROLLBAR", "NO");
  //IupSetAttribute(mat, "BGCOLOR1:2", "255 92 255");
  //IupSetAttribute(mat, "BGCOLOR2:*", "92 92 255");
  //IupSetAttribute(mat, "BGCOLOR*:3", "255 92 92");
  //IupSetAttribute(mat, "FGCOLOR1:2", "255 0 0");
  //IupSetAttribute(mat, "FGCOLOR2:*", "0 128 0");
  //IupSetAttribute(mat, "FGCOLOR*:3", "0 0 255");
  IupSetAttribute(mat, "FONT3:3", "Helvetica, 24");
  //IupSetAttribute(mat, "FONT2:*", "Courier, 14");
  //IupSetAttribute(mat, "FONT*:3", "Times, Bold 14");
  //IupSetAttribute(mat, "ALIGNMENT2", "ARIGHT");
//  IupSetAttribute(mat, "ACTIVE", "NO");
  IupSetAttribute(mat, "EXPAND", "NO");
//  IupSetAttribute(mat, "ALIGNMENT", "ALEFT");

  IupSetAttribute(mat,"MARKMODE","CELL");
//  IupSetAttribute(mat,"MARKMODE","LIN");
//  IupSetAttribute(mat,"MARKMULTIPLE","NO");
  IupSetAttribute(mat,"MARKMULTIPLE","YES");
//  IupSetAttribute(mat,"MARKAREA","NOT_CONTINUOUS");
  IupSetAttribute(mat,"MARK2:2","YES");
  IupSetAttribute(mat,"MARK2:3","YES");
  IupSetAttribute(mat,"MARK3:3","YES");

  IupSetAttribute(mat,"FRAMEVERTCOLOR1:2","BGCOLOR");
  IupSetAttribute(mat,"FRAMEHORIZCOLOR1:2","0 0 255");
  IupSetAttribute(mat,"FRAMEHORIZCOLOR1:3","0 255 0");
  IupSetAttribute(mat,"FRAMEVERTCOLOR2:2","255 255 0");
  IupSetAttribute(mat,"FRAMEVERTCOLOR*:4","0 255 0");

//  IupSetAttribute(mat,"MARKMODE","LINCOL");

  //IupSetAttribute(mat, "NUMCOL_VISIBLE_LAST", "YES");
  //IupSetAttribute(mat, "NUMLIN_VISIBLE_LAST", "YES");
//  IupSetAttribute(mat, "WIDTHDEF", "15");
  IupSetAttribute(mat, "20:8", "The End");
  IupSetAttribute(mat, "10:0", "Middle Line");
  IupSetAttribute(mat, "15:0", "Middle Line");
  IupSetAttribute(mat, "0:4", "Middle Column");
  IupSetAttribute(mat, "20:0", "Line Title Test");
  IupSetAttribute(mat, "0:8", "Column Title Test");
  IupSetAttribute(mat, "NUMCOL_VISIBLE", "3");
  IupSetAttribute(mat, "NUMLIN_VISIBLE", "3");
//  IupSetAttribute(mat,"EDITNEXT","COLCR");
//  IupSetAttribute(mat, "LIMITEXPAND", "Yes");
  IupSetAttribute(mat, "NUMCOL_NOSCROLL", "1");

  return mat;
}

void ScrollBoxTest(void)
{
  Ihandle *mnu, *_hbox_1, *_cnv_1, *_vbox_1, *dlg, *img, *_vbox_2,
    *_frm_1, *_frm_2, *_frm_3, *_frm_4, *_frm_5, *pbar, *val, *tabs,
    *_list_1, *_list_2, *_list_3, *_text_1, *_ml_1, *tree;

  //img = IupImage(32,32, img_bits1);
  //IupSetHandle ("img1", img); 
  //IupSetAttribute (img, "0", "0 0 0"); 
  //IupSetAttribute (img, "1", "BGCOLOR");
  //IupSetAttribute (img, "2", "255 0 0");

  img = load_image_Tecgraf();
  IupSetHandle ("img1", img); 

  img = IupImage(32,32, img_bits2);
  IupSetHandle ("img2", img); 
  IupSetAttribute (img, "0", "0 0 0"); 
  IupSetAttribute (img, "1", "0 255 0");
  IupSetAttribute (img, "2", "BGCOLOR");
  IupSetAttribute (img, "3", "255 0 0");

  mnu = IupMenu(
    IupSubmenu("IupSubmenu 1", IupMenu(
      IupSetAttributes(IupItem("IupItem 1 Checked", NULL), "VALUE=ON"),
      IupSeparator(),

      IupSetAttributes(IupItem("IupItem 2 Disabled", NULL), "ACTIVE=NO"),
      NULL)),
    IupItem("IupItem 3", NULL),
    IupItem("IupItem 4", NULL), 
    NULL);
  IupSetHandle("mnu",mnu);

  _frm_1 = IupFrame(
    IupVbox(
      set_callbacks(IupSetAttributes(IupButton("Button Text", NULL), "PADDING=5x5, TIP=\"Button TIP\"")), 
      IupSetCallbacks(set_callbacks(IupSetAttributes(IupButton("Text", NULL), "IMAGE=img1, PADDING=5x5")),"ACTION", action1_cb, NULL), 
      IupSetCallbacks(set_callbacks(IupSetAttributes(IupButton(NULL, NULL), "IMAGE=img1")),"ACTION", action2_cb, NULL), 
      IupSetCallbacks(set_callbacks(IupSetAttributes(IupButton("", NULL), "IMAGE=img1,IMPRESS=img2")),"ACTION", action3_cb, NULL), 
      IupSetCallbacks(set_callbacks(IupSetAttributes(IupButton(NULL, NULL), "BGCOLOR=\"255 0 128\", SIZE=20x10")),"ACTION", action3_cb, NULL), 
      NULL));
  IupSetAttribute(_frm_1,"TITLE","IupButton");

  _frm_2 = IupFrame(
    IupVbox(
      IupSetAttributes(IupLabel("Label Text"), "TIP=\"Label TIP\""),
      IupSetAttributes(IupLabel(NULL), "SEPARATOR=HORIZONTAL, NAME=SAMP_SEP"),
      IupSetAttributes(IupLabel(NULL), "IMAGE=img1"),
      NULL));
  IupSetAttribute(_frm_2,"TITLE","IupLabel");

  _frm_3 = IupFrame(
    IupVbox(
      set_callbacks(IupSetAttributes(IupToggle("Toggle Text", NULL), "VALUE=ON, TIP=\"Toggle TIP\"")),
      set_callbacks(IupSetAttributes(IupToggle(NULL, NULL), "VALUE=ON,IMAGE=img1,IMPRESS=img2")),
      set_callbacks(IupSetAttributes(IupToggle(NULL, NULL), "VALUE=ON,IMAGE=img1")),
      IupSetAttributes(IupFrame(IupRadio(IupVbox(
        set_callbacks(IupToggle("Toggle Text", NULL)), 
        set_callbacks(IupToggle("Toggle Text", NULL)), 
        NULL))), "TITLE=IupRadio"),
      NULL));
  IupSetAttribute(_frm_3,"TITLE","IupToggle");

  _text_1 = IupText( NULL);
  IupSetAttribute(_text_1,"VALUE","Single Line Text");
  IupSetAttribute(_text_1,"SIZE","80x");
  IupSetAttribute(_text_1,"TIP","Text TIP");

  _ml_1 = IupMultiLine( NULL);
  IupSetAttribute(_ml_1,"VALUE","Multiline Text\nSecond Line\nThird Line");
  IupSetAttribute(_ml_1,"EXPAND","YES");
  IupSetAttribute(_ml_1,"SIZE","80x40");
  IupSetAttribute(_ml_1,"TIP","Multiline TIP");

  _frm_4 = IupFrame(IupVbox(
    set_callbacks(_text_1),
    set_callbacks(_ml_1),
    NULL));
  IupSetAttribute(_frm_4,"TITLE","IupText");

  _list_1 = IupList( NULL);
//  IupSetAttribute(_list_1,"EXPAND","YES");
  IupSetAttribute(_list_1,"VALUE","1");
  IupSetAttribute(_list_1,"1","Item 1 Text");
  IupSetAttribute(_list_1,"2","Item 2 Text");
  IupSetAttribute(_list_1,"3","Item 3 Text");
  IupSetAttribute(_list_1,"TIP","List 1");

  _list_2 = IupList( NULL);
  IupSetAttribute(_list_2,"DROPDOWN","YES");
//  IupSetAttribute(_list_2,"EXPAND","YES");
  IupSetAttribute(_list_2,"VALUE","2");
  IupSetAttribute(_list_2,"1","Item 1 Text");
  IupSetAttribute(_list_2,"2","Item 2 Text");
  IupSetAttribute(_list_2,"3","Item 3 Text");
  IupSetAttribute(_list_2,"TIP","List 2");

  _list_3 = IupList( NULL);
  IupSetAttribute(_list_3,"EDITBOX","YES");
//  IupSetAttribute(_list_3,"EXPAND","YES");
  IupSetAttribute(_list_3,"VALUE","3");
  IupSetAttribute(_list_3,"1","Item 1 Text");
  IupSetAttribute(_list_3,"2","Item 2 Text");
  IupSetAttribute(_list_3,"3","Item 3 Text");
  IupSetAttribute(_list_3,"TIP","List 3");

  _frm_5 =  IupFrame(IupVbox(
      set_callbacks(_list_1),
      set_callbacks(_list_2),
      set_callbacks(_list_3),
      NULL));
  IupSetAttribute(_frm_5,"TITLE","IupList");

  _hbox_1 = IupHbox(
    _frm_1,
    _frm_2,
    _frm_3,
    _frm_4,
    _frm_5,
    NULL);

  val = IupVal(NULL);
  IupSetAttribute(val,"TIP","Valuator TIP");
  set_callbacks(val);
  
  pbar = IupProgressBar();
  IupSetFloat(pbar, "VALUE", 0.5);
  IupSetHandle("PBAR", pbar);
  IupSetAttribute(pbar, "TIP", "ProgressBar TIP");
  set_callbacks(pbar);

  tabs = IupTabs(IupVbox(IupLabel("Tab0"), NULL), IupVbox(IupLabel("Tab1"), NULL), IupVbox(IupLabel("Tab2"), NULL), NULL);
  IupSetAttribute(tabs, "TABTITLE0", "Tab Title 0");
  IupSetAttribute(tabs, "TABTITLE1", "Tab Title 1");
  IupSetAttributeHandle(tabs,"TABIMAGE1", load_image_LogoTecgraf());
  IupSetAttribute(tabs,"TABTITLE2","Tab Title 2");
  IupSetAttribute(tabs,"RASTERSIZE","300x50");
  IupSetAttribute(tabs,"TIP","Tabs TIP");
//  IupSetAttribute(tabs,"TABPADDING","5x5");
  set_callbacks(tabs);

  tree = IupTree();
  IupSetAttribute(tree, "NAME", "TREE");
  IupSetAttribute(tree, "SHOWRENAME", "YES");
  IupSetAttribute(tree, "RASTERSIZE", "300x150");
  IupSetAttribute(tree,"TIP","Tree TIP");
  IupSetAttribute(tree,"EXPAND","NO");
  set_callbacks(tree);

  _cnv_1 = IupCanvas(NULL);
  IupSetHandle("CANVASSBTEST", _cnv_1);
  IupSetAttribute(_cnv_1,"BGCOLOR","128 255 0");
  IupSetAttribute(_cnv_1,"SCROLLBAR","YES");
//  IupSetAttribute(_cnv_1,"EXPAND","HORIZONTAL");
  IupSetAttribute(_cnv_1,"EXPAND","NO");
  IupSetAttribute(_cnv_1,"RASTERSIZE","200x100");
  IupSetAttribute(_cnv_1,"TIP","Canvas TIP");
//  IupSetAttribute(_cnv_1,"CANFOCUS","NO");
  set_callbacks(_cnv_1);

  _vbox_1 = IupVbox(
    _hbox_1,
    IupHbox(IupSetAttributes(IupFrame(IupHbox(val, NULL)), "TITLE=IupVal"),
            IupSetAttributes(IupFrame(IupHbox(pbar, NULL)), "TITLE=IupProgressBar"),
            IupSetAttributes(IupFrame(IupHbox(tabs, NULL)), "TITLE=IupTabs"),
            NULL),
    IupHbox(IupSetAttributes(IupFrame(IupHbox(_cnv_1, NULL)), "TITLE=IupCanvas"),
            IupSetAttributes(IupFrame(IupHbox(tree, NULL)), "TITLE=IupTree"),
            NULL),
    IupHbox(IupSetAttributes(IupFrame(IupHbox(create_matrix(), NULL)), "TITLE=IupMatrix"),
            NULL),
    NULL);
  IupSetAttribute(_vbox_1,"MARGIN","5x5");
  IupSetAttribute(_vbox_1,"GAP","5");

//  _vbox_2 = IupVbox(IupSetAttributes(IupScrollBox(_vbox_1), "RASTERSIZE=400x300"), NULL);
  _vbox_2 = IupVbox(IupSetAttributes(IupScrollBox(_vbox_1), "NAME=SCROLLBOXTEST, XXX_LAYOUTDRAG=NO"), NULL);
  IupSetAttribute(_vbox_2,"MARGIN","20x20");

  dlg = IupDialog(_vbox_2);
  IupSetHandle("dlg",dlg);
  IupSetAttribute(dlg,"MENU","mnu");
  IupSetAttribute(dlg,"TITLE","IupDialog Title");
//  IupSetAttribute(dlg,"COMPOSITED","YES");   /* Windows Only */
//  IupSetAttribute(dlg, "OPACITY", "192");
//  IupSetAttribute(dlg, "RESIZE", "NO");

//  IupSetAttribute(dlg, "BGCOLOR", "173 177 194");  // Motif BGCOLOR for documentation
//  IupSetAttribute(_vbox_1, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BACKGROUND", "200 10 80");

  //IupSetGlobal("DLGBGCOLOR", "92 92 255");
  //IupSetGlobal("TXTFGCOLOR", "255 92 92");
  //IupSetGlobal("TXTBGCOLOR", "92 92 255");

//  IupSetAttribute(dlg, "FONT", "Helvetica, 24");
//  IupSetAttribute(dlg, "FONT", "-*-helvetica-*-r-*-*-18-*-*-*-*-*-*-*");
  
//  IupSetAttribute(box, "FGCOLOR", "255 0 0");

//  IupSetAttribute(dlg,"RASTERSIZE","1600x1000");
  IupSetAttribute(dlg,"RASTERSIZE","600x400");
  IupSetCallback(dlg, "RESIZE_CB", (Icallback)resize_cb);

  IupMap(dlg);

  IupSetAttribute(tree, "TITLE0",         "Figures");  
  IupSetAttribute(tree, "ADDLEAF0",      "Other");     /* new id=1 */
  IupSetAttribute(tree, "ADDBRANCH1",   "triangle");  /* new id=2 */     
  IupSetAttribute(tree, "ADDLEAF2",     "equilateral");  /* ... */
  IupSetAttribute(tree, "ADDLEAF3",     "isoceles");
  IupSetAttribute(tree, "ADDLEAF4",     "scalenus");

  IupShow(dlg);

  IupSetAttribute(dlg,"RASTERSIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  ScrollBoxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
