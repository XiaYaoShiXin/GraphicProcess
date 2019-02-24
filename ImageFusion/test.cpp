 #include<iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include <string.h>
      
using namespace cv;  



 int main(){  


		//读入图像与模版
		Mat apple = imread("clocka.bmp",IMREAD_ANYCOLOR);//   barbara1.bmp   clocka.bmp   ci2.bmp
		apple.convertTo(apple, CV_32F, 1.0 / 255.0);
		Mat orange = imread("clockb.bmp",IMREAD_ANYCOLOR);//  barbara2.bmp   clockb.bmp   ci1.bmp
	    orange.convertTo(orange, CV_32F, 1.0 / 255.0);
		Mat mask = imread("clock_mask.png");//  barbara_mask.png        clock_mask.png    ci_mask.png
		mask.convertTo(mask, CV_8UC1);
		//定义金字塔
		int level = 3;
		vector <Mat> apple_LaplacianPyramid,apple_GaussianPyramid;
		vector <Mat> orange_LaplacianPyramid, orange_GaussianPyramid;
		vector <Mat> mask_LaplacianPyramid, mask_GaussianPyramid;

		//生成橘子和苹果的拉普拉斯金字塔
		Mat maskTemp,appleTemp,orangeTemp;
		apple.copyTo(appleTemp);
		orange.copyTo(orangeTemp);
		mask.copyTo(maskTemp);
		apple_LaplacianPyramid.push_back(apple);
		orange_LaplacianPyramid.push_back(orange);
		mask_LaplacianPyramid.push_back(mask);
		for (int i = 0; i<level; i++) {
			pyrUp(appleTemp, appleTemp);
			pyrUp(orangeTemp, orangeTemp);
			pyrUp(maskTemp, maskTemp);
			apple_LaplacianPyramid.push_back(appleTemp);
			orange_LaplacianPyramid.push_back(orangeTemp);
			mask_LaplacianPyramid.push_back(maskTemp);
  		}
		//生成橘子和苹果的高斯金字塔
		apple.copyTo(appleTemp);
		orange.copyTo(orangeTemp);
		mask.copyTo(maskTemp);
		apple_GaussianPyramid.push_back(apple);
		orange_GaussianPyramid.push_back(orange);
		mask_GaussianPyramid.push_back(mask);
		for (int i = 0; i<level; i++) {
			pyrDown(appleTemp, appleTemp);
			pyrDown(orangeTemp, orangeTemp);
			pyrDown(maskTemp, maskTemp);
			apple_GaussianPyramid.push_back(appleTemp);
			orange_GaussianPyramid.push_back(orangeTemp);
			mask_GaussianPyramid.push_back(maskTemp);
  		}

		//利用模板乘以两边的图形进行融合
		vector <Mat> Fusion_Laplacian,Fusion_Gaussian;
		Mat fusionTemp;
		for (int i = 0; i<level+1; i++) {
			orange_LaplacianPyramid[i].copyTo(fusionTemp);
			apple_LaplacianPyramid[i].copyTo(fusionTemp,mask_LaplacianPyramid[i]);
			Fusion_Laplacian.push_back(fusionTemp);
  		}
		for (int i = 0; i<level+1; i++) {
			orange_GaussianPyramid[i].copyTo(fusionTemp);
			apple_GaussianPyramid[i].copyTo(fusionTemp,mask_GaussianPyramid[i]);
			Fusion_Gaussian.push_back(fusionTemp);
  		}
		
		//金字塔重建与恢复
		Mat orapple_Laplacian,orapple_Gaussian;
		for(int i=level-1;i>=0;i--){
			pyrDown(Fusion_Laplacian.at(i+1),orapple_Laplacian,Fusion_Laplacian.at(i).size());
			pyrUp(Fusion_Gaussian.at(i+1),orapple_Gaussian,Fusion_Gaussian.at(i).size());
			Fusion_Laplacian.at(i) += orapple_Laplacian/(level+1);
			Fusion_Gaussian.at(i) += orapple_Gaussian/(level+1);
		}
		Fusion_Laplacian.at(0).copyTo(orapple_Laplacian);
		Fusion_Gaussian.at(0).copyTo(orapple_Gaussian);
		
		Mat orapple = (orapple_Laplacian + orapple_Gaussian)/2;

		imshow("原始图像1", apple);
		imshow("原始图像2", orange);
		imshow("融合图像", orapple);
		

        waitKey ( 100000 ); //用system("pause");会出图像显示不出的bug，推测图像读取是异步操作所致

    } 

