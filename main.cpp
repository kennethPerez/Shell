/*
 * Proyecto Sistemas Operativos
 * Shell para Linux
 * Kenneth Perez Alfaro
 * Fauricio Rojas Hernandez
 */



#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <list>

#define length(x) (sizeof(x)/sizeof(x[0]))
#define HOME getenv("HOME")

using namespace std;
using std::string;
extern "C" int gethostname (char*, int);

/// Mantiene la ruta actual de trabajo
char* directorioActual = HOME;

/**
* Corta una cadena de chars, elimina el ultimo directorio.
* Ej: ruta = /home/kenneth  retorna /home
* Parámetros:
*    ruta: Cadena a recortar.
* Retorno:
*    ruta: Cadena recortada.
*/
char* cortarCadena(char* ruta)
{
    stringstream ss(ruta);
    stringstream tt(ruta);
    string s, t, rute;
    int cantidadPalabras = 0;

    while(getline(ss, s, '/')) {
        cantidadPalabras++;
    }

    int i=0;
    while(getline(tt, t, '/'))
    {
        if(i < cantidadPalabras-1)
        {
            rute += t;
            i++;

            if(i < cantidadPalabras-1)
                rute += "/";
        }
    }

    ruta = new char[length(rute) + 1];
    strcpy(ruta,rute.c_str());

    if(strcmp(ruta,"") == 0)
        strcpy(ruta,"/");
    return ruta;
}

/**
* Separa una cadena por espacios.
* Parámetros:
*    cadena: Cadena a separar.
* Retorna:
*    arreglo: Lista con las palabras de dicha cadena.
*/
list<string> split(char* cadena)
{
    stringstream ss(cadena);
    string s;
    list<string> arreglo;

    while(getline(ss, s, ' '))
    {
        arreglo.push_back(s);
    }

    return arreglo;
}

/**
* Obtiene el contenido de una posición de la lista.
* Parámetros:
*    arreglo: Lista en la que se busca.
*    posicion: Posicion de la lista en la que se busca.
* Retorno:
*    Contenido de la posicion de parámetro.
*/
string getValueAtPosition(list<string> arreglo, int posicion)
{
    list<string>::iterator it = arreglo.begin();

    int i=0;
    while(it != arreglo.end())
    {
        if(i == posicion)
            return *it++;
        else
        {
            it++;
            i++;
        }
    }
    return "";
}

/**
* Apartir del directorio actual, se obtine el entorno de trabajo.
* Ej: kenneth@master:~$
*/
void getEntorno()
{
    char* directorio = directorioActual;
    if(strcmp (directorio, HOME) == 0)
        directorio = "~";

    char hostname[256];
    gethostname (hostname, sizeof(hostname));

    string ambiente = getenv("USER") + string("@") + hostname + string(":") + directorio + string("$ ");

    cout<<ambiente;
}

/**
* Apartir de una ruta, muestra la informacion del directorio
*/
void ls(char* comando)
{    
    list<string> arreglo = split(comando);
    
    struct dirent *datosDirectorio; /// Estructura que almacena la informacion del directorio

    DIR *directorio;
    
    /// Obtener el directorio
    if(strcmp(getValueAtPosition(arreglo,1).c_str(),"-1") == 0)
        directorio = opendir(directorioActual);
    else
        directorio = opendir(getValueAtPosition(arreglo,1).c_str()); 

    if(directorio != NULL)
    {
        while((datosDirectorio = readdir(directorio)))
        {
            if ( !strcmp(datosDirectorio->d_name, ".") || !strcmp(datosDirectorio->d_name, "..") || datosDirectorio->d_name[0]=='.')
                continue;
            else
                printf("%-20s\n", datosDirectorio->d_name);
        }
        closedir(directorio);
    }
    else
        printf("%s no existe el directorio\n",getValueAtPosition(arreglo,1).c_str());

}

