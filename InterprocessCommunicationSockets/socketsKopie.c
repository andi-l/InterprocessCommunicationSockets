#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/socket.h>

// Definition der Variablen 'stop'
volatile sig_atomic_t stop = 0;

// Signalhandler, der das SIGINT-Signal behandelt und die globale Variable 'stop' auf 1 setzt
void handler(int signum)
{
    signal(SIGINT, SIG_DFL);
    printf(" Programm wurde abgebrochen ");
    stop = 1;
}

int main(int argc, char *argv[])
{
    // Setzt den Signalhandler für das SIGINT-Signal
    signal(SIGINT, handler);

    // Hauptprogrammschleife, läuft bis 'stop' auf 1 gesetzt wird (SIGINT-Signal empfangen)
    while (!stop)
    {
        // Definition von drei Socket-Paaren
        int sv[2], sv2[2], sv3[2];

        // Erstellt drei Socket-Paare. Beendet das Programm mit einer Fehlermeldung, falls ein Fehler auftritt
        if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) == -1 || socketpair(AF_LOCAL, SOCK_STREAM, 0, sv2) == -1 || socketpair(AF_LOCAL, SOCK_STREAM, 0, sv3) == -1)
        {
            perror("Socketpair");
            exit(1);
        }

        // Erstellt den ersten Prozess LOG
        int prozess_id1 = fork();

        if (prozess_id1 == 0)
        {
            // Erstellt den zweiten Prozess REPORT
            int prozess_id2 = fork();
            if (prozess_id2 == 0)
            {
                // Erstellt den dritten Prozess STAT
                int prozess_id3 = fork();
                if (prozess_id3 == 0)
                {
                    // Erstellt den vierten Prozess CONV
                    int prozess_id4 = fork();
                    if (prozess_id4 == 0)
                    {
                        // CONV: Generiert 10 Zufallszahlen, gibt sie aus und schreibt sie in einen Socket
                        int i;
                        int arr[10];
                        int n = 10;

                        srand(time(NULL)); // Initialisiert den Zufallszahlengenerator

                        printf("\nGeneriert in CONV: ");

                        for (i = 0; i < n; i++)
                        {
                            arr[i] = rand() % 11;  // Generiert eine Zufallszahl zwischen 0 und 10
                            printf("%d ", arr[i]); // Gibt die Zufallszahl aus
                        }
                        printf("\n");

                        // Schreibt die Zufallszahlen in den ersten & dritten Socket
                        write(sv[0], &arr, sizeof(int) * n);
                        write(sv3[0], &arr, sizeof(int) * n);
                    }
                    // STAT: Liest die Messwerte aus dem Socket, berechnet den Durchschnitt und die Summe
                    wait(NULL); // Wartet auf die Beendigung des dritten Prozesses
                    int sum = 0;
                    float mittelwert;
                    int arr[10];
                    int i;
                    int n = 10;

                    // Liest die Zahlen aus dem ersten Socket
                    read(sv[1], &arr, sizeof(int) * n);

                    // Berechnet die Summe der Zahlen
                    for (i = 0; i < n; i++)
                    {
                        sum += arr[i];
                    }

                    // Schreibt die Summe in den ersten Socket
                    write(sv[0], &sum, sizeof(sum));

                    // Berechnet den Durchschnitt
                    mittelwert = (float)sum / n;

                    // Schreibt den Mittelwert in den zweiten Socket
                    write(sv2[0], &mittelwert, sizeof(mittelwert));
                }
                // REPORT: Liest die Ergebnisse von STAT und gibt die statistischen Daten aus
                wait(NULL);
                int sum;
                float mittelwert;

                // Liest die Summe aus dem Socket und gibt es aus
                read(sv[1], &sum, sizeof(sum));
                printf("Summe in REPORT: %d\n", sum);
                close(sv[1]);

                // Liest den Mittelwert aus dem Socket und gibt ihn aus
                read(sv2[1], &mittelwert, sizeof(mittelwert));
                printf("Mittelwert in REPORT: %.2f\n", mittelwert);
                close(sv2[1]);
            }

            // LOG: Liest Zahlen aus dem Socket und schreibt sie in eine Datei
            wait(NULL); // Wartet auf die Beendigung des innersten Prozesses
            int arr[10];
            int i;
            int n = 10;

            // Liest die Zahlen aus dem dritten Socket und schließt ihn
            read(sv3[1], &arr, sizeof(int) * n);
            close(sv3[1]);

            // Öffnet eine Datei zum Schreiben und überprüft auf Fehler
            FILE *file = fopen("konverterWerte.txt", "w");
            if (file == NULL)
            {
                printf("Fehler beim Erstellen der Datei\n");
                return 1;
            }

            // Schreibt die Zahlen in die Datei und schließt sie danach
            for (int j = 0; j < n; j++)
            {
                fprintf(file, "%d,", arr[j]);
            }
            fclose(file);

            printf("Erhalten in LOG: ");

            wait(NULL);

            // Gibt die Zahlen aus
            for (i = 0; i < n; i++)
            {
                printf("%d ", arr[i]);
            }
            printf("\n");

            return 0;
        }

        sleep(1); // Fügt eine Pause von 1 Sekunde zwischen jedem Durchlauf ein
    }

    return 0;
}