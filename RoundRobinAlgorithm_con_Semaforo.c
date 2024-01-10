#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define clrscr() printf("\e[1;1H\e[2J")

#define NO_TAREAS     6
#define PAG_POR_TAREA 3
#define TAM_QUANTUM   5
#define TRUE          1
#define FALSE         0

int quantum     = TAM_QUANTUM;
int tiempo      = 0;
int tiempoTotal = 0;
int no_ciclo    = 1;
int hayProcesos = FALSE;

typedef struct Nodo_Tareas {
    int id_tarea;
    int localidad_pmt;
    int paginas[PAG_POR_TAREA];
    struct Nodo_Tareas *sig;
} nodo_tareas;

typedef struct Nodo_PCB {
    int id_proceso;
    int no_pag;
    int tiempo_llegada;
    int ciclos_CPU;
    int estado;
    int interrupcion;
    int cont_ciclo_sec_crit;
    int inicio_sec_crit;
    int duracion_sec_crit;
    struct Nodo_PCB *sig;
} nodo_PCB;

typedef struct PCB_Semaforo {
    int id_proceso;
    int no_pag;
    int tiempo_llegada;
    int ciclos_CPU;
    int estado;
    int interrupcion;
    int cont_ciclo_sec_crit;
    int inicio_sec_crit;
    int duracion_sec_crit;
    int semaforo;
    int wait;
    int signal;
    struct PCB_Semaforo *sig;
} PCB_Semaforo;

nodo_tareas *Ptarea, *Qtarea, *NuevaTarea, *AuxTarea;
nodo_PCB *Pproceso, *Qproceso, *NuevoProceso, *AuxProceso, *AuxProceso2;
PCB_Semaforo *Psemaforo, *Qsemaforo, *NuevoSemaforo, *AuxSemaforo, *AuxSemaforo2;

void crear_lista_tareas();
void copiar_a_lista_tareas_semaforo(nodo_PCB *copiando);
void ver_lista_tareas();
void imprimir_tabla(PCB_Semaforo *cabeza);
void crear_lista_PCB();
void ver_lista_PCB();
void colocarEnMemoria();
void roundRobin();
void procesarTarea();
void eliminarNodoActual(nodo_PCB *nodo_actual);
void semaforo(void);
nodo_PCB* PCBaSemaforo(PCB_Semaforo *cabeza);
void copiarAlPCB(PCB_Semaforo *cabeza);

int main(){
    srand(time(NULL));

    Pproceso=NULL;
    Qproceso=NULL;

    Ptarea = NULL;
    Qtarea = NULL;

    Psemaforo=NULL;
    Qsemaforo=NULL;

    clrscr();
    crear_lista_tareas();
    ver_lista_tareas();
    while(getchar()!='\n');

    crear_lista_PCB();
    ver_lista_PCB();

    procesarTarea();

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

            int i;
            for(i=0; i<PAG_POR_TAREA; i+=1){
                Ptarea->paginas[i] = rand()%5+1;
            }

            Ptarea->sig=NULL;
            Qtarea=Ptarea;

        }else{
            NuevaTarea= (nodo_tareas *)malloc(sizeof(nodo_tareas));
            NuevaTarea->id_tarea = contadorTareas;
            NuevaTarea->localidad_pmt = loc_pmt+j;

            int i;
            for(i=0; i<PAG_POR_TAREA; i+=1){
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
        Psemaforo= (PCB_Semaforo *)malloc(sizeof(PCB_Semaforo));
        Psemaforo->id_proceso          = copiando->id_proceso;
        Psemaforo->no_pag              = copiando->no_pag;
        Psemaforo->tiempo_llegada      = copiando->tiempo_llegada;
        Psemaforo->ciclos_CPU          = copiando->ciclos_CPU;
        Psemaforo->estado              = copiando->estado;
        Psemaforo->interrupcion        = copiando->interrupcion;
        Psemaforo->cont_ciclo_sec_crit = copiando->cont_ciclo_sec_crit;
        Psemaforo->inicio_sec_crit     = copiando->inicio_sec_crit;
        Psemaforo->duracion_sec_crit   = copiando->duracion_sec_crit;
        Psemaforo->semaforo            = 0;
        Psemaforo->wait                = 1;
        Psemaforo->signal              = 0;
        Psemaforo->sig                 = NULL;
        Qsemaforo=Psemaforo;
    }else{
        NuevoSemaforo = (PCB_Semaforo *)malloc(sizeof(PCB_Semaforo));
        NuevoSemaforo->id_proceso          = copiando->id_proceso;
        NuevoSemaforo->no_pag              = copiando->no_pag;
        NuevoSemaforo->tiempo_llegada      = copiando->tiempo_llegada;
        NuevoSemaforo->ciclos_CPU          = copiando->ciclos_CPU;
        NuevoSemaforo->estado              = copiando->estado;
        NuevoSemaforo->interrupcion        = copiando->interrupcion;
        NuevoSemaforo->cont_ciclo_sec_crit = copiando->cont_ciclo_sec_crit;
        NuevoSemaforo->inicio_sec_crit     = copiando->inicio_sec_crit;
        NuevoSemaforo->duracion_sec_crit   = copiando->duracion_sec_crit;
        NuevoSemaforo->semaforo            = 0;
        NuevoSemaforo->wait                = 1;
        NuevoSemaforo->signal              = 0;
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
        int i;
        for(i=0; i<PAG_POR_TAREA; i+=1) {
            printf(" P%d  ", AuxTarea->paginas[i]);
        }
        printf("\n");
        AuxTarea = AuxTarea->sig;
    }
}