/**
* Apartir del directorio actual y de un nombre de archivo, se la informacion del directorio en el archivo
*/
void ls_GuardarArchivo(char* comando)
{
    list<string> arreglo = split(comando);
    
    string ruta = directorioActual+string("/")+getValueAtPosition(arreglo,2);
    
    ofstream archivo(ruta.c_str());

    struct dirent *datosDirectorio;

    DIR* directorio = opendir(directorioActual);
    if(directorio != NULL)
    {
        while((datosDirectorio = readdir(directorio)))
        {
            if ( !strcmp(datosDirectorio->d_name, ".") || !strcmp(datosDirectorio->d_name, "..") || datosDirectorio->d_name[0]=='.')
                continue;
            else
            {
                archivo << datosDirectorio->d_name;
                archivo << '\n';
            }
        }
        closedir(directorio);
    }
    else
        printf("%s no existe el directorio\n\n",directorioActual);
    
}

/**
* Apartir de una ruta de un directorio, y un nombre de archivo, muestra la informacion del mismo
*/
void ls_InfoArchivo(char* comando)
{
    list<string> arreglo = split(comando);
    
    struct dirent *datosDirectorio;
    struct stat estruturaPermisos;

    string ruta = directorioActual+string("/")+getValueAtPosition(arreglo,2);

    ifstream File(ruta.c_str());

    if(File != NULL)
    {
        DIR* directorio = opendir(directorioActual);

        while((datosDirectorio = readdir(directorio)))
        {
            if (strcmp(datosDirectorio->d_name, getValueAtPosition(arreglo,2).c_str())==0)
            {
                stat(datosDirectorio->d_name, &estruturaPermisos);
                printf( (S_ISDIR(estruturaPermisos.st_mode)) ? "d" : "-" );
                printf( (estruturaPermisos.st_mode & S_IRUSR) ? "r" : "-" );//Lectura
                printf( (estruturaPermisos.st_mode & S_IWUSR) ? "w" : "-" );//Escritura
                printf( (estruturaPermisos.st_mode & S_IXUSR) ? "x" : "-" );//Ejecucion
                printf( (estruturaPermisos.st_mode & S_IRGRP) ? "r" : "-" );//Lectura Grupo
                printf( (estruturaPermisos.st_mode & S_IWGRP) ? "w" : "-" );//Escritura Grupo
                printf( (estruturaPermisos.st_mode & S_IXGRP) ? "x" : "-" );//Ejecucion Grupo
                printf( (estruturaPermisos.st_mode & S_IROTH) ? "r" : "-" );//Lectura otros
                printf( (estruturaPermisos.st_mode & S_IWOTH) ? "w" : "-" );//Escritura otros
                printf( (estruturaPermisos.st_mode & S_IXOTH) ? "x" : "-" );//Ejecucion otros
                printf("%-3s");
                printf("%-20s\n", datosDirectorio->d_name);
            }
        }

         closedir(directorio);
    }
    else
        printf("%s no existe el Archivo en el directorio %s\n\n",getValueAtPosition(arreglo,2).c_str(),directorioActual);

}

/**
* Cambia la variable de directorio acutual segun, desee el usuario.
* ~ : LLeva home
* . : Lleva a raiz
* .. : lleva al directorio arriba
* <ruta> : lleva al directorio indicado en ruta
*/
void cd(char* comando)
{
    list<string> arreglo = split(comando);
    
    if(strcmp(getValueAtPosition(arreglo,1).c_str(), "~") == 0)
        directorioActual = HOME;

    else if(strcmp(getValueAtPosition(arreglo,1).c_str(), ".") == 0)
        directorioActual = "/";

    else if(strcmp(getValueAtPosition(arreglo,1).c_str(), "..") == 0)
        directorioActual = cortarCadena(directorioActual);

    else
    {
        if(opendir(getValueAtPosition(arreglo,1).c_str()))
        {
            char* archivo = new char[length(getValueAtPosition(arreglo,1)) + 1];
            strcpy(archivo,getValueAtPosition(arreglo,1).c_str());
            
            directorioActual = archivo;
        }
        else
            printf("%s no existe el directorio\n",getValueAtPosition(arreglo,1).c_str());
    }
}

