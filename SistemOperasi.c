#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

int selesai = 0;
CRITICAL_SECTION lock;

int batas;
char kata[100];
char teks[100];

void tandaiSelesai(char* nama) {
    EnterCriticalSection(&lock);
    selesai++;
    printf(">> %s selesai (%d/3)\n", nama, selesai);
    LeaveCriticalSection(&lock);
}

// Thread 1: Bilangan prima
DWORD WINAPI thread_prima(void* arg) {
    Sleep(50);
    printf("[Prima] 1-%d: [", batas);
    int first = 1;
    for (int n = 2; n <= batas; n++) {
        int p = 1;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) { p = 0; break; }
        }
        if (p) {
            if (!first) printf(", ");
            printf("%d", n);
            first = 0;
        }
    }
    printf("]\n");
    tandaiSelesai("Prima");
    return 0;
}

// Thread 2: Balik kata dan cek kesamaan balik kata
DWORD WINAPI thread_balik(void* arg) {
    Sleep(100);
    int len = strlen(kata);
    
    printf("[Balik] \"%s\" -> \"", kata);
    for (int i = len - 1; i >= 0; i--) {
        printf("%c", kata[i]);
    }
    printf("\"\n");

    // Cek kesamaan
    char bersih[100];
    int b_idx = 0;
    for (int i = 0; i < len; i++) {
        if (kata[i] != ' ') {
            bersih[b_idx++] = tolower(kata[i]);
        }
    }
    bersih[b_idx] = '\0';

    int sama = 1;
    int b_len = strlen(bersih);
    for (int i = 0; i < b_len / 2; i++) {
        if (bersih[i] != bersih[b_len - 1 - i]) {
            sama = 0;
            break;
        }
    }

    printf("[Balik] Sama? %s\n", sama ? "Ya" : "Tidak");
    tandaiSelesai("Balik");
    return 0;
}

// Thread 3: Tulis dan Baca File
DWORD WINAPI thread_file(void* arg) {
    FILE* f = fopen("input_user.txt", "w");
    fprintf(f, "%s\n", teks);
    fclose(f);

    Sleep(150);

    f = fopen("input_user.txt", "r");
    char buffer[100];
    fgets(buffer, sizeof(buffer), f);
    fclose(f);
    
    buffer[strcspn(buffer, "\n")] = 0;
    printf("[File] Isi file: %s\n", buffer);

    tandaiSelesai("File");
    return 0;
}

int main() {
    printf("Cari bilangan prima sampai (misal. 50): ");
    scanf("%d", &batas);
    getchar();

    printf("Kata/kalimat yang mau dibalik            : ");
    fgets(kata, sizeof(kata), stdin);
    kata[strcspn(kata, "\n")] = 0;

    printf("Teks untuk ditulis ke file lalu dibaca: ");
    fgets(teks, sizeof(teks), stdin);
    teks[strcspn(teks, "\n")] = 0;

    printf("\n--- Thread dimulai ---\n");

    InitializeCriticalSection(&lock);

    HANDLE t1 = CreateThread(NULL, 0, thread_prima, NULL, 0, NULL);
    HANDLE t2 = CreateThread(NULL, 0, thread_balik, NULL, 0, NULL);
    HANDLE t3 = CreateThread(NULL, 0, thread_file, NULL, 0, NULL);

    HANDLE threads[3] = {t1, t2, t3};
    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    CloseHandle(t1);
    CloseHandle(t2);
    CloseHandle(t3);
    DeleteCriticalSection(&lock);

    printf("Semua thread selesai.\n");
    return 0;
}