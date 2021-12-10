# Use an official Python runtime as a parent image
FROM python:3.7

# install debian packages
RUN	apt-get update -qq -y 		&& 	\
	apt-get install -q -y 			\
	binutils 						\
	g++ 							\
	make 							\
	ant 						 	\
	libboost-all-dev			&&	\
	apt-get clean 				&& 	\ 
	rm -rf /var/lib/apt/lists/* 

# download, compile and install Z3
RUN	git clone https://github.com/Z3Prover/z3.git z3

RUN	cd z3						&& \	
	python scripts/mk_make.py	&& \
	cd build 					&& \
	make						&& \
	make install 

RUN	cd z3/build/	&& \	
	make examples 			

CMD	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/z3/build/	&& \
	cd app												&& \
	make												&& \
	./build/a.out						

# docker build -t hadwiger_nelson .
# docker run -it --rm -v "$(pwd)":/app --user 1000 hadwiger_nelson 