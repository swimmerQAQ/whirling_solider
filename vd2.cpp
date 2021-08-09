#include <opencv2/opencv.hpp>
 using namespace cv;
 #include <iostream>
 using namespace std;
 #include <math.h>
 
 Mat imgHSV;//frame transfer imghsv
 Mat mask;//for inrang after imghsv
 Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;
 Mat medium;

 int hmin = 0;
 int hmax = 255;
 int smin = 231;
 int smax = 255;
 int vmin = 176;
 int vmax = 255;
 //vector<Point> kval(5);// store the K value
 
 
 void detect(Mat in_img,Mat origin)
 {
 	vector<vector<Point>> contours ;
	vector<Vec4i> hierarchy;//后前子父 -1 nonesense
	findContours( in_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> fourAngle(contours.size());
	vector<Point> centre(contours.size());
	//vector<vector<Point>> conPoly(contours.size());//画点
	int radius[contours.size()]={0};
	
	for(int i=0;i<contours.size();i++)
	{
		
		//四角点储存，与conpoly重复，但后面用了很多，没必要改
		/*
		想了想，还是改一下，艹
		放入每个检索到的，图形的点，先随便放四个
		由于必须是每个(第i个)封闭面内，要在循环内
		*/
		fourAngle[i] = contours[i];
		/*
		上面这一步非常重要，不初始化使用数组存在超范围数组调用，导致程序崩溃
		*/
		
		for(int j=0;j<contours[i].size();j++)//check all points
		{
			/*
				find_up_point
				the min yyyyyy
			*/
			if(fourAngle[i][0].y >= contours[i][j].y)
			{
				fourAngle[i][0].y = contours[i][j].y;
				fourAngle[i][0].x = contours[i][j].x;
			}
			/*
				find_down_point
				the max yyyyyy
			*/
			if(fourAngle[i][1].y < contours[i][j].y)
			{
				fourAngle[i][1].y = contours[i][j].y;
				fourAngle[i][1].x = contours[i][j].x;
			}
			/*
				find_left_point
				the min xxxxx
			*/
			if(fourAngle[i][2].x >= contours[i][j].x)
			{
				fourAngle[i][2].x = contours[i][j].x;
				fourAngle[i][2].y = contours[i][j].y;
			}
			/*
				find_right_point
				the max xxxxx
			*/
			if(fourAngle[i][3].x < contours[i][j].x)
			{
				fourAngle[i][3].x = contours[i][j].x;
				fourAngle[i][3].y = contours[i][j].y;
			}
			/*
			
			
			
			
			四个点在图形中的理想理论位置
			
				fourAngle[i][0]
					#
			 	   ##############
		  fourAngle[i][2]#################fourAngle[i][3]
			 	   ##############
					#
				foourAngle[i][1]
				
				
				
				
				
				
				
			*/
		}
		cout << " upper point "<<fourAngle[i][0] <<endl;
		
		
		//rectangle(img, conPoly[i][0], conPoly[i][1], Scalar(100, 255, 50),1);
		//line(origin, fourAngle[i][0], fourAngle[i][1], Scalar(0, 255, 0),2);
		//中点
		//非常重要，如果没有多维的radius和centre，始终只能测一个点，画一个圈，切记
		radius[i] = (fourAngle[i][1].y -fourAngle[i][0].y)/2;
		centre[i].y = (fourAngle[i][0].y+fourAngle[i][1].y)/2;
		centre[i].x = (fourAngle[i][2].x+fourAngle[i][3].x)/2;
		rectangle(origin, Point(fourAngle[i][2].x,fourAngle[i][1].y), Point(fourAngle[i][3].x, fourAngle[i][0].y), Scalar(0, 200, 100),FILLED);
	}
	cout << " num_of obj "<<contours.size() <<endl;
	for(int i=1;i<contours.size()-1;i++)
	{
	
	for(int j=i-1;j<i+2;j++)
		 {
		 	if(j != i)
		 	{
		 	cout <<" deltyyyyy======= " <<fabs(fourAngle[i][1].y-fourAngle[i][0].y)<< endl;
		 		if(fabs(fourAngle[i][1].y-fourAngle[i][0].y)>30)
		 		{
		 		cout <<" 2__deltyyyyy======= " <<fabs(fourAngle[i][1].y-fourAngle[i][0].y)<< endl;
		 			if(fabs(fourAngle[j][1].y-fourAngle[j][0].y)>30)
		 			{
		 			cout <<" deltxxxxx======= " <<fabs(fourAngle[i][2].x-fourAngle[j][3].x)<< endl;
		 				if(fabs(fourAngle[i][3].x-fourAngle[j][2].x)<110)
		 				{
		 				
		 				//drawContours(origin, contours, -1,Scalar(0,0,255),1);
		 				circle(origin,centre[i],radius[i], Scalar(0, 215, 255),1);
		 				circle(origin,centre[j],radius[j], Scalar(255, 215, 0),1);
		 				line(origin, Point(fourAngle[i][2].x,fourAngle[i][0].y), Point(fourAngle[j][3].x, fourAngle[j][1].y), Scalar(0, 0, 200),2);
		 				line(origin, Point(fourAngle[i][2].x,fourAngle[i][1].y), Point(fourAngle[j][3].x, fourAngle[j][0].y), Scalar(0, 0, 200),2);
		 				circle(origin,Point((centre[i].x+centre[j].x)/2,(centre[i].y+centre[j].y)/2),3, Scalar(0, 0, 255),FILLED);
		 				cout <<"normal" << endl;
		 				}
		 			}
		 			
		 			
		 		}
		 	}
		 }
	}

	//////////////////
	
 }
 int main()
 {
 	VideoCapture capture;
 	Mat frame;
 	string path = "b2.avi" ;
 	frame = capture.open(path);
 	if(!capture.isOpened())
 	{
 		cout << " cannot open the vedio..." << endl;
 		return -1;
 	}
 	namedWindow("Trackbars",1);
 	createTrackbar("Hue min ","Trackbars",&hmin,255);
	createTrackbar("Hue max ","Trackbars",&hmax,255);
	createTrackbar("sue min ","Trackbars",&smin,255);
	createTrackbar("sue max ","Trackbars",&smax,255);
	createTrackbar("vue min ","Trackbars",&vmin,255);
	createTrackbar("vue max ","Trackbars",&vmax,255);
 	while(capture.read(frame))
 	{
 		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
 		Scalar lower(hmin, smin, vmin);
	 	Scalar upper(hmax, smax, vmax);
	 	inRange(imgHSV, lower, upper, mask);
	 	
 		imshow("HSV_img", mask);
 		cout << "hmin="<<hmin<<endl;
 		cout << "hmax="<<hmax<<endl;
 		cout << "smin="<<smin<<endl;
 		cout << "smax="<<smax<<endl;
 		cout << "vmin="<<vmin<<endl;
 		cout << "vmax="<<vmax<<endl;

 		GaussianBlur(mask, imgBlur, Size(3,3),3,0);
 		Canny(imgBlur, imgCanny, 25, 75);
 		imshow("imgcanny",imgCanny);
 		Mat kernal = getStructuringElement(MORPH_RECT, Size(5,5));
 		dilate(imgBlur, imgDil, kernal);
 		imshow("imgDil",imgDil);
 		erode(imgDil, imgErode, kernal);
 		
 		imshow("img_erode",imgErode);
 		//blur(imgErode, medium, Size(5,5), {-1,-1});
 		//imshow("medium", medium);
 		//erode(medium, medium, kernal);
 		detect(imgDil, frame);
 		imshow("detect_final",frame);
 		waitKey(100);
 	}
	capture.release();
 	return 0;
 }
 
 
 
 
 
 
