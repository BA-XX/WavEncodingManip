#include "Fonctions.h"


// Fonction pour afficher le code hexadécimal d'un fichier et déplacer le curseur au début
void afficherCodeHex(FILE *fichier)
{
    // Déplacer le curseur au début du fichier
    fseek(fichier, 0, SEEK_SET);

    // Lire et afficher le code hexadécimal
    int octet;
    int i = 0;
    while ((octet = fgetc(fichier)) != EOF)
    {
        printf("%02X ", octet);
        if(i<1000)
            i++;
        else
        {
            char car;
            printf("\n------------Cliquez sur enter pour afficher la suite (0 pour quitez)");
            fflush(stdin);
            scanf("%c",&car);

            if(car == '\n')
                i=0;
            else
                break;
        }
    }

}


// Fonction pour lire les informations d'en-tête d'un fichier WAV à partir d'un descripteur de fichier ouvert
void lireEnTeteWAV(FILE *fichier, struct EnTeteWAV *entete)
{
    fseek(fichier, 0, SEEK_SET); // Déplace le curseur au début du fichier
    fread(entete, sizeof(struct EnTeteWAV), 1, fichier);
}

// Fonction pour afficher les informations d'en-tête
void afficherEnTeteWAV(struct EnTeteWAV *entete)
{

    printf("\n******************[ Informations en-tete ]************************\n");
    printf("Chunk ID : %.4s\n", entete->chunkID);
    printf("Chunk Size : %u octets\n", entete->chunkSize);
    printf("Format : %.4s\n", entete->format);
    printf("Subchunk1 ID : %.4s\n", entete->subchunk1ID);
    printf("Subchunk1 Size : %u octets\n", entete->subchunk1Size);
    printf("Audio Format : %u\n", entete->audioFormat);
    printf("Nombre de canaux : %u\n", entete->numChannels);
    printf("Frequence d'echantillonnage : %u Hz\n", entete->sampleRate);
    printf("Debit binaire : %u Kbits/s\n", (entete->byteRate * 8)/1000);
    printf("Block Align : %u octets\n", entete->blockAlign);
    printf("Bits par echantillon : %u bits\n", entete->bitsPerSample);
    printf("Subchunk2 ID : %.4s\n", entete->subchunk2ID);
    printf("Subchunk2 Size : %u octets\n", entete->subchunk2Size);


}

// Fonction pour copier le fichier WAV avec suppression audio dans l'intervalle spécifié
void copierAvecMute(const char *cheminSource, const char *cheminDestination, int debut, int fin)
{
    FILE *fichierSource = fopen(cheminSource, "rb");
    FILE *fichierDestination = fopen(cheminDestination, "wb");

    if (fichierSource == NULL || fichierDestination == NULL)
    {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return;
    }

    // Lire et copier l'en-tête WAV
    struct EnTeteWAV enTete;
    fread(&enTete, sizeof(struct EnTeteWAV), 1, fichierSource);
    fwrite(&enTete, sizeof(struct EnTeteWAV), 1, fichierDestination);

    // Déterminer la position du début et de la fin de la période de silence
    uint32_t debutOctet = debut * enTete.byteRate;
    uint32_t finOctet = fin * enTete.byteRate;

    // Copier le reste du fichier en désactivant l'audio dans la plage spécifiée
    uint32_t octetCourant = sizeof(struct EnTeteWAV);
    uint8_t tampon;
    while (fread(&tampon, sizeof(uint8_t), 1, fichierSource) == 1)
    {
        // Si l'octet est dans la plage muette, écrire 0
        if (octetCourant >= debutOctet && octetCourant < finOctet)
        {
            tampon = 0;
        }

        fwrite(&tampon, sizeof(uint8_t), 1, fichierDestination);

        octetCourant++;
    }

    // Fermer les fichiers
    fclose(fichierSource);
    fclose(fichierDestination);
}

// Fonction pour accélérer un fichier audio par k fois
void accelererAudio(const char *cheminSource, const char *cheminDestination, double facteurAcceleration)
{
    FILE *fichierSource = fopen(cheminSource, "rb");
    FILE *fichierDestination = fopen(cheminDestination, "wb");

    if (fichierSource == NULL || fichierDestination == NULL)
    {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return;
    }

    // Lire et copier l'en-tête WAV
    struct EnTeteWAV enTete;
    fread(&enTete, sizeof(struct EnTeteWAV), 1, fichierSource);

    // Calculer la nouvelle fréquence d'échantillonnage
    uint32_t nouvelleFrequence = (uint32_t)(enTete.sampleRate * facteurAcceleration);

    // Modifier l'en-tête avec la nouvelle fréquence d'échantillonnage
    enTete.sampleRate = nouvelleFrequence;
    enTete.byteRate = enTete.numChannels * nouvelleFrequence * enTete.bitsPerSample / 8;
    enTete.subchunk2Size = enTete.byteRate * enTete.subchunk2Size / enTete.byteRate;

    // Écrire l'en-tête modifié dans le fichier de destination
    fwrite(&enTete, sizeof(struct EnTeteWAV), 1, fichierDestination);

    // Copier le reste du fichier en ajustant la fréquence d'échantillonnage
    uint8_t tampon;
    while (fread(&tampon, sizeof(uint8_t), 1, fichierSource) == 1)
    {
        fwrite(&tampon, sizeof(uint8_t), 1, fichierDestination);
    }

    // Fermer les fichiers
    fclose(fichierSource);
    fclose(fichierDestination);
}

// Fonction pour lire les données audio et générer le fichier de données pour GNU Plot
void genererDonneesGNUPlot(FILE *fichier, struct EnTeteWAV *entete)
{
    // Aller à la position des données audio
    fseek(fichier, 44, SEEK_SET);

    // Créer un fichier de données pour GNU Plot
    FILE *donneesGNUPlot = fopen("donnees_gnuplot.txt", "w");
    if (donneesGNUPlot == NULL)
    {
        printf("Erreur lors de la création du fichier de données pour GNU Plot.\n");
        return;
    }

    // Lire les données audio et écrire dans le fichier de données
    int16_t donneeAudio;
    uint32_t indiceTemps = 0;

    // Clacul le temps de chaque octect joue apratir de debit binaire
    double pasSeconds = (double)sizeof(int16_t)/entete->byteRate;

    while (fread(&donneeAudio, sizeof(int16_t), 1, fichier) == 1)
    {
        fprintf(donneesGNUPlot, "%lf %d\n", indiceTemps * pasSeconds, donneeAudio);
        indiceTemps++;
    }

    // Fermer les fichiers
    fclose(fichier);
    fclose(donneesGNUPlot);
}

void afficherFormeOndeAudio(FILE *fichierWAV, struct EnTeteWAV *entete)
{

    // Générer les données pour GNU Plot
    genererDonneesGNUPlot(fichierWAV,entete);


    // Utiliser GNU Plot pour tracer la courbe
    system("C:\\gnuplot\\bin\\gnuplot -persist -e \"set title 'Representation du fichier audio sous forme onde'; \
            set xlabel 'Temps (secondes)'; set ylabel 'Valeurs decimales'; \
            set xtics 0,2; \
            plot 'donnees_gnuplot.txt' with lines\"");
}

