#include <stdio.h>
#include <stdlib.h>
#include "vc.h"

int main(){
    IVC* src = vc_read_image("frame11.ppm");
    IVC* hsv = vc_image_new(src->width,src->height,3,255);
    IVC* hsv_s = vc_image_new(hsv->width,hsv->height,1,255);
    IVC* bc = vc_image_new(hsv_s->width,hsv_s->height,1,255);
    IVC* bc2 = vc_image_new(bc->width,bc->height,1,255);

    vc_rgb_to_hsv(src, hsv);
    vc_hsv_segmentation(hsv, hsv_s, 15, 30, 50, 100, 25, 90);
    vc_binary_open(hsv_s, bc, 15);
    vc_binary_erode(bc, bc2, 33);

    vc_write_image("imagemfinal.ppm", bc2);

    system("cmd /c start FilterGear frame11.ppm");
    system("FilterGear imagemfinal.ppm");

    vc_image_free(src);
    vc_image_free(hsv);
    vc_image_free(hsv_s);
    vc_image_free(bc);
    vc_image_free(bc2);

    return 1;
}