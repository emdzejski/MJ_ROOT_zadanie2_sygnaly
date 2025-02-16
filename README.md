# **Zad. 2 - Analiza sygnałów**

## **Opis projektu**
Program przetwarza sygnały zapisane w pliku binarnym, wyznacza ich kluczowe parametry i zapisuje je w drzewie ROOT. Następnie tworzone są histogramy, które umożliwiają analizę charakterystyk sygnału.


## **Działanie programu**
1. Odczytanie danych z pliku binarnego `wave_0.dat`.
2. Wyznaczenie najważniejszych parametrów sygnału:
   - **Amplituda** – największa wartość bezwzględna sygnału w danym oknie czasowym.
   - **Czas rozpoczęcia sygnału (T0)** – moment, w którym sygnał przekracza określony próg `Athr`.
   - **Ładunek** – całka z wartości sygnału od `T0` do `Tmax`.
   - **Czas nad progiem (TOT, Time Over Threshold)** – czas od `T0` do ponownego przecięcia progu `Athr`.
3. Przechowywanie tych parametrów w obiektach klasy **MySignal** i zapisanie ich do **drzewa ROOT**.
4. Odczytanie drzewa i **wizualizacja danych** w postaci histogramów.

---

## **Struktura projektu**
  - Implementacja klasy **MySignal** do przechowywania parametrów sygnału.
  - Funkcja **`Sig()`**, która odczytuje sygnały, oblicza ich charakterystyki i zapisuje do drzewa ROOT. Jako argument przyjmuje nazwę pliku binarnego. Domyślnie korzysta z **`wave_0.dat`**, który nie jest dołączony do repozytorium.  
  - Funkcja **`Hists()`**, która odczytuje drzewo ROOT i rysuje histogramy. Jako argument przyjmuję nazwę pliku ROOT zawierającego drzewo, domyślnie **`signal.root`**
  - Funkcja **`zad2()`**, która wykonuje funkcje **`Sig()`** i **`Hists()`**.


---

## **Kompilacja i uruchomienie**
Aby uruchomić program, wystarczy otworzyć środowisko ROOT i wpisać:

```
root -l
.L zad2.C
zad2()
```
