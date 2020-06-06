#pragma once

#include <mutex>
struct AVCodecParameters;
struct AVPacket;
struct AVFrame;
struct AVCodecContext;

class XDecode
{
public:
	XDecode();
	virtual ~XDecode();

	virtual bool Open(std::shared_ptr<AVCodecParameters> para);

	bool SendPkt(std::shared_ptr<struct AVPacket> pkt);

	std::shared_ptr<AVFrame> RecvPkt();

	void Clear();
	void Close();

protected:
	AVCodecContext* codec_ctx = nullptr;
	std::mutex mutex;
};

