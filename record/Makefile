cxx = g++
target = RecordServer
cxxflags = -g -I . -I rtp -I codec -I /usr/local/include/jrtplib3 -I /usr/local/include/jthread -I/usr/local/include -Wall -O2 -std=c++11

cur_dir = .
rtp_dir = rtp
codec_dir = codec

dylib= -lglog -lavformat -lavcodec -lavdevice -lavutil -lavresample -lswscale 
stdlib= jrtp/libjrtp.a  jrtp/libjthread.a

src = $(wildcard $(cur_dir)/*.cc) \
          $(wildcard $(rtp_dir)/*.cc) \
              $(wildcard $(codec_dir)/*.cc)


obj = $(patsubst %.cc, %.o, $(src))

$(target):$(obj)
	$(cxx) $(obj) $(stdlib) -o $@ $(dylib) 


$(obj):%.o:%.cc
	$(cxx) -c $< -o $@ $(cxxflags) 

.PHONY:
	clean

clean:
	rm -rf *.o $(rtp_dir)/*.o $(codec_dir)/*.o $(target);

.PHONY: clean
