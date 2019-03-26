/*
 * Copyright (C) 2018  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

extern "C" {
    #include <x264.h>
}
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

/*
 * @TODO remove after testing is done
 * docker run --rm -ti --init --ipc=host -v /tmp:/tmp -v $PWD:/data -w /data x264test --cid=122 --name=video0.i420
   --width=640 --height=480 --verbose --keyint-min=0 --keyint-max=0 --scenecut=0 --intra-refresh=0 --bframe=0
   --b-adapt=0 --b-pyramid=0 --open-gop=0 --cabac=0 --deblock=0 --rc-method=0 --qp=0 --bitrate=0 --crf=0
   --rc-lookahead=0 --vbv-maxrate=0 --vbv-bufsize=0 --vbv-init=0 --qpmin=0 --qpmax=0 --qpstep=0 --ratetol=0
   --ipratio=0 --pbratio=0 --chroma-qp-offset=0 --aq-mode=0 --aq-strength=0 --mbtree=0 --qcomp=0 --cplxblur=0
   --direct=0 --weightb=0 -weightp=0 --me=0 --merange=0 --subme=0 --psy=0 --mixed-refs=0 --chroma-me=0 --trellis=0
   --fast-pskip=0 --dct-decimate=0 --nr=0 --cqm=0
 */

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{1};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("cid")) ||
         (0 == commandlineArguments.count("name")) ||
         (0 == commandlineArguments.count("width")) ||
         (0 == commandlineArguments.count("height")) ||
         (0 == commandlineArguments.count("keyint-min"))||
         (0 == commandlineArguments.count("keyint-max")) ||
         (0 == commandlineArguments.count("scenecut")) ||
         (0 == commandlineArguments.count("intra-refresh")) ||
         (0 == commandlineArguments.count("bframe")) ||
         (0 == commandlineArguments.count("b-adapt")) ||
         (0 == commandlineArguments.count("b-pyramid")) ||
         (0 == commandlineArguments.count("open-gop"))||
         (0 == commandlineArguments.count("cabac")) ||
         (0 == commandlineArguments.count("deblock")) ||
         (0 == commandlineArguments.count("rc-method")) ||
         (0 == commandlineArguments.count("qp"))||
         (0 == commandlineArguments.count("bitrate")) ||
         (0 == commandlineArguments.count("crf")) ||
         (0 == commandlineArguments.count("rc-lookahead"))||
         (0 == commandlineArguments.count("vbv-maxrate")) ||
         (0 == commandlineArguments.count("vbv-bufsize")) ||
         (0 == commandlineArguments.count("vbv-init")) ||
         (0 == commandlineArguments.count("qpmin"))||
         (0 == commandlineArguments.count("qpmax")) ||
         (0 == commandlineArguments.count("qpstep")) ||
         (0 == commandlineArguments.count("ratetol"))||
         (0 == commandlineArguments.count("ipratio")) ||
         (0 == commandlineArguments.count("pbratio")) ||
         (0 == commandlineArguments.count("chroma-qp-offset")) ||
         (0 == commandlineArguments.count("aq-mode"))||
         (0 == commandlineArguments.count("aq-strength")) ||
         (0 == commandlineArguments.count("mbtree")) ||
         (0 == commandlineArguments.count("qcomp")) ||
         (0 == commandlineArguments.count("cplxblur"))||
         (0 == commandlineArguments.count("direct")) ||
         (0 == commandlineArguments.count("weightb")) ||
         (0 == commandlineArguments.count("weightp")) ||
         (0 == commandlineArguments.count("me")) ||
         (0 == commandlineArguments.count("merange")) ||
         (0 == commandlineArguments.count("subme")) ||
         (0 == commandlineArguments.count("psy")) ||
         (0 == commandlineArguments.count("mixed-refs")) ||
         (0 == commandlineArguments.count("chroma-me"))||
         (0 == commandlineArguments.count("trellis")) ||
         (0 == commandlineArguments.count("fast-pskip")) ||
         (0 == commandlineArguments.count("dct-decimate")) ||
         (0 == commandlineArguments.count("nr"))||
         (0 == commandlineArguments.count("cqm"))) {
        std::cerr << argv[0] << " attaches to an I420-formatted image residing in a shared memory area to convert it into a corresponding h264 frame for publishing to a running OD4 session." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OpenDaVINCI session> --name=<name of shared memory area> --width=<width> --height=<height> [--gop=<GOP>] [--preset=X] [--verbose] [--id=<identifier in case of multiple instances]" << std::endl;
        std::cerr << "         --cid:      CID of the OD4Session to send h264 frames" << std::endl;
        std::cerr << "         --id:       when using several instances, this identifier is used as senderStamp" << std::endl;
        std::cerr << "         --name:     name of the shared memory area to attach" << std::endl;
        std::cerr << "         --width:    width of the frame" << std::endl;
        std::cerr << "         --height:   height of the frame" << std::endl;
        std::cerr << "         --gop:      optional: length of group of pictures (default = 10)" << std::endl;
        std::cerr << "         --preset:   one of x264's presets: ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow; default: veryfast" << std::endl;
        std::cerr << "         --verbose:  print encoding information" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=111 --name=data --width=640 --height=480 --verbose" << std::endl;
    }
    else {
        const std::string NAME{commandlineArguments["name"]};
        const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
        const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
        //const uint32_t GOP_DEFAULT{10};
        //const uint32_t GOP{(commandlineArguments["gop"].size() != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["gop"])) : GOP_DEFAULT};
        const std::string PRESET{(commandlineArguments["preset"].size() != 0) ? commandlineArguments["preset"] : "veryfast"};
        const bool VERBOSE{commandlineArguments.count("verbose") != 0};
        const uint32_t ID{(commandlineArguments["id"].size() != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["id"])) : 0};

        /*
         * Thesis params
         * http://www.chaneru.com/Roku/HLS/X264_Settings.htm
         * https://code.videolan.org/videolan/x264/blob/master/x264.h
         */

        // Presets
        //const std::string PROFILE{commandlineArguments["profile"]};
        //const std::string TUNE{commandlineArguments["tune"]};

        // Frame-type options
        const uint32_t KEYINT_MIN{static_cast<uint32_t>(std::stoi(commandlineArguments["keyint-min"]))};
        const uint32_t KEYINT_MAX{static_cast<uint32_t>(std::stoi(commandlineArguments["keyint-max"]))};
        const uint32_t SCENECUT{static_cast<uint32_t>(std::stoi(commandlineArguments["scenecut"]))};
        const uint32_t INTRA_REFRESH{static_cast<uint32_t>(std::stoi(commandlineArguments["intra-refresh"]))};
        const uint32_t BFRAME{static_cast<uint32_t>(std::stoi(commandlineArguments["bframe"]))};
        const uint32_t B_ADAPT{static_cast<uint32_t>(std::stoi(commandlineArguments["b-adapt"]))};
        const uint32_t B_PYRAMID{static_cast<uint32_t>(std::stoi(commandlineArguments["b-pyramid"]))};
        const uint32_t OPEN_GOP{static_cast<uint32_t>(std::stoi(commandlineArguments["open-gop"]))};
        const uint32_t CABAC{static_cast<uint32_t>(std::stoi(commandlineArguments["cabac"]))};
        const uint32_t DEBLOCK{static_cast<uint32_t>(std::stoi(commandlineArguments["deblock"]))};

        //Rate-control
        const uint32_t RC_METHOD{static_cast<uint32_t>(std::stoi(commandlineArguments["rc-method"]))};
        const uint32_t QP{static_cast<uint32_t>(std::stoi(commandlineArguments["qp"]))};
        const uint32_t BITRATE{static_cast<uint32_t>(std::stoi(commandlineArguments["bitrate"]))};
        const float_t CRF{static_cast<float_t >(std::stof(commandlineArguments["crf"]))};
        const uint32_t RC_LOOKAHEAD{static_cast<uint32_t >(std::stoi(commandlineArguments["rc-lookahead"]))};
        const uint32_t VBV_MAXRATE{static_cast<uint32_t >(std::stoi(commandlineArguments["vbv-maxrate"]))};
        const uint32_t VBV_BUFSIZE{static_cast<uint32_t >(std::stoi(commandlineArguments["vbv-bufsize"]))};
        const float_t VBV_INIT{static_cast<float_t >(std::stof(commandlineArguments["vbv-init"]))};
        const uint32_t QPMIN{static_cast<uint32_t >(std::stoi(commandlineArguments["qpmin"]))};
        const uint32_t QPMAX{static_cast<uint32_t >(std::stoi(commandlineArguments["qpmax"]))};
        const uint32_t QPSTEP{static_cast<uint32_t >(std::stoi(commandlineArguments["qpstep"]))};
        const float_t RATETOL{static_cast<float_t >(std::stof(commandlineArguments["ratetol"]))};
        const float_t IPRATIO{static_cast<float_t >(std::stof(commandlineArguments["ipratio"]))};
        const float_t PBRATIO{static_cast<float_t >(std::stof(commandlineArguments["pbratio"]))};
        const uint32_t CHROMA_QP_OFFSET{static_cast<uint32_t >(std::stoi(commandlineArguments["chroma-qp-offset"]))};
        const uint32_t AQ_MODE{static_cast<uint32_t >(std::stoi(commandlineArguments["aq-mode"]))};
        const float_t AQ_STRENGTH{static_cast<float_t >(std::stof(commandlineArguments["aq-strength"]))};
        const uint32_t MBTREE{static_cast<uint32_t >(std::stoi(commandlineArguments["mbtree"]))};
        const float_t QCOMP{static_cast<float_t >(std::stof(commandlineArguments["qcomp"]))};
        const uint32_t CPLXBLUR{static_cast<uint32_t >(std::stoi(commandlineArguments["cplxblur"]))};
        const uint32_t DIRECT{static_cast<uint32_t >(std::stoi(commandlineArguments["direct"]))};
        const uint32_t WEIGHTB{static_cast<uint32_t >(std::stoi(commandlineArguments["weightb"]))};
        const uint32_t WEIGHTP{static_cast<uint32_t >(std::stoi(commandlineArguments["weightp"]))};
        const uint32_t ME{static_cast<uint32_t >(std::stoi(commandlineArguments["me"]))};
        const uint32_t MERANGE{static_cast<uint32_t >(std::stoi(commandlineArguments["merange"]))};
        const uint32_t SUBME{static_cast<uint32_t >(std::stoi(commandlineArguments["subme"]))};
        const uint32_t PSY{static_cast<uint32_t >(std::stoi(commandlineArguments["psy"]))};
        const uint32_t MIXED_REFS{static_cast<uint32_t >(std::stoi(commandlineArguments["mixed-refs"]))};
        const uint32_t CHROMA_ME{static_cast<uint32_t >(std::stoi(commandlineArguments["chroma-me"]))};
        const uint32_t TRELLIS{static_cast<uint32_t >(std::stoi(commandlineArguments["trellis"]))};
        const uint32_t FAST_PSKIP{static_cast<uint32_t >(std::stoi(commandlineArguments["fast-pskip"]))};
        const uint32_t DCT_DECIMATE{static_cast<uint32_t >(std::stoi(commandlineArguments["dct-decimate"]))};
        const uint32_t NR{static_cast<uint32_t >(std::stoi(commandlineArguments["nr"]))};
        const uint32_t CQM{static_cast<uint32_t >(std::stoi(commandlineArguments["cqm"]))};

        std::unique_ptr<cluon::SharedMemory> sharedMemory(new cluon::SharedMemory{NAME});
        if (sharedMemory && sharedMemory->valid()) {
            std::clog << "[opendlv-video-x264-encoder]: Attached to '" << sharedMemory->name() << "' (" << sharedMemory->size() << " bytes)." << std::endl;

            // Configure x264 parameters.
            x264_param_t parameters;
            if (0 != x264_param_default_preset(&parameters, PRESET.c_str(), "zerolatency")) {
                std::cerr << "[opendlv-video-x264-encoder]: Failed to load preset parameters (" << PRESET << ", zerolatency) for x264." << std::endl;
                return 1;
            }
            parameters.i_width  = WIDTH;
            parameters.i_height = HEIGHT;
            parameters.i_log_level = (VERBOSE ? X264_LOG_INFO : X264_LOG_NONE);
            parameters.i_csp = X264_CSP_I420;
            parameters.i_bitdepth = 8;
            parameters.i_threads = 1;
            //parameters.i_keyint_min = GOP;
            //parameters.i_keyint_max = GOP;
            parameters.i_fps_num = 20 /* implicitly derived from SharedMemory notifications */;
            parameters.b_vfr_input = 0;
            parameters.b_repeat_headers = 1;
            parameters.b_annexb = 1;

            /*
            * Thesis params
            */

            parameters.i_keyint_min = KEYINT_MIN;
            parameters.i_keyint_max = KEYINT_MAX;
            parameters.i_scenecut_threshold = SCENECUT;
            parameters.i_bframe = BFRAME;
            parameters.i_bframe_adaptive = B_ADAPT;
            parameters.i_bframe_pyramid = B_PYRAMID;
            parameters.i_cqm_preset = CQM;

            parameters.b_intra_refresh = INTRA_REFRESH;
            parameters.b_open_gop = OPEN_GOP;
            parameters.b_cabac = CABAC;
            parameters.b_deblocking_filter = DEBLOCK;

            parameters.rc.i_rc_method = RC_METHOD;
            parameters.rc.i_qp_constant = QP;
            parameters.rc.i_qp_min = QPMIN;
            parameters.rc.i_qp_max = QPMAX;
            parameters.rc.i_qp_step = QPSTEP;
            parameters.rc.i_bitrate = BITRATE;
            parameters.rc.f_rf_constant_max = CRF;
            parameters.rc.i_lookahead = RC_LOOKAHEAD;
            parameters.rc.i_vbv_max_bitrate = VBV_MAXRATE;
            parameters.rc.i_vbv_buffer_size = VBV_BUFSIZE;
            parameters.rc.f_vbv_buffer_init = VBV_INIT;
            parameters.rc.f_rate_tolerance = RATETOL;
            parameters.rc.f_ip_factor = IPRATIO;
            parameters.rc.f_pb_factor = PBRATIO;
            parameters.rc.i_aq_mode = AQ_MODE;
            parameters.rc.f_aq_strength = AQ_STRENGTH;
            parameters.rc.b_mb_tree = MBTREE;
            parameters.rc.f_qcompress = QCOMP;
            parameters.rc.f_complexity_blur = CPLXBLUR;

            parameters.analyse.i_weighted_pred = WEIGHTP;
            parameters.analyse.b_weighted_bipred = WEIGHTB;
            parameters.analyse.i_direct_mv_pred = DIRECT;
            parameters.analyse.i_chroma_qp_offset = CHROMA_QP_OFFSET;
            parameters.analyse.i_me_method = ME;
            parameters.analyse.i_me_range = MERANGE;
            parameters.analyse.i_subpel_refine = SUBME;
            parameters.analyse.b_psy = PSY;
            parameters.analyse.b_mixed_references = MIXED_REFS;
            parameters.analyse.b_chroma_me = CHROMA_ME;
            parameters.analyse.i_trellis = TRELLIS;
            parameters.analyse.b_fast_pskip = FAST_PSKIP;
            parameters.analyse.b_dct_decimate = DCT_DECIMATE;
            parameters.analyse.i_noise_reduction = NR;

            if (0 != x264_param_apply_profile(&parameters, "baseline")) {
                std::cerr << "[opendlv-video-x264-encoder]:Failed to apply parameters for x264." << std::endl;
                return 1;
            }

            // Initialize picture to pass YUV420 data into encoder.
            x264_picture_t picture_in;
            x264_picture_init(&picture_in);
            picture_in.i_type = X264_TYPE_AUTO;
            picture_in.img.i_csp = X264_CSP_I420;
            picture_in.img.i_plane = HEIGHT;

            // Directly point to the shared memory.
            sharedMemory->lock();
            {
                picture_in.img.plane[0] = reinterpret_cast<uint8_t*>(sharedMemory->data());
                picture_in.img.plane[1] = reinterpret_cast<uint8_t*>(sharedMemory->data() + (WIDTH * HEIGHT));
                picture_in.img.plane[2] = reinterpret_cast<uint8_t*>(sharedMemory->data() + (WIDTH * HEIGHT + ((WIDTH * HEIGHT) >> 2)));
                picture_in.img.i_stride[0] = WIDTH;
                picture_in.img.i_stride[1] = WIDTH/2;
                picture_in.img.i_stride[2] = WIDTH/2;
                picture_in.img.i_stride[3] = 0;
            }
            sharedMemory->unlock();

            // Open h264 encoder.
            x264_t *encoder = x264_encoder_open(&parameters);
            if (nullptr == encoder) {
                std::cerr << "[opendlv-video-x264-encoder]: Failed to open x264 encoder." << std::endl;
                return 1;
            }

            cluon::data::TimeStamp before, after, sampleTimeStamp;

            // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
            cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

            int i_frame{0};
            while ( (sharedMemory && sharedMemory->valid()) && od4.isRunning() ) {
                // Wait for incoming frame.
                sharedMemory->wait();

                sampleTimeStamp = cluon::time::now();

                std::string data;
                sharedMemory->lock();
                {
                    // Read notification timestamp.
                    auto r = sharedMemory->getTimeStamp();
                    sampleTimeStamp = (r.first ? r.second : sampleTimeStamp);
                }
                {
                    if (VERBOSE) {
                        before = cluon::time::now();
                    }
                    x264_nal_t *nals{nullptr};
                    int i_nals{0};
                    picture_in.i_pts = i_frame++;
                    x264_picture_t picture_out;
                    int frameSize{x264_encoder_encode(encoder, &nals, &i_nals, &picture_in, &picture_out)};
                    if (0 < frameSize) {
                        data = std::string(reinterpret_cast<char*>(nals->p_payload), frameSize);
                    }
                    if (VERBOSE) {
                        after = cluon::time::now();
                    }
                }
                sharedMemory->unlock();

                if (!data.empty()) {
                    opendlv::proxy::ImageReading ir;
                    ir.fourcc("h264").width(WIDTH).height(HEIGHT).data(data);
                    od4.send(ir, sampleTimeStamp, ID);

                    if (VERBOSE) {
                        std::clog << "[opendlv-video-x264-encoder]: Frame size = " << data.size() << " bytes; sample time = " << cluon::time::toMicroseconds(sampleTimeStamp) << " microseconds; encoding took " << cluon::time::deltaInMicroseconds(after, before) << " microseconds." << std::endl;
                    }
                }
            }

            x264_encoder_close(encoder);
            retCode = 0;
        }
        else {
            std::cerr << "[opendlv-video-x264-encoder]: Failed to attach to shared memory '" << NAME << "'." << std::endl;
        }
    }
    return retCode;
}
