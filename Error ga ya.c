#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ANGGOTA 40
#define MAX_TRANSAKSI 500
#define MAX_NAMA 100
#define MAX_KATEGORI 30
#define KAS_PER_BULAN 20000  // Konstanta kas tetap 20k per bulan

// Struktur tanggal
typedef struct { int d, m, y; } Tanggal;
// Struktur transaksi
typedef struct {
    int id;
    char nama[MAX_NAMA];    // anggota atau "(Umum)"
    char tipe[10];          // "kas" atau "keluar"
    int jumlah;
    char kategori[MAX_KATEGORI];
    Tanggal tgl;
    int bulan;              // 1..6 dalam semester
    int semester;           // 1 atau 2
} Transaksi;

// Struktur anggota
typedef struct {
    char nama[MAX_NAMA];
    int bayar[6];           // status bayar tiap bulan
    char label[200];        // label dinamis
    int jumlah_tunggakan;
} Anggota;

// Struktur bendahara
typedef struct {
    char nama[MAX_NAMA];
    char nim[20];
    char kode[20];
    Tanggal berjalan;
    int semesterAktif;
} Bendahara;

// Global data
Anggota anggota1[MAX_ANGGOTA]; int n1;
Anggota anggota2[MAX_ANGGOTA]; int n2;
Transaksi tx[MAX_TRANSAKSI]; int ntx;
Bendahara bh;

// Prototypes
int valid_login();
void loadAnggota();
int cmpTanggal(Tanggal a, Tanggal b);
int validTgl(Tanggal t);
int inSemester(Tanggal t, int sem);
void resetLabel(Anggota *a);
void hitungLabel(Anggota *a, int sem);
void updateAllLabels(int sem);
Anggota* findAnggota(int sem, const char *nama);
void menu();
void inputKas(int sem);
void inputKeluar(int sem);
void listAnggota(int sem);
void cariAnggota(int sem);
void ringkasan(int sem);
void listTransaksi(int sem, int bulan);
void hapusTransaksi(int sem, int bulan);
void bacaStr(char *buf, int max);

int main() {
    loadAnggota();
    if(!valid_login()) return 0;
    menu();
    return 0;
}

// Validasi login bendahara
typedef struct { char *nama, *nim, *kode; } Auth;
int valid_login() {
    Auth a = {"NAILA SANDRA UTAMI","212413707","1ST2BNDTP2025"};
    char buf[MAX_NAMA];
    int valid_nama = 0, valid_nim = 0, valid_kode = 0;

    printf("==== LOGIN BENDAHARA ===\n");

    // Validasi Nama
    while (!valid_nama) {
        printf("Nama: "); fread(buf,1,0,stdin);  // flush
        fgets(buf, MAX_NAMA, stdin); buf[strcspn(buf,"\n")] = '\0';
        if (strcmp(buf, a.nama) != 0)
            printf("Nama salah. Coba lagi.\n");
        else
            valid_nama = 1;
    }

    // Validasi NIM
    while (!valid_nim) {
        printf("NIM: ");
        fgets(buf, MAX_NAMA, stdin); buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, a.nim) != 0)
            printf("NIM salah. Coba lagi.\n");
        else
            valid_nim = 1;
    }

    // Validasi Kode
    while (!valid_kode) {
        printf("Kode Aktivasi: ");
        fgets(buf, MAX_NAMA, stdin); buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, a.kode) != 0)
            printf("Kode salah. Coba lagi.\n");
        else
            valid_kode = 1;
    }

    // Input Tanggal Berjalan
    while (1) {
        printf("Tanggal Berjalan (dd mm yyyy): ");
        if (scanf("%d %d %d", &bh.berjalan.d, &bh.berjalan.m, &bh.berjalan.y) != 3) {
            printf("Input tidak valid. Gunakan format dd mm yyyy.\n");
            while (getchar() != '\n');
            continue;
        }
    if (!validTgl(bh.berjalan) || (!inSemester(bh.berjalan, 1) && !inSemester(bh.berjalan, 2))) {
        printf("Tanggal tidak valid atau tidak termasuk dalam semester aktif.\n");
        continue;
    }
        break;
    }
    getchar(); // bersihkan newline
    // Tentukan semesterAktif berdasar tanggal berjalan
    if (inSemester(bh.berjalan, 1)) {
        bh.semesterAktif = 1;
            } else if (inSemester(bh.berjalan, 2)) {
        bh.semesterAktif = 2;
        } else {
    // (seharusnya tidak sampai ke sini karena sudah divalidasi)
        bh.semesterAktif = 0;
        }

    // Set data login
    printf("Login sukses. Waktu berjalan: %02d-%02d-%04d\n", bh.berjalan.d, bh.berjalan.m, bh.berjalan.y);
    strcpy(bh.nama, a.nama);
    strcpy(bh.nim, a.nim);
    strcpy(bh.kode, a.kode);
    return 1;
}

