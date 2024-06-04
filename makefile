CWTEXT=cwtext-0.96

all: cwtext ascii2pcm driver.cpp DualPipe.cpp Event.cpp LineReader.cpp StringUtils.cpp
	g++ -o driver -std=c++11 -pthread driver.cpp DualPipe.cpp Event.cpp LineReader.cpp StringUtils.cpp

release: cwtext ascii2pcmrel driver.cpp DualPipe.cpp Event.cpp LineReader.cpp StringUtils.cpp
	g++ -o driver -std=c++11 -pthread -Ofast driver.cpp DualPipe.cpp Event.cpp LineReader.cpp StringUtils.cpp

cwtext:
	(tar xzf ${CWTEXT}.tar.gz; patch -p0 < ${CWTEXT}.patch; cd ${CWTEXT}; make)

ascii2pcm: cwtext ascii2pcm.cpp
	g++ -o ascii2pcm ${CWTEXT}/morse.o ${CWTEXT}/pcm.o -std=c++11 ascii2pcm.cpp -I${CWTEXT}

ascii2pcmrel: cwtext ascii2pcm.cpp
	g++ -o ascii2pcm ${CWTEXT}/morse.o ${CWTEXT}/pcm.o -std=c++11 -Ofast ascii2pcm.cpp -I${CWTEXT}

clean: 
	rm driver ascii2pcm
	(cd cwtext-0.96; make clean)

