#include "XDecode.h"
#include <iostream>
#include <thread>

extern "C"
{
#include <libavcodec/avcodec.h>
}

using std::endl;
using std::cout;
using std::thread;
#pragma comment(lib,"avcodec.lib")

int GetSuitableThreadCnt()
{
	return thread::hardware_concurrency() ? thread::hardware_concurrency() : 2;
}

XDecode::XDecode()
{
}

XDecode::~XDecode()
{
}

bool XDecode::Open(std::shared_ptr<AVCodecParameters> para)
{
	if (!para)	return false;
	Close();

	AVCodecID codec_id = para->codec_id;
	AVCodec* codec = avcodec_find_decoder(codec_id);
	if (!codec)
	{
		cout << "can't find decoder " << codec_id << endl;
		return false;
	}

	std::lock_guard<std::mutex> lck(mutex);

	codec_ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(codec_ctx, para.get());
	codec_ctx->thread_count = GetSuitableThreadCnt();

	int re = avcodec_open2(codec_ctx, 0, 0);
	if (re != 0)
	{
		avcodec_free_context(&codec_ctx);
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return false;
	}
	return true;
}

bool XDecode::SendPkt(std::shared_ptr<struct AVPacket> pkt)
{
	if (!pkt || pkt->size <= 0 || !pkt->data)return false;

	int ret = 0;
	{
		std::lock_guard<std::mutex> lck(mutex);
		ret = avcodec_send_packet(codec_ctx, pkt.get());
	}
	if (ret)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "avcodec_send_packet  failed ! : " << buf << endl;
		return false;
	}
	return true;
}

std::shared_ptr<AVFrame> XDecode::RecvPkt()
{
	std::lock_guard<std::mutex> lck(mutex);

	if (!codec_ctx)	return nullptr;

	AVFrame* frame = av_frame_alloc();
	int ret = avcodec_receive_frame(codec_ctx, frame);
	if (ret)
	{
		av_frame_free(&frame);
		return nullptr;
	}
	cout << "[" << frame->linesize[0] << "] " << std::flush;
	return std::shared_ptr<AVFrame>(frame, [](AVFrame* f) {
		av_frame_free(&f);
	});
}

void XDecode::Clear()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (codec_ctx)
	{
		avcodec_flush_buffers(codec_ctx);
	}
}

void XDecode::Close()
{
	std::lock_guard<std::mutex> lck(mutex);
	if (codec_ctx)
	{
		avcodec_close(codec_ctx);
		avcodec_free_context(&codec_ctx);
	}
}