// Muat data anggota
void loadAnggota() {
    const char *list1[] = {
        "ADINDA ZAKHRA RAMADHANI","ALFIAN ZHIQRI ALFAUZAN","AL-FIAR DI FESA FASA",
        "ALFIYAH REZEQI ROZANAH","AMELIA DERANAYA SHAFIRA","ANANDA PUTRI INDRAWAN",
        "EDIGIO FELIX GODINHO DA COSTA","EMIL SALIM","FAHMI IDRIS SETIAWAN",
        "FARODISILJINAN","FAZL AQMAR KARIM AMRULLAH","FEBRIAN RUBBEN",
        "FIKRI ADI NUGRAHA","GAYUH AURA ALHAQ","GENIVEVA ROSA GUTERRES DE SOUSA BELO",
        "GHAZY RHAYSHIE","I KOMANG VITO WIRYADINATA","IFALIA HALIZAH",
        "KHAIRUNA AMANDA RIYADI","MEI INDRIYANTI SYAMSI","MUHAMMAD ABDUL LATHIF",
        "MUHAMMAD ADITYA UTAMA","MUHAMMAD DAFFA ALIMUDDIN PRATAMA","NABIL MAHASIN",
        "NAILA SANDRA UTAMI","NAKHWA AZZAHRA","NAZHIFUDDIN AKMAL",
        "NISWAH NABILA MUKHLIS","ROMALDA MITA AFOAN BELO","SHEILA BERLIANA SUNAN",
        "SITI NAIRA HASIEFAH HARAHAP","SITTI NAWWALIA SAUMY","SYIFA SHAVILA SURESWARI",
        "TAUFIQ ANANDA DIFKA","VERNINDA INRI MAULIDA RISMA","WAHDANA DJUNAEDI",
        "YEHISKIEL ERWIN TAMBUNAN", NULL
    };
    for(n1=0; list1[n1]; n1++) {
        strncpy(anggota1[n1].nama,list1[n1],MAX_NAMA);
        memset(anggota1[n1].bayar,0,sizeof(anggota1[n1].bayar));
        anggota1[n1].label[0]='\0';
    }
    const char *list2[] = {
        "ALFIAN ZHIQRI ALFAUZAN","ALFIYAH REZEQI ROZANAH","AMELIA DERANAYA SHAFIRA",
        "ANANDA PUTRI INDRAWAN","EDIGIO FELIX GODINHO DA COSTA","EMIL SALIM",
        "FAHMI IDRIS SETIAWAN","FARODISILJINAN","FAZL AQMAR KARIM AMRULLAH",
        "FEBRIAN RUBBEN","FIKRI ADI NUGRAHA","GAYUH AURA ALHAQ",
        "GENIVEVA ROSA GUTERRES DE SOUSA BELO","GHAZY RHAYSHIE","I KOMANG VITO WIRYADINATA",
        "IFALIA HALIZAH","KHAIRUNA AMANDA RIYADI","MEI INDRIYANTI SYAMSI",
        "MUHAMMAD ABDUL LATHIF","MUHAMMAD ADITYA UTAMA","MUHAMMAD DAFFA ALIMUDDIN PRATAMA",
        "NABIL MAHASIN","NAILA SANDRA UTAMI","NAKHWA AZZAHRA",
        "NAZHIFUDDIN AKMAL","NISWAH NABILA MUKHLIS","ROMALDA MITA AFOAN BELO",
        "SHEILA BERLIANA SUNAN","SITI NAIRA HASIEFAH HARAHAP","SITTI NAWWALIA SAUMY",
        "SYIFA SHAVILA SURESWARI","TAUFIQ ANANDA DIFKA","VERNINDA INRI MAULIDA RISMA",
        "WAHDANA DJUNAEDI","YEHISKIEL ERWIN TAMBUNAN", NULL
    };
    for(n2=0; list2[n2]; n2++) {
        strncpy(anggota2[n2].nama,list2[n2],MAX_NAMA);
        memset(anggota2[n2].bayar,0,sizeof(anggota2[n2].bayar));
        anggota2[n2].label[0]='\0';
    }
    ntx=0;
}

int cmpTanggal(Tanggal a, Tanggal b) {
    if(a.y!=b.y) return a.y-b.y;
    if(a.m!=b.m) return a.m-b.m;
    return a.d-b.d;
}
int validTgl(Tanggal t) {
    if(t.y<2024||t.y>2025) return 0;
    if(t.m<1||t.m>12) return 0;
    int md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(t.m==2&&(t.y%400==0||(t.y%4==0&&t.y%100!=0))) md[2]=29;
    if(t.d<1||t.d>md[t.m]) return 0;
    return 1;
}
int inSemester(Tanggal t, int sem) {
    Tanggal s,e;
    if(sem==1) { s=(Tanggal){23,9,2024}; e=(Tanggal){28,2,2025}; }
    else      { s=(Tanggal){3,3,2025};  e=(Tanggal){29,9,2025}; }
    return cmpTanggal(t,s)>=0 && cmpTanggal(t,e)<=0;
}
void resetLabel(Anggota *a) {
    a->label[0] = '\0';
    a->jumlah_tunggakan = 0;
}

