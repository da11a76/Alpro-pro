#include <stdio.h>
#include <string.h>

#define MAX_TRANSAKSI 1800
#define MAX_INPUT_PER_SESI 10

typedef struct {
    int hari;
    int bulan;
    int tahun;
    char jenis[20];     // pemasukan / pengeluaran
    char kategori[50];  // bebas, misal "gaji", "uang makan"
    float nominal;
} Transaksi;

Transaksi data[MAX_TRANSAKSI];
int jumlah_transaksi = 0;

// === Fungsi Pendukung ===
int hariDalamBulan(int bulan) {
    switch (bulan) {
        case 2: return 28;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
    }
}

int ValidJenis(const char* jenis) {
    return strcmp(jenis, "IN") == 0 || strcmp(jenis, "OUT") == 0;
}

// Fungsi pembanding untuk insertion sort
int bandingkanTanggal(const Transaksi *ta, const Transaksi *tb) {
    if (ta->bulan != tb->bulan) return ta->bulan - tb->bulan;
    return ta->hari - tb->hari;
}

// === Insertion Sort untuk transaksi ===
void urutkanTransaksi() {
    for (int i = 1; i < jumlah_transaksi; i++) {
        Transaksi key = data[i];
        int j = i - 1;
        // Geser elemen yang lebih besar ke kanan
        while (j >= 0 && bandingkanTanggal(&data[j], &key) > 0) {
            data[j + 1] = data[j];
            j--;
        }
        data[j + 1] = key;
    }
}

