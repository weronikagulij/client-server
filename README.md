# Aplikacja KLIENT - SERWER

### Kompilacja oraz uruchomienie plików

Poniższe polecenie uruchamia programy serwera i trzech klientów.

```
./makefile.sh
```

### Opis programu i zawartości plików

Aplikacja klient - serwer służy do porozumiewania się pomiędzy serwerem a klientami za pomocą kolejek komunikatów.

Klienci logując się do systemu otrzymują swój unikalny numer identyfikacyjny oraz wybierają typy wiadomości, które chcą odbierać. Klienci mogą odbierać wiadomości na dwa sposoby: automatycznie - wiadomośc pojawia się na ekranie od razu po wysłaniu przez serwer, lub manualnie,przechowując wiadomość - będzie ona czekać na odbiór. Użytkownicy mogą wysyłać wiadomości do wszystkich użytkowników za pośrednictwem serwera - będzie ona wtedy wysłana od razu po przekazaniu. Serwer w trybie ciągłym wysyła jedną z losowo wybranych wiadomości przechowywanych w tablicy.

inf136720_s.c - plik serwera. Odpowiada za rejestrowanie użytkowników i przechowywanie ich identyfikatorów oraz rozsyłanie wiadomości.

inf136720_k.c - plik klienta. Po zalogowaniu prosi o wybranie subskrybowanych typów wiadomości. Użytkownik może odbierać wiadomości, wysyłać je oraz wyświetlać swoje subskrypcje.

![alt text](https://raw.githubusercontent.com/wjankowska/screenshots/master/c.png)
