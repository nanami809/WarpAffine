# WarpAffine
仿射变换

#一、实验代码

```
#include<opencv2/opencv.hpp>
#include<vector>

using namespace cv;
using namespace std;

int main(){
	Mat scrImage,dstImage;//输入图像、输出图像、显示图像
	Point2f scrTriangle[3];
	Point2f dstTriangle[3];
	Mat WarpMat(2,3,CV_32FC1);//变换矩阵
	
	scrImage = imread("lm.jpg");
	if (!scrImage.data){ printf("读取图片错误。\n"); return false; }	
   
	scrTriangle[0] = Point2f(0, 0);
	scrTriangle[1] = Point2f(static_cast<float>(scrImage.cols - 1), 0);
	scrTriangle[2] = Point2f(0, static_cast<float>(scrImage.rows - 1));
	dstTriangle[0] = Point(static_cast<float>(scrImage.cols*0.1), static_cast<float>(scrImage.rows*0.8));
	dstTriangle[1] = Point(static_cast<float>(scrImage.cols*0.9), static_cast<float>(scrImage.rows*0.1));
	dstTriangle[2] = Point(static_cast<float>(scrImage.cols*0.3), static_cast<float>(scrImage.rows*0.2));
	WarpMat = getAffineTransform(scrTriangle, dstTriangle);
	
	dstImage.create(scrImage.size(), scrImage.type());
	warpAffine(scrImage, dstImage, WarpMat, dstImage.size());//仿射变换
	imshow("变换1", dstImage);
	
    waitKey(6000);
	return 0;
}
```
#二、效果之一

运行程序，只见图片已翻转，但大部分超出了图像范围无法显示出来。
<br>
![这里写图片描述](http://img.blog.csdn.net/20151230210846029)

#三、改进
##学习变换矩阵的原理
一个坐标通过函数变换的新的坐标位置：
$$
        \begin{bmatrix}x'\\ y'\\ \end{bmatrix}=\begin{bmatrix}
        A_{11} &A_{12}\\
        A_{21}  &A_{22}\\\end{bmatrix}*\begin{bmatrix}x\\y\\\end{bmatrix}+ \begin{bmatrix}b_1\\b_2\end{bmatrix}
$$
因此，变换矩阵为2×3的数组结构：
\begin{bmatrix}
        A_{11} &A_{12}&b_1\\
        A_{21}  &A_{22}&b_2\end{bmatrix}
	

> 这里有一点疑问，虽然初始化Mat WarpMat（2，3，CV_32FC1）；
> 	但是，在调试中仍然可以发现WarpMat中的元素都为64float，也只有通过at< double>才能取出像素值
##求得扩充图片
由公式，通过已知的WarpMat求出图片超出范围的那个角点位置，以此扩大图片。



```
    //增加变量
    Mat showImage;
	Point2f scrlastPoint;//原图到范围外去的那个角点
	Point2f dstlastPoint;
	Size dst_size;//输出图像的Size
	Point newO;//原图原点在新图像中的位置
	Mat temp;	
	
	scrlastPoint = Point2f(static_cast<float>(scrImage.cols - 1), static_cast<float>(scrImage.rows - 1));
	//根据变换矩阵计算偏移量
	dstlastPoint.x = WarpMat.at<double>(0, 0) * scrlastPoint.x + WarpMat.at<double>(0, 1) * scrlastPoint.y+	WarpMat.at<double>(0, 2);
	dstlastPoint.y = WarpMat.at<double>(1, 0) * scrlastPoint.x + WarpMat.at<double>(1, 1) * scrlastPoint.y + WarpMat.at<double>(1, 2);
	//X坐标的考察
	if (dstlastPoint.x<0){//超出边界情况1
		dst_size.width = -dstlastPoint.x+scrImage.cols;		
		newO.x = -dstlastPoint.x;
	}
	else if (dstlastPoint.x>scrImage.cols - 1){//超出边界情况2
		dst_size.width = dstlastPoint.x+ 1;
		newO.x =0;
	}
	else{
		dst_size.width = scrImage.cols;
		newO.x = 0;
	}	
	//Y坐标的考察
	if (dstlastPoint.y<0){//超出边界情况3
		dst_size.height = -dstlastPoint.y+scrImage.rows;
		newO.y = -dstlastPoint.y;
	}
	else if (dstlastPoint.y>scrImage.rows - 1){//超出边界情况4
		dst_size.height = dstlastPoint.y  + 1;
		newO.y = 0;
	}
	else{
		dst_size.height = scrImage.rows;
		newO.y = 0;
	}
	//将原图扩充
	dstImage.create(dst_size, scrImage.type());
	dstImage = Scalar::all(0);
	temp = dstImage(Rect(newO.x, newO.y, scrImage.cols, scrImage.rows));
	scrImage.copyTo(temp);	
	//重新制作转换矩阵
	scrTriangle[0] = Point2f(0 + newO.x, 0+newO.y);
	scrTriangle[1] = Point2f(static_cast<float>(scrImage.cols - 1) + newO.x, 0 + newO.y);
	scrTriangle[2] = Point2f(0 + newO.x, static_cast<float>(scrImage.rows - 1) + newO.y);
	dstTriangle[0] = Point(static_cast<float>(scrImage.cols*0.1) + newO.x, static_cast<float>(scrImage.rows*0.8) + newO.y);
	dstTriangle[1] = Point(static_cast<float>(scrImage.cols*0.9) + newO.x, static_cast<float>(scrImage.rows*0.1) + newO.y);
	dstTriangle[2] = Point(static_cast<float>(scrImage.cols*0.3) + newO.x, static_cast<float>(scrImage.rows*0.2) + newO.y);
	Mat WarpMat2 = getAffineTransform(scrTriangle, dstTriangle);
	//
	warpAffine(dstImage, dstImage, WarpMat2, dstImage.size());
	imshow("变换2", dstImage);
```
#四、改进后的效果
![这里写图片描述](http://img.blog.csdn.net/20151230220442622)

可见，经过改进，成功的将图片显示完全。

#五、变换矩阵的思考
通过imageWatch插件观察得到

##原始WarpMat：
![这里写图片描述](http://img.blog.csdn.net/20151230220811214)
##扩充图片后的WarpMat：
![这里写图片描述](http://img.blog.csdn.net/20151230220920265)

可见$\begin{bmatrix}A_{11}&A_{12}\\A_{21}&A_{22}\end{bmatrix}$没有变化，主要是偏移量$\begin{bmatrix}b_1\\b_2\end{bmatrix}$的区别。

之前求得的第四点新位置为(549,-254)
原点新位置Point newO为（0，254）
原图片499×507新图片扩充到550×761

其中关系如何求，能否通过这几点直接求得新变换矩阵，还需思考……


