#include <stdio.h>
#include <stdlib.h>
#include "vc.h"


int main(){
    IVC* image = vc_read_image("frame1.png");

    //system("cmd /c start FilterGear frame1.png");
    system("FilterGear frame1.png");

    vc_image_free(image);
    return 1;
}