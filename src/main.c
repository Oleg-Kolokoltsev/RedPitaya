


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rp.h"

#define M_PI 3.14159265358979323846

int main() {
// Inicializar la Red Pitaya
if (rp_Init() != RP_OK) {
fprintf(stderr, "Error al inicializar la Red Pitaya.\n");
return EXIT_FAILURE;
}

// Configurar los generadores de señal
rp_GenReset();
rp_GenOutDisable(RP_CH_1);
rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
rp_GenFreq(RP_CH_1, 1000.0);  // Frecuencia de la señal de salida (1 kHz)
rp_GenAmp(RP_CH_1, 1.0);     // Amplitud de la señal de salida (1V)

// Configurar los adquisidores de señal
rp_AcqReset();
rp_AcqSetDecimation(1);
rp_AcqSetSamplingRate(RP_SMP_44_1K);  // Frecuencia de muestreo de 44.1 kHz
rp_AcqSetTriggerLevel(0.1);           // Nivel de disparo (0.1V)

// Variables para almacenar los datos de entrada y salida
float* inputBuffer = (float*)malloc(rp_AcqGetBufSize() * sizeof(float));
float* outputBuffer = (float*)malloc(rp_AcqGetBufSize() * sizeof(float));

// Bucle principal para adquirir y procesar datos en tiempo real
while (1) {
// Adquirir datos de entrada
rp_AcqStart();
rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);
usleep(1000);
rp_AcqGetOldestDataV(RP_CH_1, &rp_AcqGetBufSize(), inputBuffer);

// Procesar datos y aplicar desfase de 90 grados
for (int i = 0; i < rp_AcqGetBufSize(); i++) {
float phaseShiftedValue = inputBuffer[i] * cos(90.0 * M_PI / 180.0) - inputBuffer[i] * sin(90.0 * M_PI / 180.0);
outputBuffer[i] = phaseShiftedValue;
}

// Escribir datos procesados en la salida analógica
rp_GenWaveform(RP_CH_2, RP_WAVEFORM_ARBITRARY);
rp_GenArbWaveform(RP_CH_2, outputBuffer, rp_AcqGetBufSize());

// Esperar un tiempo antes de la siguiente iteración (ajusta el bucle a la frecuencia de muestreo)
usleep(1000000 / rp_AcqGetSamplingRate());
}

// Liberar memoria y finalizar la Red Pitaya
free(inputBuffer);
free(outputBuffer);
rp_Release();

return EXIT_SUCCESS;
}