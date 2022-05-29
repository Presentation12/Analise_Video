//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de fun��es n�o seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "vc.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Alocar mem�ria para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *)malloc(sizeof(IVC));

	if (image == NULL)
		return NULL;
	if ((levels <= 0) || (levels > 255))
		return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *)malloc(image->width * image->height * image->channels * sizeof(char));

	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}

// Libertar mem�ria de uma imagem
IVC *vc_image_free(IVC *image)
{
	if (image != NULL)
	{
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;

	for (;;)
	{
		while (isspace(c = getc(file)))
			;
		if (c != '#')
			break;
		do
			c = getc(file);
		while ((c != '\n') && (c != EOF));
		if (c == EOF)
			break;
	}

	t = tok;

	if (c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while ((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));

		if (c == '#')
			ungetc(c, file);
	}

	*t = 0;

	return tok;
}

long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}

void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				// datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}

IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P4") == 0)
		{
			channels = 1;
			levels = 1;
		} // Se PBM (Binary [0,1])
		else if (strcmp(tok, "P5") == 0)
			channels = 1; // Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P6") == 0)
			channels = 3; // Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1) // PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}

int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL)
		return 0;

	if ((file = fopen(filename, "wb")) != NULL)
	{
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}

// Negativo de uma imagem em tons de cinzento (1 canal)
int vc_gray_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 1)
		return 0;

	// Inverte a imagem Gray
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			data[pos] = 255 - data[pos];
		}
	}

	return 1;
}

// Negativo de uma imagem RGB
int vc_rgb_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 3)
		return 0;

	// Inverte a imagem Gray
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			data[pos] = 255 - data[pos];
			data[pos + 1] = 255 - data[pos + 1];
			data[pos + 2] = 255 - data[pos + 2];
		}
	}

	return 1;
}

int vc_rgb_get_red_gray(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || srcdst->data == NULL)
		return 0;
	if (channels != 3)
		return 0;

	// Extrai componente Red
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos + 1] = data[pos]; // Green;
			data[pos + 2] = data[pos]; // Blue
		}
	}
	return 1;
}

int vc_rgb_get_green_gray(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || srcdst->data == NULL)
		return 0;
	if (channels != 3)
		return 0;

	// Extrai componente Green
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = data[pos + 1];	   // Red;
			data[pos + 2] = data[pos + 1]; // Blue
		}
	}
	return 1;
}

int vc_rgb_get_blue_gray(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->bytesperline;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificação de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || srcdst->data == NULL)
		return 0;
	if (channels != 3)
		return 0;

	// Extrai componente Blue
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = data[pos + 2];	   // Red;
			data[pos + 1] = data[pos + 2]; // Green
		}
	}
	return 1;
}

int vc_rgb_to_gray(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 3) || (dst->channels != 1))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((rf * 0.299) + (gf * 0.587) + (bf * 0.114)); // media ponderada, mais proxima do olho humano
																							// datadst[pos_dst] = (unsigned char) ((rf * 0.333) + (gf * 0.333) + (bf * 0.333));
		}
	}
	return 1;
}

int vc_rgb_to_hsv(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;

	int width = src->width;
	int height = src->height;

	int x, y;
	long int pos;
	float value, saturation, hue;
	float rf, gf, bf;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 3) || (dst->channels != 3))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src;

			float max = MAX(datasrc[pos], datasrc[pos + 1], datasrc[pos + 2]);
			float min = MIN(datasrc[pos], datasrc[pos + 1], datasrc[pos + 2]);

			value = (float)max;

			if (max == 0)
			{
				saturation = 0;
				hue = 0;
			}
			else
			{
				saturation = (float)(max - min) / max;

				if (saturation == 0)
					hue == 0;
				else
				{
					rf = (float)datasrc[pos];
					gf = (float)datasrc[pos + 1];
					bf = (float)datasrc[pos + 2];

					if (max == rf && gf >= bf)
					{
						hue = 60 * (gf - bf) / (max - min);
					}
					else if (max == rf && bf > gf)
					{
						hue = 360 + 60 * (gf - bf) / (max - min);
					}
					else if (max == gf)
					{
						hue = 120 + 60 * (bf - rf) / (max - min);
					}
					else if (max == bf)
					{
						hue = 240 + 60 * (rf - gf) / (max - min);
					}
				}
			}

			hue = hue / 360 * 255;
			saturation = saturation * 255;

			datadst[pos] = hue;
			datadst[pos + 1] = saturation;
			datadst[pos + 2] = value;
		}
	}
	return 1;
}

