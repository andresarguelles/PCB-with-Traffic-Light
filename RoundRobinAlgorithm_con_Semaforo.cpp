#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define NO_TAREAS 6
#define PAG_POR_TAREA 3
#define TAM_QUANTUM 5

int quantum = TAM_QUANTUM;
int tiempo = 0;
int tiempoTotal =0;
float tiempo_promedio_retorno = 0;
float quantum_ahorrado = 0;
int no_ciclo = 1;
bool hayProcesos = false;

struct nodo_tareas{
    int id_tarea;
    int localidad_pmt;
    int paginas[PAG_POR_TAREA];
    struct nodo_tareas *sig;
};

struct nodo_PCB{
    int id_proceso;
    int no_pag;
    int tiempo_llegada;
    int ciclos_CPU;
    int memoria;
    int cpu_ES;
    int recursos;
    int archivos;
    int tipo_cliente;
    int estado;
    int tiempo_espera;
    int tiempo_retorno;
    int cont_ciclo_sec_crit;
    int inicio_sec_crit;
    int duracion_sec_crit;
    struct nodo_PCB *sig;
};

nodo_tareas *Ptarea, *Qtarea, *NuevaTarea, *AuxTarea;
nodo_PCB *Pproceso, *Qproceso, *NuevoProceso, *AuxProceso, *AuxProceso2;
nodo_PCB *Psemaforo, *Qsemaforo, *NuevoSemaforo, *AuxSemaforo, *AuxSemaforo2;

void crear_lista_tareas();
void copiar_a_lista_tareas_semaforo(nodo_PCB *copiando);
void ver_lista_tareas();
void imprimir_tabla(nodo_PCB *cabeza);
void crear_lista_PCB();
void ver_lista_PCB();
void colocarEnMemoria();
void roundRobin();
void procesarTarea();
void eliminarNodoActual(nodo_PCB *nodo_actual);

int main(){
    srand(time(NULL));

    Pproceso=NULL;
    Qproceso=NULL;

    Ptarea = NULL;
    Qtarea = NULL;

    Psemaforo=NULL;
    Qsemaforo=NULL;

    crear_lista_tareas();
    ver_lista_tareas();
    while(getchar()!='\n');

    crear_lista_PCB();
    ver_lista_PCB();

    procesarTarea();
    printf("%15s%d%35s%.2f%25s%.2f\n\n", "Tiempo total: ", tiempoTotal, 
            "Tiempo promedio de retorno: ", tiempo_promedio_retorno,
            "Quantum ahorrado: ", quantum_ahorrado);

    return 0;
}

void crear_lista_tareas(void){
    int contadorTareas = 1;
    int loc_pmt = rand()%20+20;
    int j=0;

    do{
        if(Ptarea==NULL){
            Ptarea= (nodo_tareas *)malloc(sizeof(nodo_tareas));
            Ptarea->id_tarea = contadorTareas;
            Ptarea->localidad_pmt= loc_pmt+j;

            for(int i=0; i<PAG_POR_TAREA; i+=1){
                Ptarea->paginas[i] = rand()%5+1;
            }

            Ptarea->sig=NULL;
            Qtarea=Ptarea;

        }else{
            NuevaTarea= (nodo_tareas *)malloc(sizeof(nodo_tareas));
            NuevaTarea->id_tarea = contadorTareas;
            NuevaTarea->localidad_pmt = loc_pmt+j;

            for(int i=0; i<PAG_POR_TAREA; i+=1){
                NuevaTarea->paginas[i] = rand()%5+1;
            }

            NuevaTarea->sig=NULL;
            Qtarea->sig=NuevaTarea;
            Qtarea=NuevaTarea;
        }
        j++;
        contadorTareas++;
    }while(contadorTareas<=NO_TAREAS); 
}

