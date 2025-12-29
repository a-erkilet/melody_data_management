/**
 * =============================================================================
 * DATA MANAGEMENT MODULE (Task 2)
 * Arduino Melody Guessing System 
 * =============================================================================
 * Bu dosya Task 1 (admin_console.c, main.c) ile uyumlu çalışır.
 * Şarkı veritabanı ve skor yönetimini sağlar.
 * 
 * Şarkılar: github.com/robsoncouto/arduino-songs reposu ile uyumlu
 * =============================================================================
 */

#include "melody_guessing.h"

// =============================================================================
// GLOBAL DEĞİŞKENLER
// =============================================================================

#define MAX_SONGS 100
#define MAX_SCORES 50
#define SONGS_FILE "songs.txt"
#define SCORES_FILE "highscores.txt"

// Şarkı yapısı (Arduino repo ile uyumlu)
typedef struct {
    int id;
    char song_name[50];
    char artist[50];
    char category[32];          // Film, Oyun, Klasik, Pop, Dizi
    char arduino_file[50];      // Arduino repo klasör adı (örn: "starwars")
} SongData;

// Skor kaydı
typedef struct {
    char player_name[32];
    int score;
    int wins;
    int games_played;
    time_t timestamp;
} HighScore;

// Veritabanları
static SongData song_database[MAX_SONGS];
static int song_count = 0;

static HighScore score_board[MAX_SCORES];
static int score_count = 0;

// Kategori listesi
static const char* categories[] = {
    "Film",
    "Oyun", 
    "Klasik",
    "Pop",
    "Dizi"
};
static const int category_count = 5;

// Seçili kategori (-1 = hepsi)
static int selected_category = -1;

// =============================================================================
// ŞARKI VERİTABANI FONKSİYONLARI
// =============================================================================

/**
 * Şarkı veritabanını dosyadan yükler
 * Dosya formatı: ID|SongName|Artist|Category|ArduinoFile
 */
void load_song_database(void)
{
    FILE *file = fopen(SONGS_FILE, "r");
    if (file == NULL)
    {
        printf(YELLOW "[!] Warning: %s not found.\n" RESET, SONGS_FILE);
        return;
    }

    song_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL && song_count < MAX_SONGS)
    {
        // Yorum ve boş satırları atla
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        // Satır sonu karakterini temizle
        line[strcspn(line, "\r\n")] = '\0';

        SongData *song = &song_database[song_count];

        int parsed = sscanf(line, "%d|%49[^|]|%49[^|]|%31[^|]|%49[^|\n]",
                            &song->id,
                            song->song_name,
                            song->artist,
                            song->category,
                            song->arduino_file);

        if (parsed >= 5)
        {
            song_count++;
        }
    }

    fclose(file);
    printf(GREEN "[✓] Loaded %d songs from database.\n" RESET, song_count);
}

/**
 * Kategoriye göre rastgele şarkı seçer
 */
Song select_random_song(void)
{
    Song result = {0};

    if (song_count == 0)
    {
        printf(RED "[!] Error: No songs in database!\n" RESET);
        return result;
    }

    // Seçili kategoriye göre filtrele
    int valid_indices[MAX_SONGS];
    int valid_count = 0;

    for (int i = 0; i < song_count; i++)
    {
        if (selected_category == -1)
        {
            // Tüm şarkılar
            valid_indices[valid_count++] = i;
        }
        else if (selected_category >= 0 && selected_category < category_count)
        {
            // Kategori eşleşmesi
            if (strcmp(song_database[i].category, categories[selected_category]) == 0)
            {
                valid_indices[valid_count++] = i;
            }
        }
    }

    if (valid_count == 0)
    {
        printf(YELLOW "[!] No songs found in selected category. Using all songs.\n" RESET);
        for (int i = 0; i < song_count; i++)
            valid_indices[valid_count++] = i;
    }

    // Rastgele seç
    int random_idx = valid_indices[rand() % valid_count];
    SongData *selected = &song_database[random_idx];

    // Task 1'in Song struct'ına kopyala
    result.id = selected->id;
    strncpy(result.song_name, selected->song_name, sizeof(result.song_name) - 1);
    strncpy(result.artist, selected->artist, sizeof(result.artist) - 1);
    result.melody_duration = 5000; // Varsayılan

    return result;
}

