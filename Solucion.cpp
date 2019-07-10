#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <Windows.h>

using namespace std;

// Integrantes: Enrique Miguel Torrijos Gabriel (1ºG) y Daniel Piña Miguelsanz (1ºG)
// Curso: 2016 - 2017

const int MAX = 50;
const int MAXH = 10;
const int MAXE = 100;

typedef enum tCasilla
{
	Libre, Muro, DestinoL, DestinoC, DestinoJ, Jugador, Caja
};//enum tCasilla

typedef enum tTecla
{
	Arriba, Abajo, Derecha, Izquierda, Salir, Nada, Deshacer
};//enum tTecla

typedef struct tTablero
{
	tCasilla casilla[MAX][MAX];
	int nFilas;
	int nColumnas;
};//struct tTablero

typedef struct tSokoban
{
	tTablero tablero;
	int posJugadorFila;
	int posJugadorColumna;
	int numCajas;
	int numCajasDestino;
};//struct tSokoban

typedef struct tJuego
{
	tSokoban sokoban;
	int numMovimientos;
	string nFichero;
	int nivel;
};//struct tJuego

typedef struct tHistoria
{
	tJuego juego[MAXH];
	int ultimaPos; // la última pos usada de la memoria (al final será MAXH - 1)
};//struct tHistoria

typedef struct tPartida
{
	string nombreFichero;
	int level;
	int numPasos;
};//struct tPartida

typedef struct tExitos
{
	tPartida partida[MAXE];//Debe estar ordenado
};//struct tExito

typedef struct tInfo
{
	string nombre;
	tExitos exitos;
	int partidasResueltas;
};//struct tInfo

void inicializa(tJuego &juego) //Limpia todo el tablero, el registro de movimientos ...
{
	int i, j;
	for (i = 0; i < MAX; ++i)
	{
		for (j = 0; j < MAX; ++j)
		{
			juego.sokoban.tablero.casilla[i][j] = Libre;
		}//for
	}//for
	juego.sokoban.tablero.nFilas = 0;
	juego.sokoban.tablero.nColumnas = 0;
	juego.numMovimientos = 0;
	juego.sokoban.numCajas = 0;
	juego.sokoban.numCajasDestino = 0;
}//void inicializa

tCasilla cargarCasilla(char letra) //Distingue entre las distintas casillas posibles
{
	tCasilla casilla;
	switch (letra)
	{
	case '#':
		casilla = Muro;
		break;
	case ' ':
		casilla = Libre;
		break;
	case '.':
		casilla = DestinoL;
		break;
	case '$':
		casilla = Caja;
		break;
	case '@':
		casilla = Jugador;
		break;
	case '*':
		casilla = DestinoC;
		break;
	case '+':
		casilla = DestinoJ;
		break;
	}//switch
	return casilla;
}// tCasilla cargarCasilla

 //Se da por sentado que antes de usar esta funcion, se usa la funcion void inicializa
bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int &nivel) //De un archivo abierto, busca el nivel solicitado
{
	bool resultado, levelEncontrado = false;
	string nombre = "Level ";
	string lectura;
	char letra;
	int i = 0, j;

	nombre += to_string(nivel);

	getline(fichero, lectura);
	while (!fichero.eof() && lectura != nombre)//Buscamos el nivel solicitado
		getline(fichero, lectura);
	if (lectura == nombre)//El nivel existe en el fichero de texto
	{
		getline(fichero, lectura);
		while (lectura != "")
		{
			for (j = 0; j < lectura.length(); ++j)
			{
				letra = lectura.at(j);//Para poder comprobar que hay en esa parte del nivel, un muro, una caja...
				sokoban.tablero.casilla[i][j] = cargarCasilla(letra);
				if (sokoban.tablero.casilla[i][j] == Jugador)
				{
					sokoban.posJugadorFila = i;
					sokoban.posJugadorColumna = j;
				}//if
				if (sokoban.tablero.casilla[i][j] == Caja)
					++sokoban.numCajas;
				if (sokoban.tablero.casilla[i][j] == DestinoC)
				{
					++sokoban.numCajas;
					++sokoban.numCajasDestino;
				}//if
				levelEncontrado = true;
			}//for
			if (sokoban.tablero.nColumnas < j)
				sokoban.tablero.nColumnas = j;
			++i;
			getline(fichero, lectura);
		}//while
		sokoban.tablero.nFilas = i;
		resultado = true;
	}//if
	else
		resultado = false;
	if (!levelEncontrado)	//Si el archivo existe pero el nivel no, entonces no puede empezar a jugar
		resultado = false;
	return resultado;
}//bool cargarNivel

