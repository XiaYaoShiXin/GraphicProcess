#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <fstream>

#define SAMPLE_NAME "sample.xml"
#define CSV_PATH "att_faces\\1.csv"
#define FACE_SIZE_X 92
#define FACE_SIZE_Y 112
#define SCREENSHOOT_PATH "screenshoots/"
#define MODEL_NAME "model_fisher.xml"

using namespace cv;
using namespace std;


void GetFaceRect(Mat frame,vector<Rect> &rects){
	Mat gray;
	cvtColor(frame,gray,CV_RGB2GRAY);
	CascadeClassifier faceCascade;
	faceCascade.load(SAMPLE_NAME);
	
	faceCascade.detectMultiScale(gray,rects,1.1,3,0);
}
void GetFace(Mat frame,vector<Mat> &faces){
	vector<Rect> rects;
	GetFaceRect(frame,rects);
	
	for(int i=0;i<rects.size();i++){
		Point p1(rects[i].x,rects[i].y),p2(rects[i].x+rects[i].width,rects[i].y+rects[i].height);
		rectangle(frame,p1,p2,Scalar(255,255,255));

		Mat face = frame(rects[i]);
		cvtColor(face,face,CV_RGB2GRAY);
		resize(face,face,Size(FACE_SIZE_X,FACE_SIZE_Y));
		faces.push_back(face);
	}
	imshow("�沿ʶ����",frame);
}

void ScreenShoot(Mat &frame,char order,void* data){
	if(order==13){
		vector<Mat> faces;
		GetFace(frame,faces);
		int count = faces.size();
		for(int i=0;i<count;i++){
			time_t t = time(0);
			tm* tm;
			tm = localtime(&t);
			char timestamp[24];
			strftime(timestamp,sizeof(timestamp),"%Y-%m-%d-%H-%M-%S",tm);

			string filename(timestamp);
			char suffix[3] = {'-'};
			if(count>1){
				itoa(i,&suffix[1],10);
				filename.append(suffix);
			}

			string filepath(SCREENSHOOT_PATH);
			filepath.append(filename);
			filepath.append(".pgm");
			imwrite(filepath,faces[i]);
		}
	}
}

void DetectFace(Mat &frame,char order,void* data){
	if(order==13){
		vector<Mat> faces;
		GetFace(frame,faces);
		
		Ptr<FaceRecognizer> *modelFisher = (Ptr<FaceRecognizer>*)data;

		for(int i=0;i<faces.size();i++){
			int predictFisher = (*modelFisher)->predict(faces[i]);
			printf("ʶ������%d\n",predictFisher);
		}
	}
}

void OpenCamera(VideoCapture &cap,char* window,void (*eachFrame)(Mat &frame,char order,void* data),void* data,char* tip="",int escCode=27,int rate=20){
	cap.open(0);
	if(!cap.isOpened()){
		printf("�޷�������ͷ\n");
		return;
	}

	printf("%s",tip);
	Mat frame;
	
	char order = 0;
	while(order!=escCode){
		cap>>frame;
		imshow(window,frame);

		if(kbhit()){
			order = getch();
		}
		eachFrame(frame,order,data);
		order = 0;
		waitKey (rate);
	}
}

void CloseCamera(VideoCapture cap,char* window){
	cap.release();
	destroyWindow(window);
}

void MissionA(){
	VideoCapture cap;
	OpenCamera(cap,"����ͷ����",ScreenShoot,NULL,"Enter����ȡ���� Esc�����ز˵�\n");	
	CloseCamera(cap,"����ͷ����");
	fflush(stdin);
}

void readCSV(const char* csv,vector<Mat> &faces,vector<int> &labels){
	std::ifstream file(csv,ifstream::in);
	if(!file){
		CV_Error(CV_StsBadArg,"CSV PATH Error.");
	}
	string line,path,label;
	while(getline(file,line)){
		stringstream liness(line);
		getline(liness,path,',');
		getline(liness,label);
		if(!path.empty() && !label.empty()){
			faces.push_back(imread(path,0));
			labels.push_back(atoi(label.c_str()));
		}
	}
}

void MissionB(){
	vector<Mat> faces = vector<Mat>();
	vector<int> labels = vector<int>();
	printf("��ʼ��ȡcsv\n");
	readCSV(CSV_PATH,faces,labels);
	printf("��ȡ��������ʼѵ��\n");
	Ptr<FaceRecognizer> faceRecognizer;
	
	faceRecognizer = createFisherFaceRecognizer();
	faceRecognizer->train(faces,labels);
	faceRecognizer->save(MODEL_NAME);
	printf("ѵ�����\n");


	fflush(stdin);
}

void MissionC(){
	VideoCapture cap;
	
	Ptr<FaceRecognizer> modelFisher = createFisherFaceRecognizer();
	modelFisher->load(MODEL_NAME);

	OpenCamera(cap,"����ͷ����",DetectFace,&modelFisher,"Enter��ʶ����� Esc�����ز˵�\n");	
	CloseCamera(cap,"����ͷ����");
	fflush(stdin);
}

 void main(){
	
	 char order;
	 while(1){
		printf("����A/B/Cѡ��ִ������\n");
		scanf("%c",&order);
		if(order=='A'||order=='a'){
			MissionA();
		} else if(order=='B'||order=='b'){
			MissionB();
		} else if(order=='C'||order=='c'){
			MissionC();
		} else {
			printf("������Ϸ�ָ��\n");
		}
	 }
	
 } 




