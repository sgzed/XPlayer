#pragma once
#include <mutex>
#include <memory>

struct AVPacket;
struct AVFormatContext;
struct AVDictionary;
struct AVCodecParameters;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	virtual bool Open(const char*);

	//º«µ√ Õ∑≈AVPacket av_packet_unref£¨ av_packet_free(&pkt);
	virtual std::shared_ptr<AVPacket> Read();

	//0.0~1.0
	bool Seek(double pos);

	std::shared_ptr<AVCodecParameters> CopyVCodePara();
	std::shared_ptr<AVCodecParameters> CopyACodePara();

	void Clear();
	void Close();

	bool IsAudioPkt(std::shared_ptr<AVPacket>& pkt);

	long long totalMs = 0;

	int height = 0;
	int width = 0;

protected:
	int audio_stream = 0;
	int video_stream = 1;
	AVFormatContext* format_ctx = nullptr;
	std::mutex mutex;

	AVDictionary* opts = nullptr;
};