// Hitung label dan tunggakan sampai bulan yang relevan
void hitungLabel(Anggota *a, int sem) {
    resetLabel(a);
    int tung = 0;
    int bayar_berturut = 0;
    int max_berturut = 0;

    // Tentukan bulan berjalan (1..6) dalam semester sem
    int bulan_sekarang = 6;
    if (inSemester(bh.berjalan, sem)) {
        int m = bh.berjalan.m;
        if (sem == 1) {
            // Semester 1: Sep(9)->1, Oct->2, Nov->3, Dec->4, Jan->5, Feb->6
            bulan_sekarang = (m >= 9 ? m - 8 : m + 4);
        } else {
            // Semester 2: Mar(3)->1, Apr->2, ..., Sep(9)->7? Tapi kita batasi 6
            bulan_sekarang = m - 2;
        }
        if (bulan_sekarang < 1) bulan_sekarang = 1;
        if (bulan_sekarang > 6) bulan_sekarang = 6;
    }

    for (int i = 0; i < 6; i++) {
        int bl = i + 1;
        // Lewati bulan yang belum tiba
        if (bl > bulan_sekarang)
            continue;

        // Untuk bulan berjalan: hanya count tunggakan jika hari > 10
        if (bl == bulan_sekarang && bh.berjalan.d <= 10)
            continue;

        if (a->bayar[i] == 1) {
            bayar_berturut++;
            if (bayar_berturut > max_berturut)
                max_berturut = bayar_berturut;
        } else {
            tung++;
            bayar_berturut = 0;
        }
    }

    a->jumlah_tunggakan = tung;

    // Buat label sesuai jumlah tunggakan dan streak bayar
    if (tung == 0) {
        strcat(a->label, "Si Konsisten Hebat ");
    }
    if (max_berturut >= 3) {
        strcat(a->label, "Elite Global ");
    } else if (max_berturut == 2) {
        strcat(a->label, "Awal Elite Global ");
    }
    if (tung >= 3) {
        strcat(a->label, "Si Penghutang Handal ");
    } else if (tung == 2) {
        strcat(a->label, "Si Pelupa ");
    }
}


void updateAllLabels(int sem) {
    Anggota *arr = (sem == 1 ? anggota1 : anggota2);
    int n = (sem == 1 ? n1 : n2);
    for (int i = 0; i < n; i++) {
        hitungLabel(&arr[i], sem);
    }
}

Anggota* findAnggota(int sem, const char *nama) {
    Anggota *arr=(sem==1?anggota1:anggota2);
    int n=(sem==1?n1:n2);
    for(int i=0;i<n;i++) if(!strcasecmp(arr[i].nama,nama)) return &arr[i];
    return NULL;
}
void bacaStr(char *buf,int max){ fgets(buf,max,stdin); buf[strcspn(buf,"\n")]='\0'; }
// Tampilkan daftar anggota dan status bayar & label mereka
// Fungsi listAnggota: Menampilkan daftar anggota dengan format tabel rapi
void listAnggota(int sem) {
    Anggota *arr = (sem == 1 ? anggota1 : anggota2);
    int       n   = (sem == 1 ? n1 : n2);

    // Hitung bulan berjalan (1..6)
    int bulanSekarang = 6;
    if (inSemester(bh.berjalan, sem)) {
        int m = bh.berjalan.m;
        if (sem == 1) bulanSekarang = (m >= 9 ? m - 8 : m + 4);
        else          bulanSekarang = m - 2;
        bulanSekarang = bulanSekarang < 1 ? 1 : (bulanSekarang > 6 ? 6 : bulanSekarang);
    }

    // Definisi lebar kolom
    const int W_NO = 4;
    const int W_NAMA = 40;
    const int W_TAGIHAN = 7;
    const int W_LABEL = 30;

    // Cetak header tabel
    printf("\n== Daftar Anggota & Tagihan s/d Bulan %d ==\n", bulanSekarang);
    printf("+%.*s+%.*s+%.*s+%.*s+\n",
           W_NO+2, "--------------------------------",
           W_NAMA+2, "----------------------------------------",
           W_TAGIHAN+2, "--------",
           W_LABEL+2, "--------------------------------");
    printf("| %-*s | %-*s | %-*s | %-*s |\n",
           W_NO, "No",
           W_NAMA, "Nama",
           W_TAGIHAN, "Tagihan",
           W_LABEL, "Label");
    printf("+%.*s+%.*s+%.*s+%.*s+\n",
           W_NO+2, "--------------------------------",
           W_NAMA+2, "----------------------------------------",
           W_TAGIHAN+2, "--------",
           W_LABEL+2, "--------------------------------");

    // Baris data anggota
    for (int i = 0; i < n; i++) {
        // Hitung tagihan hingga bulanSekarang
        int tagihan = 0;
        for (int b = 0; b < bulanSekarang; b++) if (arr[i].bayar[b] == 0) tagihan++;

        // Tampilkan baris
        printf("| %*d | %-*.*s | %*d | %-*.*s |\n",
               W_NO, i + 1,
               W_NAMA, W_NAMA, arr[i].nama,
               W_TAGIHAN, tagihan,
               W_LABEL, W_LABEL, arr[i].label);
    }

    // Footer tabel
    printf("+%.*s+%.*s+%.*s+%.*s+\n",
           W_NO+2, "--------------------------------",
           W_NAMA+2, "----------------------------------------",
           W_TAGIHAN+2, "--------",
           W_LABEL+2, "--------------------------------");
}


