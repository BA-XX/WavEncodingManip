#include "Fonctions.h"


int main()
{

    // Variables pour stocker les informations fournies par l'utilisateur
    char cheminSource[256];
    char cheminDestination[256];
    int debutIntervalle, finIntervalle;
    double facteurAcceleration;

    FILE *fichier;
    struct EnTeteWAV entete;

    printf("Entrez le chemin du fichier WAV: ");
    scanf("%s", cheminSource);

    fichier = fopen(cheminSource, "rb");

    if (fichier == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 1;
    }



    // Lire les information dans l'entent
    lireEnTeteWAV(fichier, &entete);

    int choix;
    do
    {
        // Afficher le menu
        printf("\n******************[ MENU ]************************\n");

        printf("1. Afficher les informations de l'en-tete WAV\n");
        printf("2. Afficher le code hexadecimal\n");
        printf("3. Copier avec mute\n");
        printf("4. Accelerer l'audio\n");
        printf("5. Generer les donnees pour GNU Plot\n");
        printf("6. Afficher la forme d'onde audio\n");
        printf("0. Quitter\n");
        printf("Choisissez une option : ");
        scanf("%d", &choix);

        // Exécuter l'option choisie
        switch (choix)
        {

        case 1:
            afficherEnTeteWAV(&entete);
            break;
        case 2:
            afficherCodeHex(fichier);
            break;
        case 3:
            // Demander à l'utilisateur les informations nécessaires
            printf("Chemin destination : ");
            scanf("%s", cheminDestination);
            printf("Debut de l'intervalle : ");
            scanf("%d", &debutIntervalle);
            printf("Fin de l'intervalle : ");
            scanf("%d", &finIntervalle);

            // Appeler la fonction pour copier avec mute
            printf("en cours ....");
            copierAvecMute(cheminSource, cheminDestination, debutIntervalle, finIntervalle);
            printf("succes :)");
            break;
        case 4:
            // Demander à l'utilisateur les informations nécessaires
            printf("Chemin destination : ");
            scanf("%s", cheminDestination);
            printf("Facteur d'acceleration : ");
            scanf("%lf", &facteurAcceleration);

            // Appeler la fonction pour accélérer l'audio
            printf("en cours de conversion....");
            accelererAudio(cheminSource, cheminDestination, facteurAcceleration);
            printf("succes :)");
            break;
        case 5:
            printf("en cours ....");
            genererDonneesGNUPlot(fichier,&entete);
            printf("succes :)");
            break;
        case 6:
            printf("en cours ....");
            afficherFormeOndeAudio(fichier,&entete);
            printf("succes :)");
            break;
        case 0:
            printf("Programme termine.\n");
            break;
        default:
            printf("Option non valide. Veuillez reessayer.\n");
        }
    }
    while (choix != 0);

    // Fermer le fichier WAV
    fclose(fichier);



    return 0;
}
