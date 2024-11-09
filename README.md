# Dokumentation

**Verwendung von Sockets für die Interprozesskommunikation in C**

In dieser Dokumentation stelle ich ein Programm vor, das in C geschrieben wurde und die Interprozesskommunikation mittels Sockets zeigt. Es ist ein ein informatives Beispiel für den Einsatz von Sockets, Prozessverwaltungsfunktionen und Datei-I/O in C.



**Überblick über das Programm**

Dieses Programm implementiert diverse Aspekte der Systemprogrammierung. Es baut eine Kette von Subprozessen auf, wobei jeder Subprozess seine eigene spezifische Funktion hat. Die Kommunikation zwischen diesen Subprozessen wird durch Sockets ermöglicht, die mit der Funktion ''socketpair()'' erstellt werden. Darüber hinaus sorgt eine Signalhandler-Routine für einen geordneten Abschluss des Programms, wenn die Tastenkombination '''Ctrl-C'' erkannt wird. 



**Abhängigkeiten und globale Variablen**

Im Programm werden mehrere mehrere Standardbibliotheken verwendet, um Funktionen und Datentypen bereitzustellen, die für Systemprogrammierung, Datei-I/O, Signalverarbeitung und weitere Anforderungen benötigt sind.

Der Datentyp ''volatile sig_atomic_t'' ist speziell für diese Zwecke gedacht, um Wettlaufbedingungen und unerwartete Verhaltensweisen zu verhindern.



**Hauptfunktion und Prozess-Generierung**

Die Hauptfunktion ''main()'' ist der Startpunkt des Programms. Sie legt zunächst den Signalhandler für das SIGINT-Signal fest. Anschließend startet sie die Hauptschleife des Programms, die erst dann endet, wenn ein SIGINT-Signal empfangen wird.

In dieser Schleife werden zunächst zwei Sockets über die Funktion ''socketpair()'' erstellt. Wenn dabei ein Fehler auftritt, wird eine Fehlermeldung ausgegeben und das Programm beendet.

Anschließend erstellt das Programm eine Kette von vier Subprozessen durch fortlaufende Aufrufe der Funktion ''fork()''. Jeder dieser Subprozesse hat spezifische Aufgaben, die nachfolgend erläutert werden.



**Datenverarbeitung und -auswertung**

Der vierte Subprozess (das innerste Glied in der Kette) erzeugt eine Reihe von Zufallszahlen, gibt diese auf der Konsole aus und schreibt sie in einen der Sockets. Ich habe ''srand(time(NULL))'' verwendet, um den Zufallszahlengenerator mit der aktuellen Uhrzeit als Seed zu initialisieren. Dies stellt sicher, dass bei jedem Durchlauf des Programms eine andere Zahlenreihe generiert wird.

Der dritte Subprozess liest diese Zahlen aus dem Socket, schreibt sie in eine Datei und berechnet dann ihre Summe. Die Summe wird in den zweiten Socket geschrieben. Die Zahlen werden in eine Datei namens ''konverterWerte.txt'' geschrieben, dabei wird jede Zahl durch ein Komma getrennt. Tritt beim Öffnen der Datei ein Fehler auf, gibt das Programm eine Fehlermeldung aus und beendet sich.

Der zweite Subprozess liest die Summe aus dem zweiten Socket, berechnet den Durchschnitt der Zahlen und gibt das Ergebnis auf der Konsole aus. Der Durchschnitt wird als Gleitkommazahl berechnet und ausgegeben.

Der erste Subprozess, das oberste Glied in der Kette, hat keine spezifischen Aufgaben und beendet sich sofort, nachdem seine Subprozesse erstellt wurden.
