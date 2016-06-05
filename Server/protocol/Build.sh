#!/bin/bash
Target="../proto_out/"
sed -i '/option optimize_for = LITE_RUNTIME;/d' *.proto
for i in $(ls *.proto)
do
    echo "protoc convert $i"
    protoc --cpp_out=$Target $i;
done
sed -i '/package GameJayo/a\option optimize_for = LITE_RUNTIME;' *.proto

cd $Target
rename .pb.cc .cpp *
rename .pb.h .h *
#rename -f 's/\.pb.cc/\.cpp/' *
#rename -f 's/\.pb.h/\.h/' *
sed -i 's/\.pb.h\>/.h/g' *.cpp
sed -i 's/\.pb.h\>/.h/g' *.h

cd ../proto_out && make -j2
