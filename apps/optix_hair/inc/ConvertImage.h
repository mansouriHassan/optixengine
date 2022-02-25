#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

#ifndef CONVERTIMAGE_H_
#define CONVERTIMAGE_H_

#define CV_IMWRITE_JPEG_QUALITY 1
#define CV_IMWRITE_PNG_COMPRESSION 16
#define CV_IMWRITE_PXM_BINARY 32
  
/**
 * Classe que converte as imagens para base64 e virse e versa
 */
class ImagemConverter {
public:
	/**
	 * Constritor default da classe
	 */
	ImagemConverter();
	
	/**
	 * Método que converte uma imagem base64 em um cv::Mat
	 * @param imageBase64, imagem em base64
	 * @return imagem em cv::Mat
	 */
	cv::Mat str2mat(const string& imageBase64);
	
	/**
	 * Método que converte uma cv::Mat numa imagem em base64
	 * @param img, imagem em cv::Mat
	 * @return imagem em base64
	 */
	string mat2str(const Mat& img);
	string img2str(const string& filename);

	virtual ~ImagemConverter();

private:
	std::string base64_encode(uchar const* bytesToEncode, unsigned int inLen);

	std::string base64_decode(std::string const& encodedString);

};

#endif /* CONVERTIMAGE_H_ */