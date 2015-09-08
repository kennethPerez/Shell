/*
 * Proyecto Sistemas Operativos
 * Shell para Linux
 * Kenneth Perez Alfaro 2013096175
 * Fauricio Rojas Hernandez 2013235611
 * Semestre II - 2015
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

/** Mantiene la ruta actual de trabajo */
char* directorioActual = HOME;

void principalProcess(char* cadena);

/**
 * Corta un string, elimina la ultima palabra.
 * @param ruta: String a cortar.
 * @return String sin la ultima palabra.
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
 * @param cadena: Cadena a separar.
 * @return Lista con las palabras de dicha cadena.
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
 * Separa un string por operador or |.
 * @param cadena: Cadena a separar.
 * @return Lista con comandos en cada una de sus posiciones.
 */
list<string> splitByOr(char* cadena)
{
    list<string> lista;
    string find = string(cadena);
    
    if(find.find("|", 0) != string::npos)
    {
        stringstream ss(cadena);
        string s, comando = "";
        list<string> arreglo;
        
        while(getline(ss, s, ' '))
        {
            arreglo.push_back(s);
        }

        list<string>::iterator it = arreglo.begin();
        while(it != arreglo.end())
        {
            if(*it == "|")
            {
                lista.push_back(comando);
                comando = "";
                *it++;
            }
            else
            {
                comando += *it;
                *it++;

                if(*it != "|" && it != arreglo.end())
                    comando += " ";
            }
        }
        lista.push_back(comando);
    }
    
    return lista;
}

/**
 * Obtiene el contenido de una posición de la lista.
 * @param arreglo: Lista en la que se busca.
 * @param posicion: Posicion de la lista en la que se busca.
 * @return Contenido de la posicion de parámetro.
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
 * Apartir de una ruta, muestra la informacion del directorio.
 * @param comando: Comando a ejecutar.
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
 * A partir del directorio actual y de un nombre de archivo, se la informacion del directorio en el archivo
 * @param comando: Comando a ejecutar.
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
 * A partir de una ruta de un directorio, y un nombre de archivo, muestra la informacion del mismo.
 * @param comando: Comando a ejecutar.
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
 * Filtra el directorio actual mediante uno o varios grep
 * @param comando: Comando a ejecutar.
 */
void lsEspecial(char* comando)
{
    list<string> arreglo = splitByOr(comando);    
    
    list<string> letras;
    
    if(arreglo.size() >= 2)
    {
        for(int i = 0; i < arreglo.size(); i++)
        {
            char* comando = new char[length(getValueAtPosition(arreglo,i)) + 1];;
            strcpy(comando,getValueAtPosition(arreglo,i).c_str());
            list<string> elementos = split(comando);
            
            if(elementos.size() == 2)
            {
                if(i == 0)
                {
                    if(getValueAtPosition(elementos,0) != "ls" && getValueAtPosition(elementos,1) != "-1")
                    {
                        cout << "Orden no valido para <ls-Especial>." << endl;
                        break;
                    }
                }
                else
                {
                    if(getValueAtPosition(elementos,0) == "grep")
                        letras.push_back(getValueAtPosition(elementos,1));
                    else
                    {
                        cout << "Elemento no valido cerca de "<<getValueAtPosition(elementos,0)<<" para <ls-Especial>." << endl;
                        letras.clear();
                        break;
                    }
                }                
            }
            else
                if(elementos.size() > 2)
                    cout << "Sobran argumentos para <ls-Especial>." << endl;
                else
                    cout << "Faltan argumentos para <ls-Especial>." << endl;            
        }
        
        struct dirent *datosDirectorio;

        DIR* directorio = opendir(directorioActual);
        if(directorio != NULL)
        {
            list<string> elementosBuscados;
            while((datosDirectorio = readdir(directorio)))
            {
                for(int i = 0; i < letras.size();i++)
                {
                    if(strstr(datosDirectorio->d_name, getValueAtPosition(letras,i).c_str()) != NULL)
                        elementosBuscados.push_back(datosDirectorio->d_name);
                }                
            }
            elementosBuscados.unique();
            
            for(int i=0; i < elementosBuscados.size(); i++)
                cout << getValueAtPosition(elementosBuscados, i) << endl;
        }
    }
    else
    {
        if(arreglo.size() < 2)
            cout << "Faltan argumentos para la orden <ls-Especial>." << endl;
    }    
}