bool cargarJuego(tJuego &juego) //Le pide al usuario un archivo y un nivel y el programa carga el sokoban
{
	bool resultado = false;
	int nivel;
	string nombre;
	ifstream fichero;
	cout << "Por favor, introduzca el nombre del archivo en el que desea buscar el nivel (no necesita escribir la extension):\n";
	cin >> nombre;
	cout << "Por favor, introduzca el nivel que desea que se busque (el 0, el 1 ...):\n";
	cin >> nivel;

	nombre += ".txt";
	juego.nFichero = nombre;
	juego.nivel = nivel;

	fichero.open(nombre);
	if (fichero.is_open())
	{
		inicializa(juego);
		if (cargarNivel(fichero, juego.sokoban, nivel))
			resultado = true;
		fichero.close();
	}//if
	else
		resultado = false;
	return resultado;
}//bool cargarJuego

void colorFondo(int color)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, 15 | (color << 4));
}//void colorFondo

void dibujaCasilla(tCasilla casilla) {	//Muestra una casilla de tablero

	switch (casilla) {
	case Libre:
		colorFondo(1);
		cout << setw(2) << " ";
		break;
	case Muro:
		colorFondo(2);
		cout << setw(2) << " ";
		break;
	case DestinoL:
		colorFondo(12);
		cout << setw(2) << ".";
		break;
	case DestinoC:
		colorFondo(12);
		cout << setw(2) << "()";
		break;
	case DestinoJ:
		colorFondo(6);
		cout << setw(2) << "00";
		//cout << numMovimientos;
		break;
	case Jugador:
		colorFondo(6);
		cout << setw(2) << "00";
		break;
	case Caja:
		colorFondo(7);
		cout << setw(2) << "()";
		break;
	}//switch
}//void dibujaCasilla

void dibujar(const tJuego &juego) //Muestra tablero, nombreFich, nivel y numMov
{
	int i, j;
	system("cls");

	cout << "Fichero: " << juego.nFichero << "   Nivel: " << juego.nivel << "\n";

	for (i = 0; i < juego.sokoban.tablero.nFilas; ++i) {
		for (j = 1; j < juego.sokoban.tablero.nColumnas; ++j) {
			dibujaCasilla(juego.sokoban.tablero.casilla[i][j]);
		}//for
		cout << "\n";
	}//for
	colorFondo(0);
	cout << "Numero de movimientos: " << juego.numMovimientos << "\n";
}//void dibujar

tTecla leerTecla() //Reconoce las teclas especiales
{
	tTecla tecla;
	int dir = 0;

	cin.sync();
	dir = _getch();
	if (dir == 0xe0) //No es la tecla 'Esc'
		dir = _getch();//Guarda la tecla de direccion

	if (!(dir == 72) || !(dir == 80) || !(dir == 77) || !(dir == 75) || !(dir == 27))
		switch (dir)
		{
		case 72:
			tecla = Arriba;
			break;
		case 80:
			tecla = Abajo;
			break;
		case 77:
			tecla = Derecha;
			break;
		case 75:
			tecla = Izquierda;
			break;
		case 27:
			tecla = Salir;
			break;
		case 100:
		case 68:
			tecla = Deshacer;
			break;
		default:
			tecla = Nada;
			break;
		}//switch
	else
		tecla = Nada;
	return tecla;
}//tTecla