// Cari anggota per nama dan tunjukkan jumlah tunggakan
void cariAnggota(int sem) {
    char namaCari[MAX_NAMA];
    printf("\n== CARI ANGGOTA & TUNGGAKAN (Semester %d) ==\n", sem);
    printf("Masukkan nama anggota (atau '0' untuk batal): ");
    bacaStr(namaCari, MAX_NAMA);
    if (strcmp(namaCari, "0") == 0) return;

    Anggota *a = findAnggota(sem, namaCari);
    if (!a) {
        printf("[ERROR] Anggota '%s' tidak ditemukan di semester %d.\n", namaCari, sem);
        return;
    }

    // Hitung ulang tunggakan sesuai logika 10 hari
    resetLabel(a);
    int tung = 0, streak = 0, maxStreak = 0;

    // Tentukan bulan berjalan (1..6)
    int bulanSekarang = 6;
    if (inSemester(bh.berjalan, sem)) {
        int m = bh.berjalan.m;
        if (sem == 1) {
            bulanSekarang = (m >= 9 ? m - 8 : m + 4);
        } else {
            bulanSekarang = m - 2;
        }
        if (bulanSekarang < 1) bulanSekarang = 1;
        if (bulanSekarang > 6) bulanSekarang = 6;
    }

    for (int i = 0; i < bulanSekarang; i++) {
        // untuk bulan berjalan, skip jika hari <= 10
        if (i == bulanSekarang-1 && bh.berjalan.d <= 10) break;

        if (a->bayar[i]) {
            streak++;
            if (streak > maxStreak) maxStreak = streak;
        } else {
            tung++;
            streak = 0;
        }
    }
    a->jumlah_tunggakan = tung;

    // Susun label
    if (tung == 0)           strcat(a->label, "Si Konsisten Hebat ");
    if (maxStreak >= 3)      strcat(a->label, "Elite Global ");
    else if (maxStreak == 2) strcat(a->label, "Awal Elite Global ");
    if (tung >= 3)           strcat(a->label, "Si Penghutang Handal ");
    else if (tung == 2)      strcat(a->label, "Si Pelupa ");

    // Tampilkan hasil
    printf("\nNama          : %s\n", a->nama);
    printf("Status Bayar  : ");
    for (int i = 0; i < bulanSekarang; i++) {
        if (i == bulanSekarang-1 && bh.berjalan.d <= 10) break;
        printf("%d ", a->bayar[i]);
    }
    printf("\nTunggakan     : %d bulan\n", a->jumlah_tunggakan);
    printf("Label         : %s\n", a->label);
    printf("----------------------------------------\n");
}


void menu() {
    int choice;
    int sem = bh.semesterAktif;
    while(1) {
        printf("\n+==============================================+\n");
        printf("|      APLIKASI KEUANGAN BENDAHARA KELAS      |\n");
        printf("|              Semester %d               |\n",sem);
        printf("|          Tanggal: %02d-%02d-%04d           |\n",bh.berjalan.d,bh.berjalan.m,bh.berjalan.y);
        printf("|          Kas per bulan: Rp %d,00          |\n", KAS_PER_BULAN);
        printf("+==============================================+\n");
        printf("| 1. Input Kas Masuk                          |\n");
        printf("| 2. Input Pengeluaran                        |\n");
        printf("| 3. Lihat Daftar Anggota & Status            |\n");
        printf("| 4. Ringkasan Keuangan Semester              |\n");
        printf("| 5. Cari Anggota & Tunggakan                 |\n");
        printf("| 6. Lihat Transaksi Bulan & Semester         |\n");
        printf("| 7. Hapus Transaksi                          |\n");
        printf("| 8. Keluar                                   |\n");
        printf("+==============================================+\n");
        printf("Pilih menu [1-8]: ");
        if(scanf("%d",&choice)!=1){ getchar(); continue; }
        getchar();
        switch(choice) {
            case 1: inputKas(sem); break;
            case 2: inputKeluar(sem); break;
            case 3: listAnggota(sem); break;
            case 4: ringkasan(sem); break;
            case 5: cariAnggota(sem); break;
            case 6: {
        // Hitung bulanSekarang untuk semester aktif
        int bulanSekarang = 6;
        if (inSemester(bh.berjalan, sem)) {
            int m = bh.berjalan.m;
            if (sem == 1) bulanSekarang = (m >= 9 ? m - 8 : m + 4);
            else          bulanSekarang = m - 2;
            bulanSekarang = bulanSekarang < 1 ? 1 : (bulanSekarang > 6 ? 6 : bulanSekarang);
            }

            int b;
            printf("Masukkan bulan (1-%d): ", bulanSekarang);
            if (scanf("%d",&b)!=1) { getchar(); break; }
            getchar();
            if (b < 1 || b > bulanSekarang) {
            printf("Waktu belum terlewati!\n");
            } else {
            listTransaksi(sem, b);
            }
            break;
            }
            case 7: {
                int b; printf("Masukkan bulan untuk hapus (1-6): "); scanf("%d",&b); getchar();
                hapusTransaksi(sem,b); break;
            }
            case 8: printf("\nSimpan data dan keluar...\n"); return;
            default: printf("Pilihan tidak valid! Silakan coba lagi.\n");
        }
    }
}