/**
 * Şarkının Arduino dosya adını döndürür
 * Arduino'ya gönderilecek komut: "PLAY:starwars" gibi
 */
const char* get_arduino_filename(int song_id)
{
    for (int i = 0; i < song_count; i++)
    {
        if (song_database[i].id == song_id)
        {
            return song_database[i].arduino_file;
        }
    }
    return "";
}

/**
 * Şarkının kategorisini döndürür
 */
const char* get_song_category(int song_id)
{
    for (int i = 0; i < song_count; i++)
    {
        if (song_database[i].id == song_id)
        {
            return song_database[i].category;
        }
    }
    return "Unknown";
}

/**
 * Toplam şarkı sayısını döndürür
 */
int get_total_song_count(void)
{
    return song_count;
}

// =============================================================================
// KATEGORİ FONKSİYONLARI
// =============================================================================

/**
 * Kategori seçim menüsünü gösterir ve seçimi alır
 */
int display_category_menu(void)
{
    printf("\n");
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "         - SELECT CATEGORY -            " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [1] " RESET "Film Müzikleri                   " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [2] " RESET "Oyun Müzikleri                   " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [3] " RESET "Klasik Müzik                     " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [4] " RESET "Pop                              " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [5] " RESET "Dizi Müzikleri                   " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [6] " RESET "Tüm Kategoriler (Karışık)        " LILA " ║\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
    printf("\n                                                  " PINK "» " LILA "SELECT: " RESET);

    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 6)
    {
        getchar();
        return -1;
    }
    getchar();

    if (choice == 6)
        selected_category = -1;  // Hepsi
    else
        selected_category = choice - 1;  // 0-indexed

    return choice;
}

/**
 * Seçili kategorideki şarkı sayısını döndürür
 */
int get_category_song_count(int category_index)
{
    if (category_index < 0)
        return song_count;

    int count = 0;
    for (int i = 0; i < song_count; i++)
    {
        if (strcmp(song_database[i].category, categories[category_index]) == 0)
            count++;
    }
    return count;
}

// =============================================================================
// SKOR TABLOSU FONKSİYONLARI
// =============================================================================

/**
 * Skor tablosunu dosyadan yükler
 */
void load_scores(void)
{
    FILE *file = fopen(SCORES_FILE, "r");
    if (file == NULL)
        return;

    score_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL && score_count < MAX_SCORES)
    {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        line[strcspn(line, "\r\n")] = '\0';

        HighScore *hs = &score_board[score_count];
        long ts;

        if (sscanf(line, "%31[^|]|%d|%d|%d|%ld",
                   hs->player_name, &hs->score, &hs->wins,
                   &hs->games_played, &ts) >= 4)
        {
            hs->timestamp = (time_t)ts;
            score_count++;
        }
    }

    fclose(file);
}

/**
 * Skor tablosunu dosyaya kaydeder
 */
void save_scores(void)
{
    FILE *file = fopen(SCORES_FILE, "w");
    if (file == NULL)
        return;

    fprintf(file, "# High Scores - Melody Guessing Battle\n");
    fprintf(file, "# Format: Name|Score|Wins|GamesPlayed|Timestamp\n\n");

    for (int i = 0; i < score_count; i++)
    {
        fprintf(file, "%s|%d|%d|%d|%ld\n",
                score_board[i].player_name,
                score_board[i].score,
                score_board[i].wins,
                score_board[i].games_played,
                (long)score_board[i].timestamp);
    }

    fclose(file);
}

/**
 * Yeni skor ekler
 */