int vc_hsv_segmentation(IVC *src, IVC *dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax)
{
	unsigned char *data_src = (unsigned char *)src->data, *data_dst = (unsigned char *)dst->data;
	int width_src = src->width, width_dst = dst->width;
	int height_src = src->height, height_dst = dst->height;
	int channels_src = src->channels, channel_dst = dst->channels;
	int bytesperline_src = width_src * channels_src, bytesperline_dst = width_dst * channel_dst;
	int x, y;
	float hue, sat, value;

	if ((width_src <= 0) || (height_src <= 0) || (data_src == NULL))
		return 0;
	if (channels_src != 3 || channel_dst != 1)
		return 0;

	if ((width_src != width_dst) || (height_src != height_dst))
		return 0;

	hmin = HSV_2_RGB((float)hmin);
	hmax = HSV_2_RGB((float)hmax);

	smin = CONV_RANGE((float)smin, 100, 255);
	smax = CONV_RANGE((float)smax, 100, 255);

	vmin = CONV_RANGE((float)vmin, 100, 255);
	vmax = CONV_RANGE((float)vmax, 100, 255);

	for (y = 0; y < height_src; y++)
	{
		for (x = 0; x < width_src; x++)
		{
			int pos_src = y * bytesperline_src + x * channels_src;
			int pos_dst = y * bytesperline_dst + x * channel_dst;

			hue = (float)data_src[pos_src];
			sat = (float)data_src[pos_src + 1];
			value = (float)data_src[pos_src + 2];

			data_dst[pos_dst] = ((hue >= hmin && hue <= hmax) &&
								 (sat >= smin && sat <= smax) &&
								 (value >= vmin && value <= vmax))
									? 255
									: 0;
		}
	}

	return 1;
}

int vc_scale_gray_to_rgb(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	int x, y;
	long int pos_src, pos_dst;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 3))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			float value = (float)datasrc[pos_src];
			if (value < 64)
			{
				datadst[pos_dst] = 0;
				datadst[pos_dst + 1] = (value / 64) * 255;
				datadst[pos_dst + 2] = 255;
			}
			else if (value < 128)
			{
				datadst[pos_dst] = 0;
				datadst[pos_dst + 1] = 255;
				datadst[pos_dst + 2] = (1 - ((value - 64) / 64)) * 255;
			}
			else if (value < 192)
			{
				datadst[pos_dst] = (value - 128) / 64 * 255;
				datadst[pos_dst + 1] = 255;
				datadst[pos_dst + 2] = 0;
			}
			else
			{ // pode ser só else (?)
				datadst[pos_dst] = 255;
				datadst[pos_dst + 1] = (1 - ((value - 192) / 64)) * 255;
				datadst[pos_dst + 2] = 0;
			}
		}
	}

	return 1;
}

int vc_gray_to_binary(IVC *src, IVC *dst, int threshold)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src;

			if (datasrc[pos] > threshold)
			{
				datadst[pos] = dst->levels;
			}
			else
			{
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}

int vc_gray_to_binary_global_mean(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int sum = 0;

	long int pos;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src;
			sum += datasrc[pos];
		}
	}

	float threshold = (float)sum / (width * height * src->channels);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src;

			if (datasrc[pos] > threshold)
			{
				datadst[pos] = dst->levels;
			}
			else
			{
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}