void hacerMovimiento(tJuego &juego, tTecla tecla) //Realiza el movimiento por el mapeado
{
	int fDestino, cDestino, fCaja, cCaja;
	bool movimiento = true;
	int f = juego.sokoban.posJugadorFila;
	int c = juego.sokoban.posJugadorColumna;

	switch (tecla)
	{
	case Arriba:
		fDestino = f - 1;
		cDestino = c;
		fCaja = f - 2;
		cCaja = c;
		break;
	case Abajo:
		fDestino = f + 1;
		cDestino = c;
		fCaja = f + 2;
		cCaja = c;
		break;
	case Derecha:
		fDestino = f;
		cDestino = c + 1;
		fCaja = f;
		cCaja = c + 2;
		break;
	case Izquierda:
		fDestino = f;
		cDestino = c - 1;
		fCaja = f;
		cCaja = c - 2;
		break;
	case Nada:
		fDestino = f;
		cDestino = c;
		fCaja = f;
		cCaja = c;
		break;
	case Salir:
		movimiento = false;
		break;
	}//switch

	if (movimiento)
	{
		switch (juego.sokoban.tablero.casilla[fDestino][cDestino]) {
		case Libre:
			juego.sokoban.tablero.casilla[fDestino][cDestino] = Jugador;
			if (juego.sokoban.tablero.casilla[f][c] == DestinoJ)
				juego.sokoban.tablero.casilla[f][c] = DestinoL;
			else
				juego.sokoban.tablero.casilla[f][c] = Libre;
			break;
		case Muro:
			movimiento = false;
			break;
		case DestinoL:
			juego.sokoban.tablero.casilla[fDestino][cDestino] = DestinoJ;
			if (juego.sokoban.tablero.casilla[f][c] == DestinoJ)
				juego.sokoban.tablero.casilla[f][c] = DestinoL;
			else
				juego.sokoban.tablero.casilla[f][c] = Libre;
			break;
		case DestinoC:
			if (juego.sokoban.tablero.casilla[fCaja][cCaja] == Libre) {
				juego.sokoban.tablero.casilla[fCaja][cCaja] = Caja;
				--juego.sokoban.numCajasDestino; //Sacamos una caja de una posición destino
				juego.sokoban.tablero.casilla[fDestino][cDestino] = DestinoJ;
				if (juego.sokoban.tablero.casilla[f][c] == DestinoJ)
					juego.sokoban.tablero.casilla[f][c] = DestinoL;
				else
					juego.sokoban.tablero.casilla[f][c] = Libre;
			}//if
			else if (juego.sokoban.tablero.casilla[fCaja][cCaja] == DestinoL) {
				juego.sokoban.tablero.casilla[fCaja][cCaja] = DestinoC;
				juego.sokoban.tablero.casilla[fDestino][cDestino] = DestinoJ;
				if (juego.sokoban.tablero.casilla[f][c] == DestinoJ)
					juego.sokoban.tablero.casilla[f][c] = DestinoL;
				else
					juego.sokoban.tablero.casilla[f][c] = Libre;
			}//if
			else
				movimiento = false;
			break;
		case Caja:
			if (juego.sokoban.tablero.casilla[fCaja][cCaja] == Libre) {
				juego.sokoban.tablero.casilla[fCaja][cCaja] = Caja;
				juego.sokoban.tablero.casilla[fDestino][cDestino] = Jugador;
				if (juego.sokoban.tablero.casilla[f][c] == DestinoJ)
					juego.sokoban.tablero.casilla[f][c] = DestinoL;
				else
					juego.sokoban.tablero.casilla[f][c] = Libre;
			}//if
			else if (juego.sokoban.tablero.casilla[fCaja][cCaja] == DestinoL) {
				juego.sokoban.tablero.casilla[fCaja][cCaja] = DestinoC;
				++juego.sokoban.numCajasDestino; //Introducimos una caja en una posición destino
				juego.sokoban.tablero.casilla[fDestino][cDestino] = Jugador;
				if (juego.sokoban.tablero.casilla[f][c] == DestinoJ)
					juego.sokoban.tablero.casilla[f][c] = DestinoL;
				else
					juego.sokoban.tablero.casilla[f][c] = Libre;
			}//if
			else
				movimiento = false;
			//case Jugador y/o DestinoJ no se puede dar;
		} //switch
		if (tecla != Nada)
			++juego.numMovimientos;
	}//if
	if (movimiento)
	{
		juego.sokoban.posJugadorFila = fDestino;
		juego.sokoban.posJugadorColumna = cDestino;
		if (juego.sokoban.tablero.casilla[f][c] == Jugador)
			juego.sokoban.tablero.casilla[f][c] = Jugador;
	}

	if (!movimiento && tecla != Salir)
		cout << "No se ha podido realizar el movimiento.\n";

}//void hacerMovimiento