void copiar_a_lista_tareas_semaforo(nodo_PCB *copiando){
    if(Psemaforo==NULL){
        Psemaforo= (nodo_PCB *)malloc(sizeof(nodo_PCB));
        Psemaforo->id_proceso          = copiando->id_proceso;
        Psemaforo->no_pag              = copiando->no_pag;
        Psemaforo->tiempo_llegada      = copiando->tiempo_llegada;
        Psemaforo->ciclos_CPU          = copiando->ciclos_CPU;
        Psemaforo->memoria             = copiando->memoria;
        Psemaforo->cpu_ES              = copiando->cpu_ES;
        Psemaforo->recursos            = copiando->recursos;
        Psemaforo->archivos            = copiando->archivos;
        Psemaforo->tipo_cliente        = copiando->tipo_cliente;
        Psemaforo->estado              = copiando->estado;
        Psemaforo->tiempo_espera       = copiando->tiempo_espera;
        Psemaforo->tiempo_retorno      = copiando->tiempo_retorno;
        Psemaforo->cont_ciclo_sec_crit = copiando->cont_ciclo_sec_crit;
        Psemaforo->inicio_sec_crit     = copiando->inicio_sec_crit;
        Psemaforo->duracion_sec_crit   = copiando->duracion_sec_crit;
        Psemaforo->sig=NULL;
        Qsemaforo=Psemaforo;
    }else{
        NuevoSemaforo = (nodo_PCB *)malloc(sizeof(nodo_PCB));
        NuevoSemaforo->id_proceso          = copiando->id_proceso;
        NuevoSemaforo->no_pag              = copiando->no_pag;
        NuevoSemaforo->tiempo_llegada      = copiando->tiempo_llegada;
        NuevoSemaforo->ciclos_CPU          = copiando->ciclos_CPU;
        NuevoSemaforo->memoria             = copiando->memoria;
        NuevoSemaforo->cpu_ES              = copiando->cpu_ES;
        NuevoSemaforo->recursos            = copiando->recursos;
        NuevoSemaforo->archivos            = copiando->archivos;
        NuevoSemaforo->tipo_cliente        = copiando->tipo_cliente;
        NuevoSemaforo->estado              = copiando->estado;
        NuevoSemaforo->tiempo_espera       = copiando->tiempo_espera;
        NuevoSemaforo->tiempo_retorno      = copiando->tiempo_retorno;
        NuevoSemaforo->cont_ciclo_sec_crit = copiando->cont_ciclo_sec_crit;
        NuevoSemaforo->inicio_sec_crit     = copiando->inicio_sec_crit;
        NuevoSemaforo->duracion_sec_crit   = copiando->duracion_sec_crit;
        NuevoSemaforo->sig                 = NULL;
        Qsemaforo->sig=NuevoSemaforo;
        Qsemaforo=NuevoSemaforo;
    }
}

void ver_lista_tareas() {
    AuxTarea =Ptarea;
    printf("\n%22s\n","LISTA DE TAREAS");
    printf("\n%-10s%-10s%9s\n", "ID.Tarea", "Loc.PMT", "Paginas");
    while(AuxTarea!=NULL) {
        printf(" J%d%10d     ", AuxTarea->id_tarea, AuxTarea->localidad_pmt);
        for(int i=0; i<PAG_POR_TAREA; i+=1) {
            printf(" P%d  ", AuxTarea->paginas[i]);
        }
        printf("\n");
        AuxTarea = AuxTarea->sig;
    }
}

void imprimir_tabla(nodo_PCB *cabeza) {
    nodo_PCB *AuxImpresion = NULL;
    AuxImpresion = cabeza;

    printf("\n%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n", "Proceso", "T.Llega", 
            "Ciclos", "Estado", "Memoria", "CPU/E/S","Num_Disp", "No.Arch", 
            "Tipo", "Ciclo SC", "Inicio SC", "Durac. SC");
    while(AuxImpresion!=NULL){
        char tipo[4], tipoCliente[8];
        switch(AuxImpresion->cpu_ES){
            case 0:  strcpy(tipo, "CPU");
                     break;
            case 1: strcpy(tipo, "E  ");
                    break;
            default: strcpy(tipo, "S  ");
                     break;
        }

        switch(AuxImpresion->tipo_cliente){
            case 0: strcpy(tipoCliente, "Sistema");
                    break;
            case 1: strcpy(tipoCliente, "Usuario");
                    break;
        }

        printf(" J%dP%-7d%2dt%9dms%9d%8dKB %9s%10d%9d%15s%10d%10d%10d\n", AuxImpresion->id_proceso, 
                AuxImpresion->no_pag, AuxImpresion->tiempo_llegada, AuxImpresion->ciclos_CPU, 
                AuxImpresion->estado, AuxImpresion->memoria, tipo, 
                AuxImpresion->recursos, AuxImpresion->archivos, tipoCliente, AuxImpresion->cont_ciclo_sec_crit,
                AuxImpresion->inicio_sec_crit, AuxImpresion->duracion_sec_crit);
        AuxImpresion=AuxImpresion->sig;
    }
    while(getchar()!='\n');
}

