#include <jni.h>
#include <string>
#include <unistd.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <libavutil/imgutils.h>
#include <libyuv.h>
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"ffmpeg",FORMAT,##__VA_ARGS__);
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_division(
        JNIEnv *,
        jobject /* this */,
        jint a,
        jint b) {
    return a / b;
}

//获取编码器信息
extern "C"
JNIEXPORT jstring JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_getAvcodecInfo(
        JNIEnv *env,
        jobject /* this */) {
    char info[40000] = {0};

    av_register_all();

    AVCodec *c_temp = av_codec_next(NULL);

    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }

    return env->NewStringUTF(info);
}

//播放
extern "C"
JNIEXPORT void JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_play(
        JNIEnv *env,
        jobject,
        jstring path, jobject surface) {
//    const char *new_path = env->GetStringUTFChars(path, JNI_FALSE);
//    //注册组件
//    av_register_all();
//    //初始化AVFormatContext的上下文
//    AVFormatContext *format_context = avformat_alloc_context();
//    //打开输入视频文件
//    if (avformat_open_input(&format_context, new_path, NULL, NULL) != 0) {
//        LOGE("%s", "打开输入视频文件失败！");
//        return;
//    };
//    //获取视频信息
//    if (avformat_find_stream_info(format_context, NULL) < 0) {
//        LOGE("%s", "获取视频信息失败");
//        return;
//    }
//    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
//    int video_stream_index = -1;
//    for (int i = 0; i < format_context->nb_streams; i++) {
//        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            video_stream_index = i;
//            break;
//        }
//    }
//    if (video_stream_index == -1) {
//        LOGE("%s", "找不到视频流");
//        return;
//    }
//    // 根据编解码上下文中的编码id查找对应的解码器
//    // 只有知道视频的编码方式，才能够根据编码方式去找到解码器
//    // avFormatContext->streams[video_stream_idx]->codec已经过时了，这里用codecpar代替
//    AVCodecParameters *avCodecParameters = format_context->streams[video_stream_index]->codecpar;
//    AVCodecParameters *avCodecParameters = format_context->streams[video_stream_index]->codecpar;
//    AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
//    if (avCodec == NULL) {
//        LOGE("%s", "找不到编码器，或视频已加密");
//        return;
//    }
//    //打开解码器
//    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
//    avcodec_parameters_to_context(avCodecContext, avCodecParameters);
//    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
//        LOGE("%s", "解码器无法打开");
//        return;
//    }
//    //解码并绘制
//    // AVPacket用于存储一帧一帧的压缩数据（H264）
//    AVPacket *avPacket = av_packet_alloc();
//    // AVFrame用于存储解码后的像素数据(YUV)
//    AVFrame *yuvAVFrame = av_frame_alloc();
//    AVFrame *rgbAVFrame = av_frame_alloc();
//
//    int frame_count = 0;
//    int width = avCodecContext->width;
//    int height = avCodecContext->height;
//
//    //窗体
//    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, surface);
//    // 绘制时的缓冲区
//    ANativeWindow_Buffer *aNativeWindow_buffer;
//    //一帧一帧的读取压缩数据
//    while (av_read_frame(format_context, avPacket) >= 0) {
//        //只要视频压缩数据（根据流的索引位置判断）
//        if (avPacket->stream_index == video_stream_index) {
//            //解码一帧视频压缩数据，得到视频像素数据
//            if (avcodec_send_packet(avCodecContext, avPacket) == 0) {
//                // 一个avPacket可能包含多帧数据，所以需要使用while循环一直读取
//                while (avcodec_receive_frame(avCodecContext, yuvAVFrame) == 0) {
//                    // lock window
//                    // 设置缓冲区的属性：宽高、像素格式（需要与Java层的格式一致）
//                    ANativeWindow_setBuffersGeometry(aNativeWindow, width, height,
//                                                     WINDOW_FORMAT_RGBA_8888);
//                    ANativeWindow_lock(aNativeWindow, aNativeWindow_buffer, NULL);
//                    // fix buffer
//                    // 初始化缓冲区
//                    // 设置属性，像素格式、宽高
//                    av_image_fill_arrays(rgbAVFrame->data, rgbAVFrame->linesize,
//                                         static_cast<const uint8_t *>(aNativeWindow_buffer->bits),
//                                         AV_PIX_FMT_RGBA,
//                                         width, height, 1);
//                    // YUV格式的数据转换成RGBA 8888格式的数据
//                    libyuv::I420ToARGB(yuvAVFrame->data[0], yuvAVFrame->linesize[0],
//                                       yuvAVFrame->data[2], yuvAVFrame->linesize[2],
//                                       yuvAVFrame->data[1], yuvAVFrame->linesize[1],
//                                       yuvAVFrame->data[0], yuvAVFrame->linesize[0],
//                                       width, height);
//                    // unlock window
//                    ANativeWindow_unlockAndPost(aNativeWindow);
//                    frame_count++;
//                    LOGE("解码绘制第%d帧", frame_count);
//                    // 每绘制一帧便休眠16毫秒，避免绘制过快导致播放的视频速度加快
//                    usleep(1000 * 16);
//                }
//            }
//        }
//        av_packet_unref(avPacket);
//    }
//    //释放资源
//    av_frame_free(&yuvAVFrame);
//    av_frame_free(&rgbAVFrame);
//    avcodec_close(avCodecContext);
//    avformat_free_context(format_context);
//    ANativeWindow_release(aNativeWindow);
//    env->GetStringUTFChars(path, JNI_FALSE);
}

