# 🎵 Data Management Module (Task 2)
## Arduino Melody Guessing System 

Şarkı veritabanı ve skor yönetimi modülü.

---

## 📁 Dosyalar

| Dosya | Açıklama |
|-------|----------|
| `data_management.c` | Ana kod - tüm fonksiyonlar |
| `songs.txt` | Şarkı veritabanı (34 şarkı, 5 kategori) |
| `highscores.txt` | Skor tablosu |

---

## 🔧 Task 1 Entegrasyonu

Derleme:
```bash
gcc -Wall -o melody_game main.c admin_console.c data_management.c serial_comm.c
```

Fonksiyonlar:
```c
load_song_database();      // Program başında
select_random_song();      // Rastgele şarkı
display_scoreboard();      // Skor tablosu
reset_game();              // Oyunu sıfırla
display_category_menu();   // Kategori seçimi
```

---

## 🤖 Task 4 (Arduino) Entegrasyonu

### C Programından Gelen Komut Formatı:
```
PLAY:starwars
PLAY:tetris
PLAY:supermariobros
```

### Arduino'da Yapılması Gereken:

**1. robsoncouto/arduino-songs reposundan melodileri alın:**
https://github.com/robsoncouto/arduino-songs

**2. loop() içine şu kodu ekleyin:**
```cpp
void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("PLAY:")) {
            String song = command.substring(5);
            playSong(song);
        }
    }
}

void playSong(String song) {
    if (song == "starwars") playStarWars();
    else if (song == "imperialmarch") playImperialMarch();
    else if (song == "tetris") playTetris();
    else if (song == "supermariobros") playSuperMario();
    else if (song == "harrypotter") playHarryPotter();
    else if (song == "gameofthrones") playGameOfThrones();
    else if (song == "furelise") playFurElise();
    else if (song == "pacman") playPacman();
    // ... diğer şarkılar
}
```

### Şarkı Listesi (songs.txt'den):

**Film:**
- imperialmarch, cantinaband, starwars, harrypotter
- thegodfather, pinkpanther, thelionsleepstonight

**Oyun:**
- supermariobros, tetris, zeldatheme, zeldaslullaby
- songofstorms, pacman, greenhill, miichannel
- doom, bloodytears, professorlayton

**Klasik:**
- furelise, odetojoy, cannonind, minuetg
- greensleeves, brahmslullaby, thebadinerie, princeigor

**Pop:**
- takeonme, nevergonnagiveyouup, nokia, keyboardcat
- happybirthday, merrychristmas, silentnight

**Dizi:**
- gameofthrones

---

## 📊 Dosya Formatları

### songs.txt
```
# Format: ID|SongName|Artist|Category|ArduinoFile
1|Imperial March|Star Wars|Film|imperialmarch
8|Super Mario Bros|Nintendo|Oyun|supermariobros
```

### highscores.txt
```
# Format: Name|Score|Wins|GamesPlayed|Timestamp
Player 1|150|3|5|1703847200
```

---

**Task 2 - Data Management**