// Fungsi inputKas yang dimodifikasi dengan kas tetap 20k per bulan
// Fungsi inputKas yang dimodifikasi dengan validasi input yang lebih baik
void inputKas(int sem) {
    char lanjut = 'y';

    while (lanjut == 'y' || lanjut == 'Y') {
        printf("\n== Input Kas Masuk (Semester %d) ==\n", sem);
        printf("** Kas tetap: Rp %d,00 per bulan **\n", KAS_PER_BULAN);

        char nama[MAX_NAMA];
        Anggota *a = NULL;

        // Loop untuk input nama anggota
        while (a == NULL) {
            printf("Nama anggota (0=kembali ke menu): ");
            bacaStr(nama, MAX_NAMA);

            // Cek jika user ingin keluar
            if (strcmp(nama, "0") == 0) {
                printf("Kembali ke menu utama.\n");
                return;
            }

            a = findAnggota(sem, nama);
            if (a == NULL) {
                printf(" Anggota tidak ditemukan. Silakan coba lagi.\n");
                printf(" Pastikan nama sudah benar dan sesuai dengan daftar anggota.\n\n");
            }
        }

        // Tampilkan status pembayaran saat ini
        printf("\n Anggota ditemukan: %s\n", a->nama);
        printf("Status pembayaran saat ini: ");
        for(int i = 0; i < 6; i++) {
            printf("%s%d", (i > 0 ? " " : ""), a->bayar[i]);
        }
        printf(" (1=sudah bayar, 0=belum bayar)\n");

        // Loop untuk pilihan metode pembayaran
        int pilihan = 0;
        while (pilihan != 1 && pilihan != 2) {
            printf("\nPilih metode pembayaran:\n");
            printf("1. Bayar bulan tertentu\n");
            printf("2. Bayar cicilan berturut-turut dari bulan 1\n");
            printf("0. Kembali ke menu utama\n");
            printf("Pilihan (0/1/2): ");

            if (scanf("%d", &pilihan) != 1) {
                printf(" Input tidak valid! Harap masukkan angka 0, 1, atau 2.\n");
                while (getchar() != '\n'); // bersihkan buffer
                continue;
            }

            if (pilihan == 0) {
                printf("Kembali ke menu utama.\n");
                return;
            }

            if (pilihan != 1 && pilihan != 2) {
                printf(" Pilihan tidak valid! Harap pilih 0, 1, atau 2.\n");
            }
        }

        if (pilihan == 1) {
            // Bayar bulan tertentu
            int bulan = -1;
            while (bulan < 0 || bulan > 6) {
                printf("Masukkan bulan yang akan dibayar (1-6, 0=batal): ");
                if (scanf("%d", &bulan) != 1) {
                    printf(" Input tidak valid! Harap masukkan angka antara 0-6.\n");
                    while (getchar() != '\n'); // bersihkan buffer
                    continue;
                }

                if (bulan == 0) {
                    printf("Pembayaran dibatalkan.\n");
                    break;
                }

                if (bulan < 1 || bulan > 6) {
                    printf("Bulan tidak valid! Harap masukkan angka antara 1-6.\n");
                }
            }

            if (bulan == 0) continue; // kembali ke awal loop while lanjut

            if (a->bayar[bulan-1] == 1) {
                printf(" Bulan %d sudah dibayar sebelumnya.\n", bulan);
                printf("Apakah Anda ingin melanjutkan input kas? (y/n): ");
                char lanjut_input;
                scanf(" %c", &lanjut_input);
                if (lanjut_input != 'y' && lanjut_input != 'Y') {
                    continue; // kembali ke awal loop while lanjut
                }
            }

            // Konfirmasi pembayaran
            printf("\n=== KONFIRMASI PEMBAYARAN ===\n");
            printf("Anggota: %s\n", a->nama);
            printf("Bulan: %d\n", bulan);
            printf("Jumlah: Rp %d,00\n", KAS_PER_BULAN);

            char konfirmasi = 'n';
            while (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N') {
                printf("Lanjutkan pembayaran? (y/n): ");
                scanf(" %c", &konfirmasi);
                if (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N') {
                    printf(" Input tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak.\n");
                }
            }

            if (konfirmasi == 'n' || konfirmasi == 'N') {
                printf("Pembayaran dibatalkan.\n");
                continue; // kembali ke awal loop while lanjut
            }

            // Input tanggal transaksi dengan validasi
            Tanggal t;
            int tanggal_valid = 0;
            while (!tanggal_valid) {
                printf("Tanggal transaksi (dd mm yyyy): ");
                if (scanf("%d %d %d", &t.d, &t.m, &t.y) != 3) {
                    printf(" Format tanggal tidak valid! Harap gunakan format: dd mm yyyy\n");
                    printf(" Contoh: 15 10 2024\n");
                    while (getchar() != '\n'); // bersihkan buffer
                    continue;
                }

                if (!validTgl(t)) {
                    printf(" Tanggal tidak valid! Periksa kembali tanggal yang dimasukkan.\n");
                    continue;
                }

                if (!inSemester(t, sem)) {
                    printf(" Tanggal tidak berada dalam semester %d!\n", sem);
                    printf(" Pastikan tanggal berada dalam rentang semester yang aktif.\n");
                    continue;
                }

                if (cmpTanggal(t, bh.berjalan) > 0) {
                    printf(" Tanggal transaksi tidak boleh melebihi tanggal berjalan (%02d-%02d-%04d)!\n",
                           bh.berjalan.d, bh.berjalan.m, bh.berjalan.y);
                    continue;
                }

                tanggal_valid = 1;
            }
            getchar(); // bersihkan newline

            // Catat transaksi
            Transaksi *p = &tx[ntx++];
            p->id = ntx;
            strcpy(p->nama, a->nama);
            strcpy(p->tipe, "kas");
            p->jumlah = KAS_PER_BULAN;
            sprintf(p->kategori, "Kas Bulan %d", bulan);
            p->tgl = t;
            p->bulan = bulan;
            p->semester = sem;

            // Update status pembayaran
            a->bayar[bulan-1] = 1;

            printf(" Kas bulan %d berhasil dicatat untuk %s.\n", bulan, a->nama);

        } else if (pilihan == 2) {
            // Bayar cicilan berturut-turut
            int jumlahBulan = -1;
            while (jumlahBulan < 0 || jumlahBulan > 6) {
                printf("Jumlah bulan yang akan dibayar (1-6, 0=batal): ");
                if (scanf("%d", &jumlahBulan) != 1) {
                    printf(" Input tidak valid! Harap masukkan angka antara 0-6.\n");
                    while (getchar() != '\n'); // bersihkan buffer
                    continue;
                }

                if (jumlahBulan == 0) {
                    printf("Pembayaran dibatalkan.\n");
                    break;
                }

                if (jumlahBulan < 1 || jumlahBulan > 6) {
                    printf(" Jumlah bulan tidak valid! Harap masukkan angka antara 1-6.\n");
                }
            }

            if (jumlahBulan == 0) continue; // kembali ke awal loop while lanjut

            // Cek apakah ada bulan yang sudah dibayar
            int bulanSudahBayar = 0;
            for (int i = 0; i < jumlahBulan; i++) {
                if (a->bayar[i] == 1) {
                    bulanSudahBayar = 1;
                    break;
                }
            }

            if (bulanSudahBayar) {
                printf(" Ada bulan yang sudah dibayar dalam rentang tersebut.\n");
                printf(" Gunakan opsi 'Bayar bulan tertentu' untuk bulan yang belum dibayar.\n");
                printf("Apakah Anda ingin kembali ke pilihan metode pembayaran? (y/n): ");
                char kembali;
                scanf(" %c", &kembali);
                if (kembali == 'y' || kembali == 'Y') {
                    continue; // kembali ke awal loop while lanjut
                }
                continue; // kembali ke awal loop while lanjut
            }

            // Konfirmasi pembayaran
            printf("\n=== KONFIRMASI PEMBAYARAN ===\n");
            printf("Anggota: %s\n", a->nama);
            printf("Bulan: 1 sampai %d\n", jumlahBulan);
            printf("Total: Rp %d,00\n", KAS_PER_BULAN * jumlahBulan);

            char konfirmasi = 'x';
            while (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N') {
                printf("Lanjutkan pembayaran? (y/n): ");
                scanf(" %c", &konfirmasi);
                if (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N') {
                    printf(" Input tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak.\n");
                }
            }

            if (konfirmasi == 'n' || konfirmasi == 'N') {
                printf("Pembayaran dibatalkan.\n");
                continue; // kembali ke awal loop while lanjut
            }

            // Input tanggal transaksi dengan validasi
            Tanggal t;
            int tanggal_valid = 0;
            while (!tanggal_valid) {
                printf("Tanggal transaksi (dd mm yyyy): ");
                if (scanf("%d %d %d", &t.d, &t.m, &t.y) != 3) {
                    printf(" Format tanggal tidak valid! Harap gunakan format: dd mm yyyy\n");
                    printf(" Contoh: 15 10 2024\n");
                    while (getchar() != '\n'); // bersihkan buffer
                    continue;
                }

                if (!validTgl(t)) {
                    printf(" Tanggal tidak valid! Periksa kembali tanggal yang dimasukkan.\n");
                    continue;
                }

                if (!inSemester(t, sem)) {
                    printf(" Tanggal tidak berada dalam semester %d!\n", sem);
                    printf(" Pastikan tanggal berada dalam rentang semester yang aktif.\n");
                    continue;
                }

                if (cmpTanggal(t, bh.berjalan) > 0) {
                    printf(" Tanggal transaksi tidak boleh melebihi tanggal berjalan (%02d-%02d-%04d)!\n",
                           bh.berjalan.d, bh.berjalan.m, bh.berjalan.y);
                    continue;
                }

                tanggal_valid = 1;
            }
            getchar(); // bersihkan newline

            // Catat transaksi untuk setiap bulan
            for (int i = 0; i < jumlahBulan; i++) {
                Transaksi *p = &tx[ntx++];
                p->id = ntx;
                strcpy(p->nama, a->nama);
                strcpy(p->tipe, "kas");
                p->jumlah = KAS_PER_BULAN;
                sprintf(p->kategori, "Kas Bulan %d", i + 1);
                p->tgl = t;
                p->bulan = i + 1;
                p->semester = sem;

                // Update status pembayaran
                a->bayar[i] = 1;
            }

            printf(" Kas untuk %d bulan berhasil dicatat untuk %s.\n", jumlahBulan, a->nama);
        }

        // Update label untuk semua anggota
        updateAllLabels(sem);
        printf(" Label anggota diperbarui.\n");

        // Tanya apakah ingin menambah transaksi lagi
        printf("\nApakah Anda ingin menambah transaksi kas lagi? (y/n): ");
        scanf(" %c", &lanjut);
        while (lanjut != 'y' && lanjut != 'Y' && lanjut != 'n' && lanjut != 'N') {
            printf(" Input tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak.\n");
            printf("Apakah Anda ingin menambah transaksi kas lagi? (y/n): ");
            scanf(" %c", &lanjut);
        }
    }

    printf("Selesai input kas masuk.\n");
}

