#include<opencv2/opencv.hpp>
#include<vector>

using namespace cv;
using namespace std;

int main(){
	Mat scrImage, dstImage, showImage;
	Point2f scrTriangle[3];
	Point2f dstTriangle[3];
	Point2f scrlastPoint;
	Point2f dstlastPoint;
	Size dst_size;
	Point newO;
	Mat WarpMat(2, 3, CV_32FC1);
	Mat temp;


	scrImage = imread("C:/Users/eva72/Documents/VS2013_Projects/Projects/StudyOpenCV/mn.jpg");
	if (!scrImage.data){ printf("读取图片错误。\n"); return false; }


	scrTriangle[0] = Point2f(0, 0);
	scrTriangle[1] = Point2f(static_cast<float>(scrImage.cols - 1), 0);
	scrTriangle[2] = Point2f(0, static_cast<float>(scrImage.rows - 1));
	dstTriangle[0] = Point(static_cast<float>(scrImage.cols*0.1), static_cast<float>(scrImage.rows*0.8));
	dstTriangle[1] = Point(static_cast<float>(scrImage.cols*0.9), static_cast<float>(scrImage.rows*0.1));
	dstTriangle[2] = Point(static_cast<float>(scrImage.cols*0.3), static_cast<float>(scrImage.rows*0.2));
	WarpMat = getAffineTransform(scrTriangle, dstTriangle);
	//展示1
	showImage.create(scrImage.size(), scrImage.type());
	warpAffine(scrImage, showImage, WarpMat, showImage.size());
	imshow("变换1", showImage);

	//计算偏移量
	scrlastPoint = Point2f(static_cast<float>(scrImage.cols - 1), static_cast<float>(scrImage.rows - 1));
	dstlastPoint.x = WarpMat.at<double>(0, 0) * scrlastPoint.x + WarpMat.at<double>(0, 1) * scrlastPoint.y + WarpMat.at<double>(0, 2);
	dstlastPoint.y = WarpMat.at<double>(1, 0) * scrlastPoint.x + WarpMat.at<double>(1, 1) * scrlastPoint.y + WarpMat.at<double>(1, 2);
	if (dstlastPoint.x<0){//超出边界1
		dst_size.width = -dstlastPoint.x + scrImage.cols;
		newO.x = -dstlastPoint.x;
	}
	else if (dstlastPoint.x>scrImage.cols - 1){//超出边界2
		dst_size.width = dstlastPoint.x + 1;
		newO.x = 0;
	}
	else{
		dst_size.width = scrImage.cols;
		newO.x = 0;
	}

	if (dstlastPoint.y<0){//超出边界3
		dst_size.height = -dstlastPoint.y + scrImage.rows;
		newO.y = -dstlastPoint.y;
	}
	else if (dstlastPoint.y>scrImage.rows - 1){//超出边界2
		dst_size.height = dstlastPoint.y + 1;
		newO.y = 0;
	}
	else{
		dst_size.height = scrImage.rows;
		newO.y = 0;
	}
	dstImage.create(dst_size, scrImage.type());
	dstImage = Scalar::all(0);
	temp = dstImage(Rect(newO.x, newO.y, scrImage.cols, scrImage.rows));
	scrImage.copyTo(temp);
	//重新制作转换矩阵
	scrTriangle[0] = Point2f(0 + newO.x, 0 + newO.y);
	scrTriangle[1] = Point2f(static_cast<float>(scrImage.cols - 1) + newO.x, 0 + newO.y);
	scrTriangle[2] = Point2f(0 + newO.x, static_cast<float>(scrImage.rows - 1) + newO.y);
	dstTriangle[0] = Point(static_cast<float>(scrImage.cols*0.1) + newO.x, static_cast<float>(scrImage.rows*0.8) + newO.y);
	dstTriangle[1] = Point(static_cast<float>(scrImage.cols*0.9) + newO.x, static_cast<float>(scrImage.rows*0.1) + newO.y);
	dstTriangle[2] = Point(static_cast<float>(scrImage.cols*0.3) + newO.x, static_cast<float>(scrImage.rows*0.2) + newO.y);
	Mat WarpMat2 = getAffineTransform(scrTriangle, dstTriangle);
	//


	warpAffine(dstImage, dstImage, WarpMat2, dstImage.size());
	imshow("变换2", dstImage);







	waitKey(10000);
	return 0;





}
