/*!
 *  Copyright (c) 2019 by Contributors if not otherwise specified
 * \file video_interface.cc
 * \brief Video file reader implementations
 */

#include "video_reader.h"
#include "video_loader.h"
#include "../runtime/str_util.h"

#include <decord/video_interface.h>
#include <decord/runtime/registry.h>


#include <dlpack/dlpack.h>
#include <dmlc/logging.h>

namespace decord {

VideoReaderPtr GetVideoReader(std::string fn, Decoder be) {
    std::shared_ptr<VideoReaderInterface> ptr;
    if (be == Decoder::FFMPEG()) {
        ptr = std::make_shared<VideoReader>(fn, kCPU);
    } else {
        LOG(FATAL) << "Not supported Decoder type " << be;
    }
    return ptr;
}

namespace runtime {
DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderGetVideoReader")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    std::string fn = args[0];
    int device_type = args[1];
    int device_id = args[2];
    int width = args[3];
    int height = args[4];
    DLContext ctx;
    ctx.device_type = static_cast<DLDeviceType>(device_type);
    ctx.device_id = device_id;
    VideoReaderInterfaceHandle handle = static_cast<VideoReaderInterfaceHandle>(new VideoReader(fn, ctx, width, height));
    *rv = handle;
  });

DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderNextFrame")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoReaderInterfaceHandle handle = args[0];
    NDArray arr = static_cast<VideoReaderInterface*>(handle)->NextFrame();
    *rv = arr;
  });

DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderGetFrameCount")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoReaderInterfaceHandle handle = args[0];
    int64_t ret = static_cast<VideoReaderInterface*>(handle)->GetFrameCount();
    *rv = ret;
  });

DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderGetKeyIndices")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoReaderInterfaceHandle handle = args[0];
    NDArray ret = static_cast<VideoReaderInterface*>(handle)->GetKeyIndices();
    *rv = ret;
  });

DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderSeek")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoReaderInterfaceHandle handle = args[0];
    int64_t pos = args[1];
    bool ret = static_cast<VideoReaderInterface*>(handle)->Seek(pos);
    *rv = ret;
  });

DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderSeekAccurate")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoReaderInterfaceHandle handle = args[0];
    int64_t pos = args[1];
    bool ret = static_cast<VideoReaderInterface*>(handle)->SeekAccurate(pos);
    *rv = ret;
  });

DECORD_REGISTER_GLOBAL("video_reader._CAPI_VideoReaderSkipFrames")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoReaderInterfaceHandle handle = args[0];
    int64_t num = args[1];
    static_cast<VideoReaderInterface*>(handle)->SkipFrames(num);
  });


// VideoLoader
DECORD_REGISTER_GLOBAL("video_loader._CAPI_VideoLoaderGetVideoLoader")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    CHECK_EQ(args.size(), 9);
    // for convenience, pass in comma separated filenames
    std::string filenames = args[0];
    int bs = args[1];
    int height = args[2];
    int width = args[3];
    int channel = args[4];
    int intvl = args[5];
    int skip = args[6];
    int shuffle = args[7];
    int prefetch = args[8];
    auto fns = SplitString(filenames, ',');
    std::vector<int> shape({bs, height, width, channel});
    VideoLoaderInterfaceHandle handle = static_cast<VideoLoaderInterfaceHandle>(new VideoLoader(fns, {kCPU}, shape, intvl, skip, shuffle, prefetch));
    *rv = handle;
  });

DECORD_REGISTER_GLOBAL("video_loader._CAPI_VideoLoaderReset")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoLoaderInterfaceHandle handle = args[0]; 
    static_cast<VideoLoaderInterface*>(handle)->Reset();
  });

DECORD_REGISTER_GLOBAL("video_loader._CAPI_VideoLoaderLength")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoLoaderInterfaceHandle handle = args[0]; 
    auto len = static_cast<VideoLoaderInterface*>(handle)->Length();
    *rv = len;
  });

DECORD_REGISTER_GLOBAL("video_loader._CAPI_VideoLoaderHasNext")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoLoaderInterfaceHandle handle = args[0]; 
    bool ret = static_cast<VideoLoaderInterface*>(handle)->HasNext();
    *rv = ret;
  });

DECORD_REGISTER_GLOBAL("video_loader._CAPI_VideoLoaderNext")
.set_body([] (DECORDArgs args, DECORDRetValue* rv) {
    VideoLoaderInterfaceHandle handle = args[0]; 
    NDArray ret = static_cast<VideoLoaderInterface*>(handle)->Next();
    *rv = ret;
  });
}  // namespace runtime
}  // namespace decord