// Fungsi inputKeluar yang dimodifikasi dengan validasi input yang lebih baik
void inputKeluar(int sem) {
    char lanjut = 'y';

    while (lanjut == 'y' || lanjut == 'Y') {
        printf("\n== Input Pengeluaran (Semester %d) ==\n", sem);

        int nominal = 0;
        char kategori[MAX_KATEGORI];
        Tanggal t;

        // Loop untuk input nominal dengan validasi
        while (nominal <= 0) {
            printf("Nominal pengeluaran (Rp, ketik 0 untuk batal): ");
            if (scanf("%d", &nominal) != 1) {
                printf(" Input tidak valid! Harap masukkan angka untuk nominal pengeluaran.\n");
                printf(" Contoh: 50000 (untuk Rp 50.000)\n");
                while (getchar() != '\n'); // bersihkan buffer
                continue;
            }

            if (nominal == 0) {
                printf("Input pengeluaran dibatalkan.\n");
                return;
            }

            if (nominal < 0) {
                printf(" Nominal tidak boleh negatif! Silakan masukkan nominal yang valid.\n");
                nominal = 0; // reset untuk loop
            }
        }
        getchar(); // bersihkan newline setelah scanf

        // Loop untuk input kategori dengan validasi
        int kategori_valid = 0;
        while (!kategori_valid) {
            printf("Kategori pengeluaran (ketik '0' untuk batal): ");
            bacaStr(kategori, MAX_KATEGORI);

            if (strcmp(kategori, "0") == 0) {
                printf("Input pengeluaran dibatalkan.\n");
                return;
            }

            // Cek apakah kategori kosong atau hanya berisi spasi
            int kosong = 1;
            for (int i = 0; kategori[i] != '\0'; i++) {
                if (kategori[i] != ' ' && kategori[i] != '\t') {
                    kosong = 0;
                    break;
                }
            }

            if (kosong || strlen(kategori) == 0) {
                printf(" Kategori tidak boleh kosong! Silakan masukkan kategori pengeluaran.\n");
                printf(" Contoh: Snack, ATK, Transport, dll.\n");
            } else {
                kategori_valid = 1;
            }
        }

        // Loop untuk input tanggal dengan validasi
        int tanggal_valid = 0;
        while (!tanggal_valid) {
            printf("Tanggal transaksi (dd mm yyyy): ");
            if (scanf("%d %d %d", &t.d, &t.m, &t.y) != 3) {
                printf(" Format tanggal tidak valid! Harap gunakan format: dd mm yyyy\n");
                printf(" Contoh: 15 10 2024\n");
                while (getchar() != '\n'); // bersihkan buffer
                continue;
            }

            if (!validTgl(t)) {
                printf(" Tanggal tidak valid! Periksa kembali tanggal yang dimasukkan.\n");
                printf(" Pastikan format tanggal benar (dd mm yyyy) dan tanggal ada dalam kalender.\n");
                continue;
            }

            if (!inSemester(t, sem)) {
                printf(" Tanggal tidak berada dalam semester %d!\n", sem);
                printf(" Pastikan tanggal berada dalam rentang semester yang aktif.\n");
                continue;
            }

            if (cmpTanggal(t, bh.berjalan) > 0) {
                printf("Tanggal transaksi tidak boleh melebihi tanggal berjalan (%02d-%02d-%04d)!\n",
                       bh.berjalan.d, bh.berjalan.m, bh.berjalan.y);
                continue;
            }

            tanggal_valid = 1;
        }
        getchar(); // bersihkan newline

        // Konfirmasi sebelum menyimpan
        printf("\n=== KONFIRMASI PENGELUARAN ===\n");
        printf("Nominal: Rp %d,00\n", nominal);
        printf("Kategori: %s\n", kategori);
        printf("Tanggal: %02d-%02d-%04d\n", t.d, t.m, t.y);

        char konfirmasi = 'x';
        while (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N') {
            printf("Simpan pengeluaran ini? (y/n): ");
            scanf(" %c", &konfirmasi);
            if (konfirmasi != 'y' && konfirmasi != 'Y' && konfirmasi != 'n' && konfirmasi != 'N') {
                printf("[ERROR] Input tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak.\n");
            }
        }

        if (konfirmasi == 'n' || konfirmasi == 'N') {
            printf("Pengeluaran dibatalkan.\n");
            // Tanya apakah ingin menambah transaksi lagi
            printf("\nApakah Anda ingin menambah transaksi pengeluaran lagi? (y/n): ");
            scanf(" %c", &lanjut);
            while (lanjut != 'y' && lanjut != 'Y' && lanjut != 'n' && lanjut != 'N') {
                printf("Input tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak.\n");
                printf("Apakah Anda ingin menambah transaksi pengeluaran lagi? (y/n): ");
                scanf(" %c", &lanjut);
            }
            continue;
        }

        // Simpan transaksi
        Transaksi *p = &tx[ntx++];
        p->id = ntx;
        strcpy(p->nama, "(Umum)");
        strcpy(p->tipe, "keluar");
        p->jumlah = nominal;
        strcpy(p->kategori, kategori);
        p->tgl = t;
        p->bulan = (t.m >= 9 || t.m <= 2 ? (t.m >= 9 ? t.m - 8 : t.m + 4) : t.m - 2);
        p->semester = sem;

        printf("[SUKSES] Pengeluaran berhasil dicatat!\n");
        printf("   ID Transaksi: %d\n", p->id);
        printf("   Nominal: Rp %d,00\n", nominal);
        printf("   Kategori: %s\n", kategori);

        // Tanya apakah ingin menambah transaksi lagi
        printf("\nApakah Anda ingin menambah transaksi pengeluaran lagi? (y/n): ");
        scanf(" %c", &lanjut);
        while (lanjut != 'y' && lanjut != 'Y' && lanjut != 'n' && lanjut != 'N') {
            printf("Input tidak valid! Harap masukkan 'y' untuk ya atau 'n' untuk tidak.\n");
            printf("Apakah Anda ingin menambah transaksi pengeluaran lagi? (y/n): ");
            scanf(" %c", &lanjut);
        }
    }

    printf("Selesai input pengeluaran.\n");
}
void ringkasan(int sem) {
    int in[6]={0}, out[6]={0};
    for(int i=0;i<ntx;i++){
        Transaksi *p=&tx[i]; if(p->semester!=sem) continue;
        if(cmpTanggal(p->tgl,bh.berjalan)>0) continue;
        int b=p->bulan-1;
        if(!strcmp(p->tipe,"kas")) in[b]+=p->jumlah;
        else out[b]+=p->jumlah;
    }
    printf("\n== Ringkasan Semester %d ==\n",sem);
    printf("Kas per bulan: Rp %d,00\n", KAS_PER_BULAN);
    printf("==========================================\n");
    int saldo=0;
    for(int i=0;i<6;i++){
        saldo += in[i]-out[i];
        printf("Bulan %d: Masuk=Rp %d, Keluar=Rp %d, Saldo=Rp %d\n",i+1,in[i],out[i],saldo);
    }
    printf("==========================================\n");
    printf("Total Saldo Semester: Rp %d\n", saldo);
}

