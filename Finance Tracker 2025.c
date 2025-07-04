#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TRANSAKSI 1000
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

int hariDalamBulan(int bulan) {
    switch (bulan) {
        case 2: return 28;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
    }
}

int isValidJenis(const char* jenis) {
    return strcmp(jenis, "D") == 0 || strcmp(jenis, "K") == 0;
}

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

        printf("Jenis transaksi (Debit(D)/Kredit(K)): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%s", t.jenis);

        while (!isValidJenis(t.jenis)) {
            printf("Jenis tidak valid! Masukkan 'D' atau 'K': ");
            fgets(buffer, sizeof(buffer), stdin);
            sscanf(buffer, "%s", t.jenis);
        }

        if (strcmp(t.jenis, "D") == 0)
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

void tampilkanSemuaTransaksi() {
    printf("\n=====================================================\n");
    printf("               DAFTAR SEMUA TRANSAKSI               \n");
    printf("=====================================================\n");
    if (jumlah_transaksi == 0) {
        printf("Belum ada transaksi tercatat.\n");
        return;
    }
    for (int i = 0; i < jumlah_transaksi; i++) {
        printf("%3d. %-12s | %-15s | %02d/%02d/%d | Rp %.2f\n",
               i + 1, data[i].jenis, data[i].kategori,
               data[i].hari, data[i].bulan, data[i].tahun, data[i].nominal);
    }
}

void tampilkanTotalBulanan() {
    int bulan;
    float total = 0;
    printf("\n>> Masukkan bulan (1-12): ");
    scanf("%d", &bulan);
    getchar();

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
        printf("Tidak ada pengeluaran tercatat pada bulan ini.\n");
    }
}

void tampilkanTotalMingguan() {
    int minggu, bulan;
    printf("\n>> Masukkan bulan (1-12): ");
    scanf("%d", &bulan);
    printf(">> Masukkan minggu ke- (1-5): ");
    scanf("%d", &minggu);
    getchar();

    float total = 0;
    int start = (minggu - 1) * 7 + 1;
    int end = minggu * 7;

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

void hapusTransaksi() {
    int index;
    tampilkanSemuaTransaksi();
    printf("\n>> Masukkan nomor transaksi yang ingin dihapus: ");
    scanf("%d", &index);
    getchar();

    if (index < 1 || index > jumlah_transaksi) {
        printf("Nomor transaksi tidak valid!\n");
        return;
    }

    for (int i = index - 1; i < jumlah_transaksi - 1; i++) {
        data[i] = data[i + 1];
    }
    jumlah_transaksi--;
    printf(">> Transaksi berhasil dihapus.\n");
}

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
    float persentase_sisa = (pemasukan > 0) ? (saldo / pemasukan) * 100 : 0;

    printf("\n-----------------------------------------------------\n");
    printf("LAPORAN KEUANGAN BULAN KE-%d\n", bulan);
    printf("-----------------------------------------------------\n");
    printf("Total pemasukan        : Rp %.2f\n", pemasukan);
    printf("Total pengeluaran      : Rp %.2f\n", pengeluaran);
    printf("Saldo akhir            : Rp %.2f\n", saldo);
    printf("Rata-rata pengeluaran  : Rp %.2f per hari\n", rata_rata);
    printf("-----------------------------------------------------\n");

    // Narasi laporan reflektif
    if (saldo < 0) {
        printf("Saldo di bulan ini minus. Usahakan pengeluaran lebih sedikit di bulan berikutnya.\n");
    } else if (persentase_sisa < 20) {
        printf("Saldo bulan ini hampir habis. Perlu pengelolaan yang lebih hati-hati bulan depan.\n");
    } else if (persentase_sisa < 50) {
        printf("Saldo masih tersisa, tapi mulai menipis. Jaga keseimbangan pengeluaran di bulan berikutnya.\n");
    } else {
        printf("Keuangan bulan ini cukup sehat. Pertahankan pola pengeluaran yang baik.\n");
    }
}


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
                    pengeluaran += data[i].nominal;

                hari_tercatat[data[i].hari] = 1;
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

void tampilkanSaldoAkhir() {
    float pemasukan = 0, pengeluaran = 0;
    for (int i = 0; i < jumlah_transaksi; i++) {
        if (strcmp(data[i].jenis, "pemasukan") == 0)
            pemasukan += data[i].nominal;
        else
            pengeluaran += data[i].nominal;
    }

    float saldo = pemasukan - pengeluaran;
    float persentase_sisa = (pemasukan > 0) ? (saldo / pemasukan) * 100 : 0;

    printf("\n=================================================\n");
    printf("   SALDO AKHIR KEUANGAN: Rp %.2f\n", saldo);
    printf("=================================================\n");

    // Peringatan tegas
    if (saldo < 0) {
        printf("!! PERINGATAN: SALDO NEGATIF !!\n");
        printf("   Pengeluaran melebihi pemasukan.\n");
        printf("   Segera evaluasi dan kurangi pengeluaran!\n");
    } else if (saldo == 0) {
        printf("!! PERINGATAN: SALDO HABIS !!\n");
        printf("   Semua pemasukan telah digunakan.\n");
        printf("   Hindari kondisi ini agar tidak masuk defisit!\n");
    } else if (persentase_sisa < 20) {
        printf("!! PERINGATAN: SALDO MENIPIS !!\n");
        printf("   Sisa saldo kurang dari 20%% dari total pemasukan.\n");
        printf("   Kendalikan pengeluaran secepatnya.\n");
    } else if (persentase_sisa < 50) {
        printf("! CATATAN: Saldo mulai menyusut.\n");
        printf("  Tetap waspada dalam mengatur pengeluaran.\n");
    } else {
        printf("Status: Saldo dalam kondisi sehat.\n");
        printf("         Pertahankan pengelolaan seperti ini.\n");
    }
}

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
            default: printf(">> Pilihan tidak valid.\n"); break;
        }

    } while (pilihan != 9);
}

int main() {
    menuUtama();
    return 0;
}