void crear_lista_PCB(void){
    int contadorProcesos=0;
    int tipoSeleccion;
    int cantRecursos;
    int numeroRandom;
    //Dado que no hay proceso, agarramos la primer tarea
    AuxTarea = Ptarea;
    while(AuxTarea!=NULL){
        int contadorPagina = 0;
        while(contadorPagina<PAG_POR_TAREA){
            tipoSeleccion=rand()%3;
            if(tipoSeleccion==0) cantRecursos=0;
            else cantRecursos = rand()%5+1;

            if(Pproceso==NULL){
                Pproceso= (nodo_PCB *)malloc(sizeof(nodo_PCB));
                Pproceso->id_proceso= AuxTarea->id_tarea;
                Pproceso->no_pag=AuxTarea->paginas[contadorPagina];
                Pproceso->tiempo_llegada= contadorProcesos;
                Pproceso->ciclos_CPU = rand()%11+5;
                numeroRandom = Pproceso->ciclos_CPU;
                Pproceso->memoria = rand()%5+1;
                Pproceso->cpu_ES = tipoSeleccion;
                Pproceso->recursos = cantRecursos;
                Pproceso->archivos = rand()%5+1;
                Pproceso->tipo_cliente = rand()%2;
                Pproceso->estado = 1;//Tiene 4 posibles estados 1, 2, 3 y 5
                Pproceso->tiempo_retorno = 0;
                Pproceso->cont_ciclo_sec_crit = 0;
                Pproceso->inicio_sec_crit = rand()%numeroRandom;
                /* Si la sección critica no existe no asignamos duración */
                if(Pproceso->inicio_sec_crit != 0)
                    Pproceso->duracion_sec_crit = rand()%3+1;
                else
                    Pproceso->duracion_sec_crit = 0;
                Pproceso->sig=NULL;
                Qproceso=Pproceso;

            }else{
                NuevoProceso=(nodo_PCB *)malloc(sizeof(nodo_PCB));
                NuevoProceso->id_proceso=AuxTarea->id_tarea;
                NuevoProceso->no_pag= AuxTarea->paginas[contadorPagina];
                NuevoProceso->tiempo_llegada= contadorProcesos;
                NuevoProceso->ciclos_CPU = rand()%11+5;
                numeroRandom = NuevoProceso->ciclos_CPU;
                NuevoProceso->memoria = rand()%5+1;
                NuevoProceso->cpu_ES = tipoSeleccion;
                NuevoProceso->recursos = cantRecursos;
                NuevoProceso->archivos = rand()%5+1;
                NuevoProceso->tipo_cliente = rand()%2;
                NuevoProceso->estado = 1;
                NuevoProceso->tiempo_retorno = 0;
                NuevoProceso->cont_ciclo_sec_crit = 0;
                NuevoProceso->inicio_sec_crit = rand()%numeroRandom;
                if(NuevoProceso->inicio_sec_crit ==0) NuevoProceso->duracion_sec_crit = 0;
                else NuevoProceso->duracion_sec_crit = rand()%3+1;
                NuevoProceso->sig=NULL;
                Qproceso->sig=NuevoProceso;
                Qproceso=NuevoProceso;
            }
            contadorProcesos++;
            contadorPagina++;
        }
        AuxTarea= AuxTarea->sig;
    }
}

void ver_lista_PCB(){
    AuxProceso =Pproceso;
    printf("\n%s%15s%3d%15s%3d%15s%3d\n","BLOQUE DE CONTROL DE PROCESOS","Quantum: "
            , quantum, "Tiempo: ", tiempo, "No.Ciclo: ", no_ciclo);
    printf("\n%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n", "Proceso", "T.Llega", 
            "Ciclos", "Estado", "Memoria", "CPU/E/S","Num_Disp", "No.Arch", 
            "Tipo", "Ciclo SC", "Inicio SC", "Durac. SC");
    while(AuxProceso!=NULL){
        char tipo[4], tipoCliente[8];
        switch(AuxProceso->cpu_ES){
            case 0:  strcpy(tipo, "CPU");
                     break;
            case 1: strcpy(tipo, "E  ");
                    break;
            default: strcpy(tipo, "S  ");
                     break;
        }

        switch(AuxProceso->tipo_cliente){
            case 0: strcpy(tipoCliente, "Sistema");
                    break;
            case 1: strcpy(tipoCliente, "Usuario");
                    break;
        }

        printf(" J%dP%-7d%2dt%9dms%9d%8dKB %9s%10d%9d%15s%10d%10d%10d\n",                   AuxProceso->id_proceso, 
                AuxProceso->no_pag, AuxProceso->tiempo_llegada, AuxProceso->ciclos_CPU, 
                AuxProceso->estado, AuxProceso->memoria, tipo, 
                AuxProceso->recursos, AuxProceso->archivos, tipoCliente, AuxProceso->cont_ciclo_sec_crit,
                AuxProceso->inicio_sec_crit, AuxProceso->duracion_sec_crit);
        AuxProceso=AuxProceso->sig;
    }
    while(getchar()!='\n');
}

