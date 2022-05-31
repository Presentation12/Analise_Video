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
	IVC* src = NULL;
	IVC* rgb = NULL;
	IVC* hsv = NULL;
	IVC* hsv_s = NULL;
	IVC* hsv_s2 = NULL;
	IVC* hsv_s3 = NULL;
	IVC* bc = NULL;
	IVC* bc2 = NULL;
	IVC* hsv_blobed = NULL;
	IVC* hsv_blobed2 = NULL;

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
		src = vc_image_new(video.width, video.height, 3, 255);
		rgb = vc_image_new(video.width, video.height, 3, 255);
		hsv = vc_image_new(video.width, video.height, 3, 255);
		hsv_s = vc_image_new(video.width, video.height, 1, 255);
		hsv_s2 = vc_image_new(video.width, video.height, 1, 255);
		hsv_s3 = vc_image_new(video.width, video.height, 1, 255);
		bc = vc_image_new(video.width, video.height, 1, 255);
		bc2 = vc_image_new(video.width, video.height, 1, 255);
		hsv_blobed = vc_image_new(video.width, video.height, 1, 255);
		hsv_blobed2 = vc_image_new(video.width, video.height, 1, 255);
		int raio;
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
		vc_hsv_segmentation(hsv, hsv_s, 0, 360, 0, 15, 20, 100);
		// TIRAR COMMENT
		vc_invert_binary(hsv_s);

		//FRUTA
		//vc_hsv_segmentation(hsv, hsv_s, 15, 30, 40, 100, 0, 100);
		vc_hsv_segmentation(hsv, hsv_s2, 3, 30, 40, 100, 0, 90);
		//vc_hsv_segmentation_fruta(hsv, hsv_s2, 0, 12, 80, 100, 60, 100);
		//vc_hsv_segmentation(hsv, hsv_s3, 15, 30, 50, 100, 25, 90);
		//vc_hsv_segmentation(hsv, hsv_s, 27, 45, 0, 90, 70, 110);
		 
		//vc_binary_open(hsv_s, bc, 5);
		vc_binary_dilate(hsv_s, bc, 11);
		vc_binary_close(hsv_s2, bc2, 3);
		vc_binary_close(hsv_s2, bc2, 3);
		vc_binary_close(hsv_s2, bc2, 3);
		vc_binary_close(hsv_s2, bc2, 3);
		//vc_binary_erode(hsv_s, bc2, 5);

		//FAZER BINARY BLOB LABELLING E TER A INFO
		// TIRAR COMMENT
		OVC* blob = vc_binary_blob_labelling(bc, hsv_blobed, &blobs);
		OVC* blob2 = vc_binary_blob_labelling(bc2, hsv_blobed2, &blobs2);
		vc_binary_blob_info(hsv_blobed, blob, blobs);
		vc_binary_blob_info(hsv_blobed2, blob2, blobs2);

		memcpy(frame.data, src->data, video.width * video.height * 3);

		//PARTE PRINTAGENS
		for (int i = 0; i < blobs; i++) {
			for (int j = 0; j < blobs2; j++) {
				if (blob[i].area - blob2[j].area < 20000)
				{
					raio = blob2[j].xc - blob2[j].x;

					cv::circle(frame, cv::Point(blob2[j].xc, blob2[j].yc), 1, cv::Scalar(255, 50, 50, 0), 4, 4, 0);
					cv::circle(frame, cv::Point(blob2[j].xc, blob2[j].yc), raio, cv::Scalar(0, 255, 0, 0), 4, 2, 0);
					str = std::string("Area:").append(std::to_string(blob2[j].area));
					cv::putText(frame, str, cv::Point(blob2[j].xc - 20, blob2[j].yc - 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
					str = std::string("Perimetro:").append(std::to_string(blob2[j].perimeter));
					cv::putText(frame, str, cv::Point(blob2[j].xc, blob2[j].yc), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
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
	vc_image_free(bc2);
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