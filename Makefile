SWRasterizer: SWRasterizer.o BasicModel.o
	nvcc -o SWRasterizer SWRasterizer.o BasicModel.o
	
SWRasterizer.o: SWRasterizer.cu BasicModel.h Model.h Triangle.h
	nvcc -c SWRasterizer.cu
	
BasicModel.o: BasicModel.cpp BasicModel.h Triangle.h
	g++ -c BasicModel.cpp

clean:
	rm SWRasterizer *.o