bool bloqueado(const tJuego &juego) //Detecta si alguna caja se ha quedado bloqueada
{
	bool bloqueado = false;
	bool accesoArriba, accesoAbajo, accesoIzquierda, accesoDerecha;
	int i = 1, j = 1;
	//Encontrar una caja
	while ((!bloqueado) && (i < juego.sokoban.tablero.nFilas - 1))
	{
		j = 1;
		while ((!bloqueado) && (j < juego.sokoban.tablero.nColumnas - 1))
		{
			accesoArriba = true;
			accesoAbajo = true;
			accesoIzquierda = true;
			accesoDerecha = true;
			if (juego.sokoban.tablero.casilla[i][j] == Caja)
			{
				//Comprobar que tiene alrededor
				if ((juego.sokoban.tablero.casilla[i][j - 1] == Muro) ||
					(juego.sokoban.tablero.casilla[i][j - 1] == Caja) ||
					(juego.sokoban.tablero.casilla[i][j - 1] == DestinoC))
					accesoArriba = false;
				if ((juego.sokoban.tablero.casilla[i][j + 1] == Muro) ||
					(juego.sokoban.tablero.casilla[i][j + 1] == Caja) ||
					(juego.sokoban.tablero.casilla[i][j + 1] == DestinoC))
					accesoAbajo = false;
				if ((juego.sokoban.tablero.casilla[i - 1][j] == Muro) ||
					(juego.sokoban.tablero.casilla[i - 1][j] == Caja) ||
					(juego.sokoban.tablero.casilla[i - 1][j] == DestinoC))
					accesoIzquierda = false;
				if ((juego.sokoban.tablero.casilla[i + 1][j] == Muro) ||
					(juego.sokoban.tablero.casilla[i + 1][j] == Caja) ||
					(juego.sokoban.tablero.casilla[i + 1][j] == DestinoC))
					accesoDerecha = false;

				//Decir si esta bloqueada o no
				if (!accesoArriba && !accesoIzquierda)
					bloqueado = true;
				if (!accesoArriba && !accesoDerecha)
					bloqueado = true;
				if (!accesoAbajo && !accesoIzquierda)
					bloqueado = true;
				if (!accesoAbajo && !accesoDerecha)
					bloqueado = true;

			}//if
			++j;
		}//while
		++i;
	}//while	
	return bloqueado;
}//bool bloqueado

void modHistorial(tHistoria &historial, tTecla tecla) //En esta función modificamos el historial y hacemos
													  //un movimiento
{
	//Si todavía estaba alguna posición de la memoria sin usar, la habilitamos
	if (historial.ultimaPos < MAXH - 1)
		++historial.ultimaPos;
	for (int i = historial.ultimaPos; i > 0; --i) //Desplazamos todos los tableros del array
												  //una posición a la derecha
		historial.juego[i] = historial.juego[i - 1];
	hacerMovimiento(historial.juego[0], tecla);
}//void modHistorial

void deshacerMovimiento(tHistoria &historial) //carga el tablero inmediatamente anterior del array
{
	for (int i = 0; i < historial.ultimaPos; ++i)
		//Desplazamos todos los tableros una posición a la derecha del array
		historial.juego[i] = historial.juego[i + 1];
	if (historial.ultimaPos > 0) //Vamos vaciando el historial
		--historial.ultimaPos;
}//void deshacerMovimiento

bool cargarInfoJugador(tInfo &info) //Dado un nombre por el usuario, cargamos un archivo .txt si existe
									//AVISO: si había otros datos útiles antes de la función, se pierden
{
	bool carga;
	ifstream fich;
	info.partidasResueltas = 0;
	int i = 0;
	string jugador, prueba;

	jugador = info.nombre + ".txt";

	fich.open(jugador);
	if (fich.is_open())
	{
		while (!fich.eof()) // Leemos datos del archivo .txt hasta que se termine
		{

			fich >> info.exitos.partida[i].nombreFichero;
			fich >> info.exitos.partida[i].level;
			fich >> info.exitos.partida[i].numPasos;
			++i;
		}//while
		--i;
		info.partidasResueltas = i;
		carga = true;
		fich.close();
	}//if
	else
		carga = false;
	return carga;
}// bool cargarInfoJugador

