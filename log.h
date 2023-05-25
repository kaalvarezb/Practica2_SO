#include <stdio.h>
#include <time.h>
void escribirLog(const char* mensaje) {
   // Obtiene la hora actual
   time_t tiempo = time(NULL);
   struct tm* tiempoLocal = localtime(&tiempo);

   // Abre el archivo de registro en modo "agregar" (append)
   FILE* archivo = fopen("registro.log", "a");

   // Verifica si se pudo abrir el archivo correctamente
   if (archivo == NULL) {
      printf("Error al abrir el archivo de registro.\n");
      return;
   }

   // Escribe la fecha y hora actual en el archivo
   fprintf(archivo, "[%02d-%02d-%d %02d:%02d:%02d] %s\n",
           tiempoLocal->tm_mday, tiempoLocal->tm_mon + 1, tiempoLocal->tm_year + 1900,
           tiempoLocal->tm_hour, tiempoLocal->tm_min, tiempoLocal->tm_sec, mensaje);

   // Cierra el archivo
   fclose(archivo);
}