void listTransaksi(int sem,int bulan) {
    int bulanSekarang = 6;
    if (inSemester(bh.berjalan, sem)) {
        int m = bh.berjalan.m;
        if (sem == 1) bulanSekarang = (m >= 9 ? m - 8 : m + 4);
        else          bulanSekarang = m - 2;
        bulanSekarang = bulanSekarang < 1 ? 1 : (bulanSekarang > 6 ? 6 : bulanSekarang);
    }

    // Validasi input
    if (bulan < 1 || bulan > bulanSekarang) {
        printf("Waktu belum terlewati!\n");
        return;
    }
    printf("\n== Transaksi Bulan %d - Semester %d ==\n",bulan,sem);
    printf("+----+-------------------------------+--------+---------------------------+-----------+------------+\n");
    printf("| ID | Nama                          | Tipe   | Kategori                  | Jumlah    | Tanggal    |\n");
    printf("+----+-------------------------------+--------+---------------------------+-----------+------------+\n");
    int ti=0,to=0,found=0;
    for(int i=0;i<ntx;i++){
        Transaksi *p=&tx[i]; if(p->semester!=sem||p->bulan!=bulan) continue;
        found=1;
        printf("| %-2d | %-29s | %-6s | %-25s | Rp %-6d | %02d-%02d-%04d |\n",
               p->id,p->nama,
               !strcmp(p->tipe,"kas")?"DEBIT":"KREDIT",
               p->kategori,p->jumlah,p->tgl.d,p->tgl.m,p->tgl.y);
        if(!strcmp(p->tipe,"kas")) ti+=p->jumlah; else to+=p->jumlah;
    }
    if(!found) printf("|        Tidak ada transaksi untuk bulan & semester ini.                             |\n");
    printf("+----+-------------------------------+--------+---------------------------+-----------+------------+\n");
    printf("Total DEBIT  = Rp %d\n",ti);
    printf("Total KREDIT = Rp %d\n",to);
    printf("Saldo bulan  = Rp %d\n",ti-to);
}

void hapusTransaksi(int sem,int bulan) {
    int id;
    do{
        listTransaksi(sem,bulan);
        printf("ID transaksi hapus (0 selesai): "); scanf("%d",&id);
        if(id==0) break;
        int idx=-1;
        for(int i=0;i<ntx;i++) if(tx[i].id==id&&tx[i].semester==sem&&tx[i].bulan==bulan) idx=i;
        if(idx<0){ printf("ID tidak ditemukan.\n"); continue; }

        // Jika yang dihapus adalah transaksi kas, update status bayar anggota
        if(!strcmp(tx[idx].tipe, "kas")) {
            Anggota *a = findAnggota(sem, tx[idx].nama);
            if(a) {
                a->bayar[tx[idx].bulan-1] = 0; // Set kembali ke belum bayar
                printf("Status pembayaran %s bulan %d direset ke belum bayar.\n",
                       a->nama, tx[idx].bulan);
            }
        }

        for(int j=idx;j<ntx-1;j++) tx[j]=tx[j+1];
        ntx--; printf("Transaksi %d dihapus.\n",id);
    }while(1);
    updateAllLabels(sem);
}