void imprimir_tabla(PCB_Semaforo *cabeza) {
    PCB_Semaforo *AuxImpresion = NULL;
    AuxImpresion = cabeza;

    printf("\n%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n",
            "Proceso", "T.Llega", "Ciclos", "Estado", "Interrup", "Ciclo SC", "Inicio SC",
            "Durac. SC", "Semaf", "Wait   ", "Signal");
    while(AuxImpresion!=NULL){

        printf(" J%dP%-7d%2dt%9dms%9d%10d%10d%10d%10d%10d%10d%10d\n",
                AuxImpresion->id_proceso, AuxImpresion->no_pag,
                AuxImpresion->tiempo_llegada, AuxImpresion->ciclos_CPU, 
                AuxImpresion->estado, AuxImpresion->interrupcion, AuxImpresion->cont_ciclo_sec_crit,
                AuxImpresion->inicio_sec_crit, AuxImpresion->duracion_sec_crit,
                AuxImpresion->semaforo,
                AuxImpresion->wait, AuxImpresion->signal);
        AuxImpresion=AuxImpresion->sig;
    }
    while(getchar()!='\n');
}

void crear_lista_PCB(void){
    int contadorProcesos=0;
    //Dado que no hay proceso, agarramos la primer tarea
    AuxTarea = Ptarea;
    //Interrupciones
       //Error: 
            //0 - Division entre 0
            //1 - Debug exception
            //4 - Overflow
       //De entrada y/o salida:
            //33 - Entrada/Teclado PS/2
            //38 - Controlador de video
    int vectorInterrupciones[6]={-1, 0, 1, 4, 33, 38};
    while(AuxTarea!=NULL){
        int contadorPagina = 0;
        while(contadorPagina<PAG_POR_TAREA){
            if(Pproceso==NULL){
                Pproceso= (nodo_PCB *)malloc(sizeof(nodo_PCB));
                Pproceso->id_proceso= AuxTarea->id_tarea;
                Pproceso->no_pag=AuxTarea->paginas[contadorPagina];
                Pproceso->tiempo_llegada= contadorProcesos;
                Pproceso->ciclos_CPU = (rand() % 11) + 5;
                Pproceso->estado = 1;//Tiene 4 posibles estados 1, 2, 3 y 5
                Pproceso->interrupcion = vectorInterrupciones[rand()%7];
                Pproceso->cont_ciclo_sec_crit = 0;
                if(Pproceso->interrupcion!=-1){
                    Pproceso->inicio_sec_crit = rand() % Pproceso->ciclos_CPU;
                    /* Si la Sec. Crit. 3 posiciones antes del número de ciclos
                    * de CPU creamos una duración de SC entre 1 y 3 */
                    if((Pproceso->ciclos_CPU - Pproceso->inicio_sec_crit) > 2)
                        Pproceso->duracion_sec_crit = (rand() % 3) + 1;
                    else {
                        Pproceso->duracion_sec_crit = (rand() %
                            (Pproceso->ciclos_CPU - Pproceso->inicio_sec_crit) + 1);
                    }
                }else{
                    Pproceso->inicio_sec_crit = 0;
                    Pproceso->duracion_sec_crit = 0;
                }
                Pproceso->sig=NULL;
                Qproceso=Pproceso;

            }else{
                NuevoProceso=(nodo_PCB *)malloc(sizeof(nodo_PCB));
                NuevoProceso->id_proceso=AuxTarea->id_tarea;
                NuevoProceso->no_pag= AuxTarea->paginas[contadorPagina];
                NuevoProceso->tiempo_llegada= contadorProcesos;
                NuevoProceso->ciclos_CPU = (rand() % 11) + 5;
                NuevoProceso->estado = 1;
                NuevoProceso->interrupcion= vectorInterrupciones[rand()%7];
                NuevoProceso->cont_ciclo_sec_crit = 0;
                if(NuevoProceso->interrupcion!=-1){
                    NuevoProceso->inicio_sec_crit = rand() % NuevoProceso->ciclos_CPU;
                    if((NuevoProceso->ciclos_CPU - NuevoProceso->inicio_sec_crit) > 2)
                        NuevoProceso->duracion_sec_crit = (rand() % 3) + 1;
                    else {
                        NuevoProceso->duracion_sec_crit = (rand() %
                            (NuevoProceso->ciclos_CPU - NuevoProceso->inicio_sec_crit) + 1);
                    }
                }else{
                    NuevoProceso->inicio_sec_crit =0;
                    NuevoProceso->duracion_sec_crit=0;
                }
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
    clrscr();
    AuxProceso =Pproceso;
    printf("\n%s%15s%3d%15s%3d%15s%3d\n","BLOQUE DE CONTROL DE PROCESOS",
            "Quantum: ", quantum, "Tiempo: ", tiempo, "No.Ciclo: ", no_ciclo);
    printf("\n%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n", "Proceso", "T.Llega", 
            "Ciclos", "Estado", "Interrup", "Ciclo SC", "Inicio SC", "Durac. SC");
    while(AuxProceso!=NULL){
        printf(" J%dP%-7d%2dt%9dms%9d%10d%10d%10d%10d\n", AuxProceso->id_proceso, 
                AuxProceso->no_pag, AuxProceso->tiempo_llegada,
                AuxProceso->ciclos_CPU, AuxProceso->estado,
                AuxProceso->interrupcion,
                AuxProceso->cont_ciclo_sec_crit, AuxProceso->inicio_sec_crit,
                AuxProceso->duracion_sec_crit);
        AuxProceso=AuxProceso->sig;
    }
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
            hayProcesos = TRUE;
        }
        AuxProceso=AuxProceso->sig;
    }
    ver_lista_PCB();
}

void roundRobin() {
    while(hayProcesos) {
        hayProcesos = FALSE;
        AuxProceso2 = Pproceso;
        while(AuxProceso2!=NULL){
            quantum = 5;
            tiempo = AuxProceso2->tiempo_llegada;
            AuxProceso2->estado=3;
            while(AuxProceso2->ciclos_CPU>0 && quantum !=0){
                ver_lista_PCB();
                imprimir_tabla(Psemaforo);
                // Indica que se trata de un proceso E/S
                if(AuxProceso2->interrupcion==33 || AuxProceso2->interrupcion==38){
                    if(AuxProceso2->cont_ciclo_sec_crit==AuxProceso2->inicio_sec_crit){
                        // Modificar el estado del proceso antes de copiar
                        AuxProceso2->estado = 4;
                        ver_lista_PCB();
                        // Meter info en la nueva tabla de semaforo
                        copiar_a_lista_tareas_semaforo(AuxProceso2);
                        // Eliminar nodo de la tabla PCB
                        eliminarNodoActual(AuxProceso2);
                        imprimir_tabla(Psemaforo);
                        goto salir;
                    }
                    AuxProceso2->cont_ciclo_sec_crit++;
                }
                //En caso de que no sea de entrada y/o salida 
                //O bien, que sea distinto de codigo -1
                else if(AuxProceso2->interrupcion!=-1) {
                    clrscr();
                    printf("\n\tEl proceso J%dP%d sale en el tiempo %d\n\tCodigo %d: ",
                        AuxProceso2->id_proceso, AuxProceso2->no_pag,
                        AuxProceso2->tiempo_llegada, AuxProceso2->interrupcion);
                    switch (AuxProceso2->interrupcion){
                        case 0: printf("Division por cero.\n");
                            break;
                        case 1: printf("Excepcion de depuracion.\n");
                            break;
                        case 4: printf("Desbordamiento del buffer.\n");
                            break;
                    }
                    while(getchar()!='\n');
                    eliminarNodoActual(AuxProceso2);
                    //Sucede que cuando se cumple una interrupcion, se sale sin usar semaforo
                    //y se sale hasta la linea 369
                    semaforo();
                    goto salir;
                    }
                semaforo();
                quantum--;
                tiempoTotal++;
                AuxProceso2->ciclos_CPU--;
            }
            if(AuxProceso2->ciclos_CPU == 0) {
                AuxProceso2->estado = 5;
            }
            else
                AuxProceso2->estado=4;
        salir:
            AuxProceso2= AuxProceso2->sig;
        }
        colocarEnMemoria();
        no_ciclo++;
    }
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

void semaforo(void) {
    if(Psemaforo != NULL) {
        // Si el proceso ingresó con wait = 1 y la duración de SC > 0
        if(Psemaforo->wait && (Psemaforo->semaforo == 0)
                && (Psemaforo->duracion_sec_crit > 0)) {
            Psemaforo->semaforo--;
            Psemaforo->duracion_sec_crit--;
            Psemaforo->ciclos_CPU--;
            Psemaforo->wait = 0;
        }
        // Si ya se comenzó a decrementar la SC continuamos decrementando 
        else if (Psemaforo->wait == 0 && (Psemaforo->semaforo == -1)
                && (Psemaforo->duracion_sec_crit > 0)) {
            Psemaforo->duracion_sec_crit--;
            Psemaforo->ciclos_CPU--;
        }
        // Si ya se terminó la SC mandamos señal de signal
        else if(Psemaforo->wait == 0 && (Psemaforo->semaforo == -1) &&
                (Psemaforo->duracion_sec_crit == 0) && (Psemaforo->signal == 0)) {
            Psemaforo->signal = 1;
        }
        // Si tenemos una señal de signal y ya no hay SC eliminamos nodo cabeza
        else if (Psemaforo->signal && (Psemaforo->duracion_sec_crit == 0)) {
            if(Psemaforo->ciclos_CPU>0){
                Psemaforo->estado = 4;
                copiarAlPCB(Psemaforo);
            }
            Psemaforo = Psemaforo->sig; // La nueva cabeza es el siguiente
            free(Psemaforo);
        }
    }
}

void copiarAlPCB(PCB_Semaforo *cabeza){
    if(Pproceso && Pproceso->sig) {
        nodo_PCB *temporal, *temporal2;
        temporal = (nodo_PCB *)malloc(sizeof(nodo_PCB));
        temporal2 = (nodo_PCB *)malloc(sizeof(nodo_PCB));
        temporal = Pproceso;
        temporal2 = Pproceso->sig;
        nodo_PCB *semaforoCopia = PCBaSemaforo(cabeza);
        if(temporal->tiempo_llegada > semaforoCopia->tiempo_llegada){
            semaforoCopia->sig = temporal;
            Pproceso = semaforoCopia;
        }else if(semaforoCopia->tiempo_llegada > Qproceso->tiempo_llegada){
            Qproceso->sig = semaforoCopia;
            Qproceso = semaforoCopia;
        }else{
            while(temporal2->sig != NULL) {
                if(semaforoCopia->tiempo_llegada > temporal->tiempo_llegada) {
                    temporal->sig = semaforoCopia;
                    semaforoCopia->sig = temporal2;
                }
                temporal  = temporal->sig;
                temporal2 = temporal2->sig;
            }
        }
    }
}

nodo_PCB* PCBaSemaforo(PCB_Semaforo *cabeza){
    nodo_PCB *semaforoCopia = (nodo_PCB *)malloc(sizeof(nodo_PCB));
    semaforoCopia->id_proceso = cabeza->id_proceso;
    semaforoCopia->no_pag = cabeza->no_pag;
    semaforoCopia->tiempo_llegada = cabeza->tiempo_llegada;
    semaforoCopia->ciclos_CPU = cabeza->ciclos_CPU;
    semaforoCopia->estado= cabeza->estado;
    semaforoCopia->interrupcion = cabeza->interrupcion;
    semaforoCopia->cont_ciclo_sec_crit = cabeza->cont_ciclo_sec_crit;
    semaforoCopia->inicio_sec_crit = cabeza->inicio_sec_crit;
    semaforoCopia->duracion_sec_crit = cabeza->duracion_sec_crit;
    return semaforoCopia;
}