/**
* Busca en el directorio indicado todos los archivos que en su nombre contengan la palabra indicada
*/
void grepDirectorio(char* comando)
{
    list<string> arreglo = split(comando);    
    
    struct dirent *datosDirectorio;

    DIR* directorio = opendir(getValueAtPosition(arreglo,2).c_str());
    if(directorio != NULL)
    {
        while((datosDirectorio = readdir(directorio)))
        {
            if(strstr(datosDirectorio->d_name, getValueAtPosition(arreglo,1).c_str()) != NULL)
                printf("%-20s\n", datosDirectorio->d_name);
        }
    }
    else
    {
        printf("\n%s no existe el directorio\n",getValueAtPosition(arreglo,2).c_str());
    }
    closedir(directorio);
}

/**
* Busca la palabra en la ruta del archivo dada.
* Si es del directorio actual nada mas se pone el nombre del archivo
*/
void grepArchivo(char* comando)
{
    list<string> arreglo = split(comando);    
    
    string linea;
    int numero = 1;

    string directorio;

    if(strstr(getValueAtPosition(arreglo,2).c_str(), "/") != NULL)
        directorio = getValueAtPosition(arreglo,2);
    else
        directorio = directorioActual+string("/")+getValueAtPosition(arreglo,2);
    
    ifstream File(directorio.c_str());
    
    if(File.fail())
        printf("\n%s no existe el directorio\n",getValueAtPosition(arreglo,2).c_str());
    else
    {
        while(getline(File, linea))
        {
            if (linea.find(getValueAtPosition(arreglo,1), 0) != string::npos)
            {
                cout << " - Line: "<<numero<<" "<<linea<<endl;
            }
            numero++;
        }
    }
}

/**
* Obtiene el contenido de un archivo.
* Parámetros:
*    cadena: Nombre del archivo.
* Retorno:
*    contenido: Contenido del archivo.
*/
string obtenerContenidoArchivo(string cadena)
{
    string linea, contenido = "", directorio;
    directorio = string(directorioActual)+string("/")+string(cadena);
    ifstream archivo(directorio.c_str(),ios::in);

    if(archivo.fail())
    {
        return "Error al abrir el archivo " + cadena + "\n";
    }
    else
    {
        while(getline(archivo, linea))
        {
            contenido += linea + "\n";
        }
        archivo.close();
        return contenido;
    }
}

/**
* Muestra el contenido de uno o varios archivos.
* Parámetros:
*    cadena: Los archivos a mostrar contenido.
*/
void catMostrar(char* cadena)
{
    list<string> arreglo = split(cadena);
    arreglo.pop_front();
    list<string>::iterator it = arreglo.begin();

    while(it != arreglo.end())
    {
        cout << obtenerContenidoArchivo(*it++);
    }
}

/**
* Copia el contenido de uno o varios archivos en otro.
* Parámetros:
*    cadena: Los archivos a copiar y el archivo destino.
*/
void catCopiar(char* cadena)
{
    list<string> arreglo = split(cadena);
    arreglo.pop_front();
    list<string>::iterator it = arreglo.begin();
    string linea, contenido = "", directorio;
    char* texto;
    
    while(it != arreglo.end())
    {
        linea = *it;
        texto = new char[length(linea) + 1];
        strcpy(texto, linea.c_str());

        if(!strcmp(texto,">") == 0){
            contenido += obtenerContenidoArchivo(linea);
        }
        else
        {
            it++;
            linea = *it;
            directorio = string(directorioActual)+string("/")+string(linea);
            ofstream archivo(directorio.c_str());

            archivo << contenido << endl;
        }
        it++;
    }
}

