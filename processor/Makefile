cxx = g++
target = ProcessorServer
cxxflags = -g -I . -I msg -I kcp -I handle -Wall -O2

cur_dir = .
kcp_dir = kcp
msg_dir = msg
handle_dir = handle

dylib= -l glog
stdlib= uv/lib/macos_x64_i386/libuv.a 

src = $(wildcard $(cur_dir)/*.cc) \
          $(wildcard $(kcp_dir)/*.cc) \
              $(wildcard $(msg_dir)/*.cc) \
                  $(wildcard $(handle_dir)/*.cc)


obj = $(patsubst %.cc, %.o, $(src))

$(target):$(obj)
	$(cxx) $(obj) $(stdlib) -o $@ $(dylib) 


$(obj):%.o:%.cc
	$(cxx) -c $< -o $@ $(cxxflags) 

.PHONY:
	clean

clean:
	rm -rf *.o $(kcp_dir)/*.o $(msg_dir)/*.o $(handle_dir)/*.o $(target);

.PHONY: clean