void guardarInfo(const tInfo &info) //Dado un nombre, la info se guarda en un archivo .txt
									//AVISO: si existe un archivo con ese nombre, se modifica
{
	ofstream fich;
	string nombre;

	nombre = info.nombre + ".txt";
	fich.open(nombre);
	if (fich.is_open())
	{
		for (int i = 0; i < info.partidasResueltas; ++i) // Guardamos todos los datos del array en un .txt
		{
			fich << info.exitos.partida[i].nombreFichero << " ";
			fich << info.exitos.partida[i].level << " ";
			fich << info.exitos.partida[i].numPasos << "\n";
		}//for
		fich.close();
	}//if
}//void guardarInfo

void mostrarInfo(const tInfo &info) //Muestra en pantalla los datos guardados en info
{
	int i;
	if (info.partidasResueltas == 0)
	{
		cout << "Todavia no has superado ningun nivel, no te preocupes, algunos mapas son dificiles de "
			<< "resolver, pero con esfuerzo y dedicacion los superaras.\n";
	}//if
	else
	{
		cout << "Actualmente esta almacenada la informacion de " << info.nombre << ", ha superado un "
			<< "total de " << info.partidasResueltas << " niveles distintos, que son: \n";

		if (info.partidasResueltas == 1)
		{
			cout << "Del archivo " << info.exitos.partida[0].nombreFichero << " "
				<< "el nivel " << info.exitos.partida[0].level << " "
				<< "en un total de " << info.exitos.partida[0].numPasos << " pasos.\n";
		}//if
		else
			for (i = 0; i < info.partidasResueltas; ++i)
			{
				cout << "Del archivo " << info.exitos.partida[i].nombreFichero << " "
					<< "el nivel " << info.exitos.partida[i].level << " "
					<< "en un total de " << info.exitos.partida[i].numPasos << " pasos.\n";
			}//for
	}//else
}//void mostrarInfo

void modificarInfo(tInfo &info, tJuego &juego) //Modifica los datos guardados de un jugador
{
	tPartida partidaAuxiliar;
	string nombre;
	int i = 0, j = 0, posNueva;
	bool posExiste = false; // posExiste indica si ese nivel ya se ha jugado anteriormente
	bool finArray = false; // finArray indica si se ha llegado a la ultima posición del array

	while (!posExiste && i < info.partidasResueltas)
	{
		if (info.exitos.partida[i].nombreFichero == juego.nFichero &&
			info.exitos.partida[i].level == juego.nivel)						//Si ya se jugó ese nivel
		{
			posExiste = true;
			if (info.exitos.partida[i].numPasos >= juego.numMovimientos) // Si se ha hecho en menos pasos
																		 // se guarda el cambio
				info.exitos.partida[i].numPasos = juego.numMovimientos;
		}//if
		++i;
	}//while

	if (!posExiste)	//Ese nivel no se ha terminado antes
	{
		++info.partidasResueltas;	//Aumenta en 1 el tamaño del array

									//Guardamos en la nueva posición del array la nueva info
		partidaAuxiliar.nombreFichero = juego.nFichero;
		partidaAuxiliar.level = juego.nivel;
		partidaAuxiliar.numPasos = juego.numMovimientos;

		if (info.partidasResueltas == 1)
		{
			info.exitos.partida[0] = partidaAuxiliar;
		}//if
		else
		{
			//Introducimos la nueva info en el array en la posición concreta
			i = 0;
			j = 0;
			while (((info.exitos.partida[i].nombreFichero < partidaAuxiliar.nombreFichero) ||
				((info.exitos.partida[i].nombreFichero == partidaAuxiliar.nombreFichero) &&
				(info.exitos.partida[i].level < partidaAuxiliar.level))) && !finArray)
			{
				++i;
				if (i < info.partidasResueltas)
					finArray = true;
			}//while
			posNueva = i;
			for (i = info.partidasResueltas; i > posNueva; --i)
			{
				info.exitos.partida[i] = info.exitos.partida[i - 1];
			}//for
			info.exitos.partida[posNueva] = partidaAuxiliar;
		}//else
	}//if

	guardarInfo(info);
}//void modificarInfo

