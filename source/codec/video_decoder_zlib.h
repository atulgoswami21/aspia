//
// PROJECT:         Aspia
// FILE:            codec/video_decoder_zlib.h
// LICENSE:         GNU Lesser General Public License 2.1
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CODEC__VIDEO_DECODER_ZLIB_H
#define _ASPIA_CODEC__VIDEO_DECODER_ZLIB_H

#include "codec/decompressor_zlib.h"
#include "codec/pixel_translator.h"
#include "codec/video_decoder.h"
#include "desktop_capture/desktop_frame.h"

namespace aspia {

class VideoDecoderZLIB : public VideoDecoder
{
public:
    ~VideoDecoderZLIB() = default;

    static std::unique_ptr<VideoDecoderZLIB> Create();

    bool Decode(const proto::desktop::VideoPacket& packet, DesktopFrame* target_frame) override;

private:
    VideoDecoderZLIB() = default;

    DecompressorZLIB decompressor_;
    std::unique_ptr<PixelTranslator> translator_;
    std::unique_ptr<DesktopFrame> source_frame_;

    DISALLOW_COPY_AND_ASSIGN(VideoDecoderZLIB);
};

} // namespace aspia

#endif // _ASPIA_CODEC__VIDEO_DECODER_ZLIB_H
