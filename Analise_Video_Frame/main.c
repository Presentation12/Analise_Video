#include <stdio.h>
#include <stdlib.h>
#include "vc.h"


int main(){
    IVC* src = vc_read_image("frame10.ppm");
    IVC* hsv = vc_image_new(src->width,src->height,3,255);
    IVC* hsv_s = vc_image_new(hsv->width,hsv->height,1,255);
    IVC* bc = vc_image_new(hsv_s->width,hsv_s->height,1,255);
    IVC* blobs = vc_image_new(bc->width,bc->height,1,255);

    int n = 7;

    vc_rgb_to_hsv(src, hsv);
    vc_hsv_segmentation(hsv, hsv_s, 15, 30, 50, 100, 25, 90);
    vc_binary_close(hsv_s, bc, 7);
    OVC* blob = vc_binary_blob_labelling(bc, blobs, &n);
    vc_binary_blob_info(bc, blob, n);

    vc_write_image("imagemfinal.ppm", blobs);

    system("cmd /c start FilterGear frame10.ppm");
    system("FilterGear imagemfinal.ppm");

    vc_image_free(src);
    vc_image_free(hsv);
    vc_image_free(hsv_s);
    vc_image_free(bc);
    vc_image_free(blobs);
    return 1;
}