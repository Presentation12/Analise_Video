#include <iostream>
#include <list>
#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

extern "C" {
#include "vc.h"
}


int main(void) {
	// V�deo
	char videofile[20] = "video.avi";
	cv::VideoCapture capture;
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;
	// Outros
	std::string str;
	int key = 0;
	IVC* src = NULL;
	IVC* rgb = NULL;
	IVC* hsv = NULL;
	IVC* hsv_s = NULL;
	IVC* hsv_s2 = NULL;
	IVC* hsv_s3 = NULL;
	IVC* bc = NULL;
	IVC* bc_1 = NULL;
	IVC* bc2 = NULL;
	IVC* bc2_1 = NULL;
	IVC* bc2_2 = NULL;
	IVC* bc2_3 = NULL;
	IVC* bc2_4 = NULL;
	IVC* bc2_5 = NULL;
	IVC* hsv_blobed = NULL;
	IVC* hsv_blobed2 = NULL;
	int laranjas_counter = 0;
	std::list<OVC> blobshist;

	/* Leitura de v�deo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video.avi dever� estar localizado no mesmo direct�rio que o ficheiro de c�digo fonte.
	*/
	capture.open(videofile);

	/* Em alternativa, abrir captura de v�deo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);

	/* Verifica se foi poss�vel abrir o ficheiro de v�deo */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de v�deo!\n";
		return 1;
	}

	/* N�mero total de frames no v�deo */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do v�deo */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolu��o do v�deo */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	/* Cria uma janela para exibir o v�deo */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	cv::Mat frame;
	while (key != 'q') {
		int counter;
		/* Leitura de uma frame do v�deo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inser��o texto na frame */
		/*str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);*/

		str = std::string("Laranjas: ").append(std::to_string(laranjas_counter));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		
		//printf("%d\n", video.nframe);
		// Fa�a o seu c�digo aqui...
		//CRIAR IVCS NECESSARIOS
		src = vc_image_new(video.width, video.height, 3, 255);
		rgb = vc_image_new(video.width, video.height, 3, 255);
		hsv = vc_image_new(video.width, video.height, 3, 255);
		hsv_s = vc_image_new(video.width, video.height, 1, 255);
		hsv_s2 = vc_image_new(video.width, video.height, 1, 255);
		hsv_s3 = vc_image_new(video.width, video.height, 1, 255);
		bc = vc_image_new(video.width, video.height, 1, 255);
		bc_1 = vc_image_new(video.width, video.height, 1, 255);
		bc2 = vc_image_new(video.width, video.height, 1, 255);
		bc2_1 = vc_image_new(video.width, video.height, 1, 255);
		bc2_2 = vc_image_new(video.width, video.height, 1, 255);
		bc2_3 = vc_image_new(video.width, video.height, 1, 255);
		bc2_4 = vc_image_new(video.width, video.height, 1, 255);
		bc2_5 = vc_image_new(video.width, video.height, 1, 255);
		hsv_blobed = vc_image_new(video.width, video.height, 1, 255);
		hsv_blobed2 = vc_image_new(video.width, video.height, 1, 255);
		int blobs = 0;
		int blobs2 = 0;

		//CRIAR OVC PARA BLOBS
		//NUM BLOBS
		// Cria uma nova imagem IVC
		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		memcpy(src->data, frame.data, video.width * video.height * 3);

		//PASSAR BGR (FORMATO VIDEO) PARA RGB
		vc_bgr_to_rgb(src, rgb);

		//PASSAR RGB -> HSV (???)
		vc_rgb_to_hsv(rgb, hsv);

		//FAZER HSV SEGMENTATION
		
		//MESA
		// TIRAR COMMENT
		vc_hsv_segmentation(hsv, hsv_s, 0, 360, 0, 40, 13, 100);
		// TIRAR COMMENT
		vc_invert_binary(hsv_s);

		//FRUTA
		//vc_hsv_segmentation(hsv, hsv_s, 15, 30, 40, 100, 0, 100);
		vc_hsv_segmentation(hsv, hsv_s2, 3, 35, 40, 100, 0, 95);
		//vc_hsv_segmentation_fruta(hsv, hsv_s2, 0, 12, 80, 100, 60, 100);
		//vc_hsv_segmentation(hsv, hsv_s3, 15, 30, 50, 100, 25, 90);
		//vc_hsv_segmentation(hsv, hsv_s, 27, 45, 0, 90, 70, 110);
		 
		//vc_binary_open(hsv_s, bc, 5);
		/*vc_binary_open(hsv_s, bc_1, 5);
		vc_binary_open(hsv_s, bc_2, 5);*/

		//DEFINITIVO NO BACKGROUND
		vc_binary_close(hsv_s, bc, 7);
		vc_binary_close(bc, bc_1, 7);

		/*vc_binary_close(hsv_s2, bc2, 3);*/

		//DEFINITIVO NA FRUTA
		vc_binary_close(hsv_s2, bc2, 7);
		vc_binary_close(bc2, bc2_1, 7);
		vc_binary_open(bc2_1, bc2_2, 7);
		vc_binary_open(bc2_2, bc2_3, 7);
		vc_binary_open(bc2_3, bc2_4, 7);
		//vc_gray_edge_prewitt(bc2_4, bc2_5, 0.2);

		//FAZER BINARY BLOB LABELLING E TER A INFO
		// TIRAR COMMENT
		OVC* blob = vc_binary_blob_labelling(bc_1, hsv_blobed, &blobs);
		OVC* blob2 = vc_binary_blob_labelling(bc2_1, hsv_blobed2, &blobs2);
		vc_binary_blob_info(hsv_blobed, blob, blobs);
		vc_binary_blob_info(hsv_blobed2, blob2, blobs2);

		memcpy(frame.data, src->data, video.width * video.height * 1);

		//PARTE PRINTAGENS
		for (int j = 0; j < blobs2; j++) {
			for (int i = 0; i < blobs; i++) {
				if (
					//Caso o píxel final do blob da segmentação da fruta esteja dentro do blob da segmentação da mesa invertida
					// (eixo do x)
					(blob2[j].x >= blob[i].x && blob2[j].x <= (blob[i].x + blob[i].width) ||
					 blob[i].x >= blob2[j].x && blob[i].x <= (blob2[j].x + blob2[j].width))
					&&

					//Caso o píxel final do blob da segmentação da fruta esteja dentro do blob da segmentação da mesa invertida
					// (eixo do y)
					(blob2[j].y >= blob[i].y && blob2[j].y <= (blob[i].y + blob[i].height) ||
						blob[i].y >= blob2[j].y && blob[i].y <= (blob2[j].y + blob2[j].height))
					
					//Caso a diferença de área dos 2 blobs seja abaixo de 15% (valor obtido por testes)
					&& (float)blob2[j].area / (float)blob[i].area >= 0.85 && (float)blob2[j].area / (float)blob[i].area <= 1.15

					//Caso a imagem esteja dentro da frame
					&& blob2[j].x > 1 && (blob2[j].x + blob2[j].width) < src->width-1 && 
					blob2[j].y > 1 && (blob2[j].y + blob2[j].height) < src->height - 1 )
				{
					/*if (blobshist.size() == 0) {
						blobshist.push_front(blob2[j]);
						laranjas_counter++;
					}

					for (OVC obj : blobshist)
					{
						//if ((float)obj.area / (float)blob2[j].area < 0.955 ||
						//(float)obj.area / (float)blob2[j].area > 1.035)
						if(!(obj.xc > blob[i].xc - blob[i].x && obj.xc < blob[i].xc + blob[i].x &&
							obj.yc > blob[i].yc - blob[i].y && obj.yc < blob[i].yc + blob[i].y))
						{
							blobshist.push_front(blob[i]);
							laranjas_counter++;
						}
					}*/

					//if ((blob[i].yc < (video.height / 3) + 2 && blob[i].yc > (video.height / 3) - 2)) laranjas_counter++;
					if (blob[i].yc >= ((video.height + 8) / 2) && blob[i].yc < ((video.height + 26) / 2)) laranjas_counter++;

					cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), 1, cv::Scalar(255, 50, 50, 0), 4, 4, 0);
					cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), blob[i].xc - blob[i].x, cv::Scalar(0, 255, 0, 0), 4, 2, 0);
					str = std::string("Area:").append(std::to_string((float)blob[i].area * 55 / 280));
					cv::putText(frame, str, cv::Point(blob[i].xc - 20, blob[i].yc - 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
					str = std::string("Perimetro:").append(std::to_string((float)blob[i].perimeter * 55 / 280));
					cv::putText(frame, str, cv::Point(blob[i].xc, blob[i].yc), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
				}
			}
		}



		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
	}

	// Liberta a mem�ria da imagem IVC que havia sido criada
	vc_image_free(src);
	vc_image_free(rgb);
	vc_image_free(hsv);
	vc_image_free(bc);
	vc_image_free(bc_1);
	vc_image_free(bc2);
	vc_image_free(bc2_1);
	vc_image_free(bc2_2);
	vc_image_free(bc2_3);
	vc_image_free(bc2_4);
	vc_image_free(bc2_5);
	vc_image_free(hsv_s);
	vc_image_free(hsv_s2);
	vc_image_free(hsv_s3);
	vc_image_free(hsv_blobed);
	vc_image_free(hsv_blobed2);
	// +++++++++++++++++++++++++

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}