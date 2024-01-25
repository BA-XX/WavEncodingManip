#include "Fonctions.h"


// Fonction pour afficher le code hexad�cimal d'un fichier et d�placer le curseur au d�but
void afficherCodeHex(FILE *fichier)
{
    // D�placer le curseur au d�but du fichier
    fseek(fichier, 0, SEEK_SET);

    // Lire et afficher le code hexad�cimal
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


// Fonction pour lire les informations d'en-t�te d'un fichier WAV � partir d'un descripteur de fichier ouvert
void lireEnTeteWAV(FILE *fichier, struct EnTeteWAV *entete)
{
    fseek(fichier, 0, SEEK_SET); // D�place le curseur au d�but du fichier
    fread(entete, sizeof(struct EnTeteWAV), 1, fichier);
}

// Fonction pour afficher les informations d'en-t�te
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

// Fonction pour copier le fichier WAV avec suppression audio dans l'intervalle sp�cifi�
void copierAvecMute(const char *cheminSource, const char *cheminDestination, int debut, int fin)
{
    FILE *fichierSource = fopen(cheminSource, "rb");
    FILE *fichierDestination = fopen(cheminDestination, "wb");

    if (fichierSource == NULL || fichierDestination == NULL)
    {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return;
    }

    // Lire et copier l'en-t�te WAV
    struct EnTeteWAV enTete;
    fread(&enTete, sizeof(struct EnTeteWAV), 1, fichierSource);
    fwrite(&enTete, sizeof(struct EnTeteWAV), 1, fichierDestination);

    // D�terminer la position du d�but et de la fin de la p�riode de silence
    uint32_t debutOctet = debut * enTete.byteRate;
    uint32_t finOctet = fin * enTete.byteRate;

    // Copier le reste du fichier en d�sactivant l'audio dans la plage sp�cifi�e
    uint32_t octetCourant = sizeof(struct EnTeteWAV);
    uint8_t tampon;
    while (fread(&tampon, sizeof(uint8_t), 1, fichierSource) == 1)
    {
        // Si l'octet est dans la plage muette, �crire 0
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

// Fonction pour acc�l�rer un fichier audio par k fois
void accelererAudio(const char *cheminSource, const char *cheminDestination, double facteurAcceleration)
{
    FILE *fichierSource = fopen(cheminSource, "rb");
    FILE *fichierDestination = fopen(cheminDestination, "wb");

    if (fichierSource == NULL || fichierDestination == NULL)
    {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return;
    }

    // Lire et copier l'en-t�te WAV
    struct EnTeteWAV enTete;
    fread(&enTete, sizeof(struct EnTeteWAV), 1, fichierSource);

    // Calculer la nouvelle fr�quence d'�chantillonnage
    uint32_t nouvelleFrequence = (uint32_t)(enTete.sampleRate * facteurAcceleration);

    // Modifier l'en-t�te avec la nouvelle fr�quence d'�chantillonnage
    enTete.sampleRate = nouvelleFrequence;
    enTete.byteRate = enTete.numChannels * nouvelleFrequence * enTete.bitsPerSample / 8;
    enTete.subchunk2Size = enTete.byteRate * enTete.subchunk2Size / enTete.byteRate;

    // �crire l'en-t�te modifi� dans le fichier de destination
    fwrite(&enTete, sizeof(struct EnTeteWAV), 1, fichierDestination);

    // Copier le reste du fichier en ajustant la fr�quence d'�chantillonnage
    uint8_t tampon;
    while (fread(&tampon, sizeof(uint8_t), 1, fichierSource) == 1)
    {
        fwrite(&tampon, sizeof(uint8_t), 1, fichierDestination);
    }

    // Fermer les fichiers
    fclose(fichierSource);
    fclose(fichierDestination);
}

// Fonction pour lire les donn�es audio et g�n�rer le fichier de donn�es pour GNU Plot
void genererDonneesGNUPlot(FILE *fichier, struct EnTeteWAV *entete)
{
    // Aller � la position des donn�es audio
    fseek(fichier, 44, SEEK_SET);

    // Cr�er un fichier de donn�es pour GNU Plot
    FILE *donneesGNUPlot = fopen("donnees_gnuplot.txt", "w");
    if (donneesGNUPlot == NULL)
    {
        printf("Erreur lors de la cr�ation du fichier de donn�es pour GNU Plot.\n");
        return;
    }

    // Lire les donn�es audio et �crire dans le fichier de donn�es
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

    // G�n�rer les donn�es pour GNU Plot
    genererDonneesGNUPlot(fichierWAV,entete);


    // Utiliser GNU Plot pour tracer la courbe
    system("C:\\gnuplot\\bin\\gnuplot -persist -e \"set title 'Representation du fichier audio sous forme onde'; \
            set xlabel 'Temps (secondes)'; set ylabel 'Valeurs decimales'; \
            set xtics 0,2; \
            plot 'donnees_gnuplot.txt' with lines\"");
}