void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
}

//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_example_administrator_ffmpegplayer_MainActivity_decode(
//        JNIEnv *env,
//        jobject obj,
//        jstring input_jstr,
//        jstring output_jstr) {
//    AVFormatContext *pFormatCtx;
//    int i, videoindex;
//    AVCodecContext *pCodecCtx;
//    AVCodec *pCodec;
//    AVFrame *pFrame, *pFrameYUV;
//    uint8_t *out_buffer;
//    AVPacket *packet;
//    int y_size;
//    int ret, got_picture;
//    struct SwsContext *img_convert_ctx;
//    FILE *fp_yuv;
//    int frame_cnt;
//    clock_t time_start, time_finish;
//    double time_duration = 0.0;
//
//    char input_str[500] = {0};
//    char output_str[500] = {0};
//    char info[1000] = {0};
//    sprintf(input_str, "%s", env->GetStringUTFChars(input_jstr, NULL));
//    sprintf(output_str, "%s", env->GetStringUTFChars(output_jstr, NULL));
//
////FFmpeg av_log() callback
//    av_log_set_callback(custom_log);
//
//    av_register_all();
//    avformat_network_init();
//    pFormatCtx = avformat_alloc_context();
//
//    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
//        LOGE("Couldn't open input stream.\n");
//        return -1;
//    }
//    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
//        LOGE("Couldn't find stream information.\n");
//        return -1;
//    }
//    videoindex = -1;
//    for (i = 0; i < pFormatCtx->nb_streams; i++)
//        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoindex = i;
//            break;
//        }
//    if (videoindex == -1) {
//        LOGE("Couldn't find a video stream.\n");
//        return -1;
//    }
//    AVCodecParameters *avCodecParameters = pFormatCtx->streams[videoindex]->codecpar;
//    AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
//    if (avCodec == NULL) {
//        LOGE("%s", "找不到编码器，或视频已加密");
//        return -1;
//    }
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
//        LOGE("Couldn't open codec.\n");
//        return -1;
//    }
//
//    pFrame = av_frame_alloc();
//    pFrameYUV = av_frame_alloc();
//    out_buffer = (unsigned char *) av_malloc(
//            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
//    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
//                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
//
//
//    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
//
//    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
//                                     SWS_BICUBIC, NULL, NULL, NULL);
//
//
//    sprintf(info, "[Input     ]%s\n", input_str);
//    sprintf(info, "%s[Output    ]%s\n", info, output_str);
//    sprintf(info, "%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
//    sprintf(info, "%s[Codec     ]%s\n", info, pCodecCtx->codec->name);
//    sprintf(info, "%s[Resolution]%dx%d\n", info, pCodecCtx->width, pCodecCtx->height);
//
//
//    fp_yuv = fopen(output_str, "wb+");
//    if (fp_yuv == NULL) {
//        printf("Cannot open output file.\n");
//        return -1;
//    }
//
//    frame_cnt = 0;
//    time_start = clock();
//
//    while (av_read_frame(pFormatCtx, packet) >= 0) {
//        if (packet->stream_index == videoindex) {
//            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
//            if (ret < 0) {
//                LOGE("Decode Error.\n");
//                return -1;
//            }
//            if (got_picture) {
//                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
//                          0, pCodecCtx->height,
//                          pFrameYUV->data, pFrameYUV->linesize);
//
//                y_size = pCodecCtx->width * pCodecCtx->height;
//                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
//                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
//                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
////Output info
//                char pictype_str[10] = {0};
//                switch (pFrame->pict_type) {
//                    case AV_PICTURE_TYPE_I:
//                        sprintf(pictype_str, "I");
//                        break;
//                    case AV_PICTURE_TYPE_P:
//                        sprintf(pictype_str, "P");
//                        break;
//                    case AV_PICTURE_TYPE_B:
//                        sprintf(pictype_str, "B");
//                        break;
//                    default:
//                        sprintf(pictype_str, "Other");
//                        break;
//                }
//                LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
//                frame_cnt++;
//            }
//        }
//        av_packet_unref(packet);
////        av_free_packet(packet);//函数已经过时
//    }
////flush decoder
////FIX: Flush Frames remained in Codec
//    while (1) {
//        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
//        if (ret < 0)
//            break;
//        if (!got_picture)
//            break;
//        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
//                  pCodecCtx->height,
//                  pFrameYUV->data, pFrameYUV->linesize);
//        int y_size = pCodecCtx->width * pCodecCtx->height;
//        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
//        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
//        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
////Output info
//        char pictype_str[10] = {0};
//        switch (pFrame->pict_type) {
//            case AV_PICTURE_TYPE_I:
//                sprintf(pictype_str, "I");
//                break;
//            case AV_PICTURE_TYPE_P:
//                sprintf(pictype_str, "P");
//                break;
//            case AV_PICTURE_TYPE_B:
//                sprintf(pictype_str, "B");
//                break;
//            default:
//                sprintf(pictype_str, "Other");
//                break;
//        }
//        LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
//        frame_cnt++;
//    }
//    time_finish = clock();
//    time_duration = (double) (time_finish - time_start);
//
//    sprintf(info, "%s[Time      ]%fms\n", info, time_duration);
//    sprintf(info, "%s[Count     ]%d\n", info, frame_cnt);
//
//    sws_freeContext(img_convert_ctx);
//
//    fclose(fp_yuv);
//
//    av_frame_free(&pFrameYUV);
//    av_frame_free(&pFrame);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
//
//    return 0;
//}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_playVideo(
        JNIEnv *env,
        jobject instance,
        jstring path_,
        jobject surface) {

    const char *path = env->GetStringUTFChars(path_, 0);

    int width = 0;
    int height = 0;
    int bufferSize = 0;
    int videoIndex = -1;

    AVPacket *vPacket = NULL;
    AVFrame *vFrame = NULL, *pFrameRGBA = NULL;
    AVCodecContext *vCodecCtx = NULL;
    SwsContext *sws_ctx = NULL;
    AVFormatContext *pFormatCtx = NULL;
    uint8_t *out_buffer = NULL;
    ANativeWindow_Buffer windowBuffer;

    AVCodec *vCodec = NULL;

    //1.初始化所有组件
    av_register_all();
    //分配一个AVFormatContext结构
    pFormatCtx = avformat_alloc_context();
    //2.打开输入文件
    if (avformat_open_input(&pFormatCtx, path, NULL, NULL) != 0) {
        LOGE("Could not open input stream");
        return -1;
    }
    //3.查找文件的流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Could not find stream information");
        return -1;
    }
    //4.查找视频轨
    for (int index = 0; index < pFormatCtx->nb_streams; index++) {
        if (pFormatCtx->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }
    if (videoIndex == -1) {
        LOGE("Could not find a video stream");
        return -1;;
    }
    //5.查找解码器
    vCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
    if (vCodec == NULL) {
        LOGE("could not find codec");
        return -1;;
    }
    //6.配置解码器
    vCodecCtx = avcodec_alloc_context3(vCodec);
    avcodec_parameters_to_context(vCodecCtx, pFormatCtx->streams[videoIndex]->codecpar);
    //7.打开解码器
    if (avcodec_open2(vCodecCtx, vCodec, NULL) < 0) {
        LOGE("Could not open codec");
        return -1;;
    }
    width = vCodecCtx->width;
    height = vCodecCtx->height;
    //分配一个帧指针，指向解码后的原始帧
    vFrame = av_frame_alloc();
    vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    pFrameRGBA = av_frame_alloc();
    //绑定输出buffer
    bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
    out_buffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, out_buffer, AV_PIX_FMT_RGBA,
                         width, height, 1);
    sws_ctx = sws_getContext(width, height, vCodecCtx->pix_fmt,
                             width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    //取到nativewindow
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //默认为RGB565
    if (ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888) <
        0) {
        LOGE("Could not set buffers geometry");
        ANativeWindow_release(nativeWindow);
        return -1;;
    }
    //读取帧
    while (av_read_frame(pFormatCtx, vPacket) >= 0) {
        if (vPacket->stream_index == videoIndex) {
            //视频解码
            if (avcodec_send_packet(vCodecCtx, vPacket) != 0) {
                return -1;
            }
            while (avcodec_receive_frame(vCodecCtx, vFrame) == 0) {
                //转化格式
                sws_scale(sws_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize,
                          0,
                          vCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);
                if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
                    LOGE("cannot lock window");
                } else {
                    //逐行复制
                    uint8_t *bufferBits = (uint8_t *) windowBuffer.bits;
                    for (int h = 0; h < height; h++) {
                        memcpy(bufferBits + h * windowBuffer.stride * 4,
                               out_buffer + h * pFrameRGBA->linesize[0],
                               pFrameRGBA->linesize[0]);
                    }
                    ANativeWindow_unlockAndPost(nativeWindow);
                }
            }
        }
        av_packet_unref(vPacket);
    }
    //释放内存
    sws_freeContext(sws_ctx);

    av_free(vPacket);
    av_free(pFrameRGBA);
    avcodec_close(vCodecCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}