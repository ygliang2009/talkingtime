cxx = g++
target = HarborServer
cxxflags = -g -I . -I msg -I kcp -I handle -I rtp -I /usr/local/include/jrtplib3 -I /usr/local/include/jthread -Wall -O2 -std=c++11

cur_dir = .
kcp_dir = kcp
msg_dir = msg
handle_dir = handle
rtp_dir = rtp

dylib= -l glog 
stdlib= uv/lib/macos_x64_i386/libuv.a jrtp/libjrtp.a  jrtp/libjthread.a

src = $(wildcard $(cur_dir)/*.cc) \
          $(wildcard $(kcp_dir)/*.cc) \
              $(wildcard $(msg_dir)/*.cc) \
                  $(wildcard $(handle_dir)/*.cc) \
                      $(wildcard $(rtp_dir)/*.cc)


obj = $(patsubst %.cc, %.o, $(src))

$(target):$(obj)
	$(cxx) $(obj) $(stdlib) -o $@ $(dylib) 


$(obj):%.o:%.cc
	$(cxx) -c $< -o $@ $(cxxflags) 

.PHONY:
	clean

clean:
	rm -rf *.o $(kcp_dir)/*.o $(msg_dir)/*.o $(handle_dir)/*.o $(rtp_dir)/*.o $(target);

.PHONY: clean
