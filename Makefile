SWRasterizer: SWRasterizer.o BasicModel.o
	g++ -o SWRasterizer SWRasterizer.o BasicModel.o -pg
	
SWRasterizer.o: SWRasterizer.cpp BasicModel.h Model.h Triangle.h
	g++ -c -pg SWRasterizer.cpp
	
BasicModel.o: BasicModel.cpp BasicModel.h Triangle.h
	g++ -c -pg BasicModel.cpp

clean:
	rm SWRasterizer *.o