all: project

clean:
	rm -rf ./build
	rm -rf ./dist

project:
	mkdir -p ./build
	mkdir -p ./dist
	g++ -Wall Logger.cpp -c -o ./build/Logger.o
	g++ -Wall Semaforo.cpp -c -o ./build/Semaforo.o
	g++ -Wall ./build/Logger.o ./build/Semaforo.o initializer.cpp -o ./dist/initializer
	g++ -Wall ./build/Logger.o ./build/Semaforo.o puerta.cpp -o ./dist/puerta
	g++ -Wall ./build/Logger.o ./build/Semaforo.o destroyer.cpp -o ./dist/destroyer
	g++ -Wall ./build/Logger.o ./build/Semaforo.o abrirMuseo.cpp -o ./dist/abrirMuseo
	g++ -Wall ./build/Logger.o ./build/Semaforo.o cerrarMuseo.cpp -o ./dist/cerrarMuseo