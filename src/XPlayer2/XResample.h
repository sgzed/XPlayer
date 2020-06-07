#pragma once

#include <mutex>

struct SwrContext;
struct AVFrame;
struct AVCodecParameters;

class XResample
{
public:
	XResample();

	virtual ~XResample();

	//传入的格式和采样率，统一转换为S16
	virtual bool Open(std::shared_ptr<AVCodecParameters> para);

	int ReSample(std::shared_ptr<AVFrame> in_frame,unsigned char* out_data);

	int out_format = 1;

protected:
	SwrContext* swr_ctx = nullptr;

	std::mutex mutex;
};

