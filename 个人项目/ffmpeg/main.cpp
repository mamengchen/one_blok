//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>
//using namespace cv;
//
//int main(int argc, char * argv[])
//{
//	/*Mat image = imread("1.png");
//	namedWindow("img");
//	imshow("img", image);*/
//
//	Mat mat(3000, 4000, CV_8UC3);
//	//另外一种创建大小 mat.create(3000, 4000, CV_8UC3);
//
//	int es = mat.elemSize();
//	int size = mat.rows*mat.cols*es;
//	
//	for (int i = 0; i < size; i += es)
//	{
//		mat.data[i] = 255; //B
//		mat.data[i + 1] = 0; //G
//		mat.data[i + 2] = 255; //R
//	}
//
//	namedWindow("mat");
//	imshow("mat", mat);
//	waitKey(0);
//
//	return 0;
//}

//#include <opencv2/highgui.hpp>
//#include <string>
//#include <iostream>
//using namespace cv;
//
//#pragma comment(lib, "opencv_world347.lib")
//
//int main()
//{
//	VideoCapture cam;
//	std::string url = "rtsp://test:test123456@10.201.208.92";;
//	cvNamedWindow("video");
//	Mat frame;
//	//if (!cam.open(url))
//	if (!cam.open(0))
//	{
//		std::cout << "open cam failed" << std::endl;
//		waitKey(0);
//		return -1;
//	}
//	for (;;)
//	{
//		cam.read(frame);
//		imshow("video", frame);
//		waitKey(1);
//	}
//	return 0;
//}



#include <iostream>
using namespace std;

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/time.h"
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")

int XError(int errNum)
{
	char buf[1024] = { 0 };
	av_strerror(errNum, buf, sizeof(buf));
	cout << buf << endl;
	getchar();
	return -1;
}

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

int main()
{
	const char *inUrl = "test.flv";
	//const char *inUrl = "rtsp://test:test123456@10.201.208.92";
	const char *outUrl = "rtmp://10.201.208.92/live";

	av_register_all();

	avformat_network_init();
	/*--------------------------------------  输入流  ---------------------*/
	//输入格式上下文
	AVFormatContext *ictx = NULL;

	//设置rtsp协议延时
	AVDictionary *opts = NULL;
	char key[] = "max_delay";
	char val[] = "500";
	av_dict_set(&opts, key, val, 0);
	
	char key2[] = "rtsp_transport";
	char val2[] = "tcp";
	av_dict_set(&opts, key2, val2, 0);

	//打开rtsp流，解封文件头
	int re = avformat_open_input(&ictx, inUrl, 0, &opts);
	if (re != 0)
	{
		return XError(re);
	}
	cout << "open file " << inUrl << " Success" << endl;


	//获取音频视频流信息，flv h264

	re = avformat_find_stream_info(ictx, 0);
	if (re != 0)
	{
		return XError(re);
	}

	av_dump_format(ictx, 0, inUrl, 0);


	/*--------------------------------------  输出流  ---------------------*/
	//创建输出流上下文
	AVFormatContext *octx = NULL;

	re = avformat_alloc_output_context2(&octx, 0, "flv", outUrl);
	if (!octx)
	{
		return XError(re);
	}
	cout << "octx create success" << endl;

	//配置输出流
	//先遍历输入的avstream
	for (int i = 0; i < ictx->nb_streams; i++)
	{
		//创建输出流
		AVStream *out = avformat_new_stream(octx, ictx->streams[i]->codec->codec);
		if (!out)
		{
			return XError(0);
		}
		//复制配置信息
		//re = avcodec_copy_context(out->codec, ictx->streams[i]->codec);

		re = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);

		out->codec->codec_tag = 0;
	}

	av_dump_format(octx, 0, outUrl, 1);

	/*-------------------------------------------------------------------------------------------------*/
	//rtmp 推流

	//打开io
	re = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
	if (!octx->pb)
	{
		return XError(re);
	}

	//写入头信息
	re = avformat_write_header(octx, 0);
	if (re < 0)
	{
		cout << re << endl;
		return XError(re);
	}

	cout << "avformat_write_header " << re << endl;

	//推流每一帧数据:wq
	AVPacket pkt;
	//获取当前微秒时间戳
	long long startTime = av_gettime();
	for (;;)
	{
		re = av_read_frame(ictx, &pkt);
		if (re != 0 || pkt.size <= 0)
		{
			continue;
		}

		cout << pkt.pts << " " << flush;

		//计算转换pts\dts
		AVRational itime = ictx->streams[pkt.stream_index]->time_base;
		AVRational otime = octx->streams[pkt.stream_index]->time_base;
		pkt.pts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.pos = -1; //位置

		////视频帧才处理
		//if (ictx->streams[pkt.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		//{
		//	AVRational tb = ictx->streams[pkt.stream_index]->time_base;
		//	//已经过去的时间
		//	long long now = av_gettime() - startTime;
		//	long long dts = 0;
		//	dts = pkt.dts * 1000 * 1000 * r2d(tb);
		//	if (dts > now)
		//		av_usleep(dts - now);
		//}

		//根据pts dts排序，还会把pkt中的空间释放掉
		re = av_interleaved_write_frame(octx, &pkt);



		if (re < 0)
		{
			//XError(re);
		}


		//av_packet_unref(&pkt);
	}


	cout << "欢迎来到直播间" << endl;
	getchar();
	return 0;
}


//#include <opencv2/highgui.hpp>
//#include <iostream>
//
//#pragma comment(lib, "opencv_world347.lib")
//using namespace std;
//using namespace cv;
//
//int main()
//{
//	1.打开计算机相机
//	const char *inUrl = "0";
//	const char *outUrl = "rtmp://10.201.208.92/live";
//	VideoCapture cam;
//	namedWindow("video");
//	Mat frame;
//
//	try
//	{
//		cam.open(0);
//		if (!cam.isOpened())
//		{
//			throw exception("cam open failed!");
//		}
//		cout << "cam open success" << endl;
//
//		for (;;)
//		{
//			if (!cam.grab()) //读取rtsp视频帧，解码视频帧
//			{
//				continue;
//			}
//
//			
//			if (!cam.retrieve(frame))
//			{
//				continue;
//			}
//			imshow("video", frame);
//			waitKey(1);
//
//			yuv转换为rgb
//		}
//	}
//	catch (const std::exception& ex)
//	{
//		if (cam.isOpened()) cam.release();
//		cerr << ex.what() << endl;
//	}
//	getchar();
//	return 0;
//}