/*****************************************************************************************************/
/**
* Procesa un comando obtenido de consola.
* Parámetros:
*    cadena: Comando a procesar.
*/
void processCommand(char* cadena)
{
    stringstream ss(cadena);
    stringstream tt(cadena);
    string s, t;
    int cantidadPalabras = 0;

    while(getline(ss, s, ' '))
    {
        cantidadPalabras++;
    }

    string arreglo[cantidadPalabras];

    int i=0;
    while(getline(tt, t, ' '))
    {
        if(i < cantidadPalabras)
        {
            arreglo[i] = t;
            i++;
        }
    }

    if(length(arreglo) > 0)
    {
        ///-----------------------------------------------
        if(arreglo[0] == "ls")
        {
            if(length(arreglo) == 2)
            {
                if(arreglo[1] == "-1")
                    ls(cadena);
                else
                    ls(cadena);
            }
            else if(length(arreglo) == 3)
            {
                if(arreglo[1] == ">")
                    ls_InfoArchivo(cadena);
                else if(arreglo[1] == ">>")
                    ls_GuardarArchivo(cadena);
                else
                    cout << "Error en <ls>, esperado '>>', recibido '"<<arreglo[1]<<"'"<< endl;
            }
            else
            {
                if(length(arreglo) > 3)
                    cout << "Sobran argumentos para la orden <ls>." << endl;
                else
                    cout << "Faltan argumentos para la orden <ls>." << endl;
            }
        }

        ///-----------------------------------------------
        else if(arreglo[0] == "grep")
        {
            if(length(arreglo) == 3)
            {
                string Stringcadena = string(cadena);
                
                if(Stringcadena.find(".", 0) != string::npos)
                    grepArchivo(cadena);
                else
                    grepDirectorio(cadena);
            }
            else
            {
                if(length(arreglo) > 3)
                    cout << "Sobran argumentos para la orden <greb>." << endl;
                else
                    cout << "Faltan argumentos para la orden <greb>." << endl;
            }
        }

        ///-----------------------------------------------
        else if(arreglo[0] == "cat")
        {
            string cadena1 = string(cadena);
            int mayor = 0;
            if(cadena1.find(">", 0) != string::npos)
            {
                if(length(arreglo) >= 3)
                {
                    for(int i=1; i<length(arreglo); i++)
                    {
                        if(arreglo[i] == ">")
                            mayor++;
                        if(i == length(arreglo)-2)
                        {
                            if(mayor == 1)
                            {
                                if(arreglo[i] == ">")
                                {
                                    cout << "GO TO COPY" << endl;
                                    catCopiar(cadena);
                                }
                                else
                                {
                                    cout << "Sobran/faltan argumentos para la orden <cat>." << endl;
                                    break;
                                }
                            }
                            else if(mayor == 0)
                            {
                                cout << "Sobran/faltan argumentos para la orden <cat>." << endl;
                                break;
                            }
                            else
                            {
                                cout << "Sobran argumentos para la orden <cat>." << endl;
                                break;
                            }
                        }
                    }
                }
                else
                    cout << "Faltan argumentos para la orden <cat>." << endl;
            }
            else
            {
                if(length(arreglo) >= 2)
                    catMostrar(cadena);
                else
                    cout << "Faltan argumentos para la orden <cat>." << endl;
            }
        }

        ///-----------------------------------------------
        else if(arreglo[0] == "cd")
        {
            if(length(arreglo) == 2)
            {
                cd(cadena);
            }
            else
            {
                if(length(arreglo) > 2)
                    cout << "Sobran argumentos para la orden <cd>." << endl;
                else
                    cout << "Faltan argumentos para la orden <cd>." << endl;
            }
        }

        ///-----------------------------------------------
        else if(arreglo[0] == "clear")
        {
            system("clear");
        }

        else
            cout << "No se ha encontrado la orden <" + arreglo[0] + ">." << endl;
    }
    return;
}

main()
{ 
    char comando[256];
    char* p;

    do
    {
        getEntorno();
        fflush(stdin); 
        p = gets(comando);

        processCommand(p);
        p = NULL;

    }while(true);

   /*

    grepArchivo("a", "/home/kenneth/Descargas/h.txt");
    
    cd("/home/kenneth/clusterdir");
    ls(directorioActual);

    ls_InfoArchivo("/home/kenneth/clusterdir","primos.c++");
    grepDirectorio("p", "/home/kenneth/");
    
    catMostrar("cat datos.txt");

    getEntorno();

    */
}