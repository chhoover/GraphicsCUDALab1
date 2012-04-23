SWRasterizer: SWRasterizer.o BasicModel.o
	g++ -o SWRasterizer SWRasterizer.o BasicModel.o
	
SWRasterizer.o: SWRasterizer.cpp BasicModel.h Model.h Triangle.h
	g++ -c SWRasterizer.cpp
	
BasicModel.o: BasicModel.cpp BasicModel.h Triangle.h
	g++ -c BasicModel.cpp

clean:
	rm SWRasterizer *.o