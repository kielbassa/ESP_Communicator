# Bun-1 Communicator

> Ten projekt posiada również dokumentację w języku angielski: [README.md](README.md)
>
Komunikator ESP32 LoRa z wyświetlaczem OLED i sterowaniem przez Bluetooth.

Projekt jest zaprojektowany dla płytki ESP32 TTGO LoRa32 i wykorzystuje:
- radio LoRa do bezprzewodowej komunikacji
- Bluetooth Serial jako interfejs użytkownika
- wyświetlacz OLED do menu i statusu
- szyfrowanie AES-256 dla wysyłanych i odbieranych danych
- pamięć nieulotną (NVS) do zapisywania nazwy urządzenia i klucza AES

## Funkcje

- Interfejs menu przez Bluetooth do:
  - wysyłania wiadomości przez LoRa
  - przeglądania odebranych wiadomości
  - konfiguracji ustawień urządzenia
- szyfrowanie/deszyfrowanie LoRa AES-256 CBC
- klucz AES przechowywany w pamięci flash
- bufor odbiorczy dla maksymalnie 5 odebranych wiadomości
- nawigacja przez menu OLED przy użyciu przycisku

## Sprzęt

Zalecana płytka:
- `ttgo-lora32-v1`

Piny skonfigurowane w kodzie:
- OLED / I2C: `SDA=4`, `SCL=15`, `RST=16`
- LoRa: `SCK=5`, `MISO=19`, `MOSI=27`, `SS=18`, `RST=14`, `DIO0=26`
- Przycisk: `GPIO0`
- Częstotliwość LoRa: `866 MHz`

## Oprogramowanie

Projekt zbudowany przy pomocy PlatformIO, używając dostarczonego pliku `platformio.ini`.

Zależności:
- `sandeepmistry/LoRa@^0.8.0`
- `adafruit/Adafruit GFX Library@^1.12.5`
- `adafruit/Adafruit SSD1306@^2.5.16`

## Obsługa

1. Zbuduj i wgraj firmware za pomocą PlatformIO:
   ```bash
   pio run
   pio run -t upload
   ```
2. Otwórz aplikację Bluetooth terminal i połącz się z urządzeniem ESP32. Nazwa urządzenia jest wyświetlana na ekranie OLED i zwykle ma postać `ESP32_OLED_##`.
3. Użyj terminala Bluetooth, aby wysłać proste polecenia:
   - `1` → Wyślij wiadomość
   - `2` → Otwórz skrzynkę odbiorczą
   - `3` → Konfiguracja

### Wysyłanie wiadomości

- Wybierz `1` w terminalu Bluetooth.
- Wyślij tekst wiadomości w jednej linii.
- Naciśnij przycisk na płytce, aby przesłać wiadomość przez LoRa.

### Odczytywanie otrzymanych wiadomości

- Wybierz `2` w menu Bluetooth.
- Otrzymane wiadomości LoRa są odszyfrowywane i wyświetlane na OLED.
- Podwójne naciśnięcie przycisku pozwala przechodzić między zapisanymi wiadomościami skrzynki odbiorczej.
- Pojedyncze naciśnięcie powraca do menu głównego.

### Konfiguracja

- Wybierz `3` w menu Bluetooth.
- Pojedyncze naciśnięcie powraca do menu głównego.
- Podwójne naciśnięcie przechodzi do trybu edycji klucza AES.
- Potrójne naciśnięcie generuje nową nazwę urządzenia i restartuje Bluetooth.

### Edycja klucza AES

- W trybie konfiguracji naciśnij przycisk podwójnie.
- Wyślij 64-znakowy klucz AES w formacie szesnastkowym przez Bluetooth.
- Klucz zostanie zapisany w NVS i będzie używany do wszystkich przyszłych operacji szyfrowania/deszyfrowania.

## Uwagi

- Firmware używa stałego wektora inicjalizującego (IV) dla AES-CBC, co jest odpowiednie do nauki i projektów hobbystycznych, ale nie jest optymalne dla produkcyjnego bezpieczeństwa.
- Upewnij się, że wszystkie komunikujące się urządzenia mają ten sam klucz AES, aby odszyfrowanie się powiodło.

## Struktura projektu

- `src/main.cpp` — główna logika firmware i aplikacji
- `src/encryption_aes.h` — pomocnicze funkcje szyfrowania AES
- `src/graphics.h` — dane bitmap OLED i definicje zasobów wyświetlacza
- `platformio.ini` — konfiguracja budowania i zależności

