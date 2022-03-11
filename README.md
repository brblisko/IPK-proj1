# IPK-proj1

Ulohou projektu bolo vytvorenie serveru v jazyku C/C++ komunikujuceho pomocou protokolu HTTP, ktory bude poskytovat rozne informacie o systeme. Server pocuva na zadanom porte a podla url vracia pozadovane informacie

## Spustenie

Na spustenie projektu si staci stiahnut subory s kodom a Makefile. Nasledne pomocou prikazu `make` projekt prelozit s spustit ako `./hinfosvc "pozadovany port"`

### Prerekvizity

Na spravne fungovanie projektu je treba tento software.

- gcc
- make

## Pouzitie

Ukazka ako projekt pouzivat.

```
$ ./hinfosvc 12345 & curl http://localhost:12345/hostname
$ ./hinfosvc 12345 & curl http://localhost:12345/cpu-name
$ ./hinfosvc 12345 & curl http://localhost:12345/load
```
