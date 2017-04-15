
static Ihandle* load_image_Lua8(void)
{
  unsigned char imgdata[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 23, 19, 19, 23, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 19, 1, 1, 1, 1, 20, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 26, 1, 1, 1, 1, 1, 1, 23, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 19, 1, 1, 1, 1, 1, 1, 19, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 25, 21, 17, 17, 17, 17, 20, 23, 255, 255, 255, 255, 255, 19, 1, 1, 1, 1, 1, 1, 19, 255,
    255, 255, 255, 255, 255, 255, 255, 23, 18, 6, 1, 1, 1, 1, 1, 1, 1, 2, 1, 16, 25, 255, 255, 24, 1, 1, 1, 1, 1, 1, 23, 255,
    255, 255, 255, 255, 255, 255, 23, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 23, 255, 14, 1, 1, 1, 1, 14, 255, 255,
    255, 255, 255, 255, 255, 21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 11, 5, 1, 9, 23, 255, 23, 15, 17, 24, 255, 255, 255,
    255, 255, 255, 255, 23, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 17, 28, 28, 28, 18, 1, 9, 23, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 26, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 18, 28, 28, 28, 28, 28, 19, 1, 15, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 28, 28, 28, 28, 28, 28, 28, 9, 1, 23, 255, 255, 255, 255, 255, 255,
    255, 255, 24, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 28, 28, 28, 28, 28, 28, 28, 17, 1, 15, 255, 255, 255, 255, 255, 255,
    255, 255, 16, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 28, 28, 28, 28, 28, 28, 28, 11, 1, 1, 23, 255, 255, 255, 255, 255,
    255, 255, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 19, 28, 28, 28, 28, 28, 23, 1, 1, 1, 22, 255, 255, 255, 255, 255,
    255, 24, 1, 1, 1, 7, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 21, 28, 28, 28, 22, 4, 1, 1, 1, 18, 255, 255, 255, 255, 255,
    255, 23, 1, 1, 1, 16, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 9, 4, 1, 1, 1, 1, 1, 14, 255, 255, 255, 255, 255,
    255, 17, 1, 1, 1, 16, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 11, 255, 255, 255, 255, 255,
    255, 17, 1, 1, 1, 16, 27, 1, 1, 1, 1, 11, 21, 1, 1, 8, 17, 1, 13, 27, 27, 27, 11, 1, 1, 1, 11, 255, 255, 255, 255, 255,
    255, 17, 1, 1, 1, 16, 27, 1, 1, 1, 1, 16, 27, 1, 1, 11, 27, 1, 27, 8, 1, 14, 27, 1, 1, 1, 16, 255, 255, 255, 255, 255,
    255, 24, 1, 1, 1, 16, 27, 1, 1, 1, 1, 16, 27, 1, 1, 11, 27, 1, 3, 1, 6, 19, 27, 1, 1, 1, 19, 255, 255, 255, 255, 255,
    255, 255, 8, 1, 1, 16, 27, 1, 1, 1, 1, 16, 27, 1, 1, 11, 27, 1, 20, 27, 27, 27, 27, 1, 1, 1, 23, 255, 255, 255, 255, 255,
    255, 255, 19, 1, 1, 16, 27, 1, 1, 1, 1, 16, 27, 1, 1, 15, 27, 4, 27, 1, 1, 15, 27, 1, 1, 1, 23, 255, 255, 255, 255, 255,
    255, 255, 24, 1, 1, 16, 27, 19, 19, 19, 13, 10, 27, 15, 14, 21, 27, 3, 27, 13, 13, 21, 27, 8, 1, 14, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 10, 1, 16, 27, 27, 27, 27, 11, 1, 11, 27, 27, 27, 11, 1, 12, 27, 27, 27, 21, 27, 5, 23, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 23, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 19, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 23, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 16, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 23, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 25, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 23, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 24, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 23, 17, 5, 1, 1, 1, 1, 1, 1, 1, 13, 21, 23, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 23, 20, 17, 17, 17, 20, 22, 23, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  Ihandle* image = IupImage(32, 32, imgdata);

  IupSetAttribute(image, "0", "0 0 0");
  IupSetAttribute(image, "1", "0 0 123");
  IupSetAttribute(image, "2", "0 0 132");
  IupSetAttribute(image, "3", "8 8 132");
  IupSetAttribute(image, "4", "16 16 132");
  IupSetAttribute(image, "5", "24 24 140");
  IupSetAttribute(image, "6", "33 33 140");
  IupSetAttribute(image, "7", "33 33 148");
  IupSetAttribute(image, "8", "41 41 148");
  IupSetAttribute(image, "9", "49 49 148");
  IupSetAttribute(image, "10", "57 57 156");
  IupSetAttribute(image, "11", "66 66 156");
  IupSetAttribute(image, "12", "66 66 165");
  IupSetAttribute(image, "13", "74 74 165");
  IupSetAttribute(image, "14", "82 82 165");
  IupSetAttribute(image, "15", "90 90 173");
  IupSetAttribute(image, "16", "99 99 173");
  IupSetAttribute(image, "17", "107 107 181");
  IupSetAttribute(image, "18", "115 115 181");
  IupSetAttribute(image, "19", "123 123 189");
  IupSetAttribute(image, "20", "132 132 189");
  IupSetAttribute(image, "21", "140 140 198");
  IupSetAttribute(image, "22", "148 148 198");
  IupSetAttribute(image, "23", "156 156 206");
  IupSetAttribute(image, "24", "165 165 206");
  IupSetAttribute(image, "25", "173 173 214");
  IupSetAttribute(image, "26", "189 189 222");
  IupSetAttribute(image, "27", "214 214 231");
  IupSetAttribute(image, "28", "255 255 255");
  IupSetAttribute(image, "29", "0 0 0");
  IupSetAttribute(image, "30", "0 0 0");
  IupSetAttribute(image, "31", "0 0 0");
  IupSetAttribute(image, "32", "0 0 0");
  IupSetAttribute(image, "33", "0 0 0");
  IupSetAttribute(image, "34", "0 0 0");
  IupSetAttribute(image, "35", "0 0 0");
  IupSetAttribute(image, "36", "0 0 0");
  IupSetAttribute(image, "37", "0 0 0");
  IupSetAttribute(image, "38", "0 0 0");
  IupSetAttribute(image, "39", "0 0 0");
  IupSetAttribute(image, "40", "0 0 0");
  IupSetAttribute(image, "41", "0 0 0");
  IupSetAttribute(image, "42", "0 0 0");
  IupSetAttribute(image, "43", "0 0 0");
  IupSetAttribute(image, "44", "0 0 0");
  IupSetAttribute(image, "45", "0 0 0");
  IupSetAttribute(image, "46", "0 0 0");
  IupSetAttribute(image, "47", "0 0 0");
  IupSetAttribute(image, "48", "0 0 0");
  IupSetAttribute(image, "49", "0 0 0");
  IupSetAttribute(image, "50", "0 0 0");
  IupSetAttribute(image, "51", "0 0 0");
  IupSetAttribute(image, "52", "0 0 0");
  IupSetAttribute(image, "53", "0 0 0");
  IupSetAttribute(image, "54", "0 0 0");
  IupSetAttribute(image, "55", "0 0 0");
  IupSetAttribute(image, "56", "0 0 0");
  IupSetAttribute(image, "57", "0 0 0");
  IupSetAttribute(image, "58", "0 0 0");
  IupSetAttribute(image, "59", "0 0 0");
  IupSetAttribute(image, "60", "0 0 0");
  IupSetAttribute(image, "61", "0 0 0");
  IupSetAttribute(image, "62", "0 0 0");
  IupSetAttribute(image, "63", "0 0 0");
  IupSetAttribute(image, "64", "0 0 0");
  IupSetAttribute(image, "65", "0 0 0");
  IupSetAttribute(image, "66", "0 0 0");
  IupSetAttribute(image, "67", "0 0 0");
  IupSetAttribute(image, "68", "0 0 0");
  IupSetAttribute(image, "69", "0 0 0");
  IupSetAttribute(image, "70", "0 0 0");
  IupSetAttribute(image, "71", "0 0 0");
  IupSetAttribute(image, "72", "0 0 0");
  IupSetAttribute(image, "73", "0 0 0");
  IupSetAttribute(image, "74", "0 0 0");
  IupSetAttribute(image, "75", "0 0 0");
  IupSetAttribute(image, "76", "0 0 0");
  IupSetAttribute(image, "77", "0 0 0");
  IupSetAttribute(image, "78", "0 0 0");
  IupSetAttribute(image, "79", "0 0 0");
  IupSetAttribute(image, "80", "0 0 0");
  IupSetAttribute(image, "81", "0 0 0");
  IupSetAttribute(image, "82", "0 0 0");
  IupSetAttribute(image, "83", "0 0 0");
  IupSetAttribute(image, "84", "0 0 0");
  IupSetAttribute(image, "85", "0 0 0");
  IupSetAttribute(image, "86", "0 0 0");
  IupSetAttribute(image, "87", "0 0 0");
  IupSetAttribute(image, "88", "0 0 0");
  IupSetAttribute(image, "89", "0 0 0");
  IupSetAttribute(image, "90", "0 0 0");
  IupSetAttribute(image, "91", "0 0 0");
  IupSetAttribute(image, "92", "0 0 0");
  IupSetAttribute(image, "93", "0 0 0");
  IupSetAttribute(image, "94", "0 0 0");
  IupSetAttribute(image, "95", "0 0 0");
  IupSetAttribute(image, "96", "0 0 0");
  IupSetAttribute(image, "97", "0 0 0");
  IupSetAttribute(image, "98", "0 0 0");
  IupSetAttribute(image, "99", "0 0 0");
  IupSetAttribute(image, "100", "0 0 0");
  IupSetAttribute(image, "101", "0 0 0");
  IupSetAttribute(image, "102", "0 0 0");
  IupSetAttribute(image, "103", "0 0 0");
  IupSetAttribute(image, "104", "0 0 0");
  IupSetAttribute(image, "105", "0 0 0");
  IupSetAttribute(image, "106", "0 0 0");
  IupSetAttribute(image, "107", "0 0 0");
  IupSetAttribute(image, "108", "0 0 0");
  IupSetAttribute(image, "109", "0 0 0");
  IupSetAttribute(image, "110", "0 0 0");
  IupSetAttribute(image, "111", "0 0 0");
  IupSetAttribute(image, "112", "0 0 0");
  IupSetAttribute(image, "113", "0 0 0");
  IupSetAttribute(image, "114", "0 0 0");
  IupSetAttribute(image, "115", "0 0 0");
  IupSetAttribute(image, "116", "0 0 0");
  IupSetAttribute(image, "117", "0 0 0");
  IupSetAttribute(image, "118", "0 0 0");
  IupSetAttribute(image, "119", "0 0 0");
  IupSetAttribute(image, "120", "0 0 0");
  IupSetAttribute(image, "121", "0 0 0");
  IupSetAttribute(image, "122", "0 0 0");
  IupSetAttribute(image, "123", "0 0 0");
  IupSetAttribute(image, "124", "0 0 0");
  IupSetAttribute(image, "125", "0 0 0");
  IupSetAttribute(image, "126", "0 0 0");
  IupSetAttribute(image, "127", "0 0 0");
  IupSetAttribute(image, "128", "0 0 0");
  IupSetAttribute(image, "129", "0 0 0");
  IupSetAttribute(image, "130", "0 0 0");
  IupSetAttribute(image, "131", "0 0 0");
  IupSetAttribute(image, "132", "0 0 0");
  IupSetAttribute(image, "133", "0 0 0");
  IupSetAttribute(image, "134", "0 0 0");
  IupSetAttribute(image, "135", "0 0 0");
  IupSetAttribute(image, "136", "0 0 0");
  IupSetAttribute(image, "137", "0 0 0");
  IupSetAttribute(image, "138", "0 0 0");
  IupSetAttribute(image, "139", "0 0 0");
  IupSetAttribute(image, "140", "0 0 0");
  IupSetAttribute(image, "141", "0 0 0");
  IupSetAttribute(image, "142", "0 0 0");
  IupSetAttribute(image, "143", "0 0 0");
  IupSetAttribute(image, "144", "0 0 0");
  IupSetAttribute(image, "145", "0 0 0");
  IupSetAttribute(image, "146", "0 0 0");
  IupSetAttribute(image, "147", "0 0 0");
  IupSetAttribute(image, "148", "0 0 0");
  IupSetAttribute(image, "149", "0 0 0");
  IupSetAttribute(image, "150", "0 0 0");
  IupSetAttribute(image, "151", "0 0 0");
  IupSetAttribute(image, "152", "0 0 0");
  IupSetAttribute(image, "153", "0 0 0");
  IupSetAttribute(image, "154", "0 0 0");
  IupSetAttribute(image, "155", "0 0 0");
  IupSetAttribute(image, "156", "0 0 0");
  IupSetAttribute(image, "157", "0 0 0");
  IupSetAttribute(image, "158", "0 0 0");
  IupSetAttribute(image, "159", "0 0 0");
  IupSetAttribute(image, "160", "0 0 0");
  IupSetAttribute(image, "161", "0 0 0");
  IupSetAttribute(image, "162", "0 0 0");
  IupSetAttribute(image, "163", "0 0 0");
  IupSetAttribute(image, "164", "0 0 0");
  IupSetAttribute(image, "165", "0 0 0");
  IupSetAttribute(image, "166", "0 0 0");
  IupSetAttribute(image, "167", "0 0 0");
  IupSetAttribute(image, "168", "0 0 0");
  IupSetAttribute(image, "169", "0 0 0");
  IupSetAttribute(image, "170", "0 0 0");
  IupSetAttribute(image, "171", "0 0 0");
  IupSetAttribute(image, "172", "0 0 0");
  IupSetAttribute(image, "173", "0 0 0");
  IupSetAttribute(image, "174", "0 0 0");
  IupSetAttribute(image, "175", "0 0 0");
  IupSetAttribute(image, "176", "0 0 0");
  IupSetAttribute(image, "177", "0 0 0");
  IupSetAttribute(image, "178", "0 0 0");
  IupSetAttribute(image, "179", "0 0 0");
  IupSetAttribute(image, "180", "0 0 0");
  IupSetAttribute(image, "181", "0 0 0");
  IupSetAttribute(image, "182", "0 0 0");
  IupSetAttribute(image, "183", "0 0 0");
  IupSetAttribute(image, "184", "0 0 0");
  IupSetAttribute(image, "185", "0 0 0");
  IupSetAttribute(image, "186", "0 0 0");
  IupSetAttribute(image, "187", "0 0 0");
  IupSetAttribute(image, "188", "0 0 0");
  IupSetAttribute(image, "189", "0 0 0");
  IupSetAttribute(image, "190", "0 0 0");
  IupSetAttribute(image, "191", "0 0 0");
  IupSetAttribute(image, "192", "0 0 0");
  IupSetAttribute(image, "193", "0 0 0");
  IupSetAttribute(image, "194", "0 0 0");
  IupSetAttribute(image, "195", "0 0 0");
  IupSetAttribute(image, "196", "0 0 0");
  IupSetAttribute(image, "197", "0 0 0");
  IupSetAttribute(image, "198", "0 0 0");
  IupSetAttribute(image, "199", "0 0 0");
  IupSetAttribute(image, "200", "0 0 0");
  IupSetAttribute(image, "201", "0 0 0");
  IupSetAttribute(image, "202", "0 0 0");
  IupSetAttribute(image, "203", "0 0 0");
  IupSetAttribute(image, "204", "0 0 0");
  IupSetAttribute(image, "205", "0 0 0");
  IupSetAttribute(image, "206", "0 0 0");
  IupSetAttribute(image, "207", "0 0 0");
  IupSetAttribute(image, "208", "0 0 0");
  IupSetAttribute(image, "209", "0 0 0");
  IupSetAttribute(image, "210", "0 0 0");
  IupSetAttribute(image, "211", "0 0 0");
  IupSetAttribute(image, "212", "0 0 0");
  IupSetAttribute(image, "213", "0 0 0");
  IupSetAttribute(image, "214", "0 0 0");
  IupSetAttribute(image, "215", "0 0 0");
  IupSetAttribute(image, "216", "0 0 0");
  IupSetAttribute(image, "217", "0 0 0");
  IupSetAttribute(image, "218", "0 0 0");
  IupSetAttribute(image, "219", "0 0 0");
  IupSetAttribute(image, "220", "0 0 0");
  IupSetAttribute(image, "221", "0 0 0");
  IupSetAttribute(image, "222", "0 0 0");
  IupSetAttribute(image, "223", "0 0 0");
  IupSetAttribute(image, "224", "0 0 0");
  IupSetAttribute(image, "225", "0 0 0");
  IupSetAttribute(image, "226", "0 0 0");
  IupSetAttribute(image, "227", "0 0 0");
  IupSetAttribute(image, "228", "0 0 0");
  IupSetAttribute(image, "229", "0 0 0");
  IupSetAttribute(image, "230", "0 0 0");
  IupSetAttribute(image, "231", "0 0 0");
  IupSetAttribute(image, "232", "0 0 0");
  IupSetAttribute(image, "233", "0 0 0");
  IupSetAttribute(image, "234", "0 0 0");
  IupSetAttribute(image, "235", "0 0 0");
  IupSetAttribute(image, "236", "0 0 0");
  IupSetAttribute(image, "237", "0 0 0");
  IupSetAttribute(image, "238", "0 0 0");
  IupSetAttribute(image, "239", "0 0 0");
  IupSetAttribute(image, "240", "0 0 0");
  IupSetAttribute(image, "241", "0 0 0");
  IupSetAttribute(image, "242", "0 0 0");
  IupSetAttribute(image, "243", "0 0 0");
  IupSetAttribute(image, "244", "0 0 0");
  IupSetAttribute(image, "245", "0 0 0");
  IupSetAttribute(image, "246", "0 0 0");
  IupSetAttribute(image, "247", "0 0 0");
  IupSetAttribute(image, "248", "0 0 0");
  IupSetAttribute(image, "249", "0 0 0");
  IupSetAttribute(image, "250", "0 0 0");
  IupSetAttribute(image, "251", "0 0 0");
  IupSetAttribute(image, "252", "0 0 0");
  IupSetAttribute(image, "253", "0 0 0");
  IupSetAttribute(image, "254", "0 0 0");
  IupSetAttribute(image, "255", "BGCOLOR");

  return image;
}

static Ihandle* load_image_PUC_Rio8(void)
{
  unsigned char imgdata[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 242, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 214, 186, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 189, 170, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 234, 218, 167, 118, 123, 166, 216, 234, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 186, 123, 113, 110, 110, 110, 113, 181, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 243, 195, 249, 255, 194, 113, 123, 123, 123, 123, 113, 187, 255, 252, 197, 236, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 243, 14, 195, 246, 124, 126, 125, 126, 133, 124, 133, 122, 241, 225, 14, 233, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 195, 155, 249, 252, 110, 101, 111, 110, 113, 102, 109, 110, 246, 252, 155, 189, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 252, 235, 255, 255, 155, 191, 255, 255, 124, 117, 13, 13, 13, 13, 117, 117, 252, 255, 197, 17, 252, 255, 236, 249, 255, 255, 255, 255,
    255, 255, 255, 255, 252, 184, 183, 245, 106, 2, 191, 252, 117, 134, 133, 114, 131, 125, 135, 116, 249, 195, 3, 105, 246, 189, 169, 249, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 246, 132, 132, 216, 104, 12, 255, 179, 101, 101, 100, 100, 7, 109, 166, 255, 15, 103, 195, 14, 9, 236, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 146, 140, 140, 112, 6, 164, 252, 243, 99, 97, 107, 108, 96, 98, 242, 255, 169, 6, 4, 17, 158, 17, 252, 255, 255, 255, 255,
    255, 255, 255, 255, 231, 142, 176, 148, 168, 40, 22, 26, 44, 39, 18, 61, 61, 38, 38, 44, 27, 19, 20, 170, 17, 158, 17, 225, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 243, 185, 183, 228, 24, 30, 34, 25, 60, 31, 33, 36, 32, 28, 25, 35, 30, 23, 228, 189, 169, 236, 252, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 252, 242, 255, 63, 29, 62, 41, 42, 42, 78, 78, 42, 42, 41, 33, 43, 60, 255, 243, 249, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 37, 49, 57, 52, 10, 83, 91, 11, 52, 51, 50, 36, 63, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 80, 46, 58, 66, 145, 144, 95, 201, 138, 145, 119, 58, 45, 64, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 84, 63, 68, 167, 146, 170, 203, 203, 173, 146, 130, 155, 63, 65, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 252, 191, 170, 246, 252, 71, 82, 55, 150, 190, 207, 208, 208, 207, 175, 151, 53, 79, 59, 249, 249, 212, 212, 249, 255, 255, 255, 255,
    255, 255, 255, 255, 225, 195, 228, 158, 8, 47, 81, 69, 144, 193, 237, 238, 238, 237, 193, 150, 68, 80, 56, 115, 140, 220, 223, 221, 255, 255, 255, 255,
    255, 255, 255, 249, 158, 155, 169, 17, 170, 74, 92, 70, 145, 236, 17, 243, 234, 17, 197, 153, 69, 87, 84, 186, 142, 183, 146, 146, 246, 255, 255, 255,
    255, 255, 255, 228, 195, 243, 170, 243, 170, 88, 84, 158, 141, 236, 1, 243, 158, 252, 155, 152, 161, 74, 84, 217, 232, 216, 235, 230, 222, 255, 255, 255,
    255, 255, 255, 195, 155, 158, 160, 210, 219, 198, 94, 159, 139, 14, 191, 14, 165, 236, 17, 209, 156, 90, 84, 231, 210, 172, 140, 146, 212, 255, 255, 255,
    255, 255, 255, 255, 252, 240, 46, 5, 215, 198, 93, 72, 136, 244, 252, 249, 244, 249, 243, 147, 160, 92, 198, 224, 139, 160, 237, 252, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 252, 86, 121, 162, 205, 93, 75, 129, 141, 143, 176, 176, 141, 141, 137, 73, 93, 204, 170, 120, 77, 247, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 251, 130, 149, 229, 171, 92, 89, 198, 198, 206, 207, 198, 198, 89, 93, 76, 236, 67, 128, 247, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 228, 155, 157, 247, 228, 205, 200, 198, 202, 202, 199, 200, 205, 226, 250, 159, 16, 196, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 252, 178, 67, 211, 252, 255, 255, 246, 77, 85, 243, 255, 255, 255, 215, 148, 180, 250, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 250, 178, 182, 233, 227, 247, 255, 246, 243, 255, 247, 227, 228, 182, 177, 247, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 250, 188, 154, 149, 54, 134, 69, 188, 186, 169, 17, 118, 149, 154, 213, 250, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 252, 224, 174, 178, 127, 21, 134, 48, 67, 149, 163, 192, 230, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 252, 255, 248, 236, 231, 231, 239, 247, 255, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  Ihandle* image = IupImage(32, 32, imgdata);

  IupSetAttribute(image, "0", "0 0 0");
  IupSetAttribute(image, "1", "40 40 40");
  IupSetAttribute(image, "2", "127 10 11");
  IupSetAttribute(image, "3", "127 18 19");
  IupSetAttribute(image, "4", "95 95 95");
  IupSetAttribute(image, "5", "90 102 126");
  IupSetAttribute(image, "6", "98 84 81");
  IupSetAttribute(image, "7", "127 116 77");
  IupSetAttribute(image, "8", "101 101 101");
  IupSetAttribute(image, "9", "110 110 110");
  IupSetAttribute(image, "10", "100 111 126");
  IupSetAttribute(image, "11", "102 113 127");
  IupSetAttribute(image, "12", "115 107 107");
  IupSetAttribute(image, "13", "125 122 108");
  IupSetAttribute(image, "14", "116 116 116");
  IupSetAttribute(image, "15", "125 119 119");
  IupSetAttribute(image, "16", "121 125 119");
  IupSetAttribute(image, "17", "124 125 124");
  IupSetAttribute(image, "18", "63 63 147");
  IupSetAttribute(image, "19", "31 68 163");
  IupSetAttribute(image, "20", "60 83 142");
  IupSetAttribute(image, "21", "61 84 148");
  IupSetAttribute(image, "22", "43 78 160");
  IupSetAttribute(image, "23", "41 79 175");
  IupSetAttribute(image, "24", "47 80 163");
  IupSetAttribute(image, "25", "57 90 172");
  IupSetAttribute(image, "26", "52 87 177");
  IupSetAttribute(image, "27", "54 90 181");
  IupSetAttribute(image, "28", "61 95 176");
  IupSetAttribute(image, "29", "58 96 188");
  IupSetAttribute(image, "30", "26 81 221");
  IupSetAttribute(image, "31", "59 98 197");
  IupSetAttribute(image, "32", "62 101 201");
  IupSetAttribute(image, "33", "61 105 219");
  IupSetAttribute(image, "34", "39 96 243");
  IupSetAttribute(image, "35", "40 97 243");
  IupSetAttribute(image, "36", "61 108 228");
  IupSetAttribute(image, "37", "60 108 232");
  IupSetAttribute(image, "38", "65 69 153");
  IupSetAttribute(image, "39", "65 74 159");
  IupSetAttribute(image, "40", "68 88 139");
  IupSetAttribute(image, "41", "93 112 149");
  IupSetAttribute(image, "42", "94 114 154");
  IupSetAttribute(image, "43", "65 98 176");
  IupSetAttribute(image, "44", "70 104 190");
  IupSetAttribute(image, "45", "84 108 162");
  IupSetAttribute(image, "46", "84 109 170");
  IupSetAttribute(image, "47", "88 109 162");
  IupSetAttribute(image, "48", "106 115 129");
  IupSetAttribute(image, "49", "111 122 134");
  IupSetAttribute(image, "50", "111 123 136");
  IupSetAttribute(image, "51", "106 123 154");
  IupSetAttribute(image, "52", "114 123 131");
  IupSetAttribute(image, "53", "115 126 143");
  IupSetAttribute(image, "54", "120 127 132");
  IupSetAttribute(image, "55", "113 126 148");
  IupSetAttribute(image, "56", "99 121 174");
  IupSetAttribute(image, "57", "104 124 161");
  IupSetAttribute(image, "58", "101 126 180");
  IupSetAttribute(image, "59", "99 123 188");
  IupSetAttribute(image, "60", "67 102 192");
  IupSetAttribute(image, "61", "79 111 193");
  IupSetAttribute(image, "62", "67 108 208");
  IupSetAttribute(image, "63", "82 114 194");
  IupSetAttribute(image, "64", "91 120 196");
  IupSetAttribute(image, "65", "98 126 196");
  IupSetAttribute(image, "66", "127 130 116");
  IupSetAttribute(image, "67", "125 129 124");
  IupSetAttribute(image, "68", "122 131 139");
  IupSetAttribute(image, "69", "124 133 146");
  IupSetAttribute(image, "70", "123 134 152");
  IupSetAttribute(image, "71", "108 129 186");
  IupSetAttribute(image, "72", "126 139 164");
  IupSetAttribute(image, "73", "116 135 178");
  IupSetAttribute(image, "74", "114 135 185");
  IupSetAttribute(image, "75", "115 137 187");
  IupSetAttribute(image, "76", "120 136 176");
  IupSetAttribute(image, "77", "124 141 189");
  IupSetAttribute(image, "78", "86 131 244");
  IupSetAttribute(image, "79", "101 130 203");
  IupSetAttribute(image, "80", "106 132 196");
  IupSetAttribute(image, "81", "106 135 204");
  IupSetAttribute(image, "82", "101 132 211");
  IupSetAttribute(image, "83", "101 135 223");
  IupSetAttribute(image, "84", "115 137 195");
  IupSetAttribute(image, "85", "124 143 192");
  IupSetAttribute(image, "86", "113 137 210");
  IupSetAttribute(image, "87", "114 143 216");
  IupSetAttribute(image, "88", "127 147 197");
  IupSetAttribute(image, "89", "123 147 205");
  IupSetAttribute(image, "90", "122 150 222");
  IupSetAttribute(image, "91", "104 140 234");
  IupSetAttribute(image, "92", "115 147 227");
  IupSetAttribute(image, "93", "115 149 235");
  IupSetAttribute(image, "94", "122 152 228");
  IupSetAttribute(image, "95", "126 156 235");
  IupSetAttribute(image, "96", "151 5 7");
  IupSetAttribute(image, "97", "157 1 2");
  IupSetAttribute(image, "98", "145 71 69");
  IupSetAttribute(image, "99", "148 88 84");
  IupSetAttribute(image, "100", "138 117 77");
  IupSetAttribute(image, "101", "130 121 82");
  IupSetAttribute(image, "102", "137 127 86");
  IupSetAttribute(image, "103", "130 113 114");
  IupSetAttribute(image, "104", "137 121 120");
  IupSetAttribute(image, "105", "145 105 105");
  IupSetAttribute(image, "106", "152 120 120");
  IupSetAttribute(image, "107", "161 67 68");
  IupSetAttribute(image, "108", "168 69 70");
  IupSetAttribute(image, "109", "139 131 86");
  IupSetAttribute(image, "110", "139 131 89");
  IupSetAttribute(image, "111", "145 133 89");
  IupSetAttribute(image, "112", "148 137 81");
  IupSetAttribute(image, "113", "148 138 91");
  IupSetAttribute(image, "114", "152 143 95");
  IupSetAttribute(image, "115", "157 147 87");
  IupSetAttribute(image, "116", "135 130 100");
  IupSetAttribute(image, "117", "139 133 101");
  IupSetAttribute(image, "118", "142 136 101");
  IupSetAttribute(image, "119", "132 133 114");
  IupSetAttribute(image, "120", "128 132 126");
  IupSetAttribute(image, "121", "134 137 127");
  IupSetAttribute(image, "122", "143 138 114");
  IupSetAttribute(image, "123", "148 139 101");
  IupSetAttribute(image, "124", "146 140 108");
  IupSetAttribute(image, "125", "154 146 100");
  IupSetAttribute(image, "126", "154 147 105");
  IupSetAttribute(image, "127", "145 144 118");
  IupSetAttribute(image, "128", "150 149 122");
  IupSetAttribute(image, "129", "152 149 116");
  IupSetAttribute(image, "130", "157 155 124");
  IupSetAttribute(image, "131", "160 150 94");
  IupSetAttribute(image, "132", "165 155 92");
  IupSetAttribute(image, "133", "163 153 101");
  IupSetAttribute(image, "134", "163 155 104");
  IupSetAttribute(image, "135", "168 158 102");
  IupSetAttribute(image, "136", "169 155 106");
  IupSetAttribute(image, "137", "160 155 114");
  IupSetAttribute(image, "138", "175 164 101");
  IupSetAttribute(image, "139", "175 163 112");
  IupSetAttribute(image, "140", "172 165 123");
  IupSetAttribute(image, "141", "179 163 109");
  IupSetAttribute(image, "142", "181 170 107");
  IupSetAttribute(image, "143", "184 166 110");
  IupSetAttribute(image, "144", "187 173 101");
  IupSetAttribute(image, "145", "186 172 105");
  IupSetAttribute(image, "146", "180 171 122");
  IupSetAttribute(image, "147", "186 171 123");
  IupSetAttribute(image, "148", "191 177 107");
  IupSetAttribute(image, "149", "188 179 118");
  IupSetAttribute(image, "150", "196 181 108");
  IupSetAttribute(image, "151", "202 187 108");
  IupSetAttribute(image, "152", "196 180 126");
  IupSetAttribute(image, "153", "205 188 127");
  IupSetAttribute(image, "154", "209 194 112");
  IupSetAttribute(image, "155", "131 132 132");
  IupSetAttribute(image, "156", "130 137 143");
  IupSetAttribute(image, "157", "143 143 128");
  IupSetAttribute(image, "158", "139 139 139");
  IupSetAttribute(image, "159", "128 137 149");
  IupSetAttribute(image, "160", "128 139 157");
  IupSetAttribute(image, "161", "140 144 135");
  IupSetAttribute(image, "162", "143 146 138");
  IupSetAttribute(image, "163", "138 146 157");
  IupSetAttribute(image, "164", "148 143 135");
  IupSetAttribute(image, "165", "149 142 142");
  IupSetAttribute(image, "166", "152 149 129");
  IupSetAttribute(image, "167", "155 153 129");
  IupSetAttribute(image, "168", "154 152 140");
  IupSetAttribute(image, "169", "148 148 147");
  IupSetAttribute(image, "170", "154 156 154");
  IupSetAttribute(image, "171", "128 142 177");
  IupSetAttribute(image, "172", "157 160 151");
  IupSetAttribute(image, "173", "153 160 162");
  IupSetAttribute(image, "174", "157 164 170");
  IupSetAttribute(image, "175", "158 167 178");
  IupSetAttribute(image, "176", "172 162 130");
  IupSetAttribute(image, "177", "164 165 145");
  IupSetAttribute(image, "178", "162 164 153");
  IupSetAttribute(image, "179", "169 165 148");
  IupSetAttribute(image, "180", "176 171 133");
  IupSetAttribute(image, "181", "179 175 153");
  IupSetAttribute(image, "182", "185 179 130");
  IupSetAttribute(image, "183", "187 179 139");
  IupSetAttribute(image, "184", "191 184 143");
  IupSetAttribute(image, "185", "183 178 149");
  IupSetAttribute(image, "186", "180 177 156");
  IupSetAttribute(image, "187", "184 179 153");
  IupSetAttribute(image, "188", "188 186 158");
  IupSetAttribute(image, "189", "163 163 163");
  IupSetAttribute(image, "190", "160 168 172");
  IupSetAttribute(image, "191", "170 170 170");
  IupSetAttribute(image, "192", "160 168 177");
  IupSetAttribute(image, "193", "181 175 165");
  IupSetAttribute(image, "194", "191 187 164");
  IupSetAttribute(image, "195", "180 180 180");
  IupSetAttribute(image, "196", "188 189 179");
  IupSetAttribute(image, "197", "186 186 186");
  IupSetAttribute(image, "198", "133 151 197");
  IupSetAttribute(image, "199", "129 151 209");
  IupSetAttribute(image, "200", "155 166 193");
  IupSetAttribute(image, "201", "133 165 248");
  IupSetAttribute(image, "202", "151 178 250");
  IupSetAttribute(image, "203", "156 182 250");
  IupSetAttribute(image, "204", "172 178 193");
  IupSetAttribute(image, "205", "177 182 196");
  IupSetAttribute(image, "206", "171 192 247");
  IupSetAttribute(image, "207", "172 194 250");
  IupSetAttribute(image, "208", "178 199 251");
  IupSetAttribute(image, "209", "192 177 130");
  IupSetAttribute(image, "210", "195 187 133");
  IupSetAttribute(image, "211", "192 188 151");
  IupSetAttribute(image, "212", "194 188 155");
  IupSetAttribute(image, "213", "192 190 163");
  IupSetAttribute(image, "214", "192 189 174");
  IupSetAttribute(image, "215", "194 192 169");
  IupSetAttribute(image, "216", "202 196 164");
  IupSetAttribute(image, "217", "206 200 166");
  IupSetAttribute(image, "218", "205 201 175");
  IupSetAttribute(image, "219", "197 198 186");
  IupSetAttribute(image, "220", "207 203 185");
  IupSetAttribute(image, "221", "208 203 175");
  IupSetAttribute(image, "222", "210 205 179");
  IupSetAttribute(image, "223", "208 205 188");
  IupSetAttribute(image, "224", "210 209 189");
  IupSetAttribute(image, "225", "194 194 194");
  IupSetAttribute(image, "226", "199 200 204");
  IupSetAttribute(image, "227", "206 207 195");
  IupSetAttribute(image, "228", "202 203 202");
  IupSetAttribute(image, "229", "202 206 211");
  IupSetAttribute(image, "230", "212 211 195");
  IupSetAttribute(image, "231", "210 211 203");
  IupSetAttribute(image, "232", "217 215 205");
  IupSetAttribute(image, "233", "211 212 209");
  IupSetAttribute(image, "234", "218 214 210");
  IupSetAttribute(image, "235", "221 219 212");
  IupSetAttribute(image, "236", "220 221 221");
  IupSetAttribute(image, "237", "205 211 241");
  IupSetAttribute(image, "238", "209 212 240");
  IupSetAttribute(image, "239", "223 224 222");
  IupSetAttribute(image, "240", "221 226 242");
  IupSetAttribute(image, "241", "224 223 216");
  IupSetAttribute(image, "242", "227 225 220");
  IupSetAttribute(image, "243", "227 227 226");
  IupSetAttribute(image, "244", "234 230 229");
  IupSetAttribute(image, "245", "233 232 227");
  IupSetAttribute(image, "246", "234 234 233");
  IupSetAttribute(image, "247", "234 237 244");
  IupSetAttribute(image, "248", "237 240 248");
  IupSetAttribute(image, "249", "243 243 243");
  IupSetAttribute(image, "250", "243 245 250");
  IupSetAttribute(image, "251", "247 248 253");
  IupSetAttribute(image, "252", "251 251 251");
  IupSetAttribute(image, "253", "0 0 0");
  IupSetAttribute(image, "254", "0 0 0");
  IupSetAttribute(image, "255", "BGCOLOR");

  return image;
}

static Ihandle* load_image_Petrobras8(void)
{
  unsigned char imgdata[] = {
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 8, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 8, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 32, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 32, 2, 2, 53, 2, 2, 53, 2, 2, 53, 2, 2, 53, 2, 2, 53, 2, 2, 53, 2, 2, 53, 2, 32, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 66, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 32, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 66, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 66, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 44, 67, 33, 67, 12, 73, 73, 73, 73, 62, 18, 18, 33, 67, 76, 73, 73, 73, 73, 76, 67, 24, 67, 15, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 15, 12, 12, 63, 12, 76, 73, 73, 73, 73, 12, 18, 12, 12, 63, 12, 15, 73, 17, 12, 18, 12, 12, 18, 12, 12, 76, 73, 15, 12, 12, 63, 12, 15, 73, 73, 73, 73, 73, 73, 41, 45, 50, 69, 76, 73, 73, 73, 73, 12, 12, 63, 12, 12, 76, 73, 73, 73, 73, 12, 12, 18, 12, 12, 76, 73, 73, 73, 73, 73, 73, 76, 12, 12, 40, 73, 73, 73, 73, 73, 17, 45, 50, 35, 76, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 29, 0, 0, 0, 48, 73, 73, 73, 73, 73, 15, 68, 0, 0, 67, 73, 73, 73, 73, 73, 35, 0, 0, 61, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 34, 0, 0, 0, 0, 0, 54, 76, 73, 45, 0, 0, 0, 0, 0, 0, 72, 73, 52, 0, 0, 0, 0, 0, 0, 0, 23, 73, 60, 0, 0, 0, 0, 0, 64, 21, 73, 73, 73, 23, 19, 0, 0, 0, 42, 69, 73, 73, 17, 0, 0, 0, 0, 0, 0, 39, 17, 73, 15, 0, 0, 0, 0, 0, 0, 13, 15, 73, 73, 73, 62, 56, 0, 0, 1, 73, 73, 73, 73, 23, 42, 0, 0, 0, 42, 4, 73, 73, 73, 73,
    73, 73, 73, 73, 16, 0, 0, 0, 69, 73, 15, 52, 52, 12, 73, 6, 0, 37, 73, 73, 26, 65, 38, 73, 73, 68, 0, 29, 73, 73, 73, 73, 73, 73, 73, 73, 73, 12, 0, 0, 42, 5, 0, 0, 0, 20, 73, 36, 0, 0, 5, 5, 5, 5, 38, 73, 39, 0, 0, 0, 0, 0, 0, 0, 15, 15, 0, 0, 42, 5, 42, 0, 0, 16, 73, 73, 48, 0, 0, 68, 68, 0, 0, 19, 73, 73, 21, 0, 0, 19, 5, 0, 0, 0, 4, 73, 20, 0, 0, 37, 5, 0, 0, 0, 6, 73, 73, 73, 28, 0, 0, 0, 22, 73, 73, 73, 38, 0, 0, 43, 54, 0, 0, 48, 73, 73, 73, 73,
    73, 73, 73, 73, 29, 0, 0, 0, 17, 73, 65, 0, 0, 6, 73, 11, 0, 48, 73, 73, 37, 0, 51, 62, 73, 34, 0, 61, 73, 73, 73, 73, 73, 73, 73, 73, 73, 38, 0, 0, 20, 73, 38, 0, 0, 11, 73, 5, 0, 42, 17, 73, 73, 73, 73, 73, 46, 15, 27, 0, 0, 50, 15, 15, 73, 57, 0, 0, 52, 73, 18, 0, 0, 47, 73, 38, 0, 0, 58, 73, 73, 68, 0, 0, 12, 73, 27, 0, 0, 45, 73, 61, 0, 64, 17, 73, 43, 0, 0, 12, 73, 77, 0, 0, 18, 73, 73, 18, 0, 0, 19, 0, 48, 73, 73, 73, 60, 0, 0, 26, 17, 6, 57, 4, 73, 73, 73, 73,
    73, 73, 73, 73, 61, 0, 0, 3, 73, 73, 17, 46, 17, 73, 12, 37, 0, 69, 73, 62, 65, 65, 33, 73, 26, 0, 0, 29, 73, 73, 73, 73, 73, 73, 73, 73, 73, 43, 0, 0, 79, 21, 13, 0, 42, 46, 12, 0, 0, 0, 42, 42, 42, 4, 73, 73, 73, 73, 68, 0, 0, 62, 73, 73, 73, 29, 0, 0, 27, 31, 60, 0, 5, 76, 41, 37, 0, 30, 41, 73, 73, 39, 0, 0, 76, 73, 37, 0, 0, 54, 39, 0, 10, 67, 73, 41, 51, 0, 42, 65, 47, 19, 0, 58, 73, 73, 73, 13, 0, 48, 39, 0, 43, 73, 73, 73, 68, 0, 0, 0, 42, 16, 63, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 48, 0, 0, 52, 73, 62, 55, 4, 40, 73, 67, 0, 51, 17, 73, 73, 73, 73, 73, 4, 0, 0, 0, 61, 73, 73, 73, 73, 73, 73, 73, 73, 41, 51, 0, 0, 0, 0, 0, 0, 6, 73, 20, 0, 0, 0, 0, 0, 30, 73, 73, 73, 73, 76, 0, 0, 39, 73, 73, 73, 73, 19, 0, 0, 0, 0, 0, 19, 12, 73, 18, 0, 0, 61, 73, 73, 17, 0, 0, 68, 73, 12, 0, 0, 42, 5, 42, 0, 0, 21, 73, 45, 0, 0, 0, 0, 0, 0, 72, 73, 73, 73, 23, 0, 42, 15, 54, 0, 60, 73, 73, 73, 33, 13, 0, 0, 0, 0, 37, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 61, 0, 0, 18, 73, 21, 0, 0, 27, 73, 73, 25, 3, 73, 73, 16, 5, 59, 73, 41, 19, 0, 0, 29, 73, 73, 73, 73, 73, 73, 73, 73, 45, 0, 0, 71, 39, 54, 27, 18, 73, 73, 43, 0, 0, 67, 46, 46, 76, 73, 73, 73, 73, 38, 0, 0, 23, 73, 73, 73, 33, 0, 0, 72, 31, 3, 0, 0, 47, 73, 69, 0, 0, 36, 73, 73, 28, 0, 0, 6, 73, 23, 0, 0, 11, 73, 4, 0, 0, 23, 73, 22, 0, 0, 70, 52, 37, 0, 0, 18, 73, 12, 0, 0, 68, 1, 71, 0, 5, 73, 73, 67, 67, 12, 15, 11, 0, 0, 30, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 29, 0, 37, 73, 73, 35, 58, 36, 12, 73, 73, 19, 52, 73, 17, 0, 0, 13, 73, 73, 5, 0, 0, 16, 73, 73, 73, 73, 73, 73, 73, 73, 22, 0, 0, 14, 73, 73, 73, 73, 73, 62, 51, 0, 0, 27, 27, 27, 27, 76, 73, 73, 73, 48, 0, 0, 18, 73, 73, 73, 52, 0, 0, 57, 73, 69, 0, 0, 38, 73, 33, 0, 0, 42, 52, 7, 0, 0, 43, 73, 73, 43, 0, 0, 16, 70, 56, 0, 0, 12, 73, 68, 0, 0, 41, 73, 61, 0, 0, 76, 73, 54, 0, 0, 0, 0, 0, 0, 19, 73, 73, 0, 0, 42, 6, 23, 0, 0, 36, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 61, 0, 58, 73, 73, 73, 73, 73, 73, 41, 36, 0, 18, 41, 21, 0, 0, 34, 73, 73, 34, 0, 0, 29, 73, 73, 73, 73, 73, 73, 73, 73, 34, 0, 42, 73, 73, 73, 73, 73, 73, 45, 0, 0, 0, 0, 0, 0, 37, 73, 73, 73, 41, 19, 0, 71, 73, 73, 73, 73, 60, 0, 0, 17, 73, 58, 0, 0, 15, 73, 73, 72, 0, 0, 0, 0, 0, 58, 73, 73, 17, 42, 0, 0, 0, 0, 0, 42, 6, 73, 12, 0, 0, 43, 73, 73, 19, 0, 60, 73, 38, 0, 0, 52, 21, 26, 68, 0, 0, 73, 73, 16, 0, 0, 0, 0, 0, 43, 62, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 29, 0, 39, 29, 7, 29, 7, 7, 16, 37, 0, 0, 29, 36, 13, 0, 0, 0, 28, 22, 37, 0, 0, 16, 73, 73, 73, 73, 73, 73, 73, 73, 23, 23, 6, 73, 73, 73, 73, 73, 73, 67, 23, 23, 23, 23, 23, 23, 69, 73, 73, 73, 17, 23, 23, 21, 73, 73, 73, 73, 20, 23, 79, 73, 73, 21, 23, 23, 17, 73, 73, 73, 11, 48, 29, 47, 63, 73, 73, 73, 76, 23, 23, 23, 23, 20, 21, 62, 73, 73, 12, 23, 23, 33, 73, 73, 23, 23, 6, 73, 6, 23, 38, 73, 73, 73, 69, 23, 20, 73, 73, 73, 38, 48, 61, 28, 75, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 57, 59, 9, 59, 59, 9, 59, 59, 9, 59, 59, 9, 59, 59, 9, 59, 59, 9, 59, 59, 9, 59, 59, 6, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73};

  Ihandle* image = IupImage(128, 32, imgdata);

  IupSetAttribute(image, "0", "4 134 68");
  IupSetAttribute(image, "1", "132 194 164");
  IupSetAttribute(image, "2", "252 202 44");
  IupSetAttribute(image, "3", "68 162 116");
  IupSetAttribute(image, "4", "196 226 212");
  IupSetAttribute(image, "5", "36 150 92");
  IupSetAttribute(image, "6", "164 210 188");
  IupSetAttribute(image, "7", "100 178 140");
  IupSetAttribute(image, "8", "252 230 164");
  IupSetAttribute(image, "9", "116 190 148");
  IupSetAttribute(image, "10", "20 146 84");
  IupSetAttribute(image, "11", "148 206 180");
  IupSetAttribute(image, "12", "220 238 228");
  IupSetAttribute(image, "13", "52 158 100");
  IupSetAttribute(image, "14", "180 222 204");
  IupSetAttribute(image, "15", "228 242 236");
  IupSetAttribute(image, "16", "84 174 124");
  IupSetAttribute(image, "17", "244 250 244");
  IupSetAttribute(image, "18", "212 234 220");
  IupSetAttribute(image, "19", "20 142 76");
  IupSetAttribute(image, "20", "148 202 172");
  IupSetAttribute(image, "21", "180 218 196");
  IupSetAttribute(image, "22", "116 186 148");
  IupSetAttribute(image, "23", "140 202 172");
  IupSetAttribute(image, "24", "204 234 220");
  IupSetAttribute(image, "25", "44 158 100");
  IupSetAttribute(image, "26", "172 218 196");
  IupSetAttribute(image, "27", "108 186 148");
  IupSetAttribute(image, "28", "124 190 156");
  IupSetAttribute(image, "29", "92 174 132");
  IupSetAttribute(image, "30", "12 142 76");
  IupSetAttribute(image, "31", "140 198 164");
  IupSetAttribute(image, "32", "252 218 116");
  IupSetAttribute(image, "33", "204 230 212");
  IupSetAttribute(image, "34", "44 154 92");
  IupSetAttribute(image, "35", "172 214 188");
  IupSetAttribute(image, "36", "108 182 140");
  IupSetAttribute(image, "37", "28 146 84");
  IupSetAttribute(image, "38", "156 206 180");
  IupSetAttribute(image, "39", "60 162 108");
  IupSetAttribute(image, "40", "236 250 244");
  IupSetAttribute(image, "41", "252 250 252");
  IupSetAttribute(image, "42", "12 138 76");
  IupSetAttribute(image, "43", "76 170 124");
  IupSetAttribute(image, "44", "220 242 228");
  IupSetAttribute(image, "45", "188 222 204");
  IupSetAttribute(image, "46", "236 246 236");
  IupSetAttribute(image, "47", "124 194 156");
  IupSetAttribute(image, "48", "92 178 132");
  IupSetAttribute(image, "49", "28 150 92");
  IupSetAttribute(image, "50", "156 210 180");
  IupSetAttribute(image, "51", "4 138 68");
  IupSetAttribute(image, "52", "132 198 164");
  IupSetAttribute(image, "53", "252 202 60");
  IupSetAttribute(image, "54", "68 166 116");
  IupSetAttribute(image, "55", "196 230 212");
  IupSetAttribute(image, "56", "36 154 92");
  IupSetAttribute(image, "57", "164 214 188");
  IupSetAttribute(image, "58", "100 182 140");
  IupSetAttribute(image, "59", "116 190 156");
  IupSetAttribute(image, "60", "52 158 108");
  IupSetAttribute(image, "61", "84 174 132");
  IupSetAttribute(image, "62", "244 250 252");
  IupSetAttribute(image, "63", "212 234 228");
  IupSetAttribute(image, "64", "20 142 84");
  IupSetAttribute(image, "65", "140 198 172");
  IupSetAttribute(image, "66", "252 222 124");
  IupSetAttribute(image, "67", "204 230 220");
  IupSetAttribute(image, "68", "44 154 100");
  IupSetAttribute(image, "69", "172 214 196");
  IupSetAttribute(image, "70", "108 182 148");
  IupSetAttribute(image, "71", "28 146 92");
  IupSetAttribute(image, "72", "60 162 116");
  IupSetAttribute(image, "73", "252 254 252");
  IupSetAttribute(image, "74", "220 242 236");
  IupSetAttribute(image, "75", "188 226 212");
  IupSetAttribute(image, "76", "236 246 244");
  IupSetAttribute(image, "77", "124 194 164");
  IupSetAttribute(image, "78", "92 178 140");
  IupSetAttribute(image, "79", "156 210 188");
  IupSetAttribute(image, "80", "204 204 204");
  IupSetAttribute(image, "81", "204 204 204");
  IupSetAttribute(image, "82", "204 204 204");
  IupSetAttribute(image, "83", "204 204 204");
  IupSetAttribute(image, "84", "204 204 204");
  IupSetAttribute(image, "85", "204 204 204");
  IupSetAttribute(image, "86", "204 204 204");
  IupSetAttribute(image, "87", "204 204 204");
  IupSetAttribute(image, "88", "204 204 204");
  IupSetAttribute(image, "89", "204 204 204");
  IupSetAttribute(image, "90", "204 204 204");
  IupSetAttribute(image, "91", "204 204 204");
  IupSetAttribute(image, "92", "204 204 204");
  IupSetAttribute(image, "93", "204 204 204");
  IupSetAttribute(image, "94", "204 204 204");
  IupSetAttribute(image, "95", "204 204 204");
  IupSetAttribute(image, "96", "204 204 204");
  IupSetAttribute(image, "97", "204 204 204");
  IupSetAttribute(image, "98", "204 204 204");
  IupSetAttribute(image, "99", "204 204 204");
  IupSetAttribute(image, "100", "204 204 204");
  IupSetAttribute(image, "101", "204 204 204");
  IupSetAttribute(image, "102", "204 204 204");
  IupSetAttribute(image, "103", "204 204 204");
  IupSetAttribute(image, "104", "204 204 204");
  IupSetAttribute(image, "105", "204 204 204");
  IupSetAttribute(image, "106", "204 204 204");
  IupSetAttribute(image, "107", "204 204 204");
  IupSetAttribute(image, "108", "204 204 204");
  IupSetAttribute(image, "109", "204 204 204");
  IupSetAttribute(image, "110", "204 204 204");
  IupSetAttribute(image, "111", "204 204 204");
  IupSetAttribute(image, "112", "204 204 204");
  IupSetAttribute(image, "113", "204 204 204");
  IupSetAttribute(image, "114", "204 204 204");
  IupSetAttribute(image, "115", "204 204 204");
  IupSetAttribute(image, "116", "204 204 204");
  IupSetAttribute(image, "117", "204 204 204");
  IupSetAttribute(image, "118", "204 204 204");
  IupSetAttribute(image, "119", "204 204 204");
  IupSetAttribute(image, "120", "204 204 204");
  IupSetAttribute(image, "121", "204 204 204");
  IupSetAttribute(image, "122", "204 204 204");
  IupSetAttribute(image, "123", "204 204 204");
  IupSetAttribute(image, "124", "204 204 204");
  IupSetAttribute(image, "125", "204 204 204");
  IupSetAttribute(image, "126", "204 204 204");
  IupSetAttribute(image, "127", "204 204 204");
  IupSetAttribute(image, "128", "204 204 204");
  IupSetAttribute(image, "129", "204 204 204");
  IupSetAttribute(image, "130", "204 204 204");
  IupSetAttribute(image, "131", "204 204 204");
  IupSetAttribute(image, "132", "204 204 204");
  IupSetAttribute(image, "133", "204 204 204");
  IupSetAttribute(image, "134", "204 204 204");
  IupSetAttribute(image, "135", "204 204 204");
  IupSetAttribute(image, "136", "204 204 204");
  IupSetAttribute(image, "137", "204 204 204");
  IupSetAttribute(image, "138", "204 204 204");
  IupSetAttribute(image, "139", "204 204 204");
  IupSetAttribute(image, "140", "204 204 204");
  IupSetAttribute(image, "141", "204 204 204");
  IupSetAttribute(image, "142", "204 204 204");
  IupSetAttribute(image, "143", "204 204 204");
  IupSetAttribute(image, "144", "204 204 204");
  IupSetAttribute(image, "145", "204 204 204");
  IupSetAttribute(image, "146", "204 204 204");
  IupSetAttribute(image, "147", "204 204 204");
  IupSetAttribute(image, "148", "204 204 204");
  IupSetAttribute(image, "149", "204 204 204");
  IupSetAttribute(image, "150", "204 204 204");
  IupSetAttribute(image, "151", "204 204 204");
  IupSetAttribute(image, "152", "204 204 204");
  IupSetAttribute(image, "153", "204 204 204");
  IupSetAttribute(image, "154", "204 204 204");
  IupSetAttribute(image, "155", "204 204 204");
  IupSetAttribute(image, "156", "204 204 204");
  IupSetAttribute(image, "157", "204 204 204");
  IupSetAttribute(image, "158", "204 204 204");
  IupSetAttribute(image, "159", "204 204 204");
  IupSetAttribute(image, "160", "204 204 204");
  IupSetAttribute(image, "161", "204 204 204");
  IupSetAttribute(image, "162", "204 204 204");
  IupSetAttribute(image, "163", "204 204 204");
  IupSetAttribute(image, "164", "204 204 204");
  IupSetAttribute(image, "165", "204 204 204");
  IupSetAttribute(image, "166", "204 204 204");
  IupSetAttribute(image, "167", "204 204 204");
  IupSetAttribute(image, "168", "204 204 204");
  IupSetAttribute(image, "169", "204 204 204");
  IupSetAttribute(image, "170", "204 204 204");
  IupSetAttribute(image, "171", "204 204 204");
  IupSetAttribute(image, "172", "204 204 204");
  IupSetAttribute(image, "173", "204 204 204");
  IupSetAttribute(image, "174", "204 204 204");
  IupSetAttribute(image, "175", "204 204 204");
  IupSetAttribute(image, "176", "204 204 204");
  IupSetAttribute(image, "177", "204 204 204");
  IupSetAttribute(image, "178", "204 204 204");
  IupSetAttribute(image, "179", "204 204 204");
  IupSetAttribute(image, "180", "204 204 204");
  IupSetAttribute(image, "181", "204 204 204");
  IupSetAttribute(image, "182", "204 204 204");
  IupSetAttribute(image, "183", "204 204 204");
  IupSetAttribute(image, "184", "204 204 204");
  IupSetAttribute(image, "185", "204 204 204");
  IupSetAttribute(image, "186", "204 204 204");
  IupSetAttribute(image, "187", "204 204 204");
  IupSetAttribute(image, "188", "204 204 204");
  IupSetAttribute(image, "189", "204 204 204");
  IupSetAttribute(image, "190", "204 204 204");
  IupSetAttribute(image, "191", "204 204 204");
  IupSetAttribute(image, "192", "204 204 204");
  IupSetAttribute(image, "193", "204 204 204");
  IupSetAttribute(image, "194", "204 204 204");
  IupSetAttribute(image, "195", "204 204 204");
  IupSetAttribute(image, "196", "204 204 204");
  IupSetAttribute(image, "197", "204 204 204");
  IupSetAttribute(image, "198", "204 204 204");
  IupSetAttribute(image, "199", "204 204 204");
  IupSetAttribute(image, "200", "204 204 204");
  IupSetAttribute(image, "201", "204 204 204");
  IupSetAttribute(image, "202", "204 204 204");
  IupSetAttribute(image, "203", "204 204 204");
  IupSetAttribute(image, "204", "204 204 204");
  IupSetAttribute(image, "205", "204 204 204");
  IupSetAttribute(image, "206", "204 204 204");
  IupSetAttribute(image, "207", "204 204 204");
  IupSetAttribute(image, "208", "204 204 204");
  IupSetAttribute(image, "209", "204 204 204");
  IupSetAttribute(image, "210", "204 204 204");
  IupSetAttribute(image, "211", "204 204 204");
  IupSetAttribute(image, "212", "204 204 204");
  IupSetAttribute(image, "213", "204 204 204");
  IupSetAttribute(image, "214", "204 204 204");
  IupSetAttribute(image, "215", "204 204 204");
  IupSetAttribute(image, "216", "204 204 204");
  IupSetAttribute(image, "217", "204 204 204");
  IupSetAttribute(image, "218", "204 204 204");
  IupSetAttribute(image, "219", "204 204 204");
  IupSetAttribute(image, "220", "204 204 204");
  IupSetAttribute(image, "221", "204 204 204");
  IupSetAttribute(image, "222", "204 204 204");
  IupSetAttribute(image, "223", "204 204 204");
  IupSetAttribute(image, "224", "204 204 204");
  IupSetAttribute(image, "225", "204 204 204");
  IupSetAttribute(image, "226", "204 204 204");
  IupSetAttribute(image, "227", "204 204 204");
  IupSetAttribute(image, "228", "204 204 204");
  IupSetAttribute(image, "229", "204 204 204");
  IupSetAttribute(image, "230", "204 204 204");
  IupSetAttribute(image, "231", "204 204 204");
  IupSetAttribute(image, "232", "204 204 204");
  IupSetAttribute(image, "233", "204 204 204");
  IupSetAttribute(image, "234", "204 204 204");
  IupSetAttribute(image, "235", "204 204 204");
  IupSetAttribute(image, "236", "204 204 204");
  IupSetAttribute(image, "237", "204 204 204");
  IupSetAttribute(image, "238", "204 204 204");
  IupSetAttribute(image, "239", "204 204 204");
  IupSetAttribute(image, "240", "204 204 204");
  IupSetAttribute(image, "241", "204 204 204");
  IupSetAttribute(image, "242", "204 204 204");
  IupSetAttribute(image, "243", "204 204 204");
  IupSetAttribute(image, "244", "204 204 204");
  IupSetAttribute(image, "245", "204 204 204");
  IupSetAttribute(image, "246", "204 204 204");
  IupSetAttribute(image, "247", "204 204 204");
  IupSetAttribute(image, "248", "204 204 204");
  IupSetAttribute(image, "249", "204 204 204");
  IupSetAttribute(image, "250", "204 204 204");
  IupSetAttribute(image, "251", "204 204 204");
  IupSetAttribute(image, "252", "204 204 204");
  IupSetAttribute(image, "253", "204 204 204");
  IupSetAttribute(image, "254", "204 204 204");
  IupSetAttribute(image, "255", "204 204 204");

  return image;
}

static Ihandle* load_image_BR8(void)
{
  unsigned char imgdata[] = {
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 104, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 105, 110, 110, 110, 110,
    110, 110, 110, 110, 100, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 102, 110, 110, 110, 110,
    110, 110, 110, 110, 100, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 102, 110, 110, 110, 110,
    110, 110, 110, 110, 100, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 102, 110, 110, 110, 110,
    110, 110, 110, 110, 100, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 102, 110, 110, 110, 110,
    110, 110, 110, 110, 101, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 103, 110, 110, 110, 110,
    110, 110, 110, 110, 109, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 109, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 81, 74, 74, 74, 77, 110, 110, 110, 110, 93, 75, 74, 74, 74, 86, 110, 110, 110, 110, 85, 74, 74, 74, 83, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 38, 110, 110, 110, 110, 110, 82, 25, 0, 1, 75, 110, 110, 110, 110, 110, 58, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 66, 110, 82, 49, 50, 80, 110, 59, 0, 18, 106, 110, 65, 52, 57, 107, 110, 24, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 8, 91, 110, 51, 0, 3, 61, 110, 56, 0, 40, 110, 99, 16, 0, 10, 94, 107, 23, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 31, 110, 110, 90, 84, 87, 110, 78, 17, 0, 69, 110, 90, 54, 55, 72, 110, 64, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 53, 110, 94, 71, 71, 88, 110, 73, 4, 9, 92, 110, 110, 110, 110, 110, 70, 5, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 76, 110, 67, 0, 0, 45, 110, 110, 26, 32, 110, 110, 33, 21, 48, 110, 96, 11, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 20, 110, 110, 63, 37, 43, 79, 110, 95, 12, 55, 110, 89, 6, 0, 29, 110, 110, 15, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 42, 110, 110, 110, 110, 110, 110, 96, 44, 2, 77, 110, 68, 0, 0, 27, 110, 110, 22, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 30, 39, 39, 39, 39, 39, 34, 19, 0, 7, 41, 42, 28, 0, 0, 14, 47, 47, 13, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 110, 110, 110, 110,
    110, 110, 110, 110, 60, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 62, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110};

  Ihandle* image = IupImage(32, 32, imgdata);

  IupSetAttribute(image, "0", "0 133 66");
  IupSetAttribute(image, "1", "1 133 66");
  IupSetAttribute(image, "2", "1 133 67");
  IupSetAttribute(image, "3", "1 134 67");
  IupSetAttribute(image, "4", "3 135 68");
  IupSetAttribute(image, "5", "4 135 69");
  IupSetAttribute(image, "6", "5 136 70");
  IupSetAttribute(image, "7", "6 136 71");
  IupSetAttribute(image, "8", "7 136 71");
  IupSetAttribute(image, "9", "8 137 72");
  IupSetAttribute(image, "10", "10 138 74");
  IupSetAttribute(image, "11", "12 139 75");
  IupSetAttribute(image, "12", "15 140 77");
  IupSetAttribute(image, "13", "20 143 81");
  IupSetAttribute(image, "14", "21 143 81");
  IupSetAttribute(image, "15", "22 144 82");
  IupSetAttribute(image, "16", "23 144 83");
  IupSetAttribute(image, "17", "25 145 85");
  IupSetAttribute(image, "18", "28 146 87");
  IupSetAttribute(image, "19", "29 147 88");
  IupSetAttribute(image, "20", "32 148 89");
  IupSetAttribute(image, "21", "33 149 90");
  IupSetAttribute(image, "22", "34 149 91");
  IupSetAttribute(image, "23", "35 150 92");
  IupSetAttribute(image, "24", "40 152 96");
  IupSetAttribute(image, "25", "44 154 99");
  IupSetAttribute(image, "26", "45 154 99");
  IupSetAttribute(image, "27", "49 157 103");
  IupSetAttribute(image, "28", "50 157 103");
  IupSetAttribute(image, "29", "58 161 109");
  IupSetAttribute(image, "30", "60 162 111");
  IupSetAttribute(image, "31", "65 164 114");
  IupSetAttribute(image, "32", "67 165 115");
  IupSetAttribute(image, "33", "84 173 128");
  IupSetAttribute(image, "34", "85 174 129");
  IupSetAttribute(image, "35", "86 174 130");
  IupSetAttribute(image, "36", "95 179 137");
  IupSetAttribute(image, "37", "98 180 138");
  IupSetAttribute(image, "38", "98 180 139");
  IupSetAttribute(image, "39", "99 180 139");
  IupSetAttribute(image, "40", "100 181 140");
  IupSetAttribute(image, "41", "103 182 142");
  IupSetAttribute(image, "42", "104 183 143");
  IupSetAttribute(image, "43", "107 184 145");
  IupSetAttribute(image, "44", "109 185 147");
  IupSetAttribute(image, "45", "112 186 149");
  IupSetAttribute(image, "46", "114 188 151");
  IupSetAttribute(image, "47", "122 191 157");
  IupSetAttribute(image, "48", "123 192 157");
  IupSetAttribute(image, "49", "135 198 166");
  IupSetAttribute(image, "50", "136 198 167");
  IupSetAttribute(image, "51", "137 198 167");
  IupSetAttribute(image, "52", "137 199 168");
  IupSetAttribute(image, "53", "138 199 168");
  IupSetAttribute(image, "54", "139 199 169");
  IupSetAttribute(image, "55", "140 200 170");
  IupSetAttribute(image, "56", "145 202 173");
  IupSetAttribute(image, "57", "158 209 183");
  IupSetAttribute(image, "58", "159 209 183");
  IupSetAttribute(image, "59", "161 210 185");
  IupSetAttribute(image, "60", "162 210 186");
  IupSetAttribute(image, "61", "164 212 188");
  IupSetAttribute(image, "62", "167 213 190");
  IupSetAttribute(image, "63", "168 213 191");
  IupSetAttribute(image, "64", "169 214 191");
  IupSetAttribute(image, "65", "172 215 193");
  IupSetAttribute(image, "66", "172 215 194");
  IupSetAttribute(image, "67", "174 216 195");
  IupSetAttribute(image, "68", "175 216 195");
  IupSetAttribute(image, "69", "175 217 196");
  IupSetAttribute(image, "70", "189 223 206");
  IupSetAttribute(image, "71", "197 227 212");
  IupSetAttribute(image, "72", "200 229 214");
  IupSetAttribute(image, "73", "201 229 215");
  IupSetAttribute(image, "74", "207 232 219");
  IupSetAttribute(image, "75", "210 233 221");
  IupSetAttribute(image, "76", "213 235 223");
  IupSetAttribute(image, "77", "217 237 227");
  IupSetAttribute(image, "78", "218 237 228");
  IupSetAttribute(image, "79", "220 238 229");
  IupSetAttribute(image, "80", "221 239 230");
  IupSetAttribute(image, "81", "223 240 231");
  IupSetAttribute(image, "82", "224 240 232");
  IupSetAttribute(image, "83", "225 241 233");
  IupSetAttribute(image, "84", "234 245 239");
  IupSetAttribute(image, "85", "236 246 241");
  IupSetAttribute(image, "86", "238 247 242");
  IupSetAttribute(image, "87", "239 247 243");
  IupSetAttribute(image, "88", "240 248 244");
  IupSetAttribute(image, "89", "241 248 245");
  IupSetAttribute(image, "90", "242 249 245");
  IupSetAttribute(image, "91", "242 249 246");
  IupSetAttribute(image, "92", "244 250 247");
  IupSetAttribute(image, "93", "246 251 248");
  IupSetAttribute(image, "94", "247 251 249");
  IupSetAttribute(image, "95", "248 252 250");
  IupSetAttribute(image, "96", "250 253 252");
  IupSetAttribute(image, "97", "253 200 47");
  IupSetAttribute(image, "98", "253 203 57");
  IupSetAttribute(image, "99", "253 254 254");
  IupSetAttribute(image, "100", "254 218 117");
  IupSetAttribute(image, "101", "254 220 124");
  IupSetAttribute(image, "102", "254 221 125");
  IupSetAttribute(image, "103", "254 222 131");
  IupSetAttribute(image, "104", "254 231 164");
  IupSetAttribute(image, "105", "254 232 169");
  IupSetAttribute(image, "106", "254 254 254");
  IupSetAttribute(image, "107", "254 255 254");
  IupSetAttribute(image, "108", "255 254 253");
  IupSetAttribute(image, "109", "255 255 253");
  IupSetAttribute(image, "110", "255 255 255");
  IupSetAttribute(image, "111", "111 111 111");
  IupSetAttribute(image, "112", "112 112 112");
  IupSetAttribute(image, "113", "113 113 113");
  IupSetAttribute(image, "114", "114 114 114");
  IupSetAttribute(image, "115", "115 115 115");
  IupSetAttribute(image, "116", "116 116 116");
  IupSetAttribute(image, "117", "117 117 117");
  IupSetAttribute(image, "118", "118 118 118");
  IupSetAttribute(image, "119", "119 119 119");
  IupSetAttribute(image, "120", "120 120 120");
  IupSetAttribute(image, "121", "121 121 121");
  IupSetAttribute(image, "122", "122 122 122");
  IupSetAttribute(image, "123", "123 123 123");
  IupSetAttribute(image, "124", "124 124 124");
  IupSetAttribute(image, "125", "125 125 125");
  IupSetAttribute(image, "126", "126 126 126");
  IupSetAttribute(image, "127", "127 127 127");

  return image;
}

static Ihandle* load_image_TecgrafPUC_Rio8(void)
{
  unsigned char imgdata[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 23, 2, 2, 10, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 28, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 20, 255, 255, 47, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 51, 2, 2, 2, 82, 255, 3, 2, 69, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 4, 2, 2, 2, 255, 255, 2, 2, 8, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 8, 255, 255, 21, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 88, 2, 2, 2, 44, 255, 255, 66, 2, 2, 80, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 58, 2, 2, 2, 75, 255, 255, 255, 2, 2, 46, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 29, 2, 2, 2, 255, 255, 255, 255, 2, 2, 13, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 6, 2, 2, 2, 255, 255, 255, 255, 66, 57, 45, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 65, 35, 10, 2, 2, 2, 2, 255, 35, 2, 2, 2, 2, 2, 2, 8, 31, 61, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 2, 2, 2, 2, 255, 255, 58, 2, 2, 2, 2, 2, 2, 50, 255, 255, 255, 34, 2, 2, 2, 2, 2, 255, 255, 255, 255, 12, 2, 2, 2, 2, 2, 255, 24, 2, 2, 2, 2, 2, 2, 72, 255, 255, 255, 255, 255, 2, 2, 255, 255, 255, 255, 255, 91, 3, 2, 2, 2, 2, 2, 3,
    255, 255, 255, 89, 21, 2, 2, 3, 2, 2, 2, 2, 2, 2, 255, 23, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 12, 77, 255, 255, 255, 255, 255, 255, 255, 255, 255, 38, 38, 28, 2, 6, 38, 38, 38, 255, 91, 2, 5, 35, 38, 38, 38, 38, 72, 255, 255, 5, 2, 5, 33, 38, 38, 38, 255, 255, 69, 2, 2, 14, 37, 38, 38, 38, 255, 56, 38, 38, 38, 38, 33, 2, 2, 255, 255, 255, 255, 77, 2, 2, 61, 255, 255, 255, 255, 2, 2, 29, 38, 38, 38, 38, 39,
    255, 255, 57, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 255, 34, 25, 21, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 26, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 41, 2, 255, 255, 255, 255, 255, 255, 255, 255, 16, 2, 54, 255, 255, 255, 255, 255, 255, 255, 2, 1, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 80, 2, 63, 255, 255, 255, 24, 2, 2, 10, 255, 255, 255, 81, 2, 61, 255, 255, 255, 255, 255, 255,
    255, 255, 86, 2, 2, 9, 48, 82, 255, 87, 2, 2, 2, 9, 255, 255, 255, 255, 40, 2, 2, 88, 255, 255, 86, 55, 12, 2, 2, 59, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 33, 2, 255, 255, 255, 255, 255, 255, 255, 255, 2, 21, 255, 255, 255, 255, 255, 255, 255, 52, 2, 76, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 57, 2, 68, 255, 255, 255, 2, 3, 10, 2, 255, 255, 255, 76, 2, 76, 255, 255, 255, 255, 255, 255,
    16, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 12, 255, 255, 255, 255, 42, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 29, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 33, 2, 2, 2, 2, 2, 71, 255, 255, 77, 2, 74, 255, 255, 255, 255, 255, 255, 255, 15, 2, 255, 255, 255, 255, 255, 2, 2, 255, 255, 2, 2, 2, 2, 2, 2, 3, 255, 255, 255, 255, 2, 47, 64, 2, 77, 255, 255, 76, 2, 2, 2, 2, 2, 29, 255, 255,
    2, 2, 31, 255, 255, 255, 255, 255, 255, 255, 38, 21, 8, 12, 255, 255, 255, 255, 44, 6, 18, 255, 255, 255, 255, 255, 255, 255, 255, 44, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 33, 2, 2, 2, 2, 2, 71, 255, 255, 77, 2, 74, 255, 255, 255, 255, 255, 255, 255, 15, 2, 255, 255, 255, 255, 255, 2, 2, 255, 255, 255, 2, 2, 2, 2, 21, 255, 255, 255, 255, 44, 2, 255, 255, 2, 25, 255, 255, 76, 2, 2, 2, 2, 2, 29, 255, 255,
    2, 2, 2, 2, 2, 17, 57, 87, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 63, 25, 2, 2, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 33, 2, 255, 255, 255, 255, 255, 255, 255, 255, 2, 21, 255, 255, 255, 255, 255, 255, 255, 53, 2, 77, 255, 255, 255, 255, 2, 2, 255, 255, 255, 73, 2, 3, 255, 255, 255, 255, 255, 255, 3, 2, 255, 255, 3, 2, 255, 255, 76, 2, 76, 255, 255, 255, 255, 255, 255,
    19, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7, 18, 23, 27, 26, 25, 18, 9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 40, 2, 255, 255, 255, 255, 255, 255, 255, 255, 15, 2, 57, 255, 255, 255, 255, 255, 255, 255, 2, 3, 255, 255, 255, 255, 2, 2, 255, 255, 255, 255, 58, 2, 9, 255, 255, 255, 255, 255, 2, 28, 255, 255, 46, 2, 255, 255, 76, 2, 83, 255, 255, 255, 255, 255, 255,
    255, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 88, 2, 3, 39, 42, 42, 42, 42, 75, 255, 255, 3, 2, 6, 35, 42, 42, 42, 255, 255, 67, 2, 2, 15, 40, 37, 2, 2, 255, 255, 255, 255, 255, 43, 2, 19, 255, 255, 255, 60, 2, 84, 255, 255, 255, 2, 42, 255, 76, 2, 83, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 40, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 28, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 25, 255, 255, 255, 255, 255, 56, 2, 2, 2, 2, 2, 2, 50, 255, 255, 255, 32, 2, 2, 2, 2, 2, 255, 255, 255, 255, 10, 2, 2, 2, 2, 2, 255, 255, 255, 255, 255, 255, 28, 2, 28, 255, 255, 10, 2, 255, 255, 255, 255, 2, 3, 255, 76, 2, 83, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 79, 50, 19, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 44, 72, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 5, 2, 2, 2, 75, 255, 255, 90, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 29, 2, 2, 2, 255, 255, 255, 255, 2, 2, 14, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 58, 2, 2, 2, 74, 255, 255, 255, 2, 2, 46, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 88, 2, 2, 2, 44, 255, 255, 65, 2, 2, 80, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 3, 9, 9, 5, 29, 255, 255, 47, 255, 255, 255, 255, 8, 255, 255, 255, 58, 5, 8, 9, 255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 5, 9, 9, 4, 76, 255, 255, 2, 255, 255, 255, 76, 8, 4, 22, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 8, 255, 255, 22, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 37, 255, 255, 255, 70, 80, 255, 47, 255, 255, 255, 255, 8, 255, 255, 36, 69, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 255, 255, 255, 255, 13, 255, 255, 2, 255, 255, 67, 50, 255, 255, 255, 16, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 5, 2, 2, 2, 255, 255, 2, 2, 8, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 34, 255, 255, 255, 62, 85, 255, 47, 255, 255, 255, 255, 8, 255, 255, 30, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 255, 255, 255, 255, 10, 255, 255, 2, 255, 255, 9, 255, 255, 255, 255, 255, 46, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 51, 2, 2, 2, 84, 255, 3, 2, 69, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 3, 3, 5, 50, 255, 255, 47, 255, 255, 255, 255, 8, 255, 255, 63, 255, 255, 255, 255, 255, 255, 3, 3, 3, 3, 3, 70, 255, 65, 1, 3, 3, 1, 255, 255, 255, 2, 255, 255, 30, 255, 255, 255, 255, 255, 21, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 19, 255, 255, 44, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 37, 255, 255, 255, 255, 255, 255, 47, 255, 255, 255, 255, 8, 255, 255, 25, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 255, 255, 255, 12, 255, 255, 255, 2, 255, 255, 8, 255, 255, 255, 255, 255, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 28, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 37, 255, 255, 255, 255, 255, 255, 63, 81, 255, 255, 255, 26, 255, 255, 49, 58, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 255, 255, 255, 69, 86, 255, 255, 2, 255, 255, 78, 38, 255, 255, 255, 12, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 22, 2, 2, 10, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 37, 255, 255, 255, 255, 255, 255, 255, 33, 2, 2, 19, 255, 255, 255, 255, 72, 11, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 65, 255, 255, 255, 255, 25, 255, 255, 2, 255, 255, 255, 255, 20, 3, 38, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  Ihandle* image = IupImage(104, 32, imgdata);

  IupSetAttribute(image, "0", "0 0 0");
  IupSetAttribute(image, "1", "35 103 154");
  IupSetAttribute(image, "2", "35 104 155");
  IupSetAttribute(image, "3", "36 104 155");
  IupSetAttribute(image, "4", "37 105 156");
  IupSetAttribute(image, "5", "38 106 156");
  IupSetAttribute(image, "6", "39 107 157");
  IupSetAttribute(image, "7", "40 107 157");
  IupSetAttribute(image, "8", "41 108 157");
  IupSetAttribute(image, "9", "42 108 158");
  IupSetAttribute(image, "10", "44 110 159");
  IupSetAttribute(image, "11", "46 111 160");
  IupSetAttribute(image, "12", "47 112 160");
  IupSetAttribute(image, "13", "49 113 160");
  IupSetAttribute(image, "14", "49 113 161");
  IupSetAttribute(image, "15", "49 114 161");
  IupSetAttribute(image, "16", "51 115 162");
  IupSetAttribute(image, "17", "52 115 162");
  IupSetAttribute(image, "18", "52 116 162");
  IupSetAttribute(image, "19", "54 116 163");
  IupSetAttribute(image, "20", "55 117 164");
  IupSetAttribute(image, "21", "56 118 164");
  IupSetAttribute(image, "22", "57 119 165");
  IupSetAttribute(image, "23", "58 120 165");
  IupSetAttribute(image, "24", "60 121 165");
  IupSetAttribute(image, "25", "60 121 166");
  IupSetAttribute(image, "26", "62 122 167");
  IupSetAttribute(image, "27", "63 123 168");
  IupSetAttribute(image, "28", "65 124 168");
  IupSetAttribute(image, "29", "66 125 169");
  IupSetAttribute(image, "30", "67 126 169");
  IupSetAttribute(image, "31", "68 126 169");
  IupSetAttribute(image, "32", "69 127 170");
  IupSetAttribute(image, "33", "71 128 171");
  IupSetAttribute(image, "34", "72 129 171");
  IupSetAttribute(image, "35", "74 130 172");
  IupSetAttribute(image, "36", "75 131 173");
  IupSetAttribute(image, "37", "75 132 173");
  IupSetAttribute(image, "38", "76 132 173");
  IupSetAttribute(image, "39", "77 133 174");
  IupSetAttribute(image, "40", "79 133 175");
  IupSetAttribute(image, "41", "79 134 175");
  IupSetAttribute(image, "42", "80 135 175");
  IupSetAttribute(image, "43", "81 136 175");
  IupSetAttribute(image, "44", "82 136 176");
  IupSetAttribute(image, "45", "84 137 177");
  IupSetAttribute(image, "46", "85 138 177");
  IupSetAttribute(image, "47", "86 139 178");
  IupSetAttribute(image, "48", "87 140 178");
  IupSetAttribute(image, "49", "88 140 178");
  IupSetAttribute(image, "50", "88 140 179");
  IupSetAttribute(image, "51", "90 142 180");
  IupSetAttribute(image, "52", "92 143 180");
  IupSetAttribute(image, "53", "92 143 181");
  IupSetAttribute(image, "54", "94 144 181");
  IupSetAttribute(image, "55", "95 145 182");
  IupSetAttribute(image, "56", "97 146 182");
  IupSetAttribute(image, "57", "98 146 183");
  IupSetAttribute(image, "58", "100 148 184");
  IupSetAttribute(image, "59", "101 149 185");
  IupSetAttribute(image, "60", "102 150 185");
  IupSetAttribute(image, "61", "104 151 186");
  IupSetAttribute(image, "62", "105 152 186");
  IupSetAttribute(image, "63", "106 152 187");
  IupSetAttribute(image, "64", "107 153 189");
  IupSetAttribute(image, "65", "108 154 188");
  IupSetAttribute(image, "66", "110 155 189");
  IupSetAttribute(image, "67", "113 158 190");
  IupSetAttribute(image, "68", "115 159 191");
  IupSetAttribute(image, "69", "118 160 192");
  IupSetAttribute(image, "70", "119 161 193");
  IupSetAttribute(image, "71", "119 162 193");
  IupSetAttribute(image, "72", "120 162 193");
  IupSetAttribute(image, "73", "121 163 194");
  IupSetAttribute(image, "74", "123 164 194");
  IupSetAttribute(image, "75", "123 164 195");
  IupSetAttribute(image, "76", "124 164 195");
  IupSetAttribute(image, "77", "125 166 196");
  IupSetAttribute(image, "78", "127 167 197");
  IupSetAttribute(image, "79", "128 167 197");
  IupSetAttribute(image, "80", "128 168 197");
  IupSetAttribute(image, "81", "131 169 198");
  IupSetAttribute(image, "82", "131 170 198");
  IupSetAttribute(image, "83", "132 170 198");
  IupSetAttribute(image, "84", "132 170 199");
  IupSetAttribute(image, "85", "135 172 200");
  IupSetAttribute(image, "86", "137 174 201");
  IupSetAttribute(image, "87", "139 175 202");
  IupSetAttribute(image, "88", "140 176 202");
  IupSetAttribute(image, "89", "143 177 203");
  IupSetAttribute(image, "90", "143 178 203");
  IupSetAttribute(image, "91", "143 178 204");
  IupSetAttribute(image, "92", "0 0 0");
  IupSetAttribute(image, "93", "0 0 0");
  IupSetAttribute(image, "94", "0 0 0");
  IupSetAttribute(image, "95", "0 0 0");
  IupSetAttribute(image, "96", "0 0 0");
  IupSetAttribute(image, "97", "0 0 0");
  IupSetAttribute(image, "98", "0 0 0");
  IupSetAttribute(image, "99", "0 0 0");
  IupSetAttribute(image, "100", "0 0 0");
  IupSetAttribute(image, "101", "0 0 0");
  IupSetAttribute(image, "102", "0 0 0");
  IupSetAttribute(image, "103", "0 0 0");
  IupSetAttribute(image, "104", "0 0 0");
  IupSetAttribute(image, "105", "0 0 0");
  IupSetAttribute(image, "106", "0 0 0");
  IupSetAttribute(image, "107", "0 0 0");
  IupSetAttribute(image, "108", "0 0 0");
  IupSetAttribute(image, "109", "0 0 0");
  IupSetAttribute(image, "110", "0 0 0");
  IupSetAttribute(image, "111", "0 0 0");
  IupSetAttribute(image, "112", "0 0 0");
  IupSetAttribute(image, "113", "0 0 0");
  IupSetAttribute(image, "114", "0 0 0");
  IupSetAttribute(image, "115", "0 0 0");
  IupSetAttribute(image, "116", "0 0 0");
  IupSetAttribute(image, "117", "0 0 0");
  IupSetAttribute(image, "118", "0 0 0");
  IupSetAttribute(image, "119", "0 0 0");
  IupSetAttribute(image, "120", "0 0 0");
  IupSetAttribute(image, "121", "0 0 0");
  IupSetAttribute(image, "122", "0 0 0");
  IupSetAttribute(image, "123", "0 0 0");
  IupSetAttribute(image, "124", "0 0 0");
  IupSetAttribute(image, "125", "0 0 0");
  IupSetAttribute(image, "126", "0 0 0");
  IupSetAttribute(image, "127", "0 0 0");
  IupSetAttribute(image, "128", "0 0 0");
  IupSetAttribute(image, "129", "0 0 0");
  IupSetAttribute(image, "130", "0 0 0");
  IupSetAttribute(image, "131", "0 0 0");
  IupSetAttribute(image, "132", "0 0 0");
  IupSetAttribute(image, "133", "0 0 0");
  IupSetAttribute(image, "134", "0 0 0");
  IupSetAttribute(image, "135", "0 0 0");
  IupSetAttribute(image, "136", "0 0 0");
  IupSetAttribute(image, "137", "0 0 0");
  IupSetAttribute(image, "138", "0 0 0");
  IupSetAttribute(image, "139", "0 0 0");
  IupSetAttribute(image, "140", "0 0 0");
  IupSetAttribute(image, "141", "0 0 0");
  IupSetAttribute(image, "142", "0 0 0");
  IupSetAttribute(image, "143", "0 0 0");
  IupSetAttribute(image, "144", "0 0 0");
  IupSetAttribute(image, "145", "0 0 0");
  IupSetAttribute(image, "146", "0 0 0");
  IupSetAttribute(image, "147", "0 0 0");
  IupSetAttribute(image, "148", "0 0 0");
  IupSetAttribute(image, "149", "0 0 0");
  IupSetAttribute(image, "150", "0 0 0");
  IupSetAttribute(image, "151", "0 0 0");
  IupSetAttribute(image, "152", "0 0 0");
  IupSetAttribute(image, "153", "0 0 0");
  IupSetAttribute(image, "154", "0 0 0");
  IupSetAttribute(image, "155", "0 0 0");
  IupSetAttribute(image, "156", "0 0 0");
  IupSetAttribute(image, "157", "0 0 0");
  IupSetAttribute(image, "158", "0 0 0");
  IupSetAttribute(image, "159", "0 0 0");
  IupSetAttribute(image, "160", "0 0 0");
  IupSetAttribute(image, "161", "0 0 0");
  IupSetAttribute(image, "162", "0 0 0");
  IupSetAttribute(image, "163", "0 0 0");
  IupSetAttribute(image, "164", "0 0 0");
  IupSetAttribute(image, "165", "0 0 0");
  IupSetAttribute(image, "166", "0 0 0");
  IupSetAttribute(image, "167", "0 0 0");
  IupSetAttribute(image, "168", "0 0 0");
  IupSetAttribute(image, "169", "0 0 0");
  IupSetAttribute(image, "170", "0 0 0");
  IupSetAttribute(image, "171", "0 0 0");
  IupSetAttribute(image, "172", "0 0 0");
  IupSetAttribute(image, "173", "0 0 0");
  IupSetAttribute(image, "174", "0 0 0");
  IupSetAttribute(image, "175", "0 0 0");
  IupSetAttribute(image, "176", "0 0 0");
  IupSetAttribute(image, "177", "0 0 0");
  IupSetAttribute(image, "178", "0 0 0");
  IupSetAttribute(image, "179", "0 0 0");
  IupSetAttribute(image, "180", "0 0 0");
  IupSetAttribute(image, "181", "0 0 0");
  IupSetAttribute(image, "182", "0 0 0");
  IupSetAttribute(image, "183", "0 0 0");
  IupSetAttribute(image, "184", "0 0 0");
  IupSetAttribute(image, "185", "0 0 0");
  IupSetAttribute(image, "186", "0 0 0");
  IupSetAttribute(image, "187", "0 0 0");
  IupSetAttribute(image, "188", "0 0 0");
  IupSetAttribute(image, "189", "0 0 0");
  IupSetAttribute(image, "190", "0 0 0");
  IupSetAttribute(image, "191", "0 0 0");
  IupSetAttribute(image, "192", "0 0 0");
  IupSetAttribute(image, "193", "0 0 0");
  IupSetAttribute(image, "194", "0 0 0");
  IupSetAttribute(image, "195", "0 0 0");
  IupSetAttribute(image, "196", "0 0 0");
  IupSetAttribute(image, "197", "0 0 0");
  IupSetAttribute(image, "198", "0 0 0");
  IupSetAttribute(image, "199", "0 0 0");
  IupSetAttribute(image, "200", "0 0 0");
  IupSetAttribute(image, "201", "0 0 0");
  IupSetAttribute(image, "202", "0 0 0");
  IupSetAttribute(image, "203", "0 0 0");
  IupSetAttribute(image, "204", "0 0 0");
  IupSetAttribute(image, "205", "0 0 0");
  IupSetAttribute(image, "206", "0 0 0");
  IupSetAttribute(image, "207", "0 0 0");
  IupSetAttribute(image, "208", "0 0 0");
  IupSetAttribute(image, "209", "0 0 0");
  IupSetAttribute(image, "210", "0 0 0");
  IupSetAttribute(image, "211", "0 0 0");
  IupSetAttribute(image, "212", "0 0 0");
  IupSetAttribute(image, "213", "0 0 0");
  IupSetAttribute(image, "214", "0 0 0");
  IupSetAttribute(image, "215", "0 0 0");
  IupSetAttribute(image, "216", "0 0 0");
  IupSetAttribute(image, "217", "0 0 0");
  IupSetAttribute(image, "218", "0 0 0");
  IupSetAttribute(image, "219", "0 0 0");
  IupSetAttribute(image, "220", "0 0 0");
  IupSetAttribute(image, "221", "0 0 0");
  IupSetAttribute(image, "222", "0 0 0");
  IupSetAttribute(image, "223", "0 0 0");
  IupSetAttribute(image, "224", "0 0 0");
  IupSetAttribute(image, "225", "0 0 0");
  IupSetAttribute(image, "226", "0 0 0");
  IupSetAttribute(image, "227", "0 0 0");
  IupSetAttribute(image, "228", "0 0 0");
  IupSetAttribute(image, "229", "0 0 0");
  IupSetAttribute(image, "230", "0 0 0");
  IupSetAttribute(image, "231", "0 0 0");
  IupSetAttribute(image, "232", "0 0 0");
  IupSetAttribute(image, "233", "0 0 0");
  IupSetAttribute(image, "234", "0 0 0");
  IupSetAttribute(image, "235", "0 0 0");
  IupSetAttribute(image, "236", "0 0 0");
  IupSetAttribute(image, "237", "0 0 0");
  IupSetAttribute(image, "238", "0 0 0");
  IupSetAttribute(image, "239", "0 0 0");
  IupSetAttribute(image, "240", "0 0 0");
  IupSetAttribute(image, "241", "0 0 0");
  IupSetAttribute(image, "242", "0 0 0");
  IupSetAttribute(image, "243", "0 0 0");
  IupSetAttribute(image, "244", "0 0 0");
  IupSetAttribute(image, "245", "0 0 0");
  IupSetAttribute(image, "246", "0 0 0");
  IupSetAttribute(image, "247", "0 0 0");
  IupSetAttribute(image, "248", "0 0 0");
  IupSetAttribute(image, "249", "0 0 0");
  IupSetAttribute(image, "250", "0 0 0");
  IupSetAttribute(image, "251", "0 0 0");
  IupSetAttribute(image, "252", "0 0 0");
  IupSetAttribute(image, "253", "0 0 0");
  IupSetAttribute(image, "254", "0 0 0");
  IupSetAttribute(image, "255", "BGCOLOR");

  return image;
}

static Ihandle* load_image_Tecgraf8(void)
{
  unsigned char imgdata[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 9, 2, 2, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 15, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 19, 255, 37, 29, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 23, 2, 2, 2, 255, 255, 2, 2, 44, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 255, 255, 2, 2, 10, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 9, 255, 255, 6, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 35, 2, 2, 2, 32, 255, 255, 27, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 26, 2, 2, 2, 41, 255, 255, 37, 2, 2, 33, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 11, 2, 2, 2, 255, 255, 255, 255, 2, 2, 21, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 255, 255, 255, 255, 34, 31, 28, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 33, 20, 4, 2, 2, 2, 2, 255, 11, 2, 2, 2, 2, 2, 2, 4, 20, 33, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 43, 9, 2, 2, 2, 2, 2, 2, 2, 2, 3, 255, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 44, 255, 255, 255,
    255, 255, 29, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7, 255, 17, 17, 15, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 30, 255, 255,
    255, 255, 39, 2, 2, 5, 27, 37, 255, 36, 2, 2, 2, 15, 255, 255, 255, 255, 14, 2, 2, 255, 255, 255, 38, 27, 6, 2, 2, 39, 255, 255,
    7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 17, 255, 255, 255, 255, 16, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 8,
    1, 2, 19, 255, 255, 255, 255, 255, 255, 255, 22, 12, 4, 17, 255, 255, 255, 255, 16, 4, 12, 255, 255, 255, 255, 255, 255, 255, 255, 18, 2, 2,
    1, 2, 2, 2, 2, 8, 31, 42, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 40, 31, 9, 2, 2, 2, 2, 2,
    12, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 7, 13, 16, 16, 13, 7, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11,
    255, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 255,
    255, 255, 255, 25, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 24, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 35, 26, 11, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 11, 26, 35, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 37, 255, 255, 37, 2, 2, 5, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 11, 2, 2, 2, 255, 255, 255, 255, 2, 2, 21, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 26, 2, 2, 2, 41, 255, 255, 37, 2, 2, 33, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 35, 2, 2, 2, 32, 255, 255, 27, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 9, 255, 255, 6, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 2, 255, 255, 2, 2, 10, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 23, 2, 2, 2, 255, 255, 2, 2, 44, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 2, 2, 2, 19, 255, 37, 29, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 14, 2, 2, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 10, 2, 2, 7, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  Ihandle* image = IupImage(32, 32, imgdata);

  IupSetAttribute(image, "0", "0 0 0");
  IupSetAttribute(image, "1", "50 102 151");
  IupSetAttribute(image, "2", "50 102 152");
  IupSetAttribute(image, "3", "55 105 154");
  IupSetAttribute(image, "4", "56 106 155");
  IupSetAttribute(image, "5", "58 108 155");
  IupSetAttribute(image, "6", "58 108 156");
  IupSetAttribute(image, "7", "65 113 159");
  IupSetAttribute(image, "8", "66 114 159");
  IupSetAttribute(image, "9", "66 114 160");
  IupSetAttribute(image, "10", "68 115 161");
  IupSetAttribute(image, "11", "69 116 161");
  IupSetAttribute(image, "12", "70 117 162");
  IupSetAttribute(image, "13", "71 118 162");
  IupSetAttribute(image, "14", "72 118 162");
  IupSetAttribute(image, "15", "72 118 163");
  IupSetAttribute(image, "16", "75 120 164");
  IupSetAttribute(image, "17", "76 121 164");
  IupSetAttribute(image, "18", "79 124 166");
  IupSetAttribute(image, "19", "80 125 167");
  IupSetAttribute(image, "20", "84 127 169");
  IupSetAttribute(image, "21", "85 128 169");
  IupSetAttribute(image, "22", "89 131 171");
  IupSetAttribute(image, "23", "91 132 172");
  IupSetAttribute(image, "24", "92 133 172");
  IupSetAttribute(image, "25", "92 133 173");
  IupSetAttribute(image, "26", "100 139 177");
  IupSetAttribute(image, "27", "102 141 178");
  IupSetAttribute(image, "28", "105 143 179");
  IupSetAttribute(image, "29", "106 144 180");
  IupSetAttribute(image, "30", "108 145 180");
  IupSetAttribute(image, "31", "109 146 181");
  IupSetAttribute(image, "32", "110 147 182");
  IupSetAttribute(image, "33", "118 153 186");
  IupSetAttribute(image, "34", "120 154 187");
  IupSetAttribute(image, "35", "138 168 196");
  IupSetAttribute(image, "36", "141 170 197");
  IupSetAttribute(image, "37", "142 170 198");
  IupSetAttribute(image, "38", "143 171 199");
  IupSetAttribute(image, "39", "145 172 199");
  IupSetAttribute(image, "40", "146 173 200");
  IupSetAttribute(image, "41", "147 174 200");
  IupSetAttribute(image, "42", "147 174 201");
  IupSetAttribute(image, "43", "149 175 201");
  IupSetAttribute(image, "44", "150 176 202");
  IupSetAttribute(image, "45", "0 0 0");
  IupSetAttribute(image, "46", "0 0 0");
  IupSetAttribute(image, "47", "0 0 0");
  IupSetAttribute(image, "48", "0 0 0");
  IupSetAttribute(image, "49", "0 0 0");
  IupSetAttribute(image, "50", "0 0 0");
  IupSetAttribute(image, "51", "0 0 0");
  IupSetAttribute(image, "52", "0 0 0");
  IupSetAttribute(image, "53", "0 0 0");
  IupSetAttribute(image, "54", "0 0 0");
  IupSetAttribute(image, "55", "0 0 0");
  IupSetAttribute(image, "56", "0 0 0");
  IupSetAttribute(image, "57", "0 0 0");
  IupSetAttribute(image, "58", "0 0 0");
  IupSetAttribute(image, "59", "0 0 0");
  IupSetAttribute(image, "60", "0 0 0");
  IupSetAttribute(image, "61", "0 0 0");
  IupSetAttribute(image, "62", "0 0 0");
  IupSetAttribute(image, "63", "0 0 0");
  IupSetAttribute(image, "64", "0 0 0");
  IupSetAttribute(image, "65", "0 0 0");
  IupSetAttribute(image, "66", "0 0 0");
  IupSetAttribute(image, "67", "0 0 0");
  IupSetAttribute(image, "68", "0 0 0");
  IupSetAttribute(image, "69", "0 0 0");
  IupSetAttribute(image, "70", "0 0 0");
  IupSetAttribute(image, "71", "0 0 0");
  IupSetAttribute(image, "72", "0 0 0");
  IupSetAttribute(image, "73", "0 0 0");
  IupSetAttribute(image, "74", "0 0 0");
  IupSetAttribute(image, "75", "0 0 0");
  IupSetAttribute(image, "76", "0 0 0");
  IupSetAttribute(image, "77", "0 0 0");
  IupSetAttribute(image, "78", "0 0 0");
  IupSetAttribute(image, "79", "0 0 0");
  IupSetAttribute(image, "80", "0 0 0");
  IupSetAttribute(image, "81", "0 0 0");
  IupSetAttribute(image, "82", "0 0 0");
  IupSetAttribute(image, "83", "0 0 0");
  IupSetAttribute(image, "84", "0 0 0");
  IupSetAttribute(image, "85", "0 0 0");
  IupSetAttribute(image, "86", "0 0 0");
  IupSetAttribute(image, "87", "0 0 0");
  IupSetAttribute(image, "88", "0 0 0");
  IupSetAttribute(image, "89", "0 0 0");
  IupSetAttribute(image, "90", "0 0 0");
  IupSetAttribute(image, "91", "0 0 0");
  IupSetAttribute(image, "92", "0 0 0");
  IupSetAttribute(image, "93", "0 0 0");
  IupSetAttribute(image, "94", "0 0 0");
  IupSetAttribute(image, "95", "0 0 0");
  IupSetAttribute(image, "96", "0 0 0");
  IupSetAttribute(image, "97", "0 0 0");
  IupSetAttribute(image, "98", "0 0 0");
  IupSetAttribute(image, "99", "0 0 0");
  IupSetAttribute(image, "100", "0 0 0");
  IupSetAttribute(image, "101", "0 0 0");
  IupSetAttribute(image, "102", "0 0 0");
  IupSetAttribute(image, "103", "0 0 0");
  IupSetAttribute(image, "104", "0 0 0");
  IupSetAttribute(image, "105", "0 0 0");
  IupSetAttribute(image, "106", "0 0 0");
  IupSetAttribute(image, "107", "0 0 0");
  IupSetAttribute(image, "108", "0 0 0");
  IupSetAttribute(image, "109", "0 0 0");
  IupSetAttribute(image, "110", "0 0 0");
  IupSetAttribute(image, "111", "0 0 0");
  IupSetAttribute(image, "112", "0 0 0");
  IupSetAttribute(image, "113", "0 0 0");
  IupSetAttribute(image, "114", "0 0 0");
  IupSetAttribute(image, "115", "0 0 0");
  IupSetAttribute(image, "116", "0 0 0");
  IupSetAttribute(image, "117", "0 0 0");
  IupSetAttribute(image, "118", "0 0 0");
  IupSetAttribute(image, "119", "0 0 0");
  IupSetAttribute(image, "120", "0 0 0");
  IupSetAttribute(image, "121", "0 0 0");
  IupSetAttribute(image, "122", "0 0 0");
  IupSetAttribute(image, "123", "0 0 0");
  IupSetAttribute(image, "124", "0 0 0");
  IupSetAttribute(image, "125", "0 0 0");
  IupSetAttribute(image, "126", "0 0 0");
  IupSetAttribute(image, "127", "0 0 0");
  IupSetAttribute(image, "128", "0 0 0");
  IupSetAttribute(image, "129", "0 0 0");
  IupSetAttribute(image, "130", "0 0 0");
  IupSetAttribute(image, "131", "0 0 0");
  IupSetAttribute(image, "132", "0 0 0");
  IupSetAttribute(image, "133", "0 0 0");
  IupSetAttribute(image, "134", "0 0 0");
  IupSetAttribute(image, "135", "0 0 0");
  IupSetAttribute(image, "136", "0 0 0");
  IupSetAttribute(image, "137", "0 0 0");
  IupSetAttribute(image, "138", "0 0 0");
  IupSetAttribute(image, "139", "0 0 0");
  IupSetAttribute(image, "140", "0 0 0");
  IupSetAttribute(image, "141", "0 0 0");
  IupSetAttribute(image, "142", "0 0 0");
  IupSetAttribute(image, "143", "0 0 0");
  IupSetAttribute(image, "144", "0 0 0");
  IupSetAttribute(image, "145", "0 0 0");
  IupSetAttribute(image, "146", "0 0 0");
  IupSetAttribute(image, "147", "0 0 0");
  IupSetAttribute(image, "148", "0 0 0");
  IupSetAttribute(image, "149", "0 0 0");
  IupSetAttribute(image, "150", "0 0 0");
  IupSetAttribute(image, "151", "0 0 0");
  IupSetAttribute(image, "152", "0 0 0");
  IupSetAttribute(image, "153", "0 0 0");
  IupSetAttribute(image, "154", "0 0 0");
  IupSetAttribute(image, "155", "0 0 0");
  IupSetAttribute(image, "156", "0 0 0");
  IupSetAttribute(image, "157", "0 0 0");
  IupSetAttribute(image, "158", "0 0 0");
  IupSetAttribute(image, "159", "0 0 0");
  IupSetAttribute(image, "160", "0 0 0");
  IupSetAttribute(image, "161", "0 0 0");
  IupSetAttribute(image, "162", "0 0 0");
  IupSetAttribute(image, "163", "0 0 0");
  IupSetAttribute(image, "164", "0 0 0");
  IupSetAttribute(image, "165", "0 0 0");
  IupSetAttribute(image, "166", "0 0 0");
  IupSetAttribute(image, "167", "0 0 0");
  IupSetAttribute(image, "168", "0 0 0");
  IupSetAttribute(image, "169", "0 0 0");
  IupSetAttribute(image, "170", "0 0 0");
  IupSetAttribute(image, "171", "0 0 0");
  IupSetAttribute(image, "172", "0 0 0");
  IupSetAttribute(image, "173", "0 0 0");
  IupSetAttribute(image, "174", "0 0 0");
  IupSetAttribute(image, "175", "0 0 0");
  IupSetAttribute(image, "176", "0 0 0");
  IupSetAttribute(image, "177", "0 0 0");
  IupSetAttribute(image, "178", "0 0 0");
  IupSetAttribute(image, "179", "0 0 0");
  IupSetAttribute(image, "180", "0 0 0");
  IupSetAttribute(image, "181", "0 0 0");
  IupSetAttribute(image, "182", "0 0 0");
  IupSetAttribute(image, "183", "0 0 0");
  IupSetAttribute(image, "184", "0 0 0");
  IupSetAttribute(image, "185", "0 0 0");
  IupSetAttribute(image, "186", "0 0 0");
  IupSetAttribute(image, "187", "0 0 0");
  IupSetAttribute(image, "188", "0 0 0");
  IupSetAttribute(image, "189", "0 0 0");
  IupSetAttribute(image, "190", "0 0 0");
  IupSetAttribute(image, "191", "0 0 0");
  IupSetAttribute(image, "192", "0 0 0");
  IupSetAttribute(image, "193", "0 0 0");
  IupSetAttribute(image, "194", "0 0 0");
  IupSetAttribute(image, "195", "0 0 0");
  IupSetAttribute(image, "196", "0 0 0");
  IupSetAttribute(image, "197", "0 0 0");
  IupSetAttribute(image, "198", "0 0 0");
  IupSetAttribute(image, "199", "0 0 0");
  IupSetAttribute(image, "200", "0 0 0");
  IupSetAttribute(image, "201", "0 0 0");
  IupSetAttribute(image, "202", "0 0 0");
  IupSetAttribute(image, "203", "0 0 0");
  IupSetAttribute(image, "204", "0 0 0");
  IupSetAttribute(image, "205", "0 0 0");
  IupSetAttribute(image, "206", "0 0 0");
  IupSetAttribute(image, "207", "0 0 0");
  IupSetAttribute(image, "208", "0 0 0");
  IupSetAttribute(image, "209", "0 0 0");
  IupSetAttribute(image, "210", "0 0 0");
  IupSetAttribute(image, "211", "0 0 0");
  IupSetAttribute(image, "212", "0 0 0");
  IupSetAttribute(image, "213", "0 0 0");
  IupSetAttribute(image, "214", "0 0 0");
  IupSetAttribute(image, "215", "0 0 0");
  IupSetAttribute(image, "216", "0 0 0");
  IupSetAttribute(image, "217", "0 0 0");
  IupSetAttribute(image, "218", "0 0 0");
  IupSetAttribute(image, "219", "0 0 0");
  IupSetAttribute(image, "220", "0 0 0");
  IupSetAttribute(image, "221", "0 0 0");
  IupSetAttribute(image, "222", "0 0 0");
  IupSetAttribute(image, "223", "0 0 0");
  IupSetAttribute(image, "224", "0 0 0");
  IupSetAttribute(image, "225", "0 0 0");
  IupSetAttribute(image, "226", "0 0 0");
  IupSetAttribute(image, "227", "0 0 0");
  IupSetAttribute(image, "228", "0 0 0");
  IupSetAttribute(image, "229", "0 0 0");
  IupSetAttribute(image, "230", "0 0 0");
  IupSetAttribute(image, "231", "0 0 0");
  IupSetAttribute(image, "232", "0 0 0");
  IupSetAttribute(image, "233", "0 0 0");
  IupSetAttribute(image, "234", "0 0 0");
  IupSetAttribute(image, "235", "0 0 0");
  IupSetAttribute(image, "236", "0 0 0");
  IupSetAttribute(image, "237", "0 0 0");
  IupSetAttribute(image, "238", "0 0 0");
  IupSetAttribute(image, "239", "0 0 0");
  IupSetAttribute(image, "240", "0 0 0");
  IupSetAttribute(image, "241", "0 0 0");
  IupSetAttribute(image, "242", "0 0 0");
  IupSetAttribute(image, "243", "0 0 0");
  IupSetAttribute(image, "244", "0 0 0");
  IupSetAttribute(image, "245", "0 0 0");
  IupSetAttribute(image, "246", "0 0 0");
  IupSetAttribute(image, "247", "0 0 0");
  IupSetAttribute(image, "248", "0 0 0");
  IupSetAttribute(image, "249", "0 0 0");
  IupSetAttribute(image, "250", "0 0 0");
  IupSetAttribute(image, "251", "0 0 0");
  IupSetAttribute(image, "252", "0 0 0");
  IupSetAttribute(image, "253", "0 0 0");
  IupSetAttribute(image, "254", "0 0 0");
  IupSetAttribute(image, "255", "BGCOLOR");

  return image;
}

