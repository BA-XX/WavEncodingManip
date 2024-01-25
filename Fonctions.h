#ifndef FONCTIONS_H_INCLUDED
#define FONCTIONS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


// Structure pour stocker les informations d'en-tête du fichier WAV
struct EnTeteWAV {
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2ID[4];
    uint32_t subchunk2Size;
};

void afficherCodeHex(FILE *fichier);
void lireEnTeteWAV(FILE *fichier, struct EnTeteWAV *entete);
void afficherEnTeteWAV(struct EnTeteWAV *entete);
void copierAvecMute(const char *cheminSource, const char *cheminDestination, int debut, int fin) ;
void accelererAudio(const char *cheminSource, const char *cheminDestination, double facteurAcceleration);
void genererDonneesGNUPlot(FILE *fichier,struct EnTeteWAV *entete);
void afficherFormeOndeAudio(FILE *fichierWav,struct EnTeteWAV *entete);


#endif // FONCTIONS_H_INCLUDED