void procesarTarea(){
    colocarEnMemoria();
    roundRobin();
}

void colocarEnMemoria(){
    AuxProceso = Pproceso;
    while(AuxProceso!=NULL){
        if(AuxProceso->estado==1 || AuxProceso->estado==4){
            AuxProceso->estado = 2;
            hayProcesos = true;
        }
        AuxProceso=AuxProceso->sig;
    }
    ver_lista_PCB();
}

void roundRobin() {
    while(hayProcesos){
        hayProcesos = false;
        AuxProceso2 = Pproceso;
        while(AuxProceso2!=NULL){
            quantum = 5;
            tiempo = AuxProceso2->tiempo_llegada;
            AuxProceso2->estado=3;
            while(AuxProceso2->ciclos_CPU>0 && quantum !=0){
                ver_lista_PCB();
                quantum--;
                tiempoTotal++;
                AuxProceso2->ciclos_CPU--;
                if(AuxProceso2->inicio_sec_crit>0){
                    AuxProceso2->cont_ciclo_sec_crit++;
                    if(AuxProceso2->cont_ciclo_sec_crit==AuxProceso2->inicio_sec_crit){
                        // Modificar el estado del proceso antes de copiar
                        AuxProceso2->estado = 4;
                        ver_lista_PCB();
                        // Meter info en la nueva tabla de semaforo
                        copiar_a_lista_tareas_semaforo(AuxProceso2);
                        imprimir_tabla(Psemaforo);
                        // Eliminar nodo de la tabla PCB
                        eliminarNodoActual(AuxProceso2);
                        goto salir;
                    }
                }
            } 
            if(AuxProceso2->ciclos_CPU==0){
                AuxProceso2->estado=5;
                if(AuxProceso2->tiempo_retorno==0){
                    AuxProceso2->tiempo_retorno = tiempoTotal - tiempo;
                    tiempo_promedio_retorno += (float)(tiempoTotal - tiempo);
                    quantum_ahorrado+= (float)quantum;
                }
            }
            else
                AuxProceso2->estado=4;
salir:
            AuxProceso2= AuxProceso2->sig;
        }
        colocarEnMemoria();
        no_ciclo++;
    }
    tiempo_promedio_retorno /=(float)(NO_TAREAS*PAG_POR_TAREA);
    quantum_ahorrado /= (float)TAM_QUANTUM;
}

void eliminarNodoActual(nodo_PCB *nodo_actual) {
    // Caso especial: si el nodo a eliminar es la cabeza
    if (Pproceso == nodo_actual) {
        Pproceso = nodo_actual->sig;
        // Si el nodo a eliminar también es el final, actualiza Qproceso
        if (Qproceso == nodo_actual) {
            Qproceso = NULL;
        }
        free(nodo_actual);  // Libera la memoria del nodo a eliminar
        return;
    }

    // Busca el nodo anterior al nodo_actual
    nodo_PCB *nodo_anterior = Pproceso;
    while (nodo_anterior->sig != nodo_actual && nodo_anterior != NULL) {
        nodo_anterior = nodo_anterior->sig;
    }

    // Verifica si se encontró el nodo anterior
    if (nodo_anterior != NULL) {
        // Conecta el nodo anterior con el siguiente, saltándose el nodo a eliminar
        nodo_anterior->sig = nodo_actual->sig;

        // Caso especial: si el nodo a eliminar es el final, actualiza Qproceso
        if (Qproceso == nodo_actual) {
            Qproceso = nodo_anterior;
        }

        free(nodo_actual);  // Libera la memoria del nodo a eliminar
    }
}
