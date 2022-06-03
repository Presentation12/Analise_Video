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

//TESTES FEITOS
/*
	//printf("%d\n", video.nframe);

	//Parte segmentacao da mesa e laranjas
		//vc_hsv_segmentation(hsv, hsv_s, 15, 30, 40, 80, 20, 100);
		//vc_hsv_segmentation_fruta(hsv, hsv_s2, 0, 12, 70, 100, 55, 100);
		//vc_hsv_segmentation(hsv, hsv_s3, 15, 30, 50, 95, 25, 90);
		//vc_hsv_segmentation(hsv, hsv_s, 27, 45, 0, 90, 70, 90);

		//vc_binary_open(hsv_s, bc, 5);
		/*vc_binary_open(bc, bc_1, 5);
		vc_binary_open(bc_1, bc_2, 5);
		/*vc_binary_close(hsv_s2, bc2, 3);
	
	//Tentativas de algoritmos para contar laranjas
	std::list<OVC> blobshist;

	/*if (blobshist.size() == 0) {
		blobshist.push_front(blob2[j]);
		laranjas_counter++;
	}

	for (OVC obj : blobshist)
	{
		//Incomplete
		//if ((float)obj.area / (float)blob2[j].area < 0.955 ||
		//(float)obj.area / (float)blob2[j].area > 1.035)
		if(!(obj.xc > blob[i].xc - blob[i].x && obj.xc < blob[i].xc + blob[i].x &&
			obj.yc > blob[i].yc - blob[i].y && obj.yc < blob[i].yc + blob[i].y))
		{
			blobshist.push_front(blob[i]);
			laranjas_counter++;
		}
	}

	//if ((blob[i].yc < (video.height / 3) + 2 && blob[i].yc > (video.height / 3) - 2)) laranjas_counter++;

	//Tentativas de desenhar retangulo delimitador
	//cv::Rect rect(blob[i].xc, blob[i].yc - blob[i].y, blob[i].width, blob[i].height);
	//cv::rectangle(frame, rect, cv::Scalar(255, 0, 0), 0);

*/

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
	int blobs;
	int blobs2;
	int pontuacao;
	long int pos;
	int valueMax;
	int valueMin;
	int saturationMax;
	int saturationMin;
	int diametroMax = 0;
	int diametroMin = INT_MAX;
	int calibreMax1 = 0;
	int calibreMin1 = INT_MAX;
	int calibreMax2 = 0;
	int calibreMin2 = INT_MAX;
	int calibreMax3 = 0;
	int calibreMin3 = INT_MAX;
	int hasGreen;
	std::string calibre = "Invalido";
	std::string classificacao = "Invalido";
	std::string homogeneidade = "Aprovado";

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
		blobs = 0;
		blobs2 = 0;
		pontuacao = 0;

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		memcpy(src->data, frame.data, video.width * video.height * 3);

		//Passar BGR (formato vídeo) para RGB
		vc_bgr_to_rgb(src, rgb);

		//Passar RGB para HSV
		vc_rgb_to_hsv(rgb, hsv);

		//Fazer HSV segmentation de 2 prespetivas
		
		//Segmentation Mesa (background)
		vc_hsv_segmentation(hsv, hsv_s, 0, 360, 0, 40, 13, 100);
		//Como a mesa ficou a branco e fruta a preto, de forma a favorecer as comparações, foi feita uma inversão de tonalidades
		vc_invert_binary(hsv_s);

		//Segmentation Laranjas
		vc_hsv_segmentation(hsv, hsv_s2, 3, 35, 40, 100, 0, 95);

		//Operações morfológicas para melhoramento de regiões segmentadas
		vc_binary_close(hsv_s, bc, 7);
		vc_binary_close(bc, bc_1, 7);

		vc_binary_close(hsv_s2, bc2, 7);
		vc_binary_close(bc2, bc2_1, 7);
		vc_binary_open(bc2_1, bc2_2, 7);
		vc_binary_open(bc2_2, bc2_3, 7);
		vc_binary_open(bc2_3, bc2_4, 7);

		//Fazer etiquetagem e guardar informações dos blobs detetados em cada segmentação
		OVC* blob = vc_binary_blob_labelling(bc_1, hsv_blobed, &blobs);
		OVC* blob2 = vc_binary_blob_labelling(bc2_1, hsv_blobed2, &blobs2);
		vc_binary_blob_info(hsv_blobed, blob, blobs);
		vc_binary_blob_info(hsv_blobed2, blob2, blobs2);


		memcpy(frame.data, src->data, video.width * video.height * 3);

		//Parte de printagens em cada frame
		
		//Correr laranjas
		for (int j = 0; j < blobs2; j++) {
			//Correr background (segmetation mesa)
			for (int i = 0; i < blobs; i++) {
				if (//Se o píxel do blob da segmentação da fruta esteja dentro do blob da segmentação da mesa invertida ou vice-versa
					// (eixo do x)
					(blob2[j].x >= blob[i].x && blob2[j].x <= (blob[i].width + blob[i].x) || blob[i].x >= blob2[j].x && blob[i].x <= (blob2[j].width + blob2[j].x))

					//Se o píxel do blob da segmentação da fruta esteja dentro do blob da segmentação da mesa invertida ou vice-versa
					// (eixo do y)
					&& (blob2[j].y >= blob[i].y && blob2[j].y <= (blob[i].height + blob[i].y) || blob[i].y >= blob2[j].y && blob[i].y <= (blob2[j].height + blob2[j].y))
					
					//Caso a diferença de área dos 2 blobs seja de +15% ou -15% (valor obtido por testes)
					&& (float)blob2[j].area / (float)blob[i].area >= 0.85 && (float)blob2[j].area / (float)blob[i].area <= 1.15

					//Caso a imagem esteja dentro da frame
					&& blob2[j].x > 1 && (blob2[j].x + blob2[j].width) < src->width-1 && blob2[j].y > 1 && (blob2[j].y + blob2[j].height) < src->height - 1 )
				{
					//Incrementar número de laranjas totais
					if (blob[i].yc >= ((video.height + 8) / 2) && blob[i].yc < ((video.height + 26) / 2)) laranjas_counter++;

					//Avaliar Deformação
					if ((float)blob[i].width / blob[i].height >= 0.95 && (float)blob[i].width / blob[i].height <= 1.05) pontuacao++;
					else if ((float)blob[i].width / blob[i].height >= 0.90 && (float)blob[i].width / blob[i].height <= 1.10) pontuacao += 2;

					//Avaliar coloração
					valueMax = 0;
					valueMin = 100;
					saturationMax = 0;
					saturationMin = 100;
					hasGreen = 0;
					
					//Percorrer um blob pixel a pixel
					for (int h = blob[i].y; h < blob[i].y + blob[i].height; h++) {
						for (int g = blob[i].x; g < blob[i].x + blob[i].width; g++) {
							pos = h * hsv->bytesperline + g * hsv->channels;
							
							//40, 50
							//70, 85
							//20, 30
							if ((hsv->data[pos] >= 36 && hsv->data[pos] <= 65) &&
								(hsv->data[pos + 1] >= 65 && hsv->data[pos + 1] <= 85) &&
								(hsv->data[pos + 2] >= 20 && hsv->data[pos + 2] <= 40)
								&& hasGreen == 0) {
								hasGreen = 1;
							}

							//3, 35
							//40, 100
							//0, 95
							if ((hsv->data[pos] >= 3 && hsv->data[pos] <= 35) && (hsv->data[pos + 1] >= 40 && hsv->data[pos + 1] <= 100) && (hsv->data[pos + 2] >= 0 && hsv->data[pos + 2] <= 95)) {
								if (hsv->data[pos + 1] < saturationMin) saturationMin = hsv->data[pos + 1];
								else if (hsv->data[pos + 1] > saturationMax) saturationMax = hsv->data[pos + 1];

								if (hsv->data[pos + 2] < valueMin) valueMin = hsv->data[pos + 2];
								else if (hsv->data[pos + 2] > valueMax) valueMax = hsv->data[pos + 2];
							}
						}
					}

					if(saturationMax > 55 || valueMax > 55) pontuacao += 3;
					else if ((float)saturationMin / saturationMax >= 0.95 && (float)saturationMin / saturationMax <= 1.05) pontuacao++;
					else if ((float)saturationMin / saturationMax >= 0.90 && (float)saturationMin / saturationMax <= 1.10) pontuacao+=2;

					//Atribuição de classificação
					classificacao = "Extra";

					if (pontuacao == 1) classificacao = "I";
					else if (pontuacao >= 5) classificacao = "II";
					
					if (classificacao == "II" && hasGreen == 0) classificacao = "III";

					//Diferença entre diametros
					if ((blob[i].xc - blob[i].x) * 2 > diametroMax) diametroMax = (blob[i].xc - blob[i].x) * 2;
					else if ((blob[i].xc - blob[i].x) * 2 < diametroMin) diametroMin = (blob[i].xc - blob[i].x) * 2;

					//Desenho de área delimitadora e centro de gravidade
					cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), 1, cv::Scalar(0, 255, 0, 0), 5);
					cv::circle(frame, cv::Point(blob[i].x, blob[i].y), 1, cv::Scalar(0, 255, 0, 0), 5);
					cv::circle(frame, cv::Point(blob[i].x + blob[i].width,blob[i].y + blob[i].height), 1, cv::Scalar(0, 255, 0, 0), 5);
					cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), blob[i].xc - blob[i].x, cv::Scalar(0, 0, 255, 0), 0);

					cv::Rect rect(blob[i].xc - blob[i].width / 2, blob[i].yc - blob[i].height / 2, blob[i].width, blob[i].height);
					cv::rectangle(frame, rect, cv::Scalar(255, 0, 0), 0);

					//Zona de texto informativo sobre as laranjas
					str = std::string("Area: ").append(std::to_string((int)blob[i].area * 55 / 280)).append(" mm");
					cv::putText(frame, str, cv::Point(blob[i].xc - 150, blob[i].yc + 60), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
					str = std::string("Perimetro: ").append(std::to_string((int)blob[i].perimeter * 55 / 280)).append(" mm");
					cv::putText(frame, str, cv::Point(blob[i].xc - 150, blob[i].yc + 90), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
					str = std::string("Classificacao: ").append(classificacao);
					cv::putText(frame, str, cv::Point(blob[i].xc - 150, blob[i].yc + 120), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));

					if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 53 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 60) calibre = "13";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 56 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 63) calibre = "12";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 58 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 66) calibre = "11";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 60 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 68) calibre = "10";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 62 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 70) calibre = "9";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 64 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 73) calibre = "8";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 67 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 76) calibre = "7";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 70 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 80) calibre = "6";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 73 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 84) calibre = "5";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 77 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 88) calibre = "4";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 81 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 92) calibre = "3";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 84 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 96) calibre = "2";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 87 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 100) calibre = "1";
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 100) calibre = "0";

					//Homogeneidade no calibre
					if (calibre == "0" || calibre == "1" || calibre == "2") {
						if ((float)(((blob[i].xc - blob[i].x) * 2 * 55) / 280) > calibreMax1) calibreMax1 = (int)(((blob[i].xc - blob[i].x) * 2 * 55) / 280);
						else if ((float)(((blob[i].xc - blob[i].x) * 2 * 55) / 280) < calibreMin1) calibreMin1 = (int)(((blob[i].xc - blob[i].x) * 2 * 55) / 280);
					}
					else if (calibre == "3" || calibre == "4" || calibre == "5" || calibre == "6") {
						if ((float)(((blob[i].xc - blob[i].x) * 2 * 55) / 280) > calibreMax2) calibreMax2 = (int)(((blob[i].xc - blob[i].x) * 2 * 55) / 280);
						else if ((float)(((blob[i].xc - blob[i].x) * 2 * 55) / 280) < calibreMin2) calibreMin2 = (int)(((blob[i].xc - blob[i].x) * 2 * 55) / 280);
					}
					else if (calibre == "7" || calibre == "8" || calibre == "9" || calibre == "10" || calibre == "11" || calibre == "12" || calibre == "13") {
						if ((float)(((blob[i].xc - blob[i].x) * 2 * 55) / 280) > calibreMax3) calibreMax3 = (int)(((blob[i].xc - blob[i].x) * 2 * 55) / 280);
						else if ((float)(((blob[i].xc - blob[i].x) * 2 * 55) / 280) < calibreMin3) calibreMin3 = (int)(((blob[i].xc - blob[i].x) * 2 * 55) / 280);
					}

					if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 78) calibre.append(" - XXX");
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 67 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 78) calibre.append(" - XX");
					else if ((float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 >= 63 && (float)((blob[i].xc - blob[i].x) * 2) * 55 / 280 < 74) calibre.append(" - X");

					str = std::string("Calibre: ").append(calibre);
					cv::putText(frame, str, cv::Point(blob[i].xc - 150, blob[i].yc + 150), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0));
				}
			}
		}

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
		str = std::string("Laranjas: ").append(std::to_string(laranjas_counter));
		cv::putText(frame, str, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 125, 255, 0), 1);
		str = std::string("Diferenca de Diametro: ").append(std::to_string(diametroMax - diametroMin >= 0 ? (float)(((diametroMax - diametroMin) * 55) / 280) : 0)).append(" mm");
		cv::putText(frame, str, cv::Point(20, 150), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 150), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255, 0), 1);

		if (calibreMax1 - calibreMin1 > 0) {
			if (calibreMax1 - calibreMin1 > 11) {
				homogeneidade = "Reprovado (Ultimo Visto: Calibres 0 - 2)";
			}
		}
		if (calibreMax2 - calibreMin2 > 0) {
			if (calibreMax2 - calibreMin2 > 9) {
				homogeneidade = "Reprovado (Ultimo Visto: Calibres 3 - 6)";
			}
		}
		if (calibreMax3- calibreMin3 > 0) {
			if (calibreMax3 - calibreMin3 > 7) {
				homogeneidade = "Reprovado (Ultimo Visto: Calibres 7 - 13)";
			}
		}

		str = std::string("Homogeneidade de calibre: ").append(homogeneidade);
		cv::putText(frame, str, cv::Point(20, 175), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 175), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255, 0), 1);

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