int vc_gray_to_binary_midpoint_mean(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_2;
	int offset = kernel / 2;
	int max;
	int min;
	float threshold;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			max = 0;
			min = src->levels;

			for (int k_y = -offset; k_y <= offset; k_y++)
			{
				for (int k_x = -offset; k_x <= offset; k_x++)
				{

					pos_2 = (y + k_y) * bytesperline_src + (x + k_x) * channels_src;
					if ((y + k_y) > 0 && (x + k_x) > 0 && (y + k_y) < height && (x + k_x) < width)
					{
						if (datasrc[pos_2] > max)
						{
							max = datasrc[pos_2];
						}
						if (datasrc[pos_2] < min)
						{
							min = datasrc[pos_2];
						}
					}
				}
			}

			pos = y * bytesperline_src + x * channels_src;

			threshold = 0.5 * (min + max);

			if (datasrc[pos] > threshold)
			{
				datadst[pos] = dst->levels;
			}
			else
			{
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}

// NOT DONE! PROXIMA AULA
int vc_gray_to_binary_bernsen_mean(IVC *src, IVC *dst, int kernel, int cmin)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_2;
	int offset = kernel / 2;
	int max;
	int min;
	float threshold;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			max = 0;
			min = src->levels;

			for (int k_y = -offset; k_y <= offset; k_y++)
			{
				for (int k_x = -offset; k_x <= offset; k_x++)
				{

					pos_2 = (y + k_y) * bytesperline_src + (x + k_x) * channels_src;
					if ((y + k_y) > 0 && (x + k_x) > 0 && (y + k_y) < height && (x + k_x) < width)
					{
						if (datasrc[pos_2] > max)
						{
							max = datasrc[pos_2];
						}
						if (datasrc[pos_2] < min)
						{
							min = datasrc[pos_2];
						}
					}
				}
			}

			pos = y * bytesperline_src + x * channels_src;

			threshold = 0.5 * (min + max);

			if (datasrc[pos] > threshold)
			{
				datadst[pos] = dst->levels;
			}
			else
			{
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}

int vc_binary_dilate(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_2;
	int offset = kernel / 2;

	int is_first_plan;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			is_first_plan = 0;
			for (int k_y = -offset; k_y <= offset; k_y++)
			{
				for (int k_x = -offset; k_x <= offset; k_x++)
				{

					pos_2 = (y + k_y) * bytesperline_src + (x + k_x) * channels_src;
					if ((y + k_y) > 0 && (x + k_x) > 0 && (y + k_y) < height && (x + k_x) < width)
					{
						if (datasrc[pos_2] == 0)
						{
							is_first_plan = 1;
						}
					}
				}
			}

			pos = y * bytesperline_src + x * channels_src;
			if (is_first_plan == 1)
			{
				datadst[pos] = 0;
			}
			else
			{
				datadst[pos] = dst->levels;
			}
		}
	}
	return 1;
}

int vc_binary_erode(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_2;
	int offset = kernel / 2;

	int is_not_first_plan;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			is_not_first_plan = 0;
			for (int k_y = -offset; k_y <= offset; k_y++)
			{
				for (int k_x = -offset; k_x <= offset; k_x++)
				{

					pos_2 = (y + k_y) * bytesperline_src + (x + k_x) * channels_src;
					if ((y + k_y) > 0 && (x + k_x) > 0 && (y + k_y) < height && (x + k_x) < width)
					{
						if (datasrc[pos_2] == 255)
						{
							is_not_first_plan = 1;
						}
					}
				}
			}

			pos = y * bytesperline_src + x * channels_src;
			if (is_not_first_plan == 1)
			{
				datadst[pos] = dst->levels;
			}
			else
			{
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}

int vc_binary_open(IVC *src, IVC *dst, int kernel)
{
	IVC *image_aux;
	image_aux = vc_image_new(src->width, src->height, 1, 255);
	vc_binary_erode(src, image_aux, kernel);
	vc_binary_dilate(image_aux, dst, kernel);
	vc_image_free(image_aux);
}

int vc_binary_close(IVC *src, IVC *dst, int kernel)
{
	IVC *image_aux;
	image_aux = vc_image_new(src->width, src->height, 1, 255);
	vc_binary_dilate(src, image_aux, kernel);
	vc_binary_erode(image_aux, dst, kernel);
	vc_image_free(image_aux);
}

int vc_gray_erode(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_2;
	int offset = kernel / 2;

	int min;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			min = src->levels;
			for (int k_y = -offset; k_y <= offset; k_y++)
			{
				for (int k_x = -offset; k_x <= offset; k_x++)
				{

					pos_2 = (y + k_y) * bytesperline_src + (x + k_x) * channels_src;
					if ((y + k_y) > 0 && (x + k_x) > 0 && (y + k_y) < height && (x + k_x) < width)
					{
						if (datasrc[pos_2] < min)
						{
							min = datasrc[pos_2];
						}
					}
				}
			}

			pos = y * bytesperline_src + x * channels_src;
			datadst[pos] = min;
		}
	}
	return 1;
}

int vc_gray_dilate(IVC *src, IVC *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_2;
	int offset = kernel / 2;

	int max;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			max = 0;
			for (int k_y = -offset; k_y <= offset; k_y++)
			{
				for (int k_x = -offset; k_x <= offset; k_x++)
				{

					pos_2 = (y + k_y) * bytesperline_src + (x + k_x) * channels_src;
					if ((y + k_y) > 0 && (x + k_x) > 0 && (y + k_y) < height && (x + k_x) < width)
					{
						if (datasrc[pos_2] > max)
						{
							max = datasrc[pos_2];
						}
					}
				}
			}

			pos = y * bytesperline_src + x * channels_src;
			datadst[pos] = max;
		}
	}
	return 1;
}

