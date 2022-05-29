//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define VC_DEBUG

#define MAX(r, g, b) (r > b ? (r > g ? r : g) : (b > g ? b : g))
#define MIN(r, g, b) (r < b ? (r < g ? r : g) : (b < g ? b : g))

#define MAX_2(a, b) (a > b ? a : b)
#define MIN_2(a, b) (a < b ? a : b)

#define CONV_RANGE(value, range, new_range) (value / range) * new_range
#define HSV_2_RGB(value) CONV_RANGE(value, 360, 255)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct
{
	unsigned char *data;
	int width, height;
	int channels;	  // Bin�rio/Cinzentos=1; RGB=3
	int levels;		  // Bin�rio=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline; // width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

// Negativos
int vc_gray_negative(IVC *srcdst);
int vc_rgb_negative(IVC *srcdst);

// Cores para Gray
int vc_rgb_get_red_gray(IVC *srcdst);
int vc_rgb_get_green_gray(IVC *srcdst);
int vc_rgb_get_blue_gray(IVC *srcdst);
int vc_rgb_to_gray(IVC *src, IVC *dst);

// RGB para HSV
int vc_rgb_to_hsv(IVC *src, IVC *dst);

// Segmentaçao HSV
int vc_hsv_segmentation(IVC *src, IVC *dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);

// gray (1 canal) para RGB
int vc_scale_gray_to_rgb(IVC *src, IVC *dst);

// binarização, threshold
int vc_gray_to_binary(IVC *src, IVC *dst, int threshold);
int vc_gray_to_binary_global_mean(IVC *src, IVC *dst);
int vc_gray_to_binary_midpoint_mean(IVC *src, IVC *dst, int kernel);

// dilatação binária
int vc_binary_dilate(IVC *src, IVC *dst, int kernel);

// erosao binária
int vc_binary_erode(IVC *src, IVC *dst, int kernel);

// abertura e fecho binária
int vc_binary_open(IVC *src, IVC *dst, int kernel);
int vc_binary_close(IVC *src, IVC *dst, int kernel);

// dilatação gray
int vc_gray_erode(IVC *src, IVC *dst, int kernel);

// erosão gray
int vc_gray_dilate(IVC *src, IVC *dst, int kernel);

// abertura e fecho gray
int vc_gray_open(IVC *src, IVC *dst, int kernel);
int vc_gray_close(IVC *src, IVC *dst, int kernel);

// etiquetagem blobs

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UM BLOB (OBJECTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct
{
	int x, y, width, height; // Caixa Delimitadora (Bounding Box)
	int area;				 // �rea
	int xc, yc;				 // Centro-de-massa
	int perimeter;			 // Per�metro
	int label;				 // Etiqueta
} OVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);
int vc_gray_histogram_show(IVC *src, IVC *dst);
int equalize(unsigned char *datasrc, unsigned char *datadst, int bytesperline, int channels, int offset, int width, int height);
int vc_gray_histogram_equalization(IVC *src, IVC *dst);
int vc_rgb_histogram_equalization(IVC *src, IVC *dst);
int vc_convert_bgr_to_rgb(IVC* src, IVC* dst);


int HSVFilter(IVC* srcImg, IVC* dstImg);