/**
 * Cambia la variable de directorio acutual segun, desee el usuario.
 * ~ : LLeva home
 * . : Lleva a raiz
 * .. : lleva al directorio arriba
 * @param comando: Comando a ejecutar.
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
 * Busca en el directorio indicado todos los archivos que en su nombre contengan la palabra indicada.
 * @param comando: Comando a ejecutar.
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
 * Si es del directorio actual nada mas se pone el nombre del archivo.
 * @param comando
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
                cout << " - Linea: "<<numero<<" "<<linea<<endl;
            }
            numero++;
        }
    }
}

/**
 * Obtiene el contenido de un archivo.
 * @param cadena: Nombre del archivo.
 * @return Contenido del archivo.
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
 * @param cadena: Los archivos a mostrar su contenido.
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
 * @param cadena: Los archivos a copiar y el archivo destino.
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

/**
 * Guarda en un archivo los comandos que se van ejecutando.
 * @param comando: Comando a guardar en el archivo.
 */
void guardarComando(char* comando)
{
    if(!strcmp(comando,"cmds") == 0)
    {
        ofstream archivo("Comandos.txt",ios::app);
        archivo << comando;
        archivo << '\n';
    }
}

/**
 * Procesa un comando.
 * @param cadena: Comando a ejecutar.
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
        
        ///-----------------------------------------------
        else if(arreglo[0] == "exit")
        {
            exit(1);
        }
        
        ///-----------------------------------------------
        else if(arreglo[0] == "cmds")
        {
            if(length(arreglo) == 2)
            {
                if(arreglo[1] == "del")
                {
                    remove("Comandos.txt"); 
                }
                else
                    cout << "Argumento invalido para la orden <cmds>." << endl;
            }
            else if(length(arreglo) == 1)
            {
                int i = 1;
                string linea;
                ifstream archivo("Comandos.txt",ios::in);

                while(getline(archivo, linea))
                {
                    cout << i<<". "<<linea<<endl;
                    i++;
                }

                archivo.close();
                
                if(i > 1)
                {
                    cout << "0. Salir" << endl;
                    char comando[256];
                    char* p;            
                    cout << "Digite el numero de comando: ";   
                    p = gets(comando);

                    int num = atoi(p);
                    if(num > 0)
                    {
                        ifstream archivo("Comandos.txt",ios::in);

                        i = 1;
                        while(getline(archivo, linea))
                        {
                            if(i == num)
                            {
                                char* p = new char[length(linea) + 1];
                                strcpy(p,linea.c_str());
                                principalProcess(p);
                            }
                            i++;
                        }
                    }      
                }
                else
                    cout << "No hay comandos almacenados." << endl;
            }
            else
            {
                if(length(arreglo) > 2)
                    cout << "Sobran argumentos para la orden <cmds>." << endl;
            }   
        }

        else
        {
            stringstream ss(getenv("PATH"));
            string s;
            list<string> arregloPath;

            while(getline(ss, s, ':'))
            {
                arregloPath.push_back(s);
            }

            for(int i = 0;i<arregloPath.size();i++)
            {
                char* ruta = new char[length(getValueAtPosition(arregloPath,i)) + 1];
                strcpy(ruta,getValueAtPosition(arregloPath,i).c_str());
                strcat(ruta,"/");
                strcat(ruta,arreglo[0].c_str());
                               
                if(system(ruta) == 0)
                {
                    system("clear");
                    return;
                }
            }
            system("clear");
            cout << "No se ha encontrado la orden <" + arreglo[0] + ">." << endl;
        }  
    }
    return;
}

/**
 * Procesa el comando insertado por el usuario.
 * @param cadena: Comando insertado por el usuario.
 */
void principalProcess(char* cadena)
{
    string sfind, comando_string;
    char * comando;
    
    sfind = string(cadena);
    guardarComando(cadena);
    if( (sfind.find("ls",0) != string::npos) && (sfind.find("-1",0) != string::npos) && (sfind.find("grep",0) != string::npos))
    {
        lsEspecial(cadena);
    }
    else
    {
        list<string> listaComandos = splitByOr(cadena);
    
        if(listaComandos.size() == 0)
        {
            listaComandos.push_back(cadena);
        }

        list<string>::iterator iterador = listaComandos.begin();
        while(iterador != listaComandos.end())
        {
            sfind = string(*iterador);

            if(sfind.find("&", 0) != string::npos)
            {
                cout << "*********** Proceso ejecutandose en segundo plano ***********" << endl;
            }
            else
            {
                comando_string = *iterador;
                comando = new char[length(comando_string) + 1];
                strcpy(comando,comando_string.c_str());

                processCommand(comando);
            }

            *iterador++;

            if(iterador != listaComandos.end())
            {
                cout << "*************************************************************" << endl;
            }    
        }
    }
}





main()
{
    char comando[256];
    char* p;

    do
    {
        getEntorno();
        
        p = gets(comando);
        
        principalProcess(p);
        p = NULL;

    }while(true);
    
}