void add_score(const char *name, int score, int won)
{
    // Mevcut oyuncu var mı kontrol et
    for (int i = 0; i < score_count; i++)
    {
        if (strcmp(score_board[i].player_name, name) == 0)
        {
            // Güncelle
            score_board[i].score += score;
            score_board[i].games_played++;
            if (won)
                score_board[i].wins++;
            score_board[i].timestamp = time(NULL);
            save_scores();
            return;
        }
    }

    // Yeni oyuncu ekle
    if (score_count < MAX_SCORES)
    {
        HighScore *hs = &score_board[score_count];
        strncpy(hs->player_name, name, sizeof(hs->player_name) - 1);
        hs->score = score;
        hs->wins = won ? 1 : 0;
        hs->games_played = 1;
        hs->timestamp = time(NULL);
        score_count++;
        save_scores();
    }
}

/**
 * Skor tablosunu gösterir
 */
void display_scoreboard(void)
{
    load_scores();

    printf("\n");
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "        🏆 GLOBAL RANKINGS 🏆           " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");

    if (score_count == 0)
    {
        printf(LILA "                                                  ║                                          ║\n");
        printf(LILA "                                                  ║  " RESET "       No scores recorded yet.         " LILA " ║\n");
        printf(LILA "                                                  ║                                          ║\n");
    }
    else
    {
        // Skorları sırala (bubble sort)
        for (int i = 0; i < score_count - 1; i++)
        {
            for (int j = 0; j < score_count - i - 1; j++)
            {
                if (score_board[j].score < score_board[j + 1].score)
                {
                    HighScore temp = score_board[j];
                    score_board[j] = score_board[j + 1];
                    score_board[j + 1] = temp;
                }
            }
        }

        printf(LILA "                                                  ║  " RESET "#   Player          Score   Wins       " LILA " ║\n");
        printf(LILA "                                                  ║  " RESET "─────────────────────────────────────   " LILA " ║\n");

        int show = (score_count < 10) ? score_count : 10;
        for (int i = 0; i < show; i++)
        {
            printf(LILA "                                                  ║  " RESET "%-2d  %-15s %5d   %3d        " LILA " ║\n",
                   i + 1,
                   score_board[i].player_name,
                   score_board[i].score,
                   score_board[i].wins);
        }
        printf(LILA "                                                  ║                                          ║\n");
    }

    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
    printf("\n                                                  " PINK "» " LILA "Press ENTER to continue..." RESET);
    getchar();
}

// =============================================================================
// OYUN YÖNETİM FONKSİYONLARI
// =============================================================================

/**
 * Oyunu sıfırlar
 */
void reset_game(void)
{
    game_state.current_round = 0;
    game_state.player1_score = 0;
    game_state.player2_score = 0;
    selected_category = -1;
    printf(GREEN "[✓] Game state reset.\n" RESET);
}

/**
 * Oyun sonuçlarını kaydeder
 */
void save_game_results(void)
{
    int p1_won = (game_state.player1_score > game_state.player2_score) ? 1 : 0;
    int p2_won = (game_state.player2_score > game_state.player1_score) ? 1 : 0;

    add_score("Player 1", game_state.player1_score, p1_won);
    add_score("Player 2", game_state.player2_score, p2_won);
}

// =============================================================================
// ARDUINO İLETİŞİM YARDIMCI FONKSİYONLARI
// =============================================================================

/**
 * Arduino'ya şarkı çalma komutu gönderir
 * Format: "PLAY:starwars" veya "PLAY:tetris"
 * Arduino robsoncouto/arduino-songs reposundaki melodileri çalacak
 */
void send_song_to_arduino(int song_id)
{
    const char *filename = get_arduino_filename(song_id);
    if (strlen(filename) > 0)
    {
        char command[100];
        snprintf(command, sizeof(command), "PLAY:%s", filename);
        send_to_arduino(command);
        printf(CYAN "[→] Sent to Arduino: %s\n" RESET, command);
    }
}

/**
 * Arduino'ya zorluk süresini gönderir
 */
void send_duration_to_arduino(int duration_ms)
{
    char command[50];
    snprintf(command, sizeof(command), "DURATION:%d", duration_ms);
    send_to_arduino(command);
}