// === Input Transaksi ===
void tambahTransaksi() {
    char ulangi = 'y';
    char buffer[100];
    int input_count = 0;

    while ((ulangi == 'y' || ulangi == 'Y') && input_count < MAX_INPUT_PER_SESI) {
        if (jumlah_transaksi >= MAX_TRANSAKSI) {
            printf("=================================================\n");
            printf("   PERINGATAN: Data transaksi sudah mencapai batas\n");
            printf("=================================================\n");
            return;
        }

        Transaksi t;
        printf("\n---------------------------------------------\n");
        printf("           INPUT TRANSAKSI BARU\n");
        printf("---------------------------------------------\n");

        printf("Jenis transaksi (Pemasukan(IN)/Pengeluaran(OUT)): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%s", t.jenis);

        while (!ValidJenis(t.jenis)) {
            printf("Jenis tidak valid! Masukkan 'IN' atau 'OUT': ");
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, "%s", t.jenis);
        }

        if (strcmp(t.jenis, "IN") == 0)
            strcpy(t.jenis, "pemasukan");
        else
            strcpy(t.jenis, "pengeluaran");

        printf("Bulan (1-12): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &t.bulan);
        while (t.bulan < 1 || t.bulan > 12) {
            printf("Bulan tidak valid! Masukkan angka antara 1-12: ");
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, "%d", &t.bulan);
        }

        int maxHari = hariDalamBulan(t.bulan);
        printf("Hari ke- (1-%d): ", maxHari);
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &t.hari);
        while (t.hari < 1 || t.hari > maxHari) {
            printf("Hari tidak valid! Masukkan antara 1 dan %d: ", maxHari);
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, "%d", &t.hari);
        }

        t.tahun = 2025;

        printf("Kategori transaksi: ");
        fgets(t.kategori, sizeof(t.kategori), stdin);
        t.kategori[strcspn(t.kategori, "\n")] = '\0';

        printf("Nominal (Rp): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%f", &t.nominal);
        while (t.nominal <= 0) {
            printf("Nominal tidak valid! Masukkan nominal positif: ");
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, "%f", &t.nominal);
        }

        data[jumlah_transaksi++] = t;
        input_count++;

        printf(">> Transaksi berhasil ditambahkan.\n");

        if (input_count < MAX_INPUT_PER_SESI) {
            printf("Ingin memasukkan transaksi lagi? (y/n): ");
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, " %c", &ulangi);
        } else {
            printf(">> Batas maksimum transaksi dalam satu sesi tercapai.\n");
        }
    }
}
// === Tampilkan Semua Transaksi ===
void tampilkanSemuaTransaksi() {
    urutkanTransaksi();
    int bulan;

    // Validasi input bulan
    do {
        printf("\n>> Masukkan bulan transaksi yang ingin dilihat (1-12): ");
        scanf("%d", &bulan);
        if (bulan < 1 || bulan > 12)
            printf("!! Bulan tidak valid. Masukkan angka antara 1 hingga 12.\n");
    } while (bulan < 1 || bulan > 12);

    getchar(); // bersihkan newline

    printf("\n=====================================================\n");
    printf("          DAFTAR TRANSAKSI BULAN %02d/2025          \n", bulan);
    printf("=====================================================\n");

    int count = 0;
    for (int i = 0; i < jumlah_transaksi; i++) {
        if (data[i].bulan == bulan) {
            count++;
            printf("%3d. %-12s | %-15s | %02d/%02d/%d | Rp %.2f\n",
                   count, data[i].jenis, data[i].kategori,
                   data[i].hari, data[i].bulan, data[i].tahun, data[i].nominal);
        }
    }

    if (count == 0)
        printf("Tidak ada transaksi pada bulan ini.\n");
}
// === Total Bulanan ===
void tampilkanTotalBulanan() {
    int bulan;
    float total = 0;

    // Validasi input bulan
    do {
        printf("\n>> Masukkan bulan (1-12): ");
        scanf("%d", &bulan);
        if (bulan < 1 || bulan > 12)
            printf("!! Bulan tidak valid. Masukkan angka antara 1 hingga 12.\n");
    } while (bulan < 1 || bulan > 12);

    getchar(); // bersihkan newline

    typedef struct {
        char kategori[50];
        float jumlah;
    } KategoriPengeluaran;

    KategoriPengeluaran kategori_list[MAX_TRANSAKSI];
    int kategori_count = 0;

    for (int i = 0; i < jumlah_transaksi; i++) {
        if (data[i].bulan == bulan && strcmp(data[i].jenis, "pengeluaran") == 0) {
            total += data[i].nominal;
            int found = 0;
            for (int j = 0; j < kategori_count; j++) {
                if (strcmp(kategori_list[j].kategori, data[i].kategori) == 0) {
                    kategori_list[j].jumlah += data[i].nominal;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(kategori_list[kategori_count].kategori, data[i].kategori);
                kategori_list[kategori_count].jumlah = data[i].nominal;
                kategori_count++;
            }
        }
    }

    printf("\n-----------------------------------------------------\n");
    printf("Total pengeluaran pada bulan %d: Rp %.2f\n", bulan, total);

    if (total > 0) {
        printf(">> Persentase pengeluaran per kategori:\n");
        for (int i = 0; i < kategori_count; i++) {
            float persen = (kategori_list[i].jumlah / total) * 100;
            printf("   - %-15s : Rp %.2f (%.2f%%)\n",
                   kategori_list[i].kategori, kategori_list[i].jumlah, persen);
        }
    } else {
        printf("Tidak ada pengeluaran tercatat");}}

// === Total Mingguan ===
void tampilkanTotalMingguan() {
    int bulan, minggu;

// Validasi bulan
do {
    printf("\n>> Masukkan bulan (1-12): ");
    scanf("%d", &bulan);
    if (bulan < 1 || bulan > 12)
        printf("!! Bulan tidak valid. Masukkan angka antara 1 hingga 12.\n");
} while (bulan < 1 || bulan > 12);

int hari_bulan = hariDalamBulan(bulan);

// Tentukan maksimum minggu sesuai jumlah hari
int max_minggu = (hari_bulan + 6) / 7;  // pembulatan ke atas

// Validasi minggu
do {
    printf(">> Masukkan minggu ke- (1-%d): ", max_minggu);
    scanf("%d", &minggu);
    if (minggu < 1 || minggu > max_minggu)
        printf("!! Minggu tidak valid. Masukkan angka antara 1 hingga %d.\n", max_minggu);
} while (minggu < 1 || minggu > max_minggu);

getchar(); // bersihkan newline

    float total = 0;
    int start = (minggu - 1) * 7 + 1;
    int end = minggu * 7;
    if (end > 31) end = 31; // maksimal hari dalam sebulan

    typedef struct {
        char kategori[50];
        float jumlah;
    } KategoriPengeluaran;

    KategoriPengeluaran kategori_list[MAX_TRANSAKSI];
    int kategori_count = 0;

    for (int i = 0; i < jumlah_transaksi; i++) {
        if (data[i].bulan == bulan &&
            data[i].hari >= start && data[i].hari <= end &&
            strcmp(data[i].jenis, "pengeluaran") == 0) {

            total += data[i].nominal;
            int found = 0;
            for (int j = 0; j < kategori_count; j++) {
                if (strcmp(kategori_list[j].kategori, data[i].kategori) == 0) {
                    kategori_list[j].jumlah += data[i].nominal;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(kategori_list[kategori_count].kategori, data[i].kategori);
                kategori_list[kategori_count].jumlah = data[i].nominal;
                kategori_count++;
            }
        }
    }

    printf("\n-----------------------------------------------------\n");
    printf("Total pengeluaran minggu ke-%d bulan %d: Rp %.2f\n", minggu, bulan, total);
    if (total > 0) {
        printf(">> Persentase pengeluaran per kategori:\n");
        for (int i = 0; i < kategori_count; i++) {
            float persen = (kategori_list[i].jumlah / total) * 100;
            printf("   - %-15s : Rp %.2f (%.2f%%)\n",
                   kategori_list[i].kategori, kategori_list[i].jumlah, persen);
        }
    } else {
        printf("Tidak ada pengeluaran pada minggu ini.\n");
    }
}

// === Hapus Transaksi ===
void hapusTransaksi() {
    urutkanTransaksi();
    int bulan;

    // Validasi input bulan agar antara 1-12
    do {
        printf("\n>> Masukkan bulan transaksi yang ingin dihapus (1-12): ");
        scanf("%d", &bulan);
        getchar();
        if (bulan < 1 || bulan > 12) {
            printf("!! Bulan tidak valid. Masukkan angka antara 1 hingga 12.\n");
        }
    } while (bulan < 1 || bulan > 12);

    // Tampilkan list filtered
    printf("\n=====================================================\n");
    printf("       HAPUS TRANSAKSI BULAN %02d/2025             \n", bulan);
    printf("=====================================================\n");

    int indeks[MAX_TRANSAKSI];
    int count = 0;
    for (int i = 0; i < jumlah_transaksi; i++) {
        if (data[i].bulan == bulan) {
            indeks[count] = i;
            count++;
            printf("%3d. %-12s | %-15s | %02d/%02d/%d | Rp %.2f\n",
                   count, data[i].jenis, data[i].kategori,
                   data[i].hari, data[i].bulan, data[i].tahun, data[i].nominal);
        }
    }
    if (count == 0) {
        printf("Tidak ada transaksi untuk dihapus di bulan ini.\n");
        return;
    }

    int pilih;
    printf("\n>> Masukkan nomor urutan transaksi di atas yang ingin dihapus: ");
    scanf("%d", &pilih);
    getchar();

    if (pilih < 1 || pilih > count) {
        printf("Nomor tidak valid!\n");
        return;
    }
    int idx = indeks[pilih - 1];
    // Geser data
    for (int i = idx; i < jumlah_transaksi - 1; i++) {
        data[i] = data[i + 1];
    }
    jumlah_transaksi--;
    printf(">> Transaksi berhasil dihapus.\n");
}

// === Saldo Bulanan ===
void tampilkanSaldoBulanan() {
    int bulan;
    float pemasukan = 0, pengeluaran = 0;
    printf("\n>> Masukkan bulan (1-12): ");
    scanf("%d", &bulan);
    getchar();

    // Penanda hari dengan pengeluaran
    int hari_pengeluaran[32] = {0};

    for (int i = 0; i < jumlah_transaksi; i++) {
        if (data[i].bulan == bulan) {
            if (strcmp(data[i].jenis, "pemasukan") == 0)
                pemasukan += data[i].nominal;
            else {
                pengeluaran += data[i].nominal;
                hari_pengeluaran[data[i].hari] = 1;
            }
        }
    }

    int hari_aktif = 0;
    for (int i = 1; i <= 31; i++) {
        if (hari_pengeluaran[i]) hari_aktif++;
    }

    float rata_rata = hari_aktif ? pengeluaran / hari_aktif : 0;
    float saldo = pemasukan - pengeluaran;

    printf("\n-----------------------------------------------------\n");
    printf("Saldo bulan %d         : Rp %.2f\n", bulan, saldo);
    printf("Rata-rata pengeluaran : Rp %.2f per hari\n", rata_rata);
}

// === Ringkasan Tahunan ===
void tampilkanRingkasanTahunan() {
    printf("\n==============================================================\n");
    printf("           RINGKASAN KEUANGAN BULANAN TAHUN 2025\n");
    printf("==============================================================\n");
    printf("Bulan |  Pemasukan  | Pengeluaran |   Saldo   | Rata-rata Harian\n");
    printf("--------------------------------------------------------------\n");

    for (int bulan = 1; bulan <= 12; bulan++) {
        float pemasukan = 0, pengeluaran = 0;
        int hari_tercatat[32] = {0};

        for (int i = 0; i < jumlah_transaksi; i++) {
            if (data[i].bulan == bulan && data[i].tahun == 2025) {
                if (strcmp(data[i].jenis, "pemasukan") == 0)
                    pemasukan += data[i].nominal;
                else
                    {pengeluaran += data[i].nominal;

                hari_tercatat[data[i].hari] = 1;}
            }
        }

        int hari_aktif = 0;
        for (int i = 1; i <= 31; i++) {
            if (hari_tercatat[i]) hari_aktif++;
        }

        float rata_rata = hari_aktif ? pengeluaran / hari_aktif : 0;
        float saldo = pemasukan - pengeluaran;

        printf("  %2d   | %10.2f  | %10.2f  | %8.2f | %17.2f\n",
               bulan, pemasukan, pengeluaran, saldo, rata_rata);
    }
}

// === Saldo Akhir ===
void tampilkanSaldoAkhir() {
    float pemasukan = 0, pengeluaran = 0;
    for (int i = 0; i < jumlah_transaksi; i++) {
        if (strcmp(data[i].jenis, "pemasukan") == 0)
            pemasukan += data[i].nominal;
        else
            pengeluaran += data[i].nominal;
    }
    printf("\n=================================================\n");
    printf("   SALDO AKHIR KEUANGAN: Rp %.2f\n", pemasukan - pengeluaran);
    printf("=================================================\n");
}

// === Ekspor CSV ===
void eksporCSV() {
    char namaFile[100];
    printf("\n>> Masukkan nama file untuk ekspor (contoh: data.csv): ");
    fgets(namaFile, sizeof(namaFile), stdin);
    namaFile[strcspn(namaFile, "\n")] = '\0'; // Hapus newline

    // Jika user tidak memasukkan .csv, tambahkan otomatis
    if (strstr(namaFile, ".csv") == NULL) {
        strcat(namaFile, ".csv");
    }

    FILE *file = fopen(namaFile, "w");
    if (!file) {
        printf("Gagal membuat file '%s'\n", namaFile);
        return;
    }

    fprintf(file, "Tanggal,Jenis,Kategori,Nominal\n");
    for (int i = 0; i < jumlah_transaksi; i++) {
        fprintf(file, "%02d/%02d/%04d,%s,%s,%.2f\n",
                data[i].hari, data[i].bulan, data[i].tahun,
                data[i].jenis, data[i].kategori, data[i].nominal);
    }

    fclose(file);
    printf(">> Data berhasil diekspor ke '%s'\n", namaFile);
}

// === Impor CSV ===
void imporCSV() {
    char namaFile[100];
    printf("\n>> Masukkan nama file untuk impor (contoh: data.csv): ");
    fgets(namaFile, sizeof(namaFile), stdin);
    namaFile[strcspn(namaFile, "\n")] = '\0';

    // Jika user tidak memasukkan .csv, tambahkan otomatis
    if (strstr(namaFile, ".csv") == NULL) {
        strcat(namaFile, ".csv");
    }

    FILE *file = fopen(namaFile, "r");
    if (!file) {
        printf("File '%s' tidak ditemukan.\n", namaFile);
        return;
    }

    char baris[200];
    fgets(baris, sizeof(baris), file); // Lewati header

    int jumlahSebelum = jumlah_transaksi;
    while (fgets(baris, sizeof(baris), file)) {
    if (jumlah_transaksi >= MAX_TRANSAKSI) {
        printf("Peringatan: Kapasitas maksimum tercapai, tidak semua data terimpor\n");
        break;
    }

    Transaksi t;
    if (sscanf(baris, "%d/%d/%d,%19[^,],%49[^,],%f",
               &t.hari, &t.bulan, &t.tahun,
               t.jenis, t.kategori, &t.nominal) == 6) {
        data[jumlah_transaksi++] = t;
    }
}
fclose(file);
urutkanTransaksi();
printf(">> Berhasil mengimpor %d transaksi dari '%s'\n",
       jumlah_transaksi - jumlahSebelum, namaFile);
}


// === Menu Utama ===
void menuUtama() {
    int pilihan;
    char buffer[100];

    do {
        printf("\n=============================================\n");
        printf("           FINANCE TRACKER 2025              \n");
        printf("=============================================\n");
        printf("1. Masukkan Transaksi\n");
        printf("2. Lihat Semua Transaksi\n");
        printf("3. Total Transaksi Bulanan\n");
        printf("4. Total Transaksi Mingguan\n");
        printf("5. Hapus Transaksi\n");
        printf("6. Tampilkan Saldo Bulanan\n");
        printf("7. Tampilkan Saldo Akhir\n");
        printf("8. Ringkasan Keuangan Bulanan\n");
        printf("9. Keluar\n");
        printf("10. Ekspor ke CSV\n");
        printf("11. Impor dari CSV\n");
        printf("---------------------------------------------\n");
        printf(">> Pilih menu: ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &pilihan);

        switch (pilihan) {
            case 1: tambahTransaksi(); break;
            case 2: tampilkanSemuaTransaksi(); break;
            case 3: tampilkanTotalBulanan(); break;
            case 4: tampilkanTotalMingguan(); break;
            case 5: hapusTransaksi(); break;
            case 6: tampilkanSaldoBulanan(); break;
            case 7: tampilkanSaldoAkhir(); break;
            case 8: tampilkanRingkasanTahunan(); break;
            case 9: printf(">> Terima kasih telah menggunakan Finance Tracker.\n"); break;
            case 10: eksporCSV(); break;
            case 11: imporCSV(); break;
            default: printf(">> Pilihan tidak valid.\n"); break;
        }

    } while (pilihan != 9);
}

// === Fungsi Main ===
int main() {
    menuUtama();
    return 0;
}
