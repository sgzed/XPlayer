#include "XResample.h"
#include <iostream>

extern "C"
{
#include "libswresample/swresample.h"
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"swresample.lib")

using std::cout;
using std::endl;

XResample::XResample()
{
}

XResample::~XResample()
{
}

//传入的格式和采样率，统一转换为S16
bool XResample::Open(std::shared_ptr<AVCodecParameters> para)
{
	std::lock_guard<std::mutex> lck(mutex);

	swr_ctx = swr_alloc_set_opts(swr_ctx,
		av_get_default_channel_layout(para->channels),
		(AVSampleFormat)out_format,
		para->sample_rate,
		av_get_default_channel_layout(para->channels),
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, nullptr);
	
	if (!swr_ctx)
	{
		return false;
	}
	int ret = swr_init(swr_ctx);
	if (ret)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "swr_init failed ! : " << buf << endl;
		return false;
	}
	return true;
}

int XResample::ReSample(std::shared_ptr<AVFrame> frame, unsigned char* pcm)
{
	if (!frame || !pcm || !swr_ctx)	return 0;
	
	uint8_t* data[2] = { 0 };
	data[0] = pcm;

	int ret = swr_convert(swr_ctx, data, frame->nb_samples,
		(const uint8_t **)frame->data, frame->nb_samples);
	if (ret <= 0)	return false;
	int outSize = ret * frame->channels * av_get_bytes_per_sample(AVSampleFormat(out_format));
	return outSize;
}