int vc_gray_open(IVC *src, IVC *dst, int kernel)
{
	IVC *image_aux;
	image_aux = vc_image_new(src->width, src->height, src->channels, src->levels);
	vc_gray_erode(src, image_aux, kernel);
	vc_gray_dilate(image_aux, dst, kernel);
	vc_image_free(image_aux);
}

int vc_gray_close(IVC *src, IVC *dst, int kernel)
{
	IVC *image_aux;
	image_aux = vc_image_new(src->width, src->height, src->channels, src->levels);
	vc_gray_dilate(src, image_aux, kernel);
	vc_gray_erode(image_aux, dst, kernel);
	vc_image_free(image_aux);
}
/*
int vc_binary_blob_labelling(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;

	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;

	int width = src->width;
	int height = src->height;

	long int pos;
	long int pos_a;
	long int pos_b;
	long int pos_c;
	long int pos_d;

	int label = 1; // 60 para testes; 1 default

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 1))
		return 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src;
			if (datasrc[pos] == 255)
			{
				if (y - 1 > 0 && y + 1 < height && x - 1 > 0 && x + 1 < width)
				{
					pos_a = pos-bytesperline_src-1;
					pos_b = pos-bytesperline_src;
					pos_c = pos-bytesperline_src+1;
					pos_d = pos-1;

					if (datadst[pos_a] == 0 && datadst[pos_b] == 0 && datadst[pos_c] == 0 && datadst[pos_d] == 0)
					{
						datadst[pos] = label;
						label++; // +=10 para testes; ++ default
					}
					else
					{
						int min = 255;
						if (datadst[pos_a] < min && datadst[pos_a] != 0)
						{
							min = datadst[pos_a];
						}
						else if (datadst[pos_b] < min && datadst[pos_b] != 0)
						{
							min = datadst[pos_b];
						}
						else if (datadst[pos_c] < min && datadst[pos_c] != 0)
						{
							min = datadst[pos_c];
						}
						else if (datadst[pos_d] < min && datadst[pos_d] != 0)
						{
							min = datadst[pos_d];
						}
						datadst[pos] = min;
					}
				}
			}
			else
			{
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}
*/

