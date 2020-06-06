#include "XDemux.h"
#include <iostream>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

using std::cout;
using std::endl;

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

static double r2d(AVRational rational)
{
	return rational.den == 0 ? 0 : (double)rational.num / (double)rational.den;
}

void ReleasePkt(AVPacket* pkt)
{
	av_packet_free(&pkt);
}

void ReleasePara(AVCodecParameters* para)
{
	avcodec_parameters_free(&para);
}

XDemux::XDemux()
{
	static bool first = true;
	static std::mutex mtx;

	mtx.lock();
	if (!format_ctx)
	{
		avformat_network_init();
		av_dict_set(&opts, "rtsp_transport", "tcp", 0);
		av_dict_set(&opts, "max_delay", "500", 0);
		first = false;
	}
	mtx.unlock();
}

XDemux::~XDemux()
{
}

bool XDemux::Open(const char* url)
{
	Close();

	std::lock_guard<std::mutex> lck(mutex);

	int ret = avformat_open_input(&format_ctx, url, nullptr, &opts);
	cout << "after open input : " << format_ctx << endl;

	if (ret)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "open input " << url << " failed ! : " << buf << endl;
		return false;
	}

	//获取流信息
	ret = avformat_find_stream_info(format_ctx, 0);
	//视频时长 Ms
	totalMs = format_ctx->duration / AV_TIME_BASE * 1000;
	cout << "totalMs = " << totalMs << endl;
	
	//打印流详细信息
	av_dump_format(format_ctx, 0, url, 0);

	cout << "=========================视频信息=========================" << endl;
	video_stream = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	AVStream* v_stream = format_ctx->streams[video_stream];

	height = v_stream->codecpar->height;
	width = v_stream->codecpar->width;

	cout << "format = " << v_stream->codecpar->format << endl;
	cout << "codec_id = " << v_stream->codecpar->codec_id << endl;
	cout << "width = " << v_stream->codecpar->width << endl;
	cout << "height = " << v_stream->codecpar->height << endl;
	cout << "fps = " << r2d(v_stream->avg_frame_rate) << endl;

	cout << "=========================音频信息=========================" << endl;
	audio_stream = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	AVStream* a_stream = format_ctx->streams[audio_stream];
	//AVSampleFormat	fltp解码出来,不能播放，因为32bit,重采样
	cout << "format = " << a_stream->codecpar->format << endl;
	cout << "codec_id = " << a_stream->codecpar->codec_id << endl;
	cout << "channels = " << a_stream->codecpar->channels << endl;
	cout << "sample_rate = " << a_stream->codecpar->sample_rate << endl;
	//一帧的采样个数，单通道		
	cout << "frame_size = " << a_stream->codecpar->frame_size << endl;
	double fps = a_stream->codecpar->frame_size ? (double)a_stream->codecpar->sample_rate / a_stream->codecpar->frame_size : 0;
	cout << "fps = " << fps << endl;

	return true;
}

std::shared_ptr<AVPacket> XDemux::Read()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (!format_ctx)	return nullptr;

	AVPacket* pkt = av_packet_alloc();
	int ret = av_read_frame(format_ctx, pkt);
	if (ret)
	{
		av_packet_free(&pkt);
		return nullptr;
	}

	//转换为ms
	float rate = r2d(format_ctx->streams[pkt->stream_index]->time_base);
	pkt->pts = pkt->pts * rate * 1000;
	//解码时间
	pkt->dts = pkt->dts * rate * 1000;

	cout << pkt->pts << " " << fflush;
	return std::shared_ptr<AVPacket>(pkt,ReleasePkt);
}

bool XDemux::Seek(double pos)
{
	std::lock_guard<std::mutex> lck(mutex);
	if (!format_ctx)	false;
	
	//清理读取缓存
	avformat_flush(format_ctx);

	long long position = format_ctx->streams[video_stream]->duration * pos;
	int ret = av_seek_frame(format_ctx, video_stream, position, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	if (ret)	return false;
	return true;
}

std::shared_ptr<AVCodecParameters> XDemux::CopyVCodePara()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (!format_ctx)	return nullptr;
	AVCodecParameters *pv = avcodec_parameters_alloc();
	avcodec_parameters_copy(pv, format_ctx->streams[video_stream]->codecpar);
	std::shared_ptr<AVCodecParameters> spv(pv, ReleasePara);
	return spv;
}

std::shared_ptr<AVCodecParameters> XDemux::CopyACodePara()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (!format_ctx)	return nullptr;
	AVCodecParameters *pv = avcodec_parameters_alloc();
	avcodec_parameters_copy(pv, format_ctx->streams[audio_stream]->codecpar);
	std::shared_ptr<AVCodecParameters> spv(pv, ReleasePara);
	return spv;
}

void XDemux::Clear()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (!format_ctx)	return;
	avformat_flush(format_ctx);
}

void XDemux::Close()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (!format_ctx) return;
	avformat_close_input(&format_ctx);
}

bool XDemux::IsAudioPkt(std::shared_ptr<AVPacket>& pkt)
{
	std::lock_guard<std::mutex> lck(mutex);
	
	if (!pkt || !pkt->data)
		return false;
	return pkt->stream_index == audio_stream;
}
