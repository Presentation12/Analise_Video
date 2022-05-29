#include <iostream>
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
		/* Leitura de uma frame do v�deo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inser��o texto na frame */
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
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
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		
		//printf("%d\n", video.nframe);
		// Fa�a o seu c�digo aqui...
		//CRIAR IVCS NECESSARIOS

		IVC* src = vc_image_new(video.width, video.height, 3, 255);
		IVC* rgb = vc_image_new(video.width, video.height, 3, 255);
		IVC* hsv = vc_image_new(video.width, video.height, 3, 255);
		IVC* hsv_s = vc_image_new(video.width, video.height, 1, 255);
		IVC* bc = vc_image_new(video.width, video.height, 1, 255);
		IVC* bc2 = vc_image_new(video.width, video.height, 1, 255);
		int blobs = 0;
		IVC* hsv_blobed = vc_image_new(video.width, video.height, 1, 255);
		int raio;

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
		//vc_hsv_segmentation(hsv, hsv_s, 15, 30, 50, 100, 25, 90);
		//vc_hsv_segmentation(hsv, hsv_s, 3, 30, 40, 100, 0, 90);
		//vc_hsv_segmentation(hsv, hsv_s, 27, 45, 0, 90, 70, 110);

		vc_hsv_segmentation(hsv, hsv_s, 15, 30, 50, 100, 25, 90);
		vc_binary_open(hsv_s, bc, 5);
		vc_binary_erode(bc, bc2, 5);

		//FAZER BINARY BLOB LABELLING E TER A INFO
		OVC* blob = vc_binary_blob_labelling(hsv_s, hsv_blobed, &blobs);
		vc_binary_blob_info(hsv_blobed, blob, blobs);

		memcpy(frame.data, src->data, video.width * video.height * 3);

		//PARTE PRINTAGENS
		/*
			PSEUDOCODIGO
			SE BLOBS EXISTIR
				RAIO = 0
				FOR(I < NUMERO DE BLOBS){
					SE BLOB[I].area > 4500 (NAO SEI PQ ESTE VALOR)
						RAIO = BLOBS[I].XC - BLOBS[I].X

						CV::CIRCLE(FRUTA)
						CV::CIRCLE(CENTRO DE MASSA)
						PRINTAR AREA AO LADO DA FRUTA
						PRINTAR PERIMETRO AO LADO DA FRUTA
				}
		*/

		for (int i = 0; i < blobs; i++) {
			if (blob[i].area > 20000)
			{
				raio = blob[i].xc - blob[i].x;

				cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), 1, cv::Scalar(255, 50, 50, 0), 4, 4, 0);
				cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), raio, cv::Scalar(0, 255, 0, 0), 4, 2, 0);
				str = std::string("Area:").append(std::to_string(blob[i].area));
				cv::putText(frame, str, cv::Point(blob[i].xc - 20, blob[i].yc - 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
				str = std::string("Perimetro:").append(std::to_string(blob[i].perimeter));
				cv::putText(frame, str, cv::Point(blob[i].xc, blob[i].yc), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
			}
		}

		// Liberta a mem�ria da imagem IVC que havia sido criada
		vc_image_free(src);
		vc_image_free(rgb);
		vc_image_free(hsv);
		vc_image_free(bc);
		vc_image_free(bc2);
		vc_image_free(hsv_s);
		vc_image_free(hsv_blobed);
		// +++++++++++++++++++++++++

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
	}

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}