#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream> 



using namespace cv;  
using namespace std;  


Rect b_seat1;  
Rect b_seat2;  
Rect b_seat3;
Rect drawString(Mat img, string text, Point coord, Scalar color, float fontScale = 0.6f, int thickness = 1, int fontFace = FONT_HERSHEY_COMPLEX)  
{

    int baseline=0;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
    baseline += thickness;

    if (coord.y >= 0) {

        coord.y += textSize.height;
    }
    else {

        coord.y += img.rows - baseline + 1;
    }

    if (coord.x < 0) {
        coord.x += img.cols - textSize.width + 1;
    }


    Rect boundingRect = Rect(coord.x, coord.y - textSize.height, textSize.width, baseline + textSize.height);


    putText(img, text, coord, fontFace, fontScale, color, thickness, CV_AA);


    return boundingRect;
}

Rect drawButton(Mat img, string text, Point coord, int minWidth = 0)  
{
    int B = 10;
    Point textCoord = Point(coord.x + B, coord.y + B);

    Rect rcText = drawString(img, text, textCoord, CV_RGB(0,0,0));

    Rect rcButton = Rect(rcText.x - B, rcText.y - B, rcText.width + 2*B, rcText.height + 2*B);

    if (rcButton.width < minWidth)
        rcButton.width = minWidth;

    Mat matButton = img(rcButton);
    matButton += CV_RGB(90, 90, 90);

    rectangle(img, rcButton, CV_RGB(200,200,200), 1, CV_AA);


    drawString(img, text, textCoord, CV_RGB(10,55,20));

    return rcButton;
}



Mat img, res;
cv::VideoCapture cap;
cv::VideoWriter file;

int success = 0, k = 0;
int delay = 1;
bool stop = false;
bool status = false;
bool pause = false;



void callBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        if (b_seat1.contains(Point(x, y)))
        {
            status = true;
            pause = false;
            printf("Recording...\n");

        }
        if (b_seat2.contains(Point(x, y)))
        {
            status = false;
            pause = true;
            printf ("Paused!\n");
        }
        if (b_seat3.contains(Point(x, y)))
        {
            status = false;
            pause = false;
            rectangle(img, b_seat3, Scalar(0,0,0), 1, CV_AA);
            imshow("video", img);
            stop = true;
            printf ("Close!\n");
        }
    }
}


int main()
{




    string outfile = "video.avi";


    cap.open(0);


    int width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    cv::Size size(width,height);

    double rate = cap.get(CV_CAP_PROP_FPS);

    file.open(outfile, CV_FOURCC('M', 'J', 'P', 'G'), rate, size, true);

    if(!cap.isOpened())
    {
        cout << "Can't open the camera!" << endl;
    }


    while (!stop)
    {



        cap.read(img);
        img.copyTo(res);

        if(!cap.read(img))
        {
            cout<<"Video Failed!"<<endl;
            return -1;
        }
        const int b=10;
        string b_name1="Start ";
        string b_name2="Pause";
        string b_name3="Stop";
        b_seat1=drawButton(img,b_name1,Point(b,b),0);
        b_seat2=drawButton(img,b_name2,Point(b_seat1.x,b_seat1.height+b),b_seat1.width);
        b_seat3=drawButton(img,b_name3,Point(b_seat2.x,b_seat2.height+b_seat2.y),b_seat2.width);
        string str1 = "Click [Start] to record.";
        string str2 = "Click [Pause] to pause.";
        string str3 = "Click [Stop] or press [Esc] to close window.";
        drawString(img,str1,Point(10,-30),Scalar(0,255,0),0.33f,1,3);
        drawString(img,str2,Point(10,-20),Scalar(0,255,0),0.33f,1,3);
        drawString(img,str3,Point(10,-10),Scalar(0,255,0),0.33f,1,3);


        namedWindow("video");
        setMouseCallback ("video", callBackFunc);
        imshow("video", img);

        k = waitKey(delay);
        if (k == 27)
            stop = true;


            if (status)
            {
                rectangle(img, b_seat1, Scalar(0,0,0), 1, CV_AA);
                imshow("video", img);
                waitKey(100);
                file.write(res);
                success++;

            }

            if (pause)
            {
            rectangle(img, b_seat2, Scalar(0,0,0), 1, CV_AA);
            imshow("video", img);
            waitKey(100);
            }

            /*if (close)
            {
            rectangle(img, b_seat3, Scalar(0,0,0), 1, CV_AA);
            imshow("video", img);
            waitKey(100);
            }*/



    }
    destroyAllWindows();
    printf("Done! \n");


    cap.release();
    file.release();

    return(0);
}