// Etiquetagem de blobs
// src		: Imagem bin�ria de entrada
// dst		: Imagem grayscale (ir� conter as etiquetas)
// nlabels	: Endere�o de mem�ria de uma vari�vel, onde ser� armazenado o n�mero de etiquetas encontradas.
// OVC*		: Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. � necess�rio libertar posteriormente esta mem�ria.
OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC* blobs; // Apontador para array de blobs (objectos) que será retornado desta função.

				// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem binária para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixéis de plano de fundo devem obrigatóriamente ter valor 0
	// Todos os pixéis de primeiro plano devem obrigatóriamente ter valor 255
	// Serão atribuídas etiquetas no intervalo [1,254]
	// Este algoritmo está assim limitado a 255 labels
	for (i = 0, size = bytesperline * height; i < size; i++)
	{
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem binária
	for (y = 0; y < height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

													// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A está marcado
					if (datadst[posA] != 0) num = labeltable[datadst[posA]];
					// Se B está marcado, e é menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num)) num = labeltable[datadst[posB]];
					// Se C está marcado, e é menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num)) num = labeltable[datadst[posC]];
					// Se D está marcado, e é menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num)) num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	//? printf("\nMax Label = %d\n", label);

	// Contagem do número de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++)
	{
		for (b = a + 1; b < label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que não hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a < label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se não há blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC*)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a < (*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}

int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs)
{
	unsigned char* data = (unsigned char*)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta �rea de cada blob
	for (i = 0; i < nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y < height - 1; y++)
		{
			for (x = 1; x < width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// �rea
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Per�metro
					// Se pelo menos um dos quatro vizinhos n�o pertence ao mesmo label, ent�o � um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		//blobs[i].xc = (xmax - xmin) / 2;
		//blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MAX_2(blobs[i].area, 1);
		blobs[i].yc = sumy / MAX_2(blobs[i].area, 1);
	}

	return 1;
}

int vc_gray_histogram_show(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if (channels != 1)
		return 0;

	int ni[256] = {0};

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = y * bytesperline + x * channels;
			ni[datasrc[pos]]++;
		}
	}

	float pdf[256];
	int n = width * height;

	for (int i = 0; i < 256; i++)
	{
		pdf[i] = (float)ni[i] / (float)n;
	}

	float pdfmax = pdf[0];
	for (int i = 1; i < 256; i++)
	{
		if (pdf[i] > pdfmax)
		{
			pdfmax = pdf[i];
		}
	}

	float pdfnorm[256];
	for (int i = 0; i < 256; i++)
	{
		pdfnorm[i] = (float)pdf[i] * 255 / pdfmax;
	}

	for (int i = 0; i < 256 * 256; i++)
		datadst[i] = 0;

	for (int x = 0; x < 256; x++)
	{
		for (int y = (256 - 1); y >= (256 - 1) - pdfnorm[x]; y--)
		{
			datadst[y * 256 + x] = 255;
		}
	}

	return 1;
}

int equalize(unsigned char *datasrc, unsigned char *datadst, int bytesperline, int channels, int offset, int width, int height)
{

	if (channels != 1 && channels != 3)
		return 0;

	if (channels == 1)
	{
		int ni[256] = {0};

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int pos = y * bytesperline + x * channels;
				ni[datasrc[pos]]++;
			}
		}

		float pdf[256] = {0};
		int n = width * height;

		for (int i = 0; i < 256; i++)
		{
			pdf[i] = (float)ni[i] / (float)n;
		}

		float pdfmax = pdf[0];
		for (int i = 1; i < 256; i++)
		{
			if (pdf[i] > pdfmax)
			{
				pdfmax = pdf[i];
			}
		}
		float cdf[256];
		float sum = 0;
		for (int i = 0; i < 256; i++)
		{
			sum += pdf[i];
			cdf[i] = sum;
		}

		float cdfmin = -1;
		for (int i = 0; i < 256 && cdfmin == -1; i++)
		{
			cdfmin = cdf[i];
		}

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				long int pos = y * bytesperline + x * channels;
				datadst[pos] = ((cdf[datasrc[pos]] - cdfmin) / (1 - cdfmin)) * 255;
			}
		}
	}
	else if (channels == 3)
	{
		for (int c = 0; c < channels; c++)
		{
			int ni[256] = {0};

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = y * bytesperline + x * channels + c;
					ni[datasrc[pos]]++;
				}
			}

			float pdf[256] = {0};
			int n = width * height;

			for (int i = 0; i < 256; i++)
			{
				pdf[i] = (float)ni[i] / (float)n;
			}

			float pdfmax = pdf[0];
			for (int i = 1; i < 256; i++)
			{
				if (pdf[i] > pdfmax)
				{
					pdfmax = pdf[i];
				}
			}
			float cdf[256];
			float sum = 0;
			for (int i = 0; i < 256; i++)
			{
				sum += pdf[i];
				cdf[i] = sum;
			}

			float cdfmin = -1;
			for (int i = 0; i < 256 && cdfmin == -1; i++)
			{
				cdfmin = cdf[i];
			}

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					long int pos = y * bytesperline + x * channels + c;
					datadst[pos] = ((cdf[datasrc[pos]] - cdfmin) / (1 - cdfmin)) * 255;
				}
			}
		}
	}

	return 1;
}

int vc_gray_histogram_equalization(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL) || (src->width != dst->width) || src->height != dst->height)
		return 0;
	if (channels != 1)
		return 0;
	return equalize(datasrc, datadst, bytesperline, channels, 0, width, height);
}

int vc_rgb_histogram_equalization(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	long int pos;

	if (channels != 3)
		return 0;

	int res = equalize(datasrc, datadst, bytesperline, channels, 0, width, height);

	return res;
}

int vc_bgr_to_rgb(IVC* src, IVC* dst)
{
	unsigned char* data = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->width * src->channels;
	int channels = src->channels;
	int channels_dst = dst->channels;
	int x, y, i, j;
	long int pos;

	//Verificacao de erros
	if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
	if (channels != 3 || channels_dst != 3) return 0;
	//Verifica se existe blobs

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * dst->bytesperline + x * dst->channels;
			datadst[pos] = data[pos + 2];
			datadst[pos + 1] = data[pos + 1];
			datadst[pos + 2] = data[pos];
		}
	}

	return 1;
}