/*  IupGetColor: Example in C
    Creates a predefined color selection dialog. The user receives the color in the RGB format.
*/

#include <stdlib.h>
#include <stdio.h>

#include <iup_plus.h>

int main(int argc, char **argv)
{
  unsigned char r, g, b;

  Iup::Open(argc, argv);
  Iup::Controls::Open();

  if (Iup::GetColor(100, 100, r, g, b))
    printf("RGB = %.2X%.2X%.2X\n", r, g, b);

  Iup::Close();
  return 0;
}