int menu(int &opcion, tHistoria &historial, tInfo &info) //Comprueba que la opcion introducida existe
{
	tTecla tecla;
	while ((opcion > 3) || (opcion < 0))
	{
		cout << "La opcion introducida no es valida, por favor, introduzca otra opcion:\n";
		cin >> opcion;
	}//while
	switch (opcion)
	{
	case 1:	// Jugar una partida
		while (!cargarJuego(historial.juego[0]))
		{
			cout << "El archivo o el nivel no existen, por favor, asegurese de que estan en la misma "
				<< "carpeta que el juego antes de iniciar el mismo, escriba un archivo y un nivel "
				<< "que existan:\n";
		}//while

		dibujar(historial.juego[0]);
		tecla = Nada;

		while (historial.juego[0].sokoban.numCajas != historial.juego[0].sokoban.numCajasDestino &&
			tecla != Salir)
		{
			cout << "Si crees que no puedes terminar el nivel, pulsa Esc para salir del mapa.\n";
			if (bloqueado(historial.juego[0]))
				cout << "Ten cuidado, hay alguna caja bloqueada.Algunos mapas para resolverlos hay que "
				<< "dejar alguna caja bloqueada durante unos pasos, pero revisa por si acaso el "
				<< "tablero para evitar que te quedes pillado.\n";
			tecla = leerTecla();
			if (tecla == Deshacer)
			{
				deshacerMovimiento(historial);
				dibujar(historial.juego[0]);
			}//if
			else
			{
				modHistorial(historial, tecla);
				dibujar(historial.juego[0]);
			}//else
		}//while
		if (tecla != Salir) // Ha terminado la partida
			modificarInfo(info, historial.juego[0]);
		break;
	case 2:	//Muestra la info del jugador
		mostrarInfo(info);
		break;
	case 3:	//Indica como se juega al Sokoban
		cout << "El Sokoban es un conocido rompecabezas creado por el japones Hiroyuki Imabayashi en 1981. "
			<< "El juego se desarrolla en un tablero que representa un almacen con cajas y un empleado (tu, de "
			<< "color verde) debe empujar dichas cajas (son de color gris) hasta unas determinadas posiciones "
			<< "destino (rojas) evitando muros (verde intenso). Cuando las cajas estan en una posicion "
			<< "destino pasan a ser rojas, y cuando todas estan en una posicion destino, se acaba la partida.\n";
		break;
	}//switch
	return opcion;
}//int menu

int main()
{
	tHistoria historial;
	tInfo info;
	int opcion;

	historial.ultimaPos = 0;
	historial.juego[0].numMovimientos = 0;
	historial.juego[0].sokoban.numCajas = 0;
	historial.juego[0].sokoban.numCajasDestino = 0;

	cout << "Hola jugador, en unos breves instantes podra jugar al Sokoban, sin embargo, antes de empezar, "
		"me gustaria que me dijera un nombre para poder llamarle de alguna forma y para crear o cargar "
		"sus progresos durante esta sesion: \n";
	cin >> info.nombre;
	if (cargarInfoJugador(info))
		cout << "Bienvenido de nuevo " << info.nombre << ", me alegro de que vuelvas a echarte unas partiditas.\n\n";

	else		// No hay un jugador con este nombre guardado en el archivo, por lo tanto, hay que establecer
				// que tiene un total de '0' partidas resueltas
	{
		cout << "No encuentro ningun dato tuyo en el registro, " << info.nombre << ", pero no te preocupes, "
			<< "no soy un programa dificil de usar.\n\n";
		info.partidasResueltas = 0;
	}//else
	cout << "De acuerdo, " << info.nombre << ", estas son las siguientes opciones que puedes hacer:\n"
		"1.- Empezar una partida (se te pedira un archivo del que cargar el nivel y el nivel concreto).\n"
		"2.- Mostrar el registro del usuario actual, es decir, los niveles que has superado entre otros datos.\n"
		"3.- Instrucciones de como se juega (es interesante si no has jugado nunca al Sokoban).\n"
		"0.- Salir y guardar los datos.\n\n";

	cin >> opcion;
	while (opcion != 0)
	{
		menu(opcion, historial, info);

		cout << "Que opcion desea que se ejecute:\n"
			<< "1.- Empezar una partida.\n"
			<< "2.- Mostrar el registro del usuario actual.\n"
			<< "3.- Como jugar.\n"
			<< "0.- Salir y guardar los datos.\n";
		cin >> opcion;
	}//while
	guardarInfo(info);
	return 0;
